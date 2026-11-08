#pragma once

#include "Application/Ports/PortErrors.hpp"

#include <string_view>

namespace sloptour::infrastructure {

[[nodiscard]] application::StorageResult<std::string_view> embedded_asset(std::string_view path);

}
