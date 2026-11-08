#include "Terminal/Markdown/MarkdownView.hpp"

#include "Core/Text/Text.hpp"
#include "Terminal/Markdown/InlineParser.hpp"

#include <cctype>
#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace sloptour::terminal {

namespace {

constexpr std::string_view FENCE = "```";
constexpr std::string_view CODE_GUTTER = "  ▎ ";
constexpr std::string_view BULLET_GLYPH = "  • ";
constexpr std::string_view QUOTE_GLYPH = " ▌ ";
constexpr char HEADING_MARK = '#';
constexpr char QUOTE_MARK = '>';
constexpr std::string_view BULLET_MARKS = "-*+";

[[nodiscard]] bool is_bullet(std::string_view line) {
    return line.size() > 2 && BULLET_MARKS.contains(line.front()) && line.at(1) == ' ';
}

[[nodiscard]] std::size_t ordered_marker_length(std::string_view line) {
    std::size_t digits = 0;
    while (digits < line.size() && std::isdigit(static_cast<unsigned char>(line.at(digits))) != 0) {
        ++digits;
    }
    const bool ordered = digits > 0 && digits + 1 < line.size() && line.at(digits) == '.' && line.at(digits + 1) == ' ';
    return ordered ? digits + 2 : 0;
}

class BlockBuilder {
public:
    explicit BlockBuilder(const MarkdownView& view)
        : m_view(std::cref(view)) {}

    void consume(std::string_view raw) {
        const auto line = core::trim(raw);
        if (line.starts_with(FENCE)) {
            toggle_fence(line.substr(FENCE.size()));
        } else if (m_fence_language) {
            m_fenced.push_back(m_view.get().code_line(*m_fence_language, raw));
        } else if (line.empty()) {
            gap();
        } else {
            structural(line);
        }
    }

    [[nodiscard]] ftxui::Element build() {
        if (m_fence_language) {
            toggle_fence({});
        }
        flush();
        return ftxui::vbox(std::move(m_blocks));
    }

private:
    void toggle_fence(std::string_view language) {
        if (!m_fence_language) {
            flush();
            m_fence_language = std::string(language);
            return;
        }
        block(m_view.get().code_block(m_fenced));
        m_fenced.clear();
        m_fence_language.reset();
    }

    void structural(std::string_view line) {
        const auto& view = m_view.get();
        if (line.front() == HEADING_MARK) {
            block(view.heading(line));
        } else if (is_bullet(line)) {
            block(view.bullet(BULLET_GLYPH, line.substr(2)));
        } else if (const auto marker = ordered_marker_length(line); marker > 0) {
            block(view.bullet("  " + std::string(line.substr(0, marker)), line.substr(marker)));
        } else if (line.front() == QUOTE_MARK) {
            block(view.quote(core::trim(line.substr(1))));
        } else {
            paragraph_line(line);
        }
    }

    void paragraph_line(std::string_view line) {
        if (!m_paragraph.empty()) {
            m_paragraph += ' ';
        }
        m_paragraph += line;
    }

    void block(ftxui::Element element) {
        flush();
        m_blocks.push_back(std::move(element));
    }

    void gap() {
        flush();
        if (!m_blocks.empty()) {
            m_blocks.push_back(ftxui::text(""));
        }
    }

    void flush() {
        if (!m_paragraph.empty()) {
            m_blocks.push_back(m_view.get().inline_text(m_paragraph));
            m_paragraph.clear();
        }
    }

    std::reference_wrapper<const MarkdownView> m_view;
    std::string m_paragraph;
    std::optional<std::string> m_fence_language;
    ftxui::Elements m_fenced;
    ftxui::Elements m_blocks;
};

}

ftxui::Element MarkdownView::render(std::string_view markdown) const {
    BlockBuilder builder(*this);
    for (const auto raw : core::split_lines(markdown)) {
        builder.consume(raw);
    }
    return builder.build();
}

ftxui::Element MarkdownView::code_line(std::string_view language, std::string_view line) const {
    return m_highlighter->snippet_line(language, line);
}

ftxui::Element MarkdownView::inline_text(std::string_view text) const {
    ftxui::Elements words;
    for (const auto& span : inline_spans(text)) {
        auto decorate = ftxui::color(span.code ? m_theme.inline_code : m_theme.text);
        if (span.strong) {
            decorate = decorate | ftxui::bold;
        }
        if (span.emphasis) {
            decorate = decorate | ftxui::italic;
        }
        if (span.code) {
            words.push_back(ftxui::text(span.text) | decorate);
            continue;
        }
        std::size_t start = 0;
        while (start < span.text.size()) {
            auto end = span.text.find(' ', start);
            end = end == std::string::npos ? span.text.size() : end + 1;
            words.push_back(ftxui::text(span.text.substr(start, end - start)) | decorate);
            start = end;
        }
    }
    return ftxui::hflow(std::move(words));
}

ftxui::Element MarkdownView::heading(std::string_view line) const {
    const auto level = line.find_first_not_of(HEADING_MARK);
    const auto title = core::trim(line.substr(level == std::string_view::npos ? line.size() : level));
    auto element = ftxui::text(std::string(title)) | ftxui::bold | ftxui::color(m_theme.heading);
    return level == 1 ? element | ftxui::underlined : element;
}

ftxui::Element MarkdownView::bullet(std::string_view marker, std::string_view text) const {
    return ftxui::hbox(
        {ftxui::text(std::string(marker)) | ftxui::color(m_theme.accent), inline_text(text) | ftxui::flex});
}

ftxui::Element MarkdownView::quote(std::string_view text) const {
    return ftxui::hbox({ftxui::text(std::string(QUOTE_GLYPH)) | ftxui::color(m_theme.muted),
                        inline_text(text) | ftxui::dim | ftxui::flex});
}

ftxui::Element MarkdownView::code_block(const ftxui::Elements& lines) const {
    ftxui::Elements rows;
    rows.reserve(lines.size());
    for (const auto& line : lines) {
        rows.push_back(ftxui::hbox({ftxui::text(std::string(CODE_GUTTER)) | ftxui::color(m_theme.muted), line}));
    }
    return ftxui::vbox(std::move(rows)) | ftxui::bgcolor(m_theme.code_background);
}

}
