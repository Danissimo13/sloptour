#include "Terminal/Generation/GenerationProgress.hpp"

#include "Terminal/Widgets/Glyphs.hpp"

#include <algorithm>
#include <format>

namespace sloptour::terminal {

void GenerationProgress::apply(const application::GenerationEvent& event) {
    std::visit(*this, event);
}

void GenerationProgress::operator()(const application::RepositoryInspected& event) {
    m_inspected = true;
    m_repository = event.repository;
    m_scope = event.scope;
}

void GenerationProgress::operator()(const application::OutlineStarted& event) {
    m_agent = event.agent;
}

void GenerationProgress::operator()(const application::OutlineReady& event) {
    m_outline = event.title;
    for (const auto& title : event.chapters) {
        m_chapters.push_back({.title = title, .status = ChapterQueued{}, .activity = {}});
    }
}

void GenerationProgress::operator()(const application::ChapterStarted& event) {
    if (event.index < m_chapters.size()) {
        m_chapters.at(event.index).status = ChapterWriting{.started = std::chrono::steady_clock::now()};
    }
}

void GenerationProgress::operator()(const application::ChapterReady& event) {
    if (event.index >= m_chapters.size()) {
        return;
    }
    auto& row = m_chapters.at(event.index);
    const auto took = std::holds_alternative<ChapterWriting>(row.status)
                          ? std::chrono::steady_clock::now() - std::get<ChapterWriting>(row.status).started
                          : std::chrono::steady_clock::duration{};
    row.status = ChapterWritten{.steps = event.steps, .took = took};
}

void GenerationProgress::operator()(const application::ChapterFailed& event) {
    if (event.index < m_chapters.size()) {
        m_chapters.at(event.index).status = ChapterBroken{.reason = event.reason};
    }
    remember(std::format("[{}] {} {}", event.index + 1, glyphs::FAILED, event.reason));
}

void GenerationProgress::operator()(const application::AgentOutput& event) {
    if (event.chapter && *event.chapter < m_chapters.size()) {
        m_chapters.at(*event.chapter).activity = event.line;
        remember(std::format("[{}] {}", *event.chapter + 1, event.line));
        return;
    }
    if (m_quiz) {
        m_quiz->activity = event.line;
    }
    remember(event.line);
}

void GenerationProgress::operator()(const application::TokensUsed& event) {
    m_usage += event.usage;
    if (event.chapter && *event.chapter < m_chapters.size()) {
        m_chapters.at(*event.chapter).usage += event.usage;
    } else if (m_quiz) {
        m_quiz->usage += event.usage;
    }
}

void GenerationProgress::operator()(const application::QuizStarted& /*event*/) {
    m_quiz = ChapterRow{.title = {},
                        .status = ChapterWriting{.started = std::chrono::steady_clock::now()},
                        .activity = {},
                        .usage = {}};
}

void GenerationProgress::operator()(const application::QuizReady& event) {
    if (m_quiz) {
        const auto took = std::chrono::steady_clock::now() - std::get<ChapterWriting>(m_quiz->status).started;
        m_quiz->status = ChapterWritten{.steps = event.questions, .unit = "question", .took = took};
    }
}

void GenerationProgress::operator()(const application::QuizFailed& event) {
    if (m_quiz) {
        m_quiz->status = ChapterBroken{.reason = event.reason};
    }
    remember(std::format("[{}] {} {}", glyphs::STAR, glyphs::FAILED, event.reason));
}

void GenerationProgress::operator()(const application::TourSaved& /*event*/) {}

std::size_t GenerationProgress::finished_chapters() const noexcept {
    return static_cast<std::size_t>(std::ranges::count_if(m_chapters, [](const ChapterRow& row) {
        return !std::holds_alternative<ChapterQueued>(row.status) &&
               !std::holds_alternative<ChapterWriting>(row.status);
    }));
}

void GenerationProgress::remember(std::string line) {
    m_activity.push_back(std::move(line));
    while (m_activity.size() > m_activity_limit) {
        m_activity.pop_front();
    }
}

}
