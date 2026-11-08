#include "Infrastructure/Agents/JsonReplyDecoder.hpp"

#include "Infrastructure/Json/DocumentKeys.hpp"
#include "Infrastructure/Json/OutlineDocument.hpp"
#include "Infrastructure/Json/QuizDocument.hpp"
#include "Infrastructure/Json/ReplyExtraction.hpp"
#include "Infrastructure/Json/StepListDocument.hpp"

namespace sloptour::infrastructure {

application::DecodeResult<domain::TourOutline> JsonReplyDecoder::decode_outline(std::string_view reply) const {
    auto document = json_in_reply(reply);
    if (!document) {
        return std::unexpected(document.error());
    }
    return outline_from_document(*document);
}

application::DecodeResult<domain::Quiz> JsonReplyDecoder::decode_quiz(std::string_view reply) const {
    auto document = json_in_reply(reply);
    if (!document) {
        return std::unexpected(document.error());
    }
    return quiz_from_document(*document);
}

application::DecodeResult<std::vector<domain::Step>> JsonReplyDecoder::decode_steps(std::string_view reply) const {
    auto document = json_in_reply(reply);
    if (!document) {
        return std::unexpected(document.error());
    }
    if (document->is_array()) {
        return steps_from_document(*document);
    }
    const auto steps = member(*document, keys::STEPS);
    if (!steps) {
        return std::unexpected(application::DecodeError("reply has no 'steps' array"));
    }
    return steps_from_document(*steps);
}

}
