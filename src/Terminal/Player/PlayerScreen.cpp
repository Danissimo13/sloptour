#include "Terminal/Player/PlayerScreen.hpp"

#include "Core/Text/Text.hpp"
#include "Terminal/Widgets/Chrome.hpp"
#include "Terminal/Widgets/Glyphs.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/mouse.hpp>

#include <algorithm>
#include <array>
#include <format>

namespace sloptour::terminal {

namespace {

constexpr float LINE_SCROLL = 0.06F;
constexpr float PAGE_SCROLL = 0.3F;
constexpr float CHAT_SCROLL = 0.15F;
constexpr auto TICK = std::chrono::milliseconds(100);
constexpr int OVERLAY_WIDTH = 90;
constexpr std::size_t HEADER_TITLE_WIDTH = 64;
constexpr int KEY_COLUMN_WIDTH = 16;
constexpr std::size_t ACTIVITY_WIDTH = 84;
constexpr std::size_t TASK_ACTIVITY_LINES = 6;
constexpr char FIRST_DIGIT = '1';
constexpr char LAST_DIGIT = '9';

constexpr std::array STEP_HINTS{
    KeyHint{.key = "←→", .label = "hint.step"},  KeyHint{.key = "↑↓", .label = "hint.scroll"},
    KeyHint{.key = "tab", .label = "hint.code"}, KeyHint{.key = "a", .label = "hint.ask"},
    KeyHint{.key = "d", .label = "hint.deeper"}, KeyHint{.key = "o", .label = "hint.open_editor"},
    KeyHint{.key = "c", .label = "hint.chat"},   KeyHint{.key = "?", .label = "hint.help"},
    KeyHint{.key = "q", .label = "hint.back"},
};
constexpr std::array QUIZ_HINTS{
    KeyHint{.key = "↑↓", .label = "hint.choose"},   KeyHint{.key = "enter 1-9", .label = "hint.answer"},
    KeyHint{.key = "←→", .label = "hint.question"}, KeyHint{.key = "r", .label = "hint.retake"},
    KeyHint{.key = "?", .label = "hint.help"},      KeyHint{.key = "q", .label = "hint.back"},
};

[[nodiscard]] ftxui::Event key(char character) {
    return ftxui::Event::Character(character);
}

void bind(std::vector<PlayerBinding>& table, std::vector<ftxui::Event> events, std::string_view keys,
          std::string_view description, std::function<void()> action) {
    table.push_back(
        {.events = std::move(events), .keys = keys, .description = description, .action = std::move(action)});
}

[[nodiscard]] std::string agent_for(const TerminalContext& context, const std::string& preferred) {
    for (const auto& agent : context.services.workspace->agents()) {
        if (agent.name == preferred && agent.installed) {
            return preferred;
        }
    }
    return context.services.workspace->default_agent();
}

}

PlayerScreen::PlayerScreen(std::shared_ptr<const TerminalContext> context, application::TourSession session)
    : m_context(std::move(context))
    , m_session(std::move(session))
    , m_outline(m_context->theme, m_context->settings.outline_width, m_context->localizer)
    , m_step_view(m_context->theme, m_context->markdown,
                  CodeExcerptView(m_context->theme, m_context->highlighter, m_context->markdown, m_context->settings,
                                  m_context->localizer),
                  m_context->localizer)
    , m_chat(m_context->theme, m_context->markdown, m_context->localizer)
    , m_quiz(m_context->theme, m_context->markdown, m_context->localizer)
    , m_position(m_session.resume_position())
    , m_agent(agent_for(*m_context, m_session.tour().metadata().agent))
    , m_poster(m_context->controls) {
    bind_quiz();
    bind_navigation();
    bind_actions();
    auto option = ftxui::InputOption::Default();
    option.multiline = false;
    option.placeholder = m_context->localizer->text("chat.placeholder");
    option.on_enter = [this] { submit_question(); };
    m_question_input = ftxui::Input(&m_question, option);
    remember(m_session.visit(m_position));
}

Destination PlayerScreen::run() {
    auto root = ftxui::Container::Vertical({m_question_input});
    auto component = ftxui::Renderer(root, [this] { return render(); });
    component |= ftxui::CatchEvent([this](const ftxui::Event& event) { return handle(event); });
    m_context->controls.loop(component);
    m_worker.request_stop();
    return m_next;
}

void PlayerScreen::bind_navigation() {
    auto& table = m_bindings;
    bind(table, {ftxui::Event::ArrowRight, key('l'), key(' '), key('n')}, "→ l space", "keys.next_step",
         [this] { next(); });
    bind(table, {ftxui::Event::ArrowLeft, key('h'), key('p')}, "← h", "keys.previous_step", [this] { previous(); });
    bind(table, {ftxui::Event::ArrowDown, key('j')}, "↓ j", "keys.scroll_down", [this] { scroll(LINE_SCROLL); });
    bind(table, {ftxui::Event::ArrowUp, key('k')}, "↑ k", "keys.scroll_up", [this] { scroll(-LINE_SCROLL); });
    bind(table, {ftxui::Event::PageDown}, "pgdn", "keys.page_down", [this] { scroll(PAGE_SCROLL); });
    bind(table, {ftxui::Event::PageUp}, "pgup", "keys.page_up", [this] { scroll(-PAGE_SCROLL); });
    bind(table, {key(']')}, "]", "keys.next_chapter", [this] { next_chapter(); });
    bind(table, {key('[')}, "[", "keys.previous_chapter", [this] { previous_chapter(); });
    bind(table, {ftxui::Event::Home, key('g')}, "g home", "keys.first_step", [this] { move_to({}); });
    bind(table, {ftxui::Event::End, key('G')}, "G end", "keys.last_step", [this] { last_step(); });
    bind(table, {key('Q')}, "Q", "keys.jump_quiz", [this] { enter_quiz(); });
}

void PlayerScreen::bind_actions() {
    auto& table = m_bindings;
    bind(table, {ftxui::Event::Tab}, "tab", "keys.next_location", [this] { cycle_location(1); });
    bind(table, {ftxui::Event::TabReverse}, "shift+tab", "keys.previous_location", [this] { cycle_location(-1); });
    bind(table, {ftxui::Event::Return, key('r')}, "enter r", "keys.reveal", [this] { reveal(); });
    bind(table, {key('a'), key('i')}, "a i", "keys.ask", [this] { open_question(); });
    bind(table, {key('{')}, "{", "keys.chat_up", [this] { scroll_chat(-CHAT_SCROLL); });
    bind(table, {key('}')}, "}", "keys.chat_down", [this] { scroll_chat(CHAT_SCROLL); });
    bind(table, {key('c')}, "c", "keys.toggle_chat", [this] { m_show_chat = !m_show_chat; });
    bind(table, {key('d')}, "d", "keys.deeper", [this] { deepen(); });
    bind(table, {key('o')}, "o", "keys.open", [this] { open_in_editor(); });
    bind(table, {key('t')}, "t", "keys.toggle_outline", [this] { m_show_outline = !m_show_outline; });
    bind(table, {key('?')}, "?", "keys.help", [this] { m_show_help = true; });
    bind(table, {key('q'), ftxui::Event::Escape}, "q esc", "keys.back", [this] { leave(GoHome{}); });
}

void PlayerScreen::bind_quiz() {
    auto& table = m_quiz_bindings;
    bind(table, {ftxui::Event::ArrowDown, key('j')}, "↓ j", "keys.quiz_next", [this] { quiz_cursor(1); });
    bind(table, {ftxui::Event::ArrowUp, key('k')}, "↑ k", "keys.quiz_previous", [this] { quiz_cursor(-1); });
    bind(table, {ftxui::Event::Return}, "enter", "keys.quiz_answer", [this] { quiz_confirm(); });
    bind(table, {key('r')}, "r", "keys.quiz_retake", [this] { restart_quiz(); });
}

bool PlayerScreen::handle(const ftxui::Event& event) {
    if (event == ftxui::Event::Custom) {
        return false;
    }
    if (m_task) {
        return handle_task(event);
    }
    if (m_asking) {
        return handle_question(event);
    }
    if (event.is_mouse()) {
        return handle_mouse(event);
    }
    m_notice.reset();
    if (m_show_help) {
        m_show_help = false;
        return true;
    }
    if (m_on_quiz && (handle_quiz_digit(event) || dispatch(m_quiz_bindings, event))) {
        return true;
    }
    std::ignore = dispatch(m_bindings, event);
    return true;
}

bool PlayerScreen::dispatch(const std::vector<PlayerBinding>& table, const ftxui::Event& event) {
    const auto found = std::ranges::find_if(table, [&event](const PlayerBinding& binding) {
        return std::ranges::find(binding.events, event) != binding.events.end();
    });
    if (found == table.end()) {
        return false;
    }
    found->action();
    return true;
}

bool PlayerScreen::handle_task(const ftxui::Event& event) {
    if (event == ftxui::Event::Escape) {
        m_worker.request_stop();
    }
    return true;
}

bool PlayerScreen::handle_question(const ftxui::Event& event) {
    if (event == ftxui::Event::Escape) {
        m_asking = false;
        return true;
    }
    return false;
}

bool PlayerScreen::handle_mouse(const ftxui::Event& event) {
    auto copy = event;
    const auto mouse = copy.mouse();
    if (mouse.button == ftxui::Mouse::WheelDown) {
        scroll(LINE_SCROLL);
    } else if (mouse.button == ftxui::Mouse::WheelUp) {
        scroll(-LINE_SCROLL);
    }
    return true;
}

bool PlayerScreen::handle_quiz_digit(const ftxui::Event& event) {
    if (!event.is_character() || event.character().size() != 1) {
        return false;
    }
    const auto digit = event.character().front();
    if (digit < FIRST_DIGIT || digit > LAST_DIGIT) {
        return false;
    }
    answer_quiz(static_cast<std::size_t>(digit - FIRST_DIGIT));
    return true;
}

void PlayerScreen::move_to(domain::StepPosition position) {
    m_on_quiz = false;
    m_position = position;
    m_location = 0;
    m_scroll = 0.0F;
    m_chat_scroll = 1.0F;
    remember(m_session.visit(position));
}

void PlayerScreen::next() {
    if (m_on_quiz) {
        if (const auto& quiz = m_session.tour().quiz()) {
            m_quiz.next(*quiz);
        }
        return;
    }
    if (const auto following = m_session.tour().next(m_position)) {
        move_to(*following);
        return;
    }
    enter_quiz();
}

void PlayerScreen::previous() {
    if (m_on_quiz) {
        if (!m_quiz.previous()) {
            move_to(m_position);
        }
        return;
    }
    if (const auto preceding = m_session.tour().previous(m_position)) {
        move_to(*preceding);
    }
}

void PlayerScreen::next_chapter() {
    if (m_position.chapter + 1 < m_session.tour().chapters().size()) {
        move_to({.chapter = m_position.chapter + 1, .step = 0});
    }
}

void PlayerScreen::previous_chapter() {
    move_to({.chapter = m_position.step == 0 && m_position.chapter > 0 ? m_position.chapter - 1 : m_position.chapter,
             .step = 0});
}

void PlayerScreen::last_step() {
    const auto& chapters = m_session.tour().chapters();
    move_to({.chapter = chapters.size() - 1, .step = chapters.back().steps().size() - 1});
}

void PlayerScreen::enter_quiz() {
    m_on_quiz = true;
    m_scroll = 0.0F;
    if (const auto& quiz = m_session.tour().quiz()) {
        m_quiz.start(*quiz, m_session.progress());
    }
}

void PlayerScreen::scroll(float delta) {
    m_scroll = std::clamp(m_scroll + delta, 0.0F, 1.0F);
}

void PlayerScreen::scroll_chat(float delta) {
    m_chat_scroll = std::clamp(m_chat_scroll + delta, 0.0F, 1.0F);
}

void PlayerScreen::cycle_location(int direction) {
    const auto count = current_step().locations().size();
    if (count == 0) {
        return;
    }
    const auto offset = direction > 0 ? 1U : count - 1;
    m_location = (m_location + offset) % count;
}

void PlayerScreen::reveal() {
    if (current_step().checkpoint()) {
        remember(m_session.reveal(current_step().id()));
    }
}

void PlayerScreen::open_question() {
    m_show_chat = true;
    m_question.clear();
    m_asking = true;
    m_question_input->TakeFocus();
}

void PlayerScreen::submit_question() {
    m_asking = false;
    if (core::is_blank(m_question)) {
        return;
    }
    auto request = exploration(m_question);
    const auto step = request.step;
    m_chat_scroll = 1.0F;
    launch({.label = m_context->localizer->format("player.asking", {m_agent}),
            .started = std::chrono::steady_clock::now(),
            .activity = {},
            .usage = {},
            .in_chat = true,
            .question = m_question},
           [this, asker = m_context->services.asker, request = std::move(request),
            step](const std::stop_token& stop, const application::AgentListener& listener) {
               auto result = asker->execute(request, listener, stop);
               m_poster.post([this, step, result = std::move(result)] { answered(step, result); });
           });
}

void PlayerScreen::deepen() {
    if (m_on_quiz) {
        return;
    }
    auto request = exploration({});
    const auto step = request.step;
    launch({.label = m_context->localizer->format("player.deepening", {m_agent}),
            .started = std::chrono::steady_clock::now(),
            .activity = {},
            .usage = {},
            .in_chat = false,
            .question = {}},
           [this, deepener = m_context->services.deepener, request = std::move(request),
            step](const std::stop_token& stop, const application::AgentListener& listener) {
               auto result = deepener->execute(request, listener, stop);
               m_poster.post([this, step, result = std::move(result)] { deepened(step, result); });
           });
}

void PlayerScreen::compose_quiz() {
    launch({.label = m_context->localizer->format("player.writing_quiz", {m_agent}),
            .started = std::chrono::steady_clock::now(),
            .activity = {},
            .usage = {},
            .in_chat = false,
            .question = {}},
           [this, quizzer = m_context->services.quizzer,
            request = application::ComposeQuiz::Request{.tour = m_session.tour(),
                                                        .agent = agent_choice(),
                                                        .language = m_context->localizer->reply_language()}](
               const std::stop_token& stop, const application::AgentListener& listener) {
               auto result = quizzer->execute(request, listener, stop);
               m_poster.post([this, result = std::move(result)] { quizzed(result); });
           });
}

void PlayerScreen::answer_quiz(std::size_t option) {
    const auto& quiz = m_session.tour().quiz();
    if (!quiz) {
        return;
    }
    const auto question = m_quiz.question(*quiz);
    if (!question || m_session.progress().quiz_answer(*question) ||
        option >= quiz->questions().at(*question).options().size()) {
        return;
    }
    remember(m_session.answer(*question, option));
}

void PlayerScreen::quiz_confirm() {
    if (!m_session.tour().quiz()) {
        compose_quiz();
        return;
    }
    answer_quiz(m_quiz.cursor());
}

void PlayerScreen::quiz_cursor(int delta) {
    if (const auto& quiz = m_session.tour().quiz()) {
        m_quiz.move_cursor(delta, *quiz);
    }
}

void PlayerScreen::restart_quiz() {
    remember(m_session.restart_quiz());
    m_quiz.restart();
}

void PlayerScreen::open_in_editor() {
    if (m_on_quiz) {
        return;
    }
    const auto& step = current_step();
    if (step.locations().empty()) {
        m_notice = m_context->localizer->text("player.no_location");
        return;
    }
    const auto& location = step.locations().at(std::min(m_location, step.locations().size() - 1));
    const auto line = location.lines() ? location.lines()->first() : 1;
    application::StorageResult<void> opened;
    m_context->controls.suspended([&] { opened = m_context->services.editor->open(location.path(), line); });
    if (!opened) {
        m_notice = opened.error().message();
    }
}

void PlayerScreen::launch(RunningTask task, AgentWork work) {
    m_task = std::move(task);
    m_ticker.emplace(TICK, [poster = m_poster] { poster.redraw(); });
    m_worker = std::jthread([this, work = std::move(work)](const std::stop_token& stop) {
        const application::AgentListener listener{
            .activity =
                [this](std::string_view line) {
                    m_poster.post([this, line = std::string(line)] { note_activity(line); });
                },
            .usage =
                [this](const application::TokenUsage& spent) { m_poster.post([this, spent] { note_usage(spent); }); }};
        work(stop, listener);
    });
}

void PlayerScreen::note_activity(std::string line) {
    if (!m_task) {
        return;
    }
    m_task->activity.push_back(std::move(line));
    while (m_task->activity.size() > TASK_ACTIVITY_LINES) {
        m_task->activity.pop_front();
    }
}

void PlayerScreen::note_usage(const application::TokenUsage& spent) {
    m_spent += spent;
    if (m_task) {
        m_task->usage += spent;
    }
}

void PlayerScreen::settle() {
    m_task.reset();
    m_ticker.reset();
}

void PlayerScreen::answered(const domain::StepId& step, application::ApplicationResult<domain::Exchange> result) {
    settle();
    if (!result) {
        m_notice = result.error().message();
        return;
    }
    remember(m_session.record(step, std::move(*result)));
    m_chat_scroll = 1.0F;
}

void PlayerScreen::deepened(const domain::StepId& step,
                            application::ApplicationResult<std::vector<domain::Step>> result) {
    settle();
    if (!result) {
        m_notice = result.error().message();
        return;
    }
    const auto added = result->size();
    auto inserted = m_session.insert_after(step, std::move(*result));
    if (!inserted) {
        m_notice = inserted.error().message();
        return;
    }
    m_notice = std::string(glyphs::DONE) + " " +
               m_context->localizer->format("player.deepened", {m_context->localizer->count(added, "deeper_step")});
}

void PlayerScreen::quizzed(application::ApplicationResult<domain::Quiz> result) {
    settle();
    if (!result) {
        m_notice = result.error().message();
        return;
    }
    remember(m_session.attach_quiz(std::move(*result)));
    enter_quiz();
}

void PlayerScreen::remember(const application::ApplicationResult<void>& result) {
    if (!result) {
        m_notice = result.error().message();
    }
}

application::AgentChoice PlayerScreen::agent_choice() const {
    const auto& metadata = m_session.tour().metadata();
    return {.name = m_agent, .model = m_agent == metadata.agent ? metadata.model : std::string()};
}

application::ExplorationRequest PlayerScreen::exploration(std::string question) const {
    const auto& step = current_step();
    const auto asked = m_session.progress().exchanges(step.id());
    return {.tour = m_session.tour(),
            .step = step.id(),
            .agent = agent_choice(),
            .question = std::move(question),
            .history = {asked.begin(), asked.end()},
            .language = m_context->localizer->reply_language()};
}

const PlayerScreen::SourceLines& PlayerScreen::source(std::string_view path) const {
    auto found = m_sources.find(path);
    if (found == m_sources.end()) {
        found = m_sources.emplace(std::string(path), m_context->services.library->source(path)).first;
    }
    return found->second;
}

const domain::Step& PlayerScreen::current_step() const {
    return m_session.tour().step_at(m_position);
}

ftxui::Element PlayerScreen::render() const {
    const auto& theme = m_context->theme;
    const auto& tour = m_session.tour();
    ftxui::Elements columns;
    if (m_show_outline) {
        const auto current = m_on_quiz ? std::nullopt : std::optional(m_position);
        columns.push_back(m_outline.render(tour, m_session.progress(), current));
        columns.push_back(ftxui::separatorLight() | ftxui::color(theme.accent_soft));
    }
    columns.push_back(render_page() | ftxui::flex);
    auto screen = ftxui::vbox({
                      render_header(),
                      render_progress(),
                      ftxui::hbox(std::move(columns)) | ftxui::flex,
                      ftxui::separatorLight() | ftxui::color(theme.accent_soft),
                      render_footer(),
                  }) |
                  ftxui::bgcolor(theme.background);
    if (auto overlay = render_overlay()) {
        return ftxui::dbox({std::move(screen), std::move(*overlay) | ftxui::clear_under | ftxui::center});
    }
    return screen;
}

ftxui::Element PlayerScreen::render_page() const {
    const auto& tour = m_session.tour();
    if (m_on_quiz) {
        return ftxui::hbox({ftxui::text(" "), m_quiz.render(tour.quiz(), m_session.progress(), m_agent) | ftxui::flex,
                            ftxui::text(" ")}) |
               ftxui::yframe;
    }
    const auto lookup = [this](std::string_view path) -> const SourceLines& { return source(path); };
    auto step =
        ftxui::hbox({ftxui::text(" "),
                     m_step_view.render(tour, m_session.progress(), m_position, m_location, lookup) | ftxui::flex,
                     ftxui::text(" ")}) |
        ftxui::focusPositionRelative(0.0F, m_scroll) | ftxui::vscroll_indicator | ftxui::yframe | ftxui::flex;
    if (!m_show_chat) {
        return step;
    }
    auto chat = m_chat.render(m_session.progress().exchanges(current_step().id()), m_task, m_question_input->Render(),
                              m_asking, m_agent, m_chat_scroll) |
                ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, static_cast<int>(m_context->settings.chat_height));
    return ftxui::vbox({std::move(step), std::move(chat)});
}

