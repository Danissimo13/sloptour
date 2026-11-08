#pragma once

#include "Application/Ports/PortErrors.hpp"
#include "Domain/Outline/TourOutline.hpp"
#include "Infrastructure/Json/JsonAccess.hpp"

namespace sloptour::infrastructure {

[[nodiscard]] application::DecodeResult<domain::TourOutline> outline_from_document(const Json& document);

}
