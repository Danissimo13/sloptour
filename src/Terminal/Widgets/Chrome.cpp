#include "Terminal/Widgets/Chrome.hpp"

#include "Terminal/Widgets/Glyphs.hpp"
#include "Terminal/Widgets/TourKindStyle.hpp"

#include <format>

namespace sloptour::terminal {

namespace {

constexpr int SPINNER_CHARSET = 15;
constexpr auto SPINNER_FRAME = std::chrono::milliseconds(80);
constexpr int PERCENT = 100;
constexpr int SECONDS_PER_MINUTE = 60;
constexpr std::string_view APP_NAME = " sloptour ";
constexpr std::string_view TOP_LEFT = "╭─ ";
constexpr std::string_view TOP_RIGHT = "─╮";
constexpr std::string_view BOTTOM_LEFT = "╰";
constexpr std::string_view BOTTOM_RIGHT = "╯";
constexpr std::int64_t THOUSAND = 1000;
constexpr unsigned UTF8_CONTINUATION_MASK = 0xC0U;
constexpr unsigned UTF8_CONTINUATION = 0x80U;
constexpr std::int64_t MILLION = 1000000;

}

ftxui::Element header_bar(std::string_view title, ftxui::Element right, const Theme& theme) {
    return ftxui::vbox({
        ftxui::hbox({
            ftxui::text(" " + std::string(glyphs::LOGO)) | ftxui::color(theme.accent) | ftxui::bold,
            ftxui::text(std::string(APP_NAME)) | ftxui::bold | ftxui::color(theme.text),
            ftxui::text(std::string(glyphs::SEPARATOR)) | ftxui::color(theme.muted),
            ftxui::text(std::string(title)) | ftxui::color(theme.heading) | ftxui::bold,
            ftxui::filler(),
            std::move(right),
            ftxui::text(" "),
        }),
        ftxui::separatorLight() | ftxui::color(theme.accent_soft),
    });
}

ftxui::Element panel(ftxui::Element title, ftxui::Element content, const Theme& theme) {
    const auto border = ftxui::color(theme.accent_soft);
    const auto line = [&border] { return ftxui::vbox({ftxui::separatorLight()}) | border | ftxui::flex; };
    return ftxui::vbox({
        ftxui::hbox({ftxui::text(std::string(TOP_LEFT)) | border, std::move(title), ftxui::text(" "), line(),
                     ftxui::text(std::string(TOP_RIGHT)) | border}),
        ftxui::hbox({ftxui::separatorLight() | border, ftxui::text(" "), std::move(content) | ftxui::flex,
                     ftxui::text(" "), ftxui::separatorLight() | border}) |
            ftxui::flex,
        ftxui::hbox(
            {ftxui::text(std::string(BOTTOM_LEFT)) | border, line(), ftxui::text(std::string(BOTTOM_RIGHT)) | border}),
    });
}

ftxui::Element selection_marker(bool selected, const Theme& theme) {
    if (!selected) {
        return ftxui::text("   ");
    }
    return ftxui::text(" " + std::string(glyphs::POINTER) + " ") | ftxui::color(theme.accent) | ftxui::bold;
}

ftxui::Element key_hints(std::span<const KeyHint> hints, const Theme& theme, const Localizer& localizer) {
    ftxui::Elements parts{ftxui::text(" ")};
    for (const auto& hint : hints) {
        parts.push_back(ftxui::text(" " + std::string(hint.key) + " ") | ftxui::bgcolor(theme.accent_soft) |
                        ftxui::color(theme.text) | ftxui::bold);
        parts.push_back(ftxui::text(" " + localizer.text(hint.label) + "  ") | ftxui::color(theme.muted));
    }
    return ftxui::hflow(std::move(parts));
}

ftxui::Element progress_bar(std::size_t done, std::size_t total, std::string_view unit, const Theme& theme) {
    const auto ratio = total == 0 ? 0.0F : static_cast<float>(done) / static_cast<float>(total);
    const auto percent = static_cast<int>(ratio * static_cast<float>(PERCENT));
    return ftxui::hbox({
        ftxui::text(" "),
        ftxui::gauge(ratio) | ftxui::color(ratio >= 1.0F ? theme.success : theme.accent) | ftxui::flex,
        ftxui::text(std::format(" {:>3}% ", percent)) | ftxui::bold | ftxui::color(theme.text),
        ftxui::text(std::format("{}/{} {} ", done, total, unit)) | ftxui::color(theme.muted),
    });
}

ftxui::Element badge(std::string_view label, ftxui::Color background, const Theme& theme) {
    return ftxui::text(" " + std::string(label) + " ") | ftxui::bgcolor(background) | ftxui::color(theme.badge_text) |
           ftxui::bold;
}

ftxui::Element kind_badge(domain::TourKind kind, const Theme& theme, const Localizer& localizer) {
    const auto& style = style_of(kind);
    return badge(localizer.text(style_key(style, "badge")), theme.*style.tone, theme);
}

ftxui::Element activity_spinner(std::chrono::steady_clock::duration elapsed, const Theme& theme) {
    const auto frame = static_cast<std::size_t>(elapsed / SPINNER_FRAME);
    return ftxui::spinner(SPINNER_CHARSET, frame) | ftxui::color(theme.accent);
}

ftxui::Element section_title(std::string_view title, const Theme& theme) {
    return ftxui::hbox({ftxui::text(std::string(title)) | ftxui::bold | ftxui::color(theme.accent), ftxui::text(" "),
                        ftxui::separatorLight() | ftxui::color(theme.accent_soft) | ftxui::flex});
}

std::string token_text(std::int64_t tokens) {
    const auto value = static_cast<double>(tokens);
    if (tokens >= MILLION) {
        return std::format("{:.1f}M", value / static_cast<double>(MILLION));
    }
    if (tokens >= THOUSAND) {
        return std::format("{:.1f}k", value / static_cast<double>(THOUSAND));
    }
    return std::to_string(tokens);
}

ftxui::Element token_counter(const application::TokenUsage& usage, const Theme& theme, const Localizer& localizer) {
    if (usage.total() == 0) {
        return ftxui::text(localizer.text("tokens.label") + " " + std::string(glyphs::ELLIPSIS)) |
               ftxui::color(theme.muted);
    }
    const std::string_view approximate = usage.estimated ? "~" : "";
    ftxui::Elements parts{
        ftxui::text(localizer.text("tokens.label") + " ") | ftxui::color(theme.muted),
        ftxui::text(std::format("↑{}{}", approximate, token_text(usage.prompt_side()))) | ftxui::color(theme.accent),
        ftxui::text(" "),
        ftxui::text(std::format("↓{}{}", approximate, token_text(usage.output))) | ftxui::color(theme.success),
    };
    if (usage.cost_usd > 0.0) {
        parts.push_back(ftxui::text(std::format("  ${:.2f}", usage.cost_usd)) | ftxui::color(theme.warning));
    }
    return ftxui::hbox(std::move(parts));
}

std::string clock_text(std::chrono::steady_clock::duration elapsed) {
    const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
    return std::format("{:02}:{:02}", seconds / SECONDS_PER_MINUTE, seconds % SECONDS_PER_MINUTE);
}

std::string shortened(std::string_view text, std::size_t width) {
    if (width == 0) {
        return std::string(text);
    }
    std::size_t characters = 0;
    std::size_t cut = text.size();
    for (std::size_t index = 0; index < text.size(); ++index) {
        const auto byte = static_cast<unsigned char>(text.at(index));
        if ((byte & UTF8_CONTINUATION_MASK) == UTF8_CONTINUATION) {
            continue;
        }
        if (characters == width - 1 && cut == text.size()) {
            cut = index;
        }
        ++characters;
    }
    if (characters <= width) {
        return std::string(text);
    }
    return std::string(text.substr(0, cut)) + std::string(glyphs::ELLIPSIS);
}

}
