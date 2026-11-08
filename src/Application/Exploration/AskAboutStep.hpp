#pragma once

#include "Application/Exploration/ExplorationRequest.hpp"
#include "Application/Ports/AgentListener.hpp"
#include "Application/Ports/IAgentProvider.hpp"
#include "Application/Ports/IPromptLibrary.hpp"
#include "Application/Ports/PortErrors.hpp"
#include "Domain/Progress/Exchange.hpp"

#include <memory>
#include <stop_token>
#include <utility>

namespace sloptour::application {

class AskAboutStep {
public:
    AskAboutStep(std::shared_ptr<const IAgentProvider> agents, std::shared_ptr<const IPromptLibrary> prompts)
        : m_agents(std::move(agents))
        , m_prompts(std::move(prompts)) {}

    [[nodiscard]] ApplicationResult<domain::Exchange>
    execute(const ExplorationRequest& request, const AgentListener& listener, const std::stop_token& stop) const;

private:
    std::shared_ptr<const IAgentProvider> m_agents;
    std::shared_ptr<const IPromptLibrary> m_prompts;
};

}
