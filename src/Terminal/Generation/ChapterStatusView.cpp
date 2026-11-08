#include "Terminal/Generation/ChapterStatusView.hpp"

#include "Terminal/Widgets/Chrome.hpp"
#include "Terminal/Widgets/Glyphs.hpp"

#include <format>

namespace sloptour::terminal {

namespace {

constexpr std::size_t REASON_WIDTH = 60;

struct StatusIcon {
    Theme theme;

    [[nodiscard]] ftxui::Element operator()(const ChapterQueued& /*status*/) const {
        return ftxui::text(std::string(glyphs::PENDING)) | ftxui::color(theme.muted);
    }

    [[nodiscard]] ftxui::Element operator()(const ChapterWriting& status) const {
        return activity_spinner(std::chrono::steady_clock::now() - status.started, theme);
    }

    [[nodiscard]] ftxui::Element operator()(const ChapterWritten& /*status*/) const {
        return ftxui::text(std::string(glyphs::DONE)) | ftxui::color(theme.success) | ftxui::bold;
    }

    [[nodiscard]] ftxui::Element operator()(const ChapterBroken& /*status*/) const {
        return ftxui::text(std::string(glyphs::FAILED)) | ftxui::color(theme.danger) | ftxui::bold;
    }
};

}

ftxui::Element ChapterStatusView::icon(const ChapterStatus& status) const {
    return std::visit(StatusIcon{.theme = m_theme}, status);
}

ftxui::Element ChapterStatusView::detail(const ChapterStatus& status) const {
    return std::visit(*this, status);
}

ftxui::Element ChapterStatusView::operator()(const ChapterQueued& /*status*/) const {
    return ftxui::text(m_localizer->text("generation.queued")) | ftxui::color(m_theme.muted);
}

ftxui::Element ChapterStatusView::operator()(const ChapterWriting& status) const {
    return ftxui::text(m_localizer->format("generation.writing",
                                           {clock_text(std::chrono::steady_clock::now() - status.started)})) |
           ftxui::color(m_theme.accent);
}

ftxui::Element ChapterStatusView::operator()(const ChapterWritten& status) const {
    return ftxui::text(m_localizer->format("generation.written",
                                           {m_localizer->count(status.steps, status.unit), clock_text(status.took)})) |
           ftxui::color(m_theme.success);
}

ftxui::Element ChapterStatusView::operator()(const ChapterBroken& status) const {
    return ftxui::text(m_localizer->format("generation.failed", {shortened(status.reason, REASON_WIDTH)})) |
           ftxui::color(m_theme.danger);
}

}
