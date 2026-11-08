#include "Domain/Tour/LineRange.hpp"

#include <format>

namespace sloptour::domain {

TourResult<LineRange> LineRange::create(std::size_t first, std::size_t last) {
    if (first == 0 || last < first) {
        return std::unexpected(TourError(std::format("invalid line range {}-{}", first, last)));
    }
    return LineRange(first, last);
}

}
