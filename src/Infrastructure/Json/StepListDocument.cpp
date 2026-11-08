#include "Infrastructure/Json/StepListDocument.hpp"

#include "Infrastructure/Json/StepDocument.hpp"

namespace sloptour::infrastructure {

application::DecodeResult<std::vector<domain::Step>> steps_from_document(const Json& steps) {
    if (!steps.is_array()) {
        return std::unexpected(application::DecodeError("'steps' must be an array"));
    }
    std::vector<domain::Step> decoded;
    std::optional<application::DecodeError> first_failure;
    for (const auto& item : steps) {
        auto step = step_from_document(item);
        if (step) {
            decoded.push_back(std::move(*step));
        } else if (!first_failure) {
            first_failure = step.error();
        }
    }
    if (decoded.empty()) {
        const auto reason = first_failure ? first_failure->message() : std::string("no steps given");
        return std::unexpected(application::DecodeError("no usable step: " + reason));
    }
    return decoded;
}

}
