#pragma once

#include "Infrastructure/Agents/UsageSample.hpp"

#include <optional>
#include <string>

namespace sloptour::infrastructure {

struct AgentLine {
    std::optional<std::string> activity;
    std::optional<UsageSample> usage;
};

}
