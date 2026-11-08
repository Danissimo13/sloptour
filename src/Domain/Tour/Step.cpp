#include "Domain/Tour/Step.hpp"

#include "Core/Text/Text.hpp"

#include <algorithm>

namespace sloptour::domain {

namespace {

[[nodiscard]] bool is_incomplete(const std::optional<Checkpoint>& checkpoint) {
    return checkpoint.has_value() && (core::is_blank(checkpoint->question) || core::is_blank(checkpoint->answer));
}

}

TourResult<Step> Step::create(StepDraft draft) {
    if (core::is_blank(draft.title)) {
        return std::unexpected(TourError("step '" + draft.id.value() + "' has no title"));
    }
    if (core::is_blank(draft.body)) {
        return std::unexpected(TourError("step '" + draft.id.value() + "' has no body"));
    }
    if (is_incomplete(draft.checkpoint)) {
        draft.checkpoint.reset();
    }
    std::erase_if(draft.commands, [](const std::string& command) { return core::is_blank(command); });
    return Step(std::move(draft));
}

}
