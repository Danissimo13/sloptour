#pragma once

#include "Application/Ports/PortErrors.hpp"
#include "Domain/Tour/Step.hpp"
#include "Infrastructure/Json/JsonAccess.hpp"

namespace sloptour::infrastructure {

[[nodiscard]] Json step_document(const domain::Step& step);
[[nodiscard]] application::DecodeResult<domain::Step> step_from_document(const Json& document);

}
