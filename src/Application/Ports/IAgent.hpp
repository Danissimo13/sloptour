#pragma once

#include "Application/Ports/AgentListener.hpp"
#include "Application/Ports/AgentTask.hpp"
#include "Application/Ports/PortErrors.hpp"

#include <stop_token>
#include <string>
#include <string_view>

namespace sloptour::application {

class IAgent {
public:
    IAgent() = default;
    virtual ~IAgent() = default;
    IAgent(const IAgent&) = delete;
    IAgent& operator=(const IAgent&) = delete;
    IAgent(IAgent&&) = delete;
    IAgent& operator=(IAgent&&) = delete;

    [[nodiscard]] virtual std::string_view name() const noexcept = 0;
    [[nodiscard]] virtual AgentResult<std::string> run(const AgentTask& task, const AgentListener& listener,
                                                       const std::stop_token& stop) = 0;
};

}
