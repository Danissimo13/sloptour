#pragma once

#include "Application/Exploration/ExplorationRequest.hpp"
#include "Application/Generation/GenerationSettings.hpp"
#include "Application/Ports/AgentListener.hpp"
#include "Application/Ports/IAgentProvider.hpp"
#include "Application/Ports/IPromptLibrary.hpp"
#include "Application/Ports/IReplyDecoder.hpp"
#include "Application/Ports/PortErrors.hpp"
#include "Domain/Tour/Step.hpp"

#include <memory>
#include <stop_token>
#include <utility>
#include <vector>

namespace sloptour::application {

class DeepenStep {
public:
    struct Dependencies {
        std::shared_ptr<const IAgentProvider> agents;
        std::shared_ptr<const IPromptLibrary> prompts;
        std::shared_ptr<const IReplyDecoder> decoder;
        GenerationSettings settings;
    };

    explicit DeepenStep(Dependencies dependencies)
        : m_dependencies(std::move(dependencies)) {}

    [[nodiscard]] ApplicationResult<std::vector<domain::Step>>
    execute(const ExplorationRequest& request, const AgentListener& listener, const std::stop_token& stop) const;

private:
    Dependencies m_dependencies;
};

}
