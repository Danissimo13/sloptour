#pragma once

#include "Application/Ports/IAgent.hpp"
#include "Infrastructure/Agents/AgentProfile.hpp"
#include "Infrastructure/Agents/IAgentOutput.hpp"
#include "Infrastructure/Process/ProcessRunner.hpp"

#include <filesystem>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace sloptour::infrastructure {

class CommandAgent final : public application::IAgent {
public:
    struct Dependencies {
        AgentProfile profile;
        std::vector<std::string> command;
        std::filesystem::path working_directory;
        std::shared_ptr<const ProcessRunner> runner;
        std::shared_ptr<const IAgentOutput> output;
    };

    explicit CommandAgent(Dependencies dependencies)
        : m_dependencies(std::move(dependencies)) {}

    [[nodiscard]] std::string_view name() const noexcept override {
        return m_dependencies.profile.name;
    }

    [[nodiscard]] application::AgentResult<std::string> run(const application::AgentTask& task,
                                                            const application::AgentListener& listener,
                                                            const std::stop_token& stop) override;

private:
    Dependencies m_dependencies;
};

}
