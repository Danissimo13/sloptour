#include "Domain/Tour/StepId.hpp"

#include "Core/Text/Text.hpp"

namespace sloptour::domain {

TourResult<StepId> StepId::create(std::string_view value) {
    if (core::is_blank(value)) {
        return std::unexpected(TourError("step id is empty"));
    }
    return StepId(std::string(core::trim(value)));
}

}
