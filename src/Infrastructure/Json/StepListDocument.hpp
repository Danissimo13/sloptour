#pragma once

#include "Application/Ports/PortErrors.hpp"
#include "Domain/Tour/Step.hpp"
#include "Infrastructure/Json/JsonAccess.hpp"

#include <vector>

namespace sloptour::infrastructure {

[[nodiscard]] application::DecodeResult<std::vector<domain::Step>> steps_from_document(const Json& steps);

}
