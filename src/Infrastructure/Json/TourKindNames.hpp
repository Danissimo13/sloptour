#pragma once

#include "Domain/Tour/TourKind.hpp"

#include <optional>
#include <string_view>

namespace sloptour::infrastructure {

[[nodiscard]] std::string_view name_of(domain::TourKind kind) noexcept;
[[nodiscard]] std::optional<domain::TourKind> kind_named(std::string_view name) noexcept;

}
