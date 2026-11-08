#pragma once

#include "Domain/Tour/TourKind.hpp"
#include "Terminal/Theme/Theme.hpp"

#include <ftxui/screen/color.hpp>

#include <cstddef>
#include <span>
#include <string>
#include <string_view>

namespace sloptour::terminal {

struct TourKindStyle {
    domain::TourKind kind = domain::TourKind::Onboarding;
    char shortcut = ' ';
    std::string_view key;
    ftxui::Color Theme::* tone = nullptr;
};

[[nodiscard]] std::span<const TourKindStyle> tour_kind_styles() noexcept;
[[nodiscard]] const TourKindStyle& style_of(domain::TourKind kind) noexcept;
[[nodiscard]] const TourKindStyle& style_at(std::size_t index);
[[nodiscard]] std::string style_key(const TourKindStyle& style, std::string_view field);

}
