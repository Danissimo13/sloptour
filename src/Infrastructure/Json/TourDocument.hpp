#pragma once

#include "Application/Ports/PortErrors.hpp"
#include "Domain/Tour/Tour.hpp"
#include "Infrastructure/Json/JsonAccess.hpp"

namespace sloptour::infrastructure {

[[nodiscard]] Json tour_document(const domain::Tour& tour);
[[nodiscard]] application::DecodeResult<domain::Tour> tour_from_document(const Json& document);

}
