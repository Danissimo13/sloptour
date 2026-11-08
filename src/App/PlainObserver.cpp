#include "App/PlainObserver.hpp"

#include <format>
#include <print>
#include <string>

namespace sloptour::app {

void PlainObserver::notify(const application::GenerationEvent& event) {
    const std::scoped_lock lock(m_mutex);
    std::visit(*this, event);
}

void PlainObserver::operator()(const application::RepositoryInspected& event) const {
    std::println("repository {} - scope: {}", event.repository, event.scope);
}

void PlainObserver::operator()(const application::OutlineStarted& event) const {
    std::println("drafting the outline with {}...", event.agent);
}

void PlainObserver::operator()(const application::OutlineReady& event) const {
    std::println("outline: {}", event.title);
    std::size_t number = 1;
    for (const auto& chapter : event.chapters) {
        std::println("  {:>2}. {}", number++, chapter);
    }
}

void PlainObserver::operator()(const application::ChapterStarted& event) const {
    std::println("chapter {} - writing", event.index + 1);
}

void PlainObserver::operator()(const application::ChapterReady& event) const {
    std::println("chapter {} - done, {} steps", event.index + 1, event.steps);
}

void PlainObserver::operator()(const application::ChapterFailed& event) const {
    std::println(stderr, "chapter {} - failed: {}", event.index + 1, event.reason);
}

void PlainObserver::operator()(const application::AgentOutput& event) const {
    if (event.chapter) {
        std::println("  [{}] {}", *event.chapter + 1, event.line);
    } else {
        std::println("  {}", event.line);
    }
}

void PlainObserver::operator()(const application::TokensUsed& event) {
    m_usage += event.usage;
}

void PlainObserver::operator()(const application::QuizStarted& /*event*/) const {
    std::println("writing the final quiz...");
}

void PlainObserver::operator()(const application::QuizReady& event) const {
    std::println("quiz - done, {} questions", event.questions);
}

void PlainObserver::operator()(const application::QuizFailed& event) const {
    std::println(stderr, "quiz - failed: {}", event.reason);
}

void PlainObserver::operator()(const application::TourSaved& event) const {
    const std::string_view approximate = m_usage.estimated ? "~" : "";
    std::println("saved tour {} - tokens: {}{} in, {}{} out{}", event.id.value(), approximate, m_usage.prompt_side(),
                 approximate, m_usage.output,
                 m_usage.cost_usd > 0.0 ? std::format(", ${:.2f}", m_usage.cost_usd) : std::string());
}

}