ftxui::Element PlayerScreen::render_header() const {
    const auto& theme = m_context->theme;
    const auto& metadata = m_session.tour().metadata();
    ftxui::Elements right;
    if (m_spent.total() > 0) {
        right.push_back(token_counter(m_spent, theme, *m_context->localizer));
        right.push_back(ftxui::text("   "));
    }
    right.push_back(kind_badge(metadata.kind, theme, *m_context->localizer));
    const auto model = metadata.model.empty() ? std::string() : std::string(glyphs::SEPARATOR) + metadata.model;
    right.push_back(ftxui::text(" " + metadata.scope + std::string(glyphs::SEPARATOR) + m_agent + model) |
                    ftxui::color(theme.muted));
    return header_bar(shortened(metadata.title, HEADER_TITLE_WIDTH), ftxui::hbox(std::move(right)), theme);
}

ftxui::Element PlayerScreen::render_progress() const {
    const auto& theme = m_context->theme;
    const auto& tour = m_session.tour();
    if (m_on_quiz && tour.quiz()) {
        return progress_bar(m_session.progress().snapshot().quiz_answers.size(), tour.quiz()->questions().size(),
                            m_context->localizer->text("unit.questions"), theme);
    }
    return progress_bar(m_session.progress().visited_count_in(tour), tour.step_count(),
                        m_context->localizer->text("unit.steps"), theme);
}

