#pragma once

#include "Application/Generation/GenerationRequest.hpp"
#include "Application/Ports/PortErrors.hpp"
#include "Domain/Tour/TourId.hpp"
#include "Terminal/Generation/GenerationProgress.hpp"
#include "Terminal/Generation/PostingObserver.hpp"
#include "Terminal/Shell/Destination.hpp"
#include "Terminal/Shell/GuardedPoster.hpp"
#include "Terminal/Shell/TerminalContext.hpp"
#include "Terminal/Shell/Ticker.hpp"

#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>

#include <chrono>
#include <memory>
#include <optional>
#include <thread>

namespace sloptour::terminal {

class GenerationScreen {
public:
    GenerationScreen(std::shared_ptr<const TerminalContext> context, application::GenerationRequest request);
    ~GenerationScreen();
    GenerationScreen(const GenerationScreen&) = delete;
    GenerationScreen& operator=(const GenerationScreen&) = delete;
    GenerationScreen(GenerationScreen&&) = delete;
    GenerationScreen& operator=(GenerationScreen&&) = delete;

    [[nodiscard]] Destination run();

private:
    void start();
    void finish(application::ApplicationResult<domain::TourId> result);
    [[nodiscard]] bool handle(const ftxui::Event& event);
    [[nodiscard]] ftxui::Element render() const;
    [[nodiscard]] ftxui::Element render_phases() const;
    [[nodiscard]] ftxui::Element render_chapters() const;
    [[nodiscard]] ftxui::Element chapter_row(const ChapterRow& chapter, const std::string& label) const;
    [[nodiscard]] ftxui::Element render_activity() const;
    [[nodiscard]] ftxui::Element render_outcome() const;
    [[nodiscard]] ftxui::Element phase(bool done, bool active, std::string_view label) const;
    [[nodiscard]] std::string agent_label() const;
    [[nodiscard]] std::string t(std::string_view key) const;
    void leave(Destination destination);

    std::shared_ptr<const TerminalContext> m_context;
    application::GenerationRequest m_request;
    GenerationProgress m_progress;
    GuardedPoster m_poster;
    std::shared_ptr<PostingObserver> m_observer;
    std::chrono::steady_clock::time_point m_started = std::chrono::steady_clock::now();
    std::optional<std::chrono::steady_clock::duration> m_took;
    std::optional<application::ApplicationResult<domain::TourId>> m_result;
    Destination m_next = GoHome{};
    std::optional<Ticker> m_ticker;
    std::jthread m_worker;
};

}
