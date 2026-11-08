#pragma once

#include "Application/Generation/DepthLevel.hpp"
#include "Application/Generation/DetailLevel.hpp"
#include "Application/Ports/PortErrors.hpp"

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace sloptour::application {

struct GenerationSettings {
    std::size_t parallel_chapters = 1;
    std::size_t repair_attempts = 1;
    std::string default_level;
    std::vector<DepthLevel> depths;
    std::vector<DetailLevel> details;

    [[nodiscard]] ApplicationResult<DepthLevel> depth(std::string_view name) const;
    [[nodiscard]] ApplicationResult<DetailLevel> detail(std::string_view name) const;
};

}
