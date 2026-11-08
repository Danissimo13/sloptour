#include "Terminal/Theme/ThemeFactory.hpp"

#include <array>
#include <cctype>
#include <cstdint>
#include <optional>
#include <string_view>
#include <utility>

namespace sloptour::terminal {

namespace {

constexpr char HEX_PREFIX = '#';
constexpr std::size_t HEX_DIGITS = 6;
constexpr unsigned HEX_BASE = 16U;
constexpr std::string_view HEX_DIGIT_VALUES = "0123456789abcdef";
constexpr unsigned RED_SHIFT = 16U;
constexpr unsigned GREEN_SHIFT = 8U;
constexpr unsigned CHANNEL_MASK = 0xFFU;
constexpr std::size_t NAMED_COLORS = 17;

[[nodiscard]] std::optional<ftxui::Color> parsed_hex(std::string_view hex) {
    if (hex.size() != HEX_DIGITS + 1 || hex.front() != HEX_PREFIX) {
        return std::nullopt;
    }
    unsigned value = 0;
    for (const char character : hex.substr(1)) {
        const auto digit =
            HEX_DIGIT_VALUES.find(static_cast<char>(std::tolower(static_cast<unsigned char>(character))));
        if (digit == std::string_view::npos) {
            return std::nullopt;
        }
        value = (value * HEX_BASE) + static_cast<unsigned>(digit);
    }
    return ftxui::Color::RGB(static_cast<std::uint8_t>((value >> RED_SHIFT) & CHANNEL_MASK),
                             static_cast<std::uint8_t>((value >> GREEN_SHIFT) & CHANNEL_MASK),
                             static_cast<std::uint8_t>(value & CHANNEL_MASK));
}

[[nodiscard]] std::optional<ftxui::Color> named(std::string_view name) {
    const std::array<std::pair<std::string_view, ftxui::Color>, NAMED_COLORS> colors{{
        {"default", ftxui::Color::Default},
        {"black", ftxui::Color::Black},
        {"red", ftxui::Color::Red},
        {"green", ftxui::Color::Green},
        {"yellow", ftxui::Color::Yellow},
        {"blue", ftxui::Color::Blue},
        {"magenta", ftxui::Color::Magenta},
        {"cyan", ftxui::Color::Cyan},
        {"white", ftxui::Color::GrayLight},
        {"bright-black", ftxui::Color::GrayDark},
        {"bright-red", ftxui::Color::RedLight},
        {"bright-green", ftxui::Color::GreenLight},
        {"bright-yellow", ftxui::Color::YellowLight},
        {"bright-blue", ftxui::Color::BlueLight},
        {"bright-magenta", ftxui::Color::MagentaLight},
        {"bright-cyan", ftxui::Color::CyanLight},
        {"bright-white", ftxui::Color::White},
    }};
    for (const auto& [candidate, color] : colors) {
        if (candidate == name) {
            return color;
        }
    }
    return std::nullopt;
}

[[nodiscard]] ftxui::Color color_of(const ThemePalette& palette, std::string_view key) {
    const auto found = palette.find(key);
    if (found == palette.end()) {
        return ftxui::Color::Default;
    }
    return named(found->second).or_else([&found] { return parsed_hex(found->second); }).value_or(ftxui::Color::Default);
}

}

Theme theme_from(const ThemePalette& palette) {
    return {.background = color_of(palette, "background"),
            .badge_text = color_of(palette, "badge_text"),
            .accent = color_of(palette, "accent"),
            .accent_soft = color_of(palette, "accent_soft"),
            .text = color_of(palette, "text"),
            .muted = color_of(palette, "muted"),
            .success = color_of(palette, "success"),
            .warning = color_of(palette, "warning"),
            .danger = color_of(palette, "danger"),
            .code_background = color_of(palette, "code_background"),
            .code_focus = color_of(palette, "code_focus"),
            .keyword = color_of(palette, "keyword"),
            .string = color_of(palette, "string"),
            .number = color_of(palette, "number"),
            .comment = color_of(palette, "comment"),
            .heading = color_of(palette, "heading"),
            .inline_code = color_of(palette, "inline_code")};
}

}
