#pragma once

#include "Application/Ports/IRepositoryInspector.hpp"

namespace sloptour::tests {

class FakeRepositoryInspector final : public application::IRepositoryInspector {
public:
    [[nodiscard]] application::InspectionResult<application::RepositoryOverview> overview() const override {
        return application::RepositoryOverview{
            .name = "demo", .root = "/demo", .versioned = true, .branch = "main", .head = "abc123 Initial"};
    }

    [[nodiscard]] application::InspectionResult<std::vector<application::CommitSummary>>
    recent_commits(std::size_t /*limit*/) const override {
        return std::vector<application::CommitSummary>{};
    }

    [[nodiscard]] application::InspectionResult<application::ChangeDigest>
    changes(const application::ChangeScope& scope) const override {
        return application::ChangeDigest{.range = scope.base_revision.value_or("working tree") + "..HEAD",
                                         .commits = "abc123 Add feature",
                                         .diff_stat = "1 file changed",
                                         .working_tree = {}};
    }
};

}
