#pragma once

#include "Application/Preferences/StorageMode.hpp"

#include <optional>
#include <string_view>

namespace sloptour::infrastructure {

[[nodiscard]] std::string_view name_of(application::StorageMode mode) noexcept;
[[nodiscard]] std::optional<application::StorageMode> storage_named(std::string_view name) noexcept;

}
