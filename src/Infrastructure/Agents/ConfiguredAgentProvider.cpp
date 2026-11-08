#include "Infrastructure/Agents/ConfiguredAgentProvider.hpp"

#include "Infrastructure/Agents/AgentCommand.hpp"
#include "Infrastructure/Agents/AgentOutputs.hpp"
#include "Infrastructure/Agents/CommandAgent.hpp"
#include "Infrastructure/Process/ExecutableLocator.hpp"

#include <algorithm>

namespace sloptour::infrastructure {

std::vector<application::AgentDescriptor> ConfiguredAgentProvider::agents() const {
    std::vector<application::AgentDescriptor> descriptors;
    descriptors.reserve(m_dependencies.profiles.size());
    for (const auto& profile : m_dependencies.profiles) {
        descriptors.push_back({.name = profile.name,
                               .description = profile.description,
                               .executable = profile.command.front(),
                               .models = profile.models,
                               .installed = locate_executable(profile.command.front()).has_value()});
    }
    return descriptors;
}

std::string_view ConfiguredAgentProvider::default_agent() const noexcept {
    return m_dependencies.default_agent;
}

application::AgentResult<std::shared_ptr<application::IAgent>>
ConfiguredAgentProvider::find(const application::AgentChoice& choice) const {
    const auto& profiles = m_dependencies.profiles;
    const auto found = std::ranges::find(profiles, choice.name, &AgentProfile::name);
    if (found == profiles.end()) {
        return std::unexpected(application::AgentError("unknown agent '" + choice.name +
                                                       "', see `sloptour agents` for the configured ones"));
    }
    if (!locate_executable(found->command.front())) {
        return std::unexpected(
            application::AgentError("agent '" + found->name + "' needs '" + found->command.front() + "' on PATH"));
    }
    auto command = agent_command(*found, choice.model);
    auto output = agent_output(found->output, m_dependencies.working_directory);
    if (!command || !output) {
        return std::unexpected(!command ? command.error() : output.error());
    }
    return std::make_shared<CommandAgent>(
        CommandAgent::Dependencies{.profile = *found,
                                   .command = std::move(*command),
                                   .working_directory = m_dependencies.working_directory,
                                   .runner = m_dependencies.runner,
                                   .output = std::move(*output)});
}

}
