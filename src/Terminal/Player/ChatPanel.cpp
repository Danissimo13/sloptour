#include "Terminal/Player/ChatPanel.hpp"

#include "Terminal/Widgets/Chrome.hpp"
#include "Terminal/Widgets/Glyphs.hpp"

#include <format>

namespace sloptour::terminal {

namespace {

constexpr std::size_t ACTIVITY_WIDTH = 90;

}

ftxui::Element ChatPanel::render(std::span<const domain::Exchange> history, const std::optional<RunningTask>& pending,
                                 ftxui::Element input, bool typing, std::string_view agent, float scroll) const {
    const bool busy = pending.has_value() && pending->in_chat;
    ftxui::Elements messages;
    for (const auto& exchange : history) {
        messages.push_back(question(exchange.question));
        messages.push_back(ftxui::hbox({ftxui::text("  "), m_markdown->render(exchange.answer) | ftxui::flex}));
        messages.push_back(ftxui::text(""));
    }
    if (busy) {
        messages.push_back(question(pending->question));
        messages.push_back(waiting(*pending));
    }
    if (messages.empty()) {
        messages.push_back(ftxui::paragraph(m_text->text("chat.empty")) | ftxui::color(m_theme.muted));
    }
    ftxui::Elements title{ftxui::text(m_text->format("chat.title", {agent})) | ftxui::bold |
                          ftxui::color(m_theme.accent)};
    if (busy && pending->usage.total() > 0) {
        title.push_back(ftxui::text("  "));
        title.push_back(token_counter(pending->usage, m_theme, *m_text));
    }
    return panel(ftxui::hbox(std::move(title)),
                 ftxui::vbox({
                     ftxui::vbox(std::move(messages)) | ftxui::focusPositionRelative(0.0F, scroll) |
                         ftxui::vscroll_indicator | ftxui::yframe | ftxui::flex,
                     input_row(std::move(input), typing, busy),
                 }),
                 m_theme);
}

ftxui::Element ChatPanel::question(std::string_view text) const {
    return ftxui::hbox({ftxui::text(std::string(glyphs::PROMPT) + " ") | ftxui::color(m_theme.accent),
                        ftxui::paragraph(std::string(text)) | ftxui::bold | ftxui::color(m_theme.text) | ftxui::flex});
}

ftxui::Element ChatPanel::waiting(const RunningTask& task) const {
    const auto elapsed = std::chrono::steady_clock::now() - task.started;
    const auto latest = task.activity.empty() ? m_text->text("chat.thinking") : task.activity.back();
    return ftxui::hbox({
        ftxui::text("  "),
        activity_spinner(elapsed, m_theme),
        ftxui::text(" " + shortened(latest, ACTIVITY_WIDTH)) | ftxui::color(m_theme.muted),
        ftxui::filler(),
        ftxui::text(clock_text(elapsed) + " · " + m_text->text("chat.esc_cancels")) | ftxui::color(m_theme.muted),
    });
}

ftxui::Element ChatPanel::input_row(ftxui::Element input, bool typing, bool busy) const {
    if (typing) {
        return ftxui::hbox({ftxui::text(std::string(glyphs::PROMPT) + " ") | ftxui::color(m_theme.accent),
                            std::move(input) | ftxui::flex,
                            ftxui::text(" " + m_text->text("chat.typing_hint")) | ftxui::color(m_theme.muted)}) |
               ftxui::color(m_theme.text);
    }
    const auto hint =
        busy ? m_text->text("chat.waiting") + std::string(glyphs::ELLIPSIS) : m_text->text("chat.idle_hint");
    return ftxui::text(hint) | ftxui::color(m_theme.muted);
}

}
