#pragma once

#include "Application/Generation/GenerationEvent.hpp"
#include "Terminal/Generation/ChapterStatus.hpp"

#include <chrono>
#include <cstddef>
#include <deque>
#include <optional>
#include <string>
#include <vector>

namespace sloptour::terminal {

struct ChapterRow {
    std::string title;
    ChapterStatus status;
    std::string activity;
    application::TokenUsage usage;
};

class GenerationProgress {
public:
    explicit GenerationProgress(std::size_t activity_limit)
        : m_activity_limit(activity_limit) {}

    void apply(const application::GenerationEvent& event);

    void operator()(const application::RepositoryInspected& event);
    void operator()(const application::OutlineStarted& event);
    void operator()(const application::OutlineReady& event);
    void operator()(const application::ChapterStarted& event);
    void operator()(const application::ChapterReady& event);
    void operator()(const application::ChapterFailed& event);
    void operator()(const application::AgentOutput& event);
    void operator()(const application::TokensUsed& event);
    void operator()(const application::QuizStarted& event);
    void operator()(const application::QuizReady& event);
    void operator()(const application::QuizFailed& event);
    void operator()(const application::TourSaved& event);

    [[nodiscard]] const application::TokenUsage& usage() const noexcept {
        return m_usage;
    }

    [[nodiscard]] const std::optional<ChapterRow>& quiz() const noexcept {
        return m_quiz;
    }

    [[nodiscard]] const std::string& repository() const noexcept {
        return m_repository;
    }

    [[nodiscard]] const std::string& scope() const noexcept {
        return m_scope;
    }

    [[nodiscard]] const std::string& agent() const noexcept {
        return m_agent;
    }

    [[nodiscard]] bool inspected() const noexcept {
        return m_inspected;
    }

    [[nodiscard]] const std::optional<std::string>& outline() const noexcept {
        return m_outline;
    }

    [[nodiscard]] bool outlining() const noexcept {
        return !m_agent.empty() && !m_outline;
    }

    [[nodiscard]] const std::vector<ChapterRow>& chapters() const noexcept {
        return m_chapters;
    }

    [[nodiscard]] const std::deque<std::string>& activity() const noexcept {
        return m_activity;
    }

    [[nodiscard]] std::size_t finished_chapters() const noexcept;

private:
    void remember(std::string line);

    std::size_t m_activity_limit;
    bool m_inspected = false;
    std::string m_repository;
    std::string m_scope;
    std::string m_agent;
    std::optional<std::string> m_outline;
    std::vector<ChapterRow> m_chapters;
    std::deque<std::string> m_activity;
    application::TokenUsage m_usage;
    std::optional<ChapterRow> m_quiz;
};

}
