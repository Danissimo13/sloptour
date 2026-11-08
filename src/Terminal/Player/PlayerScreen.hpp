#pragma once

#include "Application/Exploration/ExplorationRequest.hpp"
#include "Application/Library/TourSession.hpp"
#include "Application/Ports/AgentChoice.hpp"
#include "Application/Ports/AgentListener.hpp"
#include "Application/Ports/PortErrors.hpp"
#include "Application/Ports/TokenUsage.hpp"
#include "Domain/Progress/Exchange.hpp"
#include "Domain/Quiz/Quiz.hpp"
#include "Domain/Tour/Step.hpp"
#include "Domain/Tour/StepPosition.hpp"
#include "Terminal/Player/ChatPanel.hpp"
#include "Terminal/Player/OutlineView.hpp"
#include "Terminal/Player/PlayerBinding.hpp"
#include "Terminal/Player/QuizPanel.hpp"
#include "Terminal/Player/RunningTask.hpp"
#include "Terminal/Player/StepView.hpp"
#include "Terminal/Shell/Destination.hpp"
#include "Terminal/Shell/GuardedPoster.hpp"
#include "Terminal/Shell/TerminalContext.hpp"
#include "Terminal/Shell/Ticker.hpp"

#include <ftxui/component/component_base.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>

#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <stop_token>
#include <string>
#include <thread>
#include <vector>

namespace sloptour::terminal {

class PlayerScreen {
public:
    PlayerScreen(std::shared_ptr<const TerminalContext> context, application::TourSession session);

    [[nodiscard]] Destination run();

private:
    using AgentWork = std::function<void(const std::stop_token&, const application::AgentListener&)>;
    using SourceLines = application::ApplicationResult<std::vector<std::string>>;

    void bind_navigation();
    void bind_actions();
    void bind_quiz();
    [[nodiscard]] bool handle(const ftxui::Event& event);
    [[nodiscard]] bool handle_task(const ftxui::Event& event);
    [[nodiscard]] bool handle_question(const ftxui::Event& event);
    [[nodiscard]] bool handle_mouse(const ftxui::Event& event);
    [[nodiscard]] bool handle_quiz_digit(const ftxui::Event& event);
    [[nodiscard]] static bool dispatch(const std::vector<PlayerBinding>& table, const ftxui::Event& event);

    void move_to(domain::StepPosition position);
    void next();
    void previous();
    void next_chapter();
    void previous_chapter();
    void last_step();
    void enter_quiz();
    void scroll(float delta);
    void scroll_chat(float delta);
    void cycle_location(int direction);
    void reveal();
    void open_question();
    void submit_question();
    void deepen();
    void compose_quiz();
    void answer_quiz(std::size_t option);
    void quiz_confirm();
    void quiz_cursor(int delta);
    void restart_quiz();
    void open_in_editor();

    void launch(RunningTask task, AgentWork work);
    void note_activity(std::string line);
    void note_usage(const application::TokenUsage& spent);
    void settle();
    void answered(const domain::StepId& step, application::ApplicationResult<domain::Exchange> result);
    void deepened(const domain::StepId& step, application::ApplicationResult<std::vector<domain::Step>> result);
    void quizzed(application::ApplicationResult<domain::Quiz> result);
    void remember(const application::ApplicationResult<void>& result);
    [[nodiscard]] application::AgentChoice agent_choice() const;
    [[nodiscard]] application::ExplorationRequest exploration(std::string question) const;
    [[nodiscard]] const SourceLines& source(std::string_view path) const;
    [[nodiscard]] const domain::Step& current_step() const;

    [[nodiscard]] ftxui::Element render() const;
    [[nodiscard]] ftxui::Element render_page() const;
    [[nodiscard]] ftxui::Element render_header() const;
    [[nodiscard]] ftxui::Element render_progress() const;
    [[nodiscard]] ftxui::Element render_footer() const;
    [[nodiscard]] std::optional<ftxui::Element> render_overlay() const;
    [[nodiscard]] ftxui::Element render_help() const;
    [[nodiscard]] ftxui::Element render_task(const RunningTask& task) const;
    void leave(Destination destination);

    std::shared_ptr<const TerminalContext> m_context;
    application::TourSession m_session;
    OutlineView m_outline;
    StepView m_step_view;
    ChatPanel m_chat;
    QuizPanel m_quiz;
    domain::StepPosition m_position;
    std::size_t m_location = 0;
    float m_scroll = 0.0F;
    float m_chat_scroll = 1.0F;
    bool m_on_quiz = false;
    bool m_show_outline = true;
    bool m_show_chat = true;
    bool m_show_help = false;
    bool m_asking = false;
    std::string m_question;
    std::string m_agent;
    application::TokenUsage m_spent;
    std::optional<std::string> m_notice;
    std::optional<RunningTask> m_task;
    mutable std::map<std::string, SourceLines, std::less<>> m_sources;
    std::vector<PlayerBinding> m_bindings;
    std::vector<PlayerBinding> m_quiz_bindings;
    ftxui::Component m_question_input;
    Destination m_next = GoHome{};
    GuardedPoster m_poster;
    std::optional<Ticker> m_ticker;
    std::jthread m_worker;
};

}
