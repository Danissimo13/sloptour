#pragma once

#include "Application/Ports/PortErrors.hpp"
#include "Infrastructure/Agents/IAgentOutput.hpp"

#include <filesystem>
#include <memory>
#include <string_view>

namespace sloptour::infrastructure {

[[nodiscard]] application::AgentResult<std::shared_ptr<const IAgentOutput>>
agent_output(std::string_view name, const std::filesystem::path& working_directory);

}
