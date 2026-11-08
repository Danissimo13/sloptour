#include "Terminal/Generation/GenerationScreen.hpp"

#include "Terminal/Generation/ChapterStatusView.hpp"
#include "Terminal/Widgets/Chrome.hpp"
#include "Terminal/Widgets/Glyphs.hpp"

#include <ftxui/component/component.hpp>

#include <array>
#include <format>

namespace sloptour::terminal {

namespace {

constexpr auto TICK = std::chrono::milliseconds(100);
constexpr int CHAPTER_TITLE_WIDTH = 44;
constexpr std::size_t ACTIVITY_WIDTH = 110;
constexpr int STATUS_WIDTH = 26;
constexpr int TOKENS_WIDTH = 12;

constexpr std::array RUNNING_HINTS{KeyHint{.key = "esc", .label = "hint.cancel"}};
constexpr std::array DONE_HINTS{KeyHint{.key = "enter", .label = "hint.start_tour"},
                                KeyHint{.key = "esc", .label = "hint.home"}};
constexpr std::array FAILED_HINTS{KeyHint{.key = "r", .label = "hint.retry"},
                                  KeyHint{.key = "esc", .label = "hint.home"}};

}

GenerationScreen::GenerationScreen(std::shared_ptr<const TerminalContext> context,
                                   application::GenerationRequest request)
    : m_context(std::move(context))
    , m_request(std::move(request))
    , m_progress(m_context->settings.activity_lines)
    , m_poster(m_context->controls) {
    m_observer = std::make_shared<PostingObserver>([poster = m_poster, this](application::GenerationEvent event) {
        poster.post([this, event = std::move(event)] { m_progress.apply(event); });
    });
}

GenerationScreen::~GenerationScreen() {
    m_worker.request_stop();
}

Destination GenerationScreen::run() {
    start();
    auto component = ftxui::Renderer([this] { return render(); });
    component |= ftxui::CatchEvent([this](const ftxui::Event& event) { return handle(event); });
    m_context->controls.loop(component);
    m_worker.request_stop();
    return m_next;
}

void GenerationScreen::start() {
    m_progress = GenerationProgress(m_context->settings.activity_lines);
    m_result.reset();
    m_took.reset();
    m_started = std::chrono::steady_clock::now();
    m_ticker.emplace(TICK, [poster = m_poster] { poster.redraw(); });
    m_worker = std::jthread([this, context = m_context, request = m_request, observer = m_observer,
                             poster = m_poster](const std::stop_token& stop) {
        auto result = context->services.generator->execute(request, *observer, stop);
        poster.post([this, result = std::move(result)] { finish(result); });
    });
}

void GenerationScreen::finish(application::ApplicationResult<domain::TourId> result) {
    m_took = std::chrono::steady_clock::now() - m_started;
    m_result = std::move(result);
    m_ticker.reset();
}

bool GenerationScreen::handle(const ftxui::Event& event) {
    const bool done = m_result.has_value();
    const bool succeeded = done && m_result->has_value();
    if (event == ftxui::Event::Escape || event == ftxui::Event::Character('q')) {
        leave(GoHome{});
    } else if (succeeded && event == ftxui::Event::Return) {
        leave(GoPlay{.tour = **m_result});
    } else if (done && !succeeded && event == ftxui::Event::Character('r')) {
        start();
    } else {
        return false;
    }
    return true;
}

ftxui::Element GenerationScreen::render() const {
    const auto& theme = m_context->theme;
    const auto elapsed = m_took.value_or(std::chrono::steady_clock::now() - m_started);
    auto hints = std::span<const KeyHint>(RUNNING_HINTS);
    if (m_result) {
        hints = m_result->has_value() ? std::span<const KeyHint>(DONE_HINTS) : std::span<const KeyHint>(FAILED_HINTS);
    }
    auto body = ftxui::vbox({
        render_phases(),
        ftxui::text(""),
        render_chapters(),
        ftxui::text(""),
        render_outcome(),
        ftxui::filler(),
        render_activity(),
    });
    return ftxui::vbox({
               header_bar(
                   t("generation.title"),
                   ftxui::hbox({token_counter(m_progress.usage(), theme, *m_context->localizer), ftxui::text("   "),
                                ftxui::text(clock_text(elapsed)) | ftxui::color(theme.muted)}),
                   theme),
               progress_bar(m_progress.finished_chapters(), m_progress.chapters().size(), t("unit.chapters"), theme),
               std::move(body) | ftxui::borderEmpty | ftxui::flex,
               ftxui::separatorLight() | ftxui::color(theme.accent_soft),
               key_hints(hints, theme, *m_context->localizer),
           }) |
           ftxui::bgcolor(theme.background);
}

ftxui::Element GenerationScreen::render_phases() const {
    const auto& theme = m_context->theme;
    const auto& text = *m_context->localizer;
    const auto outline_label = m_progress.outline() ? text.format("generation.outline", {*m_progress.outline()})
                                                    : text.format("generation.drafting", {m_progress.agent()});
    return ftxui::vbox({
        ftxui::hbox({ftxui::text(t("generation.repository") + " ") | ftxui::color(theme.muted),
                     ftxui::text(m_progress.repository()) | ftxui::bold | ftxui::color(theme.text),
                     ftxui::text("   " + t("generation.scope") + " ") | ftxui::color(theme.muted),
                     ftxui::text(m_progress.scope()) | ftxui::color(theme.text),
                     ftxui::text("   " + t("generation.agent") + " ") | ftxui::color(theme.muted),
                     ftxui::text(agent_label()) | ftxui::color(theme.text),
                     ftxui::text("   " + t("generation.depth") + " ") | ftxui::color(theme.muted),
                     ftxui::text(t("level." + (m_request.depth.empty() ? std::string("default") : m_request.depth))) |
                         ftxui::color(theme.text),
                     ftxui::text("   " + t("generation.detail") + " ") | ftxui::color(theme.muted),
                     ftxui::text(t("level." + (m_request.detail.empty() ? std::string("default") : m_request.detail))) |
                         ftxui::color(theme.text)}),
        ftxui::text(""),
        phase(m_progress.inspected(), !m_progress.inspected(), t("generation.inspecting")),
        phase(m_progress.outline().has_value(), m_progress.outlining(), outline_label),
        phase(m_progress.quiz().has_value() || (m_result && m_result->has_value()),
              !m_progress.chapters().empty() && !m_progress.quiz() && !m_result, t("generation.chapters_phase")),
        phase(m_result && m_result->has_value(), m_progress.quiz().has_value() && !m_result,
              t("generation.quiz_phase")),
    });
}

ftxui::Element GenerationScreen::phase(bool done, bool active, std::string_view label) const {
    const auto& theme = m_context->theme;
    ftxui::Element icon = ftxui::text(std::string(glyphs::PENDING)) | ftxui::color(theme.muted);
    if (done) {
        icon = ftxui::text(std::string(glyphs::DONE)) | ftxui::color(theme.success) | ftxui::bold;
    } else if (active && !m_result) {
        icon = activity_spinner(std::chrono::steady_clock::now() - m_started, theme);
    }
    return ftxui::hbox(
        {ftxui::text("  "), icon,
         ftxui::text(" " + std::string(label)) | ftxui::color(done || active ? theme.text : theme.muted)});
}

ftxui::Element GenerationScreen::render_chapters() const {
    const auto& theme = m_context->theme;
    if (m_progress.chapters().empty()) {
        return ftxui::text("");
    }
    ftxui::Elements rows{section_title(t("generation.chapters"), theme)};
    std::size_t number = 1;
    for (const auto& chapter : m_progress.chapters()) {
        rows.push_back(chapter_row(chapter, std::format("{:>2}", number++)));
    }
    if (const auto& quiz = m_progress.quiz()) {
        auto row = *quiz;
        row.title = t("generation.quiz_title");
        rows.push_back(chapter_row(row, std::string(glyphs::STAR) + " "));
    }
    return ftxui::vbox(std::move(rows));
}

ftxui::Element GenerationScreen::chapter_row(const ChapterRow& chapter, const std::string& label) const {
    const auto& theme = m_context->theme;
    const ChapterStatusView view(theme, m_context->localizer);
    const std::string approximate = chapter.usage.estimated ? "~" : "";
    const auto tokens = chapter.usage.total() > 0
                            ? approximate + token_text(chapter.usage.total()) + " " + t("tokens.short")
                            : std::string();
    return ftxui::hbox({
        ftxui::text("  "),
        view.icon(chapter.status),
        ftxui::text(" " + label + "  ") | ftxui::color(theme.muted),
        ftxui::text(chapter.title) | ftxui::color(theme.text) |
            ftxui::size(ftxui::WIDTH, ftxui::EQUAL, CHAPTER_TITLE_WIDTH),
        view.detail(chapter.status) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, STATUS_WIDTH),
        ftxui::text(tokens) | ftxui::color(theme.muted) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, TOKENS_WIDTH),
        ftxui::text(shortened(chapter.activity, ACTIVITY_WIDTH / 2)) | ftxui::color(theme.muted) | ftxui::dim,
    });
}

