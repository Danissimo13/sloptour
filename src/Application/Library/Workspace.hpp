#pragma once

#include "Application/Generation/GenerationSettings.hpp"
#include "Application/Ports/AgentDescriptor.hpp"
#include "Application/Ports/CommitSummary.hpp"
#include "Application/Ports/IAgentProvider.hpp"
#include "Application/Ports/IRepositoryInspector.hpp"
#include "Application/Ports/PortErrors.hpp"
#include "Application/Ports/RepositoryOverview.hpp"

#include <cstddef>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace sloptour::application {

class Workspace {
public:
    Workspace(std::shared_ptr<const IRepositoryInspector> inspector, std::shared_ptr<const IAgentProvider> agents,
              GenerationSettings settings)
        : m_inspector(std::move(inspector))
        , m_agents(std::move(agents))
        , m_settings(std::move(settings)) {}

    [[nodiscard]] ApplicationResult<RepositoryOverview> overview() const {
        return lifted(m_inspector->overview());
    }

    [[nodiscard]] ApplicationResult<std::vector<CommitSummary>> recent_commits(std::size_t limit) const {
        return lifted(m_inspector->recent_commits(limit));
    }

    [[nodiscard]] std::vector<AgentDescriptor> agents() const {
        return m_agents->agents();
    }

    [[nodiscard]] std::string default_agent() const {
        return std::string(m_agents->default_agent());
    }

    [[nodiscard]] std::vector<std::string> depth_levels() const {
        std::vector<std::string> names;
        names.reserve(m_settings.depths.size());
        for (const auto& level : m_settings.depths) {
            names.push_back(level.name);
        }
        return names;
    }

    [[nodiscard]] std::vector<std::string> detail_levels() const {
        std::vector<std::string> names;
        names.reserve(m_settings.details.size());
        for (const auto& level : m_settings.details) {
            names.push_back(level.name);
        }
        return names;
    }

    [[nodiscard]] const std::string& default_level() const noexcept {
        return m_settings.default_level;
    }

private:
    std::shared_ptr<const IRepositoryInspector> m_inspector;
    std::shared_ptr<const IAgentProvider> m_agents;
    GenerationSettings m_settings;
};

}
