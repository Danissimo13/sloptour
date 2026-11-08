#pragma once

#include "Application/Ports/IAgentProvider.hpp"
#include "Infrastructure/Agents/AgentProfile.hpp"
#include "Infrastructure/Process/ProcessRunner.hpp"

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace sloptour::infrastructure {

class ConfiguredAgentProvider final : public application::IAgentProvider {
public:
    struct Dependencies {
        std::vector<AgentProfile> profiles;
        std::string default_agent;
        std::filesystem::path working_directory;
        std::shared_ptr<const ProcessRunner> runner;
    };

    explicit ConfiguredAgentProvider(Dependencies dependencies)
        : m_dependencies(std::move(dependencies)) {}

    [[nodiscard]] std::vector<application::AgentDescriptor> agents() const override;
    [[nodiscard]] std::string_view default_agent() const noexcept override;
    [[nodiscard]] application::AgentResult<std::shared_ptr<application::IAgent>>
    find(const application::AgentChoice& choice) const override;

private:
    Dependencies m_dependencies;
};

}
