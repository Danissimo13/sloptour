#include "Terminal/Markdown/InlineParser.hpp"

namespace sloptour::terminal {

namespace {

constexpr std::string_view STRONG = "**";
constexpr char CODE = '`';
constexpr char EMPHASIS = '*';
constexpr char ALTERNATE_EMPHASIS = '_';

class InlineScanner {
public:
    explicit InlineScanner(std::string_view text)
        : m_text(text) {}

    [[nodiscard]] std::vector<InlineSpan> scan() {
        while (m_cursor < m_text.size()) {
            if (!try_code() && !try_strong() && !try_emphasis()) {
                m_current.text += m_text.at(m_cursor++);
            }
        }
        flush();
        return std::move(m_spans);
    }

private:
    [[nodiscard]] bool try_code() {
        if (m_text.at(m_cursor) != CODE) {
            return false;
        }
        const auto close = m_text.find(CODE, m_cursor + 1);
        if (close == std::string_view::npos) {
            return false;
        }
        flush();
        m_spans.push_back({.text = std::string(m_text.substr(m_cursor + 1, close - m_cursor - 1)),
                           .strong = false,
                           .emphasis = false,
                           .code = true});
        m_cursor = close + 1;
        return true;
    }

    [[nodiscard]] bool try_strong() {
        if (!m_text.substr(m_cursor).starts_with(STRONG)) {
            return false;
        }
        flush();
        m_current.strong = !m_current.strong;
        m_cursor += STRONG.size();
        return true;
    }

    [[nodiscard]] bool try_emphasis() {
        const auto character = m_text.at(m_cursor);
        if (character != EMPHASIS && character != ALTERNATE_EMPHASIS) {
            return false;
        }
        const bool opens = !m_current.emphasis && m_cursor + 1 < m_text.size() && m_text.at(m_cursor + 1) != ' ' &&
                           (m_cursor == 0 || m_text.at(m_cursor - 1) == ' ');
        const bool closes = m_current.emphasis;
        if (!opens && !closes) {
            return false;
        }
        flush();
        m_current.emphasis = !m_current.emphasis;
        ++m_cursor;
        return true;
    }

    void flush() {
        if (!m_current.text.empty()) {
            m_spans.push_back(m_current);
            m_current.text.clear();
        }
    }

    std::string_view m_text;
    std::size_t m_cursor = 0;
    InlineSpan m_current;
    std::vector<InlineSpan> m_spans;
};

}

std::vector<InlineSpan> inline_spans(std::string_view text) {
    return InlineScanner(text).scan();
}

}
