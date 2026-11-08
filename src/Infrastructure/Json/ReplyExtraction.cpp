#include "Infrastructure/Json/ReplyExtraction.hpp"

#include <algorithm>
#include <array>

namespace sloptour::infrastructure {

namespace {

constexpr std::string_view FENCE = "```";
constexpr std::string_view OPENERS = "{[";
constexpr std::string_view CLOSERS = "}]";
constexpr std::size_t PREVIEW_LENGTH = 160;

[[nodiscard]] std::string_view fenced_block(std::string_view reply) {
    const auto open = reply.find(FENCE);
    if (open == std::string_view::npos) {
        return {};
    }
    const auto body = reply.find('\n', open);
    if (body == std::string_view::npos) {
        return {};
    }
    const auto close = reply.find(FENCE, body);
    if (close == std::string_view::npos) {
        return {};
    }
    return reply.substr(body + 1, close - body - 1);
}

[[nodiscard]] std::string_view outermost_value(std::string_view text) {
    const auto first = text.find_first_of(OPENERS);
    const auto last = text.find_last_of(CLOSERS);
    if (first == std::string_view::npos || last == std::string_view::npos || last < first) {
        return {};
    }
    return text.substr(first, last - first + 1);
}

[[nodiscard]] std::string preview_of(std::string_view reply) {
    return std::string(reply.substr(0, std::min(reply.size(), PREVIEW_LENGTH)));
}

}

application::DecodeResult<Json> json_in_reply(std::string_view reply) {
    const std::array candidates{outermost_value(fenced_block(reply)), outermost_value(reply)};
    for (const auto candidate : candidates) {
        if (candidate.empty()) {
            continue;
        }
        if (auto json = parsed_json(candidate)) {
            return json;
        }
    }
    return std::unexpected(
        application::DecodeError("reply contains no valid JSON value, it starts with: " + preview_of(reply)));
}

}
