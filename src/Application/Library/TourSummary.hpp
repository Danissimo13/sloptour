#pragma once

#include "Domain/Tour/TourId.hpp"
#include "Domain/Tour/TourMetadata.hpp"

#include <cstddef>

namespace sloptour::application {

struct TourSummary {
    domain::TourId id;
    domain::TourMetadata metadata;
    std::size_t chapters = 0;
    std::size_t steps = 0;
    std::size_t visited = 0;
    bool has_quiz = false;
};

}
