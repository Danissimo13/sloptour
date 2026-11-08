#include "Domain/Tour/TourId.hpp"

#include "Core/Text/Text.hpp"

namespace sloptour::domain {

TourResult<TourId> TourId::create(std::string value) {
    if (value.empty() || core::slugify(value) != value) {
        return std::unexpected(TourError("tour id must be a non-empty lowercase slug: '" + value + "'"));
    }
    return TourId(std::move(value));
}

}