ftxui::Element GenerationScreen::render_activity() const {
    const auto& theme = m_context->theme;
    ftxui::Elements lines;
    for (const auto& line : m_progress.activity()) {
        lines.push_back(ftxui::text(shortened(line, ACTIVITY_WIDTH)) | ftxui::color(theme.muted));
    }
    if (lines.empty()) {
        lines.push_back(ftxui::text(t("generation.waiting") + std::string(glyphs::ELLIPSIS)) |
                        ftxui::color(theme.muted));
    }
    return panel(ftxui::text(t("generation.activity")) | ftxui::color(theme.accent), ftxui::vbox(std::move(lines)),
                 theme);
}

ftxui::Element GenerationScreen::render_outcome() const {
    const auto& theme = m_context->theme;
    if (!m_result) {
        return ftxui::text("");
    }
    if (!m_result->has_value()) {
        return ftxui::paragraph(std::string(glyphs::FAILED) + " " + m_result->error().message()) |
               ftxui::color(theme.danger);
    }
    return ftxui::hbox({ftxui::text(std::string(glyphs::DONE) + " " + t("generation.ready") + " ") | ftxui::bold |
                            ftxui::color(theme.success),
                        ftxui::text(t("generation.press_enter")) | ftxui::color(theme.text)});
}

std::string GenerationScreen::t(std::string_view key) const {
    return m_context->localizer->text(key);
}

std::string GenerationScreen::agent_label() const {
    const auto& agent = m_request.agent;
    return agent.model.empty() ? agent.name : agent.name + std::string(glyphs::SEPARATOR) + agent.model;
}

void GenerationScreen::leave(Destination destination) {
    m_next = std::move(destination);
    m_context->controls.exit();
}

}
