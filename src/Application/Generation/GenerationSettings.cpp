#include "Application/Generation/GenerationSettings.hpp"

#include <algorithm>

namespace sloptour::application {

namespace {

template <typename TLevel>
[[nodiscard]] ApplicationResult<TLevel> level_named(const std::vector<TLevel>& levels, std::string_view name,
                                                    std::string_view fallback, std::string_view kind) {
    const auto wanted = name.empty() ? fallback : name;
    const auto found = std::ranges::find(levels, wanted, &TLevel::name);
    if (found == levels.end()) {
        return std::unexpected(
            ApplicationError("unknown " + std::string(kind) + " level '" + std::string(wanted) + "'"));
    }
    return *found;
}

}

ApplicationResult<DepthLevel> GenerationSettings::depth(std::string_view name) const {
    return level_named(depths, name, default_level, "depth");
}

ApplicationResult<DetailLevel> GenerationSettings::detail(std::string_view name) const {
    return level_named(details, name, default_level, "detail");
}

}
