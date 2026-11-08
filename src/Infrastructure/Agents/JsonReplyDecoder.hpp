#pragma once

#include "Application/Ports/IReplyDecoder.hpp"

namespace sloptour::infrastructure {

class JsonReplyDecoder final : public application::IReplyDecoder {
public:
    [[nodiscard]] application::DecodeResult<domain::TourOutline> decode_outline(std::string_view reply) const override;
    [[nodiscard]] application::DecodeResult<std::vector<domain::Step>>
    decode_steps(std::string_view reply) const override;
    [[nodiscard]] application::DecodeResult<domain::Quiz> decode_quiz(std::string_view reply) const override;
};

}
