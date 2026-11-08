#include "Terminal/Widgets/TourKindStyle.hpp"

#include <array>

namespace sloptour::terminal {

namespace {

constexpr std::string_view KIND_PREFIX = "kind.";

constexpr std::array STYLES{
    TourKindStyle{
        .kind = domain::TourKind::Onboarding,
        .shortcut = 'n',
        .key = "onboarding",
        .tone = &Theme::accent,
    },
    TourKindStyle{
        .kind = domain::TourKind::Recap,
        .shortcut = 'r',
        .key = "recap",
        .tone = &Theme::warning,
    },
    TourKindStyle{
        .kind = domain::TourKind::Functional,
        .shortcut = 'f',
        .key = "functional",
        .tone = &Theme::success,
    },
};

}

std::span<const TourKindStyle> tour_kind_styles() noexcept {
    return STYLES;
}

std::string style_key(const TourKindStyle& style, std::string_view field) {
    return std::string(KIND_PREFIX) + std::string(style.key) + "." + std::string(field);
}

const TourKindStyle& style_at(std::size_t index) {
    return STYLES.at(index);
}

const TourKindStyle& style_of(domain::TourKind kind) noexcept {
    for (const auto& style : STYLES) {
        if (style.kind == kind) {
            return style;
        }
    }
    return STYLES.front();
}

}
