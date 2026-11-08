#pragma once

#include "Domain/Progress/TourProgress.hpp"
#include "Domain/Tour/TourId.hpp"
#include "Infrastructure/Json/JsonAccess.hpp"

namespace sloptour::infrastructure {

[[nodiscard]] Json progress_document(const domain::TourProgress& progress);
[[nodiscard]] domain::TourProgress progress_from_document(domain::TourId tour, const Json& document);

}
