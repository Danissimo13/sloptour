#pragma once

#include "Terminal/Code/LanguageSyntax.hpp"
#include "Terminal/Theme/Theme.hpp"

#include <ftxui/dom/elements.hpp>

#include <functional>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <vector>

namespace sloptour::terminal {

class SyntaxHighlighter {
public:
    SyntaxHighlighter(std::vector<LanguageSyntax> languages, Theme theme);

    [[nodiscard]] ftxui::Element line(std::string_view path, std::string_view text) const;
    [[nodiscard]] ftxui::Element snippet_line(std::string_view language, std::string_view text) const;

private:
    struct CompiledSyntax {
        std::string line_comment;
        std::set<std::string, std::less<>> keywords;
    };

    [[nodiscard]] std::optional<std::reference_wrapper<const CompiledSyntax>> syntax_named(std::string_view name,
                                                                                           bool alias) const;
    [[nodiscard]] ftxui::Element colorized(std::optional<std::reference_wrapper<const CompiledSyntax>> syntax,
                                           std::string_view text) const;
    [[nodiscard]] ftxui::Element plain(std::string_view text) const;

    struct KnownLanguage {
        std::vector<std::string> extensions;
        std::vector<std::string> aliases;
        CompiledSyntax syntax;
    };

    std::vector<KnownLanguage> m_languages;
    Theme m_theme;
};

}