ftxui::Element PlayerScreen::render_footer() const {
    const auto& theme = m_context->theme;
    if (m_notice) {
        return ftxui::text(" " + *m_notice) | ftxui::color(theme.warning) | ftxui::bold;
    }
    return m_on_quiz ? key_hints(QUIZ_HINTS, theme, *m_context->localizer)
                     : key_hints(STEP_HINTS, theme, *m_context->localizer);
}

std::optional<ftxui::Element> PlayerScreen::render_overlay() const {
    if (m_task && !m_task->in_chat) {
        return render_task(*m_task);
    }
    if (m_show_help) {
        return render_help();
    }
    return std::nullopt;
}

ftxui::Element PlayerScreen::render_help() const {
    const auto& theme = m_context->theme;
    ftxui::Elements rows;
    const auto& text = *m_context->localizer;
    const auto list = [&rows, &theme, &text](const std::vector<PlayerBinding>& table) {
        for (const auto& binding : table) {
            rows.push_back(ftxui::hbox({
                ftxui::text(std::string(binding.keys)) | ftxui::bold | ftxui::color(theme.accent) |
                    ftxui::size(ftxui::WIDTH, ftxui::EQUAL, KEY_COLUMN_WIDTH),
                ftxui::text(text.text(binding.description)) | ftxui::color(theme.text),
            }));
        }
    };
    list(m_bindings);
    list(m_quiz_bindings);
    rows.push_back(ftxui::text(""));
    rows.push_back(ftxui::text(m_context->localizer->text("player.close_help")) | ftxui::color(theme.muted) |
                   ftxui::hcenter);
    return panel(ftxui::text(m_context->localizer->text("player.keys")) | ftxui::bold | ftxui::color(theme.heading),
                 ftxui::vbox(std::move(rows)), theme) |
           ftxui::bgcolor(theme.background) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, OVERLAY_WIDTH);
}

