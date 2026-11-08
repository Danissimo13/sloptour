#pragma once

#include <cstdint>

namespace sloptour::domain {

enum class TourKind : std::uint8_t {
    Onboarding,
    Recap,
    Functional,
};

}
