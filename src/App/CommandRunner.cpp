#include "App/CommandRunner.hpp"

#include "App/PlainObserver.hpp"
#include "Core/Text/Text.hpp"
#include "Infrastructure/Json/TourKindNames.hpp"
#include "Infrastructure/Platform/Platform.hpp"
#include "Infrastructure/Prompts/PromptEjection.hpp"
#include "Infrastructure/Storage/StorageLayout.hpp"
#include "Terminal/Shell/TerminalApp.hpp"

#include <format>
#include <print>
namespace sloptour::app {

namespace {

constexpr int SUCCESS = 0;
constexpr int FAILURE = 1;
constexpr std::string_view HEAD_OFFSET = "HEAD~";

[[nodiscard]] bool is_interactive_terminal() {
    return infrastructure::platform::standard_streams_are_terminal();
}

[[nodiscard]] int failed(std::string_view message) {
    std::println(stderr, "sloptour: {}", message);
    return FAILURE;
}

}

int CommandRunner::operator()(const BrowseCommand& /*command*/) const {
    return interactive(terminal::GoHome{});
}

int CommandRunner::operator()(const CreateCommand& command) const {
    if (command.kind == domain::TourKind::Functional && core::is_blank(command.functionality)) {
        return failed("describe the functionality to follow: sloptour new functional \"checkout and payment\"");
    }
    const auto request = request_of(command);
    if (command.plain) {
        return generate_plainly(request);
    }
    return interactive(terminal::GoGenerate{.request = request});
}

int CommandRunner::operator()(const PlayCommand& command) const {
    if (command.tour) {
        auto id = domain::TourId::create(*command.tour);
        if (!id) {
            return failed(id.error().message());
        }
        return interactive(terminal::GoPlay{.tour = std::move(*id)});
    }
    auto tours = m_composition.services.library->summaries();
    if (!tours) {
        return failed(tours.error().message());
    }
    if (tours->empty()) {
        return failed("no tours yet - create one with `sloptour new onboarding` or `sloptour new recap`");
    }
    return interactive(terminal::GoPlay{.tour = tours->front().id});
}

int CommandRunner::operator()(const ListCommand& /*command*/) const {
    auto tours = m_composition.services.library->summaries();
    if (!tours) {
        return failed(tours.error().message());
    }
    if (tours->empty()) {
        std::println("no tours yet - create one with `sloptour new onboarding` or `sloptour new recap`");
        return SUCCESS;
    }
    for (const auto& tour : *tours) {
        std::println("{:<40} {:<10} {:>3}/{:<3} {}  {}", tour.id.value(), infrastructure::name_of(tour.metadata.kind),
                     tour.visited, tour.steps, tour.metadata.created_at, tour.metadata.title);
    }
    return SUCCESS;
}

int CommandRunner::operator()(const AgentsCommand& /*command*/) const {
    const auto& workspace = *m_composition.services.workspace;
    const auto default_agent = workspace.default_agent();
    for (const auto& agent : workspace.agents()) {
        std::println("{} {:<10} {:<16} {}{}", agent.installed ? "✓" : "✗", agent.name, agent.executable,
                     agent.description, agent.name == default_agent ? "  (default)" : "");
        if (!agent.models.empty()) {
            std::println("    models: {}", core::join(agent.models, ", "));
        }
    }
    return SUCCESS;
}

int CommandRunner::operator()(const EjectPromptsCommand& /*command*/) const {
    const auto directory = m_composition.storage / infrastructure::storage_layout::PROMPTS;
    auto written = infrastructure::eject_prompts(directory);
    if (!written) {
        return failed(written.error().message());
    }
    std::println("wrote {} prompts to {} - edit them to customise how tours are written", *written, directory.string());
    return SUCCESS;
}

int CommandRunner::interactive(terminal::Destination destination) const {
    if (!is_interactive_terminal()) {
        return failed("the interactive screen needs a terminal; use `sloptour list` or `sloptour new --plain`");
    }
    terminal::TerminalApp(m_composition.services, m_composition.terminal).run(std::move(destination));
    return SUCCESS;
}

int CommandRunner::generate_plainly(const application::GenerationRequest& request) const {
    PlainObserver observer;
    auto tour = m_composition.services.generator->execute(request, observer, std::stop_token{});
    if (!tour) {
        return failed(tour.error().message());
    }
    std::println("tour ready - open it with: sloptour play {}", tour->value());
    return SUCCESS;
}

application::ReplyLanguage CommandRunner::reply_language() const {
    const auto code = m_composition.services.preferences->current().language;
    for (const auto& pack : m_composition.terminal.packs) {
        if (pack.code == code) {
            return {.code = pack.code, .name = pack.agent_name};
        }
    }
    return {};
}

application::GenerationRequest CommandRunner::request_of(const CreateCommand& command) const {
    application::TourFocus focus = application::OnboardingFocus{};
    if (command.kind == domain::TourKind::Functional) {
        focus = application::FunctionalFocus{.functionality = command.functionality};
    }
    if (command.kind == domain::TourKind::Recap) {
        auto base = command.since;
        if (!base && command.last) {
            base = std::format("{}{}", HEAD_OFFSET, *command.last);
        }
        focus = application::RecapFocus{
            .scope = {.base_revision = std::move(base), .include_working_tree = command.include_working_tree}};
    }
    return {.focus = std::move(focus),
            .agent = {.name = command.agent.value_or(m_composition.services.workspace->default_agent()),
                      .model = command.model},
            .hint = command.focus,
            .name = command.name,
            .depth = command.depth,
            .detail = command.detail,
            .language = reply_language()};
}

}