ftxui::Element PlayerScreen::render_task(const RunningTask& task) const {
    const auto& theme = m_context->theme;
    const auto elapsed = std::chrono::steady_clock::now() - task.started;
    ftxui::Elements rows{ftxui::hbox({
        activity_spinner(elapsed, theme),
        ftxui::text(" " + task.label + std::string(glyphs::ELLIPSIS)) | ftxui::color(theme.text),
        ftxui::filler(),
        ftxui::text(clock_text(elapsed)) | ftxui::color(theme.muted),
    })};
    rows.push_back(token_counter(task.usage, theme, *m_context->localizer));
    rows.push_back(ftxui::text(""));
    for (const auto& line : task.activity) {
        rows.push_back(ftxui::text(shortened(line, ACTIVITY_WIDTH)) | ftxui::color(theme.muted));
    }
    rows.push_back(ftxui::text(""));
    rows.push_back(ftxui::text(m_context->localizer->text("player.esc_cancel")) | ftxui::color(theme.muted) |
                   ftxui::hcenter);
    return panel(ftxui::text(m_agent) | ftxui::bold | ftxui::color(theme.accent), ftxui::vbox(std::move(rows)), theme) |
           ftxui::bgcolor(theme.background) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, OVERLAY_WIDTH);
}

void PlayerScreen::leave(Destination destination) {
    m_next = std::move(destination);
    m_context->controls.exit();
}

}
