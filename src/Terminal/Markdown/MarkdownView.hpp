#pragma once

#include "Terminal/Code/SyntaxHighlighter.hpp"
#include "Terminal/Theme/Theme.hpp"

#include <ftxui/dom/elements.hpp>

#include <memory>
#include <string_view>
#include <utility>

namespace sloptour::terminal {

class MarkdownView {
public:
    MarkdownView(Theme theme, std::shared_ptr<const SyntaxHighlighter> highlighter)
        : m_theme(theme)
        , m_highlighter(std::move(highlighter)) {}

    [[nodiscard]] ftxui::Element render(std::string_view markdown) const;
    [[nodiscard]] ftxui::Element inline_text(std::string_view text) const;
    [[nodiscard]] ftxui::Element code_line(std::string_view language, std::string_view line) const;
    [[nodiscard]] ftxui::Element heading(std::string_view line) const;
    [[nodiscard]] ftxui::Element bullet(std::string_view marker, std::string_view text) const;
    [[nodiscard]] ftxui::Element quote(std::string_view text) const;
    [[nodiscard]] ftxui::Element code_block(const ftxui::Elements& lines) const;

private:
    Theme m_theme;
    std::shared_ptr<const SyntaxHighlighter> m_highlighter;
};

}
