#pragma once

#include "Application/Ports/PortErrors.hpp"
#include "Domain/Quiz/Quiz.hpp"
#include "Infrastructure/Json/JsonAccess.hpp"

namespace sloptour::infrastructure {

[[nodiscard]] Json quiz_document(const domain::Quiz& quiz);
[[nodiscard]] application::DecodeResult<domain::Quiz> quiz_from_document(const Json& document);

}
