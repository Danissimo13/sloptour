#pragma once

#include <compare>
#include <cstddef>

namespace sloptour::domain {

struct StepPosition {
    std::size_t chapter = 0;
    std::size_t step = 0;

    [[nodiscard]] auto operator<=>(const StepPosition&) const = default;
};

}
