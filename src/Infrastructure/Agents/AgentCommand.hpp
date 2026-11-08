#pragma once

#include "Application/Ports/PortErrors.hpp"
#include "Infrastructure/Agents/AgentProfile.hpp"

#include <string>
#include <string_view>
#include <vector>

namespace sloptour::infrastructure {

[[nodiscard]] application::AgentResult<std::vector<std::string>> agent_command(const AgentProfile& profile,
                                                                               std::string_view model);

}
