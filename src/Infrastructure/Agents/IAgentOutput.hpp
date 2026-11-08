#pragma once

#include "Application/Ports/PortErrors.hpp"
#include "Infrastructure/Agents/AgentLine.hpp"

#include <optional>
#include <string>
#include <string_view>

namespace sloptour::infrastructure {

class IAgentOutput {
public:
    IAgentOutput() = default;
    virtual ~IAgentOutput() = default;
    IAgentOutput(const IAgentOutput&) = delete;
    IAgentOutput& operator=(const IAgentOutput&) = delete;
    IAgentOutput(IAgentOutput&&) = delete;
    IAgentOutput& operator=(IAgentOutput&&) = delete;

    [[nodiscard]] virtual AgentLine parse(std::string_view line) const = 0;
    [[nodiscard]] virtual application::AgentResult<std::string> reply(std::string_view output) const = 0;
};

}
