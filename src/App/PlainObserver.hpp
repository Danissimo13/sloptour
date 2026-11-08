#pragma once

#include "Application/Generation/IGenerationObserver.hpp"
#include "Application/Ports/TokenUsage.hpp"

#include <mutex>

namespace sloptour::app {

class PlainObserver final : public application::IGenerationObserver {
public:
    void notify(const application::GenerationEvent& event) override;

    void operator()(const application::RepositoryInspected& event) const;
    void operator()(const application::OutlineStarted& event) const;
    void operator()(const application::OutlineReady& event) const;
    void operator()(const application::ChapterStarted& event) const;
    void operator()(const application::ChapterReady& event) const;
    void operator()(const application::ChapterFailed& event) const;
    void operator()(const application::AgentOutput& event) const;
    void operator()(const application::TokensUsed& event);
    void operator()(const application::QuizStarted& event) const;
    void operator()(const application::QuizReady& event) const;
    void operator()(const application::QuizFailed& event) const;
    void operator()(const application::TourSaved& event) const;

private:
    std::mutex m_mutex;
    application::TokenUsage m_usage;
};

}
