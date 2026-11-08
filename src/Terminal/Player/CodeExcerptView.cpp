#include "Terminal/Player/CodeExcerptView.hpp"

#include "Terminal/Widgets/Chrome.hpp"
#include "Terminal/Widgets/Glyphs.hpp"

#include <algorithm>
#include <format>

namespace sloptour::terminal {

namespace {

constexpr std::string_view GUTTER_SEPARATOR = " │ ";
constexpr std::string_view FOCUS_MARKER = "▌";
constexpr std::string_view NO_MARKER = " ";
constexpr char TAB = '\t';

[[nodiscard]] std::size_t digits_of(std::size_t number) {
    return std::to_string(number).size();
}

}

CodeExcerptView::CodeExcerptView(Theme theme, std::shared_ptr<const SyntaxHighlighter> highlighter,
                                 std::shared_ptr<const MarkdownView> markdown, const TerminalSettings& settings,
                                 std::shared_ptr<const Localizer> text)
    : m_theme(theme)
    , m_highlighter(std::move(highlighter))
    , m_markdown(std::move(markdown))
    , m_text(std::move(text))
    , m_context_lines(settings.code_context_lines)
    , m_max_lines(settings.code_max_lines)
    , m_tab_width(settings.tab_width) {}

ftxui::Element CodeExcerptView::render(const domain::CodeLocation& location,
                                       const application::ApplicationResult<std::vector<std::string>>& lines,
                                       std::size_t index, std::size_t count) const {
    ftxui::Elements body;
    if (!location.note().empty()) {
        body.push_back(m_markdown->inline_text(location.note()) | ftxui::italic);
    }
    if (lines) {
        body.push_back(code(location, *lines));
    } else {
        body.push_back(ftxui::text(lines.error().message()) | ftxui::color(m_theme.danger));
    }
    return panel(title(location, index, count), ftxui::vbox(std::move(body)), m_theme);
}

ftxui::Element CodeExcerptView::title(const domain::CodeLocation& location, std::size_t index,
                                      std::size_t count) const {
    auto label = location.path();
    if (location.lines()) {
        label += std::format(":{}-{}", location.lines()->first(), location.lines()->last());
    }
    ftxui::Elements parts{ftxui::text(label) | ftxui::bold | ftxui::color(m_theme.heading)};
    if (count > 1) {
        parts.push_back(ftxui::text(std::format("  {}/{}", index + 1, count)) | ftxui::color(m_theme.muted));
    }
    return ftxui::hbox(std::move(parts));
}

ftxui::Element CodeExcerptView::code(const domain::CodeLocation& location,
                                     const std::vector<std::string>& lines) const {
    if (lines.empty()) {
        return ftxui::text(m_text->text("code.empty")) | ftxui::color(m_theme.muted);
    }
    const auto focus_first = location.lines() ? location.lines()->first() : 1;
    const auto focus_last = location.lines() ? location.lines()->last() : std::min(lines.size(), m_max_lines);
    const auto first =
        std::clamp(focus_first > m_context_lines ? focus_first - m_context_lines : 1, std::size_t{1}, lines.size());
    const auto last = std::min({lines.size(), focus_last + m_context_lines, first + m_max_lines - 1});
    const auto gutter = digits_of(last);
    ftxui::Elements rows;
    for (auto number = first; number <= last; ++number) {
        rows.push_back(numbered(location, number, lines.at(number - 1), gutter));
    }
    if (last < focus_last) {
        rows.push_back(ftxui::text(std::format(" {:>{}}{}{} ", "", gutter, GUTTER_SEPARATOR, glyphs::ELLIPSIS) +
                                   m_text->count(focus_last - last, "more_line")) |
                       ftxui::color(m_theme.muted));
    }
    return ftxui::vbox(std::move(rows)) | ftxui::bgcolor(m_theme.code_background);
}

ftxui::Element CodeExcerptView::numbered(const domain::CodeLocation& location, std::size_t number,
                                         const std::string& line, std::size_t gutter) const {
    const bool focused = location.lines() && location.lines()->contains(number);
    const bool context = location.lines() && !focused;
    auto marker = ftxui::text(std::string(focused ? FOCUS_MARKER : NO_MARKER)) | ftxui::color(m_theme.accent);
    auto label = ftxui::text(std::format("{:>{}}", number, gutter));
    label = focused ? std::move(label) | ftxui::color(m_theme.accent) | ftxui::bold
                    : std::move(label) | ftxui::color(m_theme.muted);
    auto code = m_highlighter->line(location.path(), expanded(line));
    if (context) {
        code = std::move(code) | ftxui::dim;
    }
    auto row = ftxui::hbox({
        std::move(marker),
        std::move(label),
        ftxui::text(std::string(GUTTER_SEPARATOR)) | ftxui::color(m_theme.muted),
        std::move(code) | ftxui::flex,
    });
    return focused ? std::move(row) | ftxui::bgcolor(m_theme.code_focus) : row;
}

std::string CodeExcerptView::expanded(const std::string& line) const {
    std::string result;
    result.reserve(line.size());
    for (const char character : line) {
        if (character == TAB) {
            result.append(m_tab_width, ' ');
        } else {
            result += character;
        }
    }
    return result;
}

}
