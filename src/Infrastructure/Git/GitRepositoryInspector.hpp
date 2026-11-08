#pragma once

#include "Application/Ports/IRepositoryInspector.hpp"
#include "Infrastructure/Git/GitSettings.hpp"
#include "Infrastructure/Process/ProcessRunner.hpp"

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace sloptour::infrastructure {

class GitRepositoryInspector final : public application::IRepositoryInspector {
public:
    GitRepositoryInspector(std::filesystem::path root, std::shared_ptr<const ProcessRunner> runner,
                           GitSettings settings)
        : m_root(std::move(root))
        , m_runner(std::move(runner))
        , m_settings(settings) {}

    [[nodiscard]] application::InspectionResult<application::RepositoryOverview> overview() const override;
    [[nodiscard]] application::InspectionResult<std::vector<application::CommitSummary>>
    recent_commits(std::size_t limit) const override;
    [[nodiscard]] application::InspectionResult<application::ChangeDigest>
    changes(const application::ChangeScope& scope) const override;

private:
    [[nodiscard]] application::InspectionResult<std::string> git(std::vector<std::string> arguments) const;
    [[nodiscard]] bool is_versioned() const;
    [[nodiscard]] std::string digest(std::vector<std::string> arguments) const;
    [[nodiscard]] application::InspectionResult<std::string> verified(const std::string& revision) const;

    std::filesystem::path m_root;
    std::shared_ptr<const ProcessRunner> m_runner;
    GitSettings m_settings;
};

}
