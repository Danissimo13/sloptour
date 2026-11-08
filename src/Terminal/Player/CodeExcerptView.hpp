#pragma once

#include "Application/Ports/PortErrors.hpp"
#include "Domain/Tour/CodeLocation.hpp"
#include "Terminal/Code/SyntaxHighlighter.hpp"
#include "Terminal/Localization/Localizer.hpp"
#include "Terminal/Markdown/MarkdownView.hpp"
#include "Terminal/Settings/TerminalSettings.hpp"
#include "Terminal/Theme/Theme.hpp"

#include <ftxui/dom/elements.hpp>

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace sloptour::terminal {

class CodeExcerptView {
public:
    CodeExcerptView(Theme theme, std::shared_ptr<const SyntaxHighlighter> highlighter,
                    std::shared_ptr<const MarkdownView> markdown, const TerminalSettings& settings,
                    std::shared_ptr<const Localizer> text);

    [[nodiscard]] ftxui::Element render(const domain::CodeLocation& location,
                                        const application::ApplicationResult<std::vector<std::string>>& lines,
                                        std::size_t index, std::size_t count) const;

private:
    [[nodiscard]] ftxui::Element title(const domain::CodeLocation& location, std::size_t index,
                                       std::size_t count) const;
    [[nodiscard]] ftxui::Element code(const domain::CodeLocation& location,
                                      const std::vector<std::string>& lines) const;
    [[nodiscard]] ftxui::Element numbered(const domain::CodeLocation& location, std::size_t number,
                                          const std::string& line, std::size_t gutter) const;
    [[nodiscard]] std::string expanded(const std::string& line) const;

    Theme m_theme;
    std::shared_ptr<const SyntaxHighlighter> m_highlighter;
    std::shared_ptr<const MarkdownView> m_markdown;
    std::shared_ptr<const Localizer> m_text;
    std::size_t m_context_lines;
    std::size_t m_max_lines;
    std::size_t m_tab_width;
};

}
