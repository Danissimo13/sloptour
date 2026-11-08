#pragma once

#include "Application/Generation/DepthLevel.hpp"
#include "Application/Generation/DetailLevel.hpp"
#include "Application/Ports/PortErrors.hpp"
#include "Infrastructure/Json/JsonAccess.hpp"

#include <vector>

namespace sloptour::infrastructure {

[[nodiscard]] application::DecodeResult<std::vector<application::DepthLevel>> depth_levels(const Json& generation);
[[nodiscard]] application::DecodeResult<std::vector<application::DetailLevel>> detail_levels(const Json& generation);

}
