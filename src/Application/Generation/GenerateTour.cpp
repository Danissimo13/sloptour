#include "Application/Generation/GenerateTour.hpp"

#include "Application/Generation/FocusBriefer.hpp"
#include "Application/Generation/GenerationPrompts.hpp"
#include "Application/Generation/StepIdRegistry.hpp"
#include "Core/Text/Text.hpp"

#include <algorithm>
#include <atomic>
#include <format>
#include <thread>

namespace sloptour::application {

namespace {

constexpr std::string_view OUTLINE_LABEL = "outline";
constexpr std::string_view CANCELLED = "generation cancelled";
constexpr std::string_view NOT_WRITTEN = "chapter was not written";

[[nodiscard]] std::vector<std::string> titles_of(const domain::TourOutline& outline) {
    std::vector<std::string> titles;
    for (const auto& plan : outline.chapters()) {
        titles.push_back(plan.title);
    }
    return titles;
}

[[nodiscard]] ApplicationResult<domain::TourOutline> limited(domain::TourOutline outline, std::size_t max_chapters) {
    if (outline.chapters().size() <= max_chapters) {
        return outline;
    }
    auto plans = outline.chapters();
    plans.resize(max_chapters);
    return lifted(domain::TourOutline::create(outline.title(), outline.summary(), std::move(plans)));
}

[[nodiscard]] ApplicationResult<domain::Chapter> chapter_from(const domain::ChapterPlan& plan,
                                                              const ApplicationResult<std::vector<domain::Step>>& steps,
                                                              StepIdRegistry& registry) {
    if (!steps) {
        return std::unexpected(steps.error());
    }
    auto claimed = registry.claim(*steps, plan.id);
    if (!claimed) {
        return std::unexpected(claimed.error());
    }
    return lifted(domain::Chapter::create(plan.title, plan.goal, std::move(*claimed)));
}

[[nodiscard]] ApplicationResult<std::vector<domain::Chapter>>
collected(const domain::TourOutline& outline,
          const std::vector<ApplicationResult<std::vector<domain::Step>>>& chapters) {
    StepIdRegistry registry;
    std::vector<domain::Chapter> written;
    std::optional<ApplicationError> first_failure;
    for (std::size_t index = 0; index < chapters.size(); ++index) {
        auto chapter = chapter_from(outline.chapters().at(index), chapters.at(index), registry);
        if (chapter) {
            written.push_back(std::move(*chapter));
        } else if (!first_failure) {
            first_failure = chapter.error();
        }
    }
    if (written.empty()) {
        const auto reason = first_failure ? first_failure->message() : std::string(NOT_WRITTEN);
        return std::unexpected(ApplicationError("no chapter could be written: " + reason));
    }
    return written;
}

[[nodiscard]] AgentListener chapter_listener(IGenerationObserver& observer, std::optional<std::size_t> chapter) {
    return {.activity =
                [&observer, chapter](std::string_view line) {
                    observer.notify(AgentOutput{.chapter = chapter, .line = std::string(line)});
                },
            .usage = [&observer, chapter](
                         const TokenUsage& spent) { observer.notify(TokensUsed{.chapter = chapter, .usage = spent}); }};
}

[[nodiscard]] ApplicationResult<std::vector<domain::Step>>
write_chapter(const StepWriter& writer, const GenerationContext& context, const domain::TourOutline& outline,
              std::size_t index, IGenerationObserver& observer, const std::stop_token& stop) {
    observer.notify(ChapterStarted{.index = index});
    const auto variables = chapter_variables(context.variables, outline, index);
    auto steps = writer.write(context.briefing.chapter_prompt, variables, outline.chapters().at(index).title,
                              chapter_listener(observer, index), stop);
    if (steps) {
        observer.notify(ChapterReady{.index = index, .steps = steps->size()});
    } else {
        observer.notify(ChapterFailed{.index = index, .reason = steps.error().message()});
    }
    return steps;
}

}

GenerateTour::GenerateTour(Dependencies dependencies)
    : m_dependencies(std::move(dependencies)) {}

ApplicationResult<domain::TourId> GenerateTour::execute(const GenerationRequest& request, IGenerationObserver& observer,
                                                        const std::stop_token& stop) const {
    auto agent = m_dependencies.agents->find(request.agent);
    if (!agent) {
        return std::unexpected(ApplicationError(agent.error()));
    }
    const auto dialogue =
        std::make_shared<const AgentDialogue>(*agent, m_dependencies.prompts, m_dependencies.settings.repair_attempts);
    auto context = prepare(request, observer);
    if (!context) {
        return std::unexpected(context.error());
    }
    observer.notify(OutlineStarted{.agent = std::string(dialogue->agent_name())});
    auto outline = draft_outline(*dialogue, *context, observer, stop);
    if (!outline) {
        return std::unexpected(outline.error());
    }
    observer.notify(OutlineReady{.title = outline->title(), .chapters = titles_of(*outline)});
    auto tour = build(request, *context, *outline, dialogue, observer, stop);
    if (!tour) {
        return std::unexpected(tour.error());
    }
    if (auto saved = m_dependencies.tours->save(*tour); !saved) {
        return std::unexpected(ApplicationError(saved.error()));
    }
    observer.notify(TourSaved{.id = tour->id()});
    return tour->id();
}

ApplicationResult<domain::Tour> GenerateTour::build(const GenerationRequest& request, const GenerationContext& context,
                                                    const domain::TourOutline& outline,
                                                    const std::shared_ptr<const AgentDialogue>& dialogue,
                                                    IGenerationObserver& observer, const std::stop_token& stop) const {
    const StepWriter writer({.dialogue = dialogue,
                             .prompts = m_dependencies.prompts,
                             .decoder = m_dependencies.decoder,
                             .max_steps = context.depth.max_steps});
    const auto chapters = write_chapters(writer, context, outline, observer, stop);
    if (stop.stop_requested()) {
        return std::unexpected(ApplicationError(std::string(CANCELLED)));
    }
    auto tour = assemble(request, context, outline, chapters, dialogue->agent_name());
    if (tour && context.depth.quiz_questions > 0) {
        add_quiz(*tour, request, context, observer, stop);
    }
    if (stop.stop_requested()) {
        return std::unexpected(ApplicationError(std::string(CANCELLED)));
    }
    return tour;
}

void GenerateTour::add_quiz(domain::Tour& tour, const GenerationRequest& request, const GenerationContext& /*context*/,
                            IGenerationObserver& observer, const std::stop_token& stop) const {
    observer.notify(QuizStarted{});
    auto quiz = m_dependencies.quiz->execute({.tour = tour, .agent = request.agent, .language = request.language},
                                             chapter_listener(observer, std::nullopt), stop);
    if (!quiz) {
        observer.notify(QuizFailed{.reason = quiz.error().message()});
        return;
    }
    observer.notify(QuizReady{.questions = quiz->questions().size()});
    tour.attach_quiz(std::move(*quiz));
}

ApplicationResult<GenerationContext> GenerateTour::prepare(const GenerationRequest& request,
                                                           IGenerationObserver& observer) const {
    auto overview = m_dependencies.inspector->overview();
    if (!overview) {
        return std::unexpected(ApplicationError(overview.error()));
    }
    auto briefing = FocusBriefer(m_dependencies.inspector).brief(request.focus);
    if (!briefing) {
        return std::unexpected(briefing.error());
    }
    observer.notify(RepositoryInspected{.repository = overview->name, .scope = briefing->scope});
    auto depth = m_dependencies.settings.depth(request.depth);
    auto detail = m_dependencies.settings.detail(request.detail);
    if (!depth || !detail) {
        return std::unexpected(!depth ? depth.error() : detail.error());
    }
    auto variables = outline_variables(request, *overview, *briefing, *depth, *detail);
    return GenerationContext{.overview = std::move(*overview),
                             .briefing = std::move(*briefing),
                             .depth = std::move(*depth),
                             .detail = std::move(*detail),
                             .variables = std::move(variables)};
}

ApplicationResult<domain::TourOutline> GenerateTour::draft_outline(const AgentDialogue& dialogue,
                                                                   const GenerationContext& context,
                                                                   IGenerationObserver& observer,
                                                                   const std::stop_token& stop) const {
    auto prompt = m_dependencies.prompts->render(context.briefing.outline_prompt, context.variables);
    if (!prompt) {
        return std::unexpected(ApplicationError(prompt.error()));
    }
    const auto& decoder = *m_dependencies.decoder;
    auto outline = dialogue.request(
        AgentTask{.label = std::string(OUTLINE_LABEL), .prompt = std::move(*prompt)},
        [&decoder](std::string_view reply) { return decoder.decode_outline(reply); },
        chapter_listener(observer, std::nullopt), stop);
    if (!outline) {
        return std::unexpected(outline.error());
    }
    return limited(std::move(*outline), context.depth.max_chapters);
}

std::vector<GenerateTour::ChapterSteps> GenerateTour::write_chapters(const StepWriter& writer,
                                                                     const GenerationContext& context,
                                                                     const domain::TourOutline& outline,
                                                                     IGenerationObserver& observer,
                                                                     const std::stop_token& stop) const {
    const auto count = outline.chapters().size();
    std::vector<ChapterSteps> results(count, std::unexpected(ApplicationError(std::string(NOT_WRITTEN))));
    std::atomic<std::size_t> next{0};
    const auto work = [&] {
        for (auto index = next++; index < count && !stop.stop_requested(); index = next++) {
            results.at(index) = write_chapter(writer, context, outline, index, observer, stop);
        }
    };
    const auto worker_count = std::clamp<std::size_t>(m_dependencies.settings.parallel_chapters, 1, count);
    std::vector<std::jthread> workers;
    workers.reserve(worker_count);
    for (std::size_t worker = 0; worker < worker_count; ++worker) {
        workers.emplace_back(work);
    }
    workers.clear();
    return results;
}

ApplicationResult<domain::Tour> GenerateTour::assemble(const GenerationRequest& request,
                                                       const GenerationContext& context,
                                                       const domain::TourOutline& outline,
                                                       const std::vector<ChapterSteps>& chapters,
                                                       std::string_view agent) const {
    auto written = collected(outline, chapters);
    if (!written) {
        return std::unexpected(written.error());
    }
    auto id = identify(request, context);
    if (!id) {
        return std::unexpected(id.error());
    }
    domain::TourMetadata metadata{.title = outline.title(),
                                  .summary = outline.summary(),
                                  .kind = context.briefing.kind,
                                  .repository = context.overview.name,
                                  .scope = context.briefing.scope,
                                  .agent = std::string(agent),
                                  .model = request.agent.model,
                                  .depth = context.depth.name,
                                  .detail = context.detail.name,
                                  .language = request.language.code,
                                  .created_at = m_dependencies.clock->timestamp()};
    return lifted(domain::Tour::create(std::move(*id), std::move(metadata), std::move(*written)));
}

ApplicationResult<domain::TourId> GenerateTour::identify(const GenerationRequest& request,
                                                         const GenerationContext& context) const {
    const auto name = request.name.has_value() && !core::is_blank(*request.name)
                          ? *request.name
                          : std::format("{}-{}", context.briefing.label, m_dependencies.clock->compact_stamp());
    return lifted(domain::TourId::create(core::slugify(name)));
}

}
