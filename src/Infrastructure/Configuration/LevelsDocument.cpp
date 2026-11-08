#include "Infrastructure/Configuration/LevelsDocument.hpp"

namespace sloptour::infrastructure {

namespace {

constexpr std::string_view DEPTHS = "depths";
constexpr std::string_view DETAILS = "details";
constexpr std::string_view NAME = "name";
constexpr std::string_view GUIDANCE = "guidance";

[[nodiscard]] application::DecodeResult<application::DepthLevel> depth_of(const Json& level) {
    auto name = required_string(level, NAME);
    const auto minimum = required_count(level, "min_chapters");
    const auto maximum = required_count(level, "max_chapters");
    const auto steps = required_count(level, "max_steps");
    const auto deeper = required_count(level, "deepen_steps");
    const auto quiz = required_count(level, "quiz_questions");
    for (const auto& count : {minimum, maximum, steps, deeper, quiz}) {
        if (!count) {
            return std::unexpected(count.error().located_at(DEPTHS));
        }
    }
    if (!name) {
        return std::unexpected(name.error().located_at(DEPTHS));
    }
    return application::DepthLevel{.name = std::move(*name),
                                   .min_chapters = *minimum,
                                   .max_chapters = *maximum,
                                   .max_steps = *steps,
                                   .deepen_steps = *deeper,
                                   .quiz_questions = *quiz,
                                   .guidance = optional_string(level, GUIDANCE)};
}

[[nodiscard]] application::DecodeResult<application::DetailLevel> detail_of(const Json& level) {
    auto name = required_string(level, NAME);
    if (!name) {
        return std::unexpected(name.error().located_at(DETAILS));
    }
    return application::DetailLevel{.name = std::move(*name), .guidance = optional_string(level, GUIDANCE)};
}

template <typename TLevel, typename TDecode>
[[nodiscard]] application::DecodeResult<std::vector<TLevel>> levels(const Json& generation, std::string_view key,
                                                                    TDecode decode) {
    std::vector<TLevel> decoded;
    for (const auto& item : object_list(generation, key)) {
        auto level = decode(item.get());
        if (!level) {
            return std::unexpected(level.error());
        }
        decoded.push_back(std::move(*level));
    }
    if (decoded.empty()) {
        return std::unexpected(application::DecodeError("'" + std::string(key) + "' needs at least one level"));
    }
    return decoded;
}

}

application::DecodeResult<std::vector<application::DepthLevel>> depth_levels(const Json& generation) {
    return levels<application::DepthLevel>(generation, DEPTHS, depth_of);
}

application::DecodeResult<std::vector<application::DetailLevel>> detail_levels(const Json& generation) {
    return levels<application::DetailLevel>(generation, DETAILS, detail_of);
}

}
