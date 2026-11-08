#include "Core/Text/Text.hpp"

#include <algorithm>
#include <cctype>

namespace sloptour::core {

namespace {

constexpr std::string_view WHITESPACE = " \t\r\n";
constexpr char SLUG_SEPARATOR = '-';
constexpr std::size_t MAX_SLUG_LENGTH = 48;

[[nodiscard]] bool is_slug_character(char character) noexcept {
    return std::isalnum(static_cast<unsigned char>(character)) != 0;
}

[[nodiscard]] char lowered(char character) noexcept {
    return static_cast<char>(std::tolower(static_cast<unsigned char>(character)));
}

}

std::string_view trim(std::string_view text) noexcept {
    const auto first = text.find_first_not_of(WHITESPACE);
    if (first == std::string_view::npos) {
        return {};
    }
    const auto last = text.find_last_not_of(WHITESPACE);
    return text.substr(first, last - first + 1);
}

std::vector<std::string_view> split_lines(std::string_view text) {
    std::vector<std::string_view> lines;
    std::size_t start = 0;
    while (start <= text.size()) {
        const auto end = text.find('\n', start);
        if (end == std::string_view::npos) {
            lines.push_back(text.substr(start));
            break;
        }
        auto line = text.substr(start, end - start);
        if (line.ends_with('\r')) {
            line.remove_suffix(1);
        }
        lines.push_back(line);
        start = end + 1;
    }
    return lines;
}

std::string join(const std::vector<std::string>& parts, std::string_view separator) {
    std::string joined;
    for (const auto& part : parts) {
        if (!joined.empty()) {
            joined += separator;
        }
        joined += part;
    }
    return joined;
}

std::string slugify(std::string_view text) {
    std::string slug;
    for (const char character : text) {
        if (is_slug_character(character)) {
            slug += lowered(character);
        } else if (!slug.empty() && slug.back() != SLUG_SEPARATOR) {
            slug += SLUG_SEPARATOR;
        }
    }
    slug.resize(std::min(slug.size(), MAX_SLUG_LENGTH));
    while (!slug.empty() && slug.back() == SLUG_SEPARATOR) {
        slug.pop_back();
    }
    return slug;
}

bool is_blank(std::string_view text) noexcept {
    return trim(text).empty();
}

}
