#pragma once

#include "Application/Ports/AgentChoice.hpp"
#include "Application/Ports/AgentDescriptor.hpp"
#include "Application/Ports/IAgent.hpp"
#include "Application/Ports/PortErrors.hpp"

#include <memory>
#include <string_view>
#include <vector>

namespace sloptour::application {

class IAgentProvider {
public:
    IAgentProvider() = default;
    virtual ~IAgentProvider() = default;
    IAgentProvider(const IAgentProvider&) = delete;
    IAgentProvider& operator=(const IAgentProvider&) = delete;
    IAgentProvider(IAgentProvider&&) = delete;
    IAgentProvider& operator=(IAgentProvider&&) = delete;

    [[nodiscard]] virtual std::vector<AgentDescriptor> agents() const = 0;
    [[nodiscard]] virtual std::string_view default_agent() const noexcept = 0;
    [[nodiscard]] virtual AgentResult<std::shared_ptr<IAgent>> find(const AgentChoice& choice) const = 0;
};

}
