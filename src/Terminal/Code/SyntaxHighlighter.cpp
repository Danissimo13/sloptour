#include "Terminal/Code/SyntaxHighlighter.hpp"

#include "Core/Text/Text.hpp"

#include <algorithm>
#include <cctype>
#include <filesystem>

namespace sloptour::terminal {

namespace {

constexpr std::string_view QUOTES = "\"'`";
constexpr char ESCAPE = '\\';

[[nodiscard]] bool is_word(char character) noexcept {
    const auto value = static_cast<unsigned char>(character);
    return std::isalnum(value) != 0 || character == '_' || character == '#';
}

[[nodiscard]] bool is_digit(char character) noexcept {
    return std::isdigit(static_cast<unsigned char>(character)) != 0;
}

[[nodiscard]] std::string lowered(std::string_view text) {
    std::string result(text);
    std::ranges::transform(result, result.begin(), [](char character) {
        return static_cast<char>(std::tolower(static_cast<unsigned char>(character)));
    });
    return result;
}

[[nodiscard]] std::string lowered_extension(std::string_view path) {
    const std::filesystem::path file(path);
    auto extension = file.extension().string();
    if (extension.empty()) {
        extension = file.filename().string();
    } else {
        extension.erase(0, 1);
    }
    return lowered(extension);
}

[[nodiscard]] std::size_t string_end(std::string_view text, std::size_t start) {
    const auto quote = text.at(start);
    for (auto index = start + 1; index < text.size(); ++index) {
        if (text.at(index) == ESCAPE) {
            ++index;
        } else if (text.at(index) == quote) {
            return index + 1;
        }
    }
    return text.size();
}

[[nodiscard]] std::size_t word_end(std::string_view text, std::size_t start) {
    auto index = start;
    while (index < text.size() && is_word(text.at(index))) {
        ++index;
    }
    return index;
}

class LineColorizer {
public:
    LineColorizer(std::string_view text, std::string_view line_comment,
                  const std::set<std::string, std::less<>>& keywords, const Theme& theme)
        : m_text(text)
        , m_line_comment(line_comment)
        , m_keywords(std::cref(keywords))
        , m_theme(theme) {}

    [[nodiscard]] ftxui::Element colorize() {
        while (m_cursor < m_text.size()) {
            step();
        }
        flush_plain(m_text.size());
        return ftxui::hbox(std::move(m_parts));
    }

private:
    void step() {
        const auto character = m_text.at(m_cursor);
        const bool word_start = m_cursor == 0 || !is_word(m_text.at(m_cursor - 1));
        if (!m_line_comment.empty() && m_text.substr(m_cursor).starts_with(m_line_comment)) {
            styled(m_text.size(), m_theme.comment);
        } else if (QUOTES.contains(character)) {
            styled(string_end(m_text, m_cursor), m_theme.string);
        } else if (word_start && is_digit(character)) {
            styled(word_end(m_text, m_cursor), m_theme.number);
        } else if (word_start && is_word(character)) {
            word();
        } else {
            ++m_cursor;
        }
    }

    void word() {
        const auto end = word_end(m_text, m_cursor);
        if (m_keywords.get().contains(m_text.substr(m_cursor, end - m_cursor))) {
            styled(end, m_theme.keyword);
        } else {
            m_cursor = end;
        }
    }

    void styled(std::size_t end, ftxui::Color color) {
        flush_plain(m_cursor);
        m_parts.push_back(ftxui::text(std::string(m_text.substr(m_cursor, end - m_cursor))) | ftxui::color(color));
        m_cursor = end;
        m_plain_start = end;
    }

    void flush_plain(std::size_t end) {
        if (end > m_plain_start) {
            m_parts.push_back(ftxui::text(std::string(m_text.substr(m_plain_start, end - m_plain_start))) |
                              ftxui::color(m_theme.text));
        }
        m_plain_start = end;
    }

    std::string_view m_text;
    std::string_view m_line_comment;
    std::reference_wrapper<const std::set<std::string, std::less<>>> m_keywords;
    Theme m_theme;
    std::size_t m_cursor = 0;
    std::size_t m_plain_start = 0;
    ftxui::Elements m_parts;
};

}

SyntaxHighlighter::SyntaxHighlighter(std::vector<LanguageSyntax> languages, Theme theme)
    : m_theme(theme) {
    for (auto& language : languages) {
        CompiledSyntax compiled{.line_comment = std::move(language.line_comment), .keywords = {}};
        compiled.keywords.insert(language.keywords.begin(), language.keywords.end());
        m_languages.push_back({.extensions = std::move(language.extensions),
                               .aliases = std::move(language.aliases),
                               .syntax = std::move(compiled)});
    }
}

ftxui::Element SyntaxHighlighter::line(std::string_view path, std::string_view text) const {
    return colorized(syntax_named(lowered_extension(path), false), text);
}

ftxui::Element SyntaxHighlighter::snippet_line(std::string_view language, std::string_view text) const {
    return colorized(syntax_named(lowered(core::trim(language)), true), text);
}

ftxui::Element SyntaxHighlighter::colorized(std::optional<std::reference_wrapper<const CompiledSyntax>> syntax,
                                            std::string_view text) const {
    if (!syntax) {
        return plain(text);
    }
    LineColorizer colorizer(text, syntax->get().line_comment, syntax->get().keywords, m_theme);
    return colorizer.colorize();
}

std::optional<std::reference_wrapper<const SyntaxHighlighter::CompiledSyntax>>
SyntaxHighlighter::syntax_named(std::string_view name, bool alias) const {
    for (const auto& language : m_languages) {
        const auto& names = alias ? language.aliases : language.extensions;
        if (std::ranges::find(names, name) != names.end()) {
            return std::cref(language.syntax);
        }
    }
    return std::nullopt;
}

ftxui::Element SyntaxHighlighter::plain(std::string_view text) const {
    return ftxui::text(std::string(text)) | ftxui::color(m_theme.text);
}

}
