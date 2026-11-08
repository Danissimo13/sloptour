#pragma once

#include "Application/Ports/TokenUsage.hpp"
#include "Domain/Tour/TourKind.hpp"
#include "Terminal/Localization/Localizer.hpp"
#include "Terminal/Theme/Theme.hpp"
#include "Terminal/Widgets/KeyHint.hpp"

#include <ftxui/dom/elements.hpp>

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <string_view>

namespace sloptour::terminal {

[[nodiscard]] ftxui::Element header_bar(std::string_view title, ftxui::Element right, const Theme& theme);
[[nodiscard]] ftxui::Element panel(ftxui::Element title, ftxui::Element content, const Theme& theme);
[[nodiscard]] ftxui::Element selection_marker(bool selected, const Theme& theme);
[[nodiscard]] ftxui::Element key_hints(std::span<const KeyHint> hints, const Theme& theme, const Localizer& localizer);
[[nodiscard]] ftxui::Element progress_bar(std::size_t done, std::size_t total, std::string_view unit,
                                          const Theme& theme);
[[nodiscard]] ftxui::Element token_counter(const application::TokenUsage& usage, const Theme& theme,
                                           const Localizer& localizer);
[[nodiscard]] std::string token_text(std::int64_t tokens);
[[nodiscard]] ftxui::Element badge(std::string_view label, ftxui::Color background, const Theme& theme);
[[nodiscard]] ftxui::Element kind_badge(domain::TourKind kind, const Theme& theme, const Localizer& localizer);
[[nodiscard]] ftxui::Element activity_spinner(std::chrono::steady_clock::duration elapsed, const Theme& theme);
[[nodiscard]] ftxui::Element section_title(std::string_view title, const Theme& theme);
[[nodiscard]] std::string clock_text(std::chrono::steady_clock::duration elapsed);
[[nodiscard]] std::string shortened(std::string_view text, std::size_t width);

}
