#pragma once

#include "Application/Ports/PortErrors.hpp"
#include "Domain/Outline/TourOutline.hpp"
#include "Domain/Quiz/Quiz.hpp"
#include "Domain/Tour/Step.hpp"

#include <string_view>
#include <vector>

namespace sloptour::application {

class IReplyDecoder {
public:
    IReplyDecoder() = default;
    virtual ~IReplyDecoder() = default;
    IReplyDecoder(const IReplyDecoder&) = delete;
    IReplyDecoder& operator=(const IReplyDecoder&) = delete;
    IReplyDecoder(IReplyDecoder&&) = delete;
    IReplyDecoder& operator=(IReplyDecoder&&) = delete;

    [[nodiscard]] virtual DecodeResult<domain::TourOutline> decode_outline(std::string_view reply) const = 0;
    [[nodiscard]] virtual DecodeResult<std::vector<domain::Step>> decode_steps(std::string_view reply) const = 0;
    [[nodiscard]] virtual DecodeResult<domain::Quiz> decode_quiz(std::string_view reply) const = 0;
};

}
