#pragma once

#include "Application/Ports/ChangeDigest.hpp"
#include "Application/Ports/ChangeScope.hpp"
#include "Application/Ports/CommitSummary.hpp"
#include "Application/Ports/PortErrors.hpp"
#include "Application/Ports/RepositoryOverview.hpp"

#include <cstddef>
#include <vector>

namespace sloptour::application {

class IRepositoryInspector {
public:
    IRepositoryInspector() = default;
    virtual ~IRepositoryInspector() = default;
    IRepositoryInspector(const IRepositoryInspector&) = delete;
    IRepositoryInspector& operator=(const IRepositoryInspector&) = delete;
    IRepositoryInspector(IRepositoryInspector&&) = delete;
    IRepositoryInspector& operator=(IRepositoryInspector&&) = delete;

    [[nodiscard]] virtual InspectionResult<RepositoryOverview> overview() const = 0;
    [[nodiscard]] virtual InspectionResult<std::vector<CommitSummary>> recent_commits(std::size_t limit) const = 0;
    [[nodiscard]] virtual InspectionResult<ChangeDigest> changes(const ChangeScope& scope) const = 0;
};

}
