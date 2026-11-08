#pragma once

#include "Application/Ports/PortErrors.hpp"
#include "Infrastructure/Json/JsonAccess.hpp"

#include <string_view>

namespace sloptour::infrastructure {

[[nodiscard]] application::DecodeResult<Json> json_in_reply(std::string_view reply);

}
