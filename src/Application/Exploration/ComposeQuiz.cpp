#include "Application/Exploration/ComposeQuiz.hpp"

#include "Application/Exploration/StepBrief.hpp"
#include "Application/Generation/AgentDialogue.hpp"
#include "Application/Generation/PromptNames.hpp"
#include "Application/Preferences/LanguageInstruction.hpp"

namespace sloptour::application {

namespace {

constexpr std::string_view QUIZ_LABEL = "quiz";

}

ApplicationResult<domain::Quiz> ComposeQuiz::execute(const Request& request, const AgentListener& listener,
                                                     const std::stop_token& stop) const {
    const auto& tour = request.tour;
    const auto& agent = request.agent;
    const auto& metadata = tour.metadata();
    auto depth = m_dependencies.settings.depth(metadata.depth);
    if (!depth) {
        return std::unexpected(depth.error());
    }
    auto found = m_dependencies.agents->find(agent);
    if (!found) {
        return std::unexpected(ApplicationError(found.error()));
    }
    auto prompt =
        m_dependencies.prompts->render(prompt_names::QUIZ, {
                                                               {"tour_title", metadata.title},
                                                               {"tour_summary", metadata.summary},
                                                               {"tour_scope", metadata.scope},
                                                               {"tour_digest", digest_of(tour)},
                                                               {"language", language_of(request.language)},
                                                               {"questions", std::to_string(depth->quiz_questions)},
                                                           });
    if (!prompt) {
        return std::unexpected(ApplicationError(prompt.error()));
    }
    const AgentDialogue dialogue(*found, m_dependencies.prompts, m_dependencies.settings.repair_attempts);
    const auto& decoder = *m_dependencies.decoder;
    return dialogue.request(
        AgentTask{.label = std::string(QUIZ_LABEL), .prompt = std::move(*prompt)},
        [&decoder](std::string_view reply) { return decoder.decode_quiz(reply); }, listener, stop);
}

}
