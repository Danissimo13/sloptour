#pragma once

#include "Application/Ports/PortErrors.hpp"
#include "Domain/Progress/TourProgress.hpp"
#include "Domain/Tour/TourId.hpp"

namespace sloptour::application {

class IProgressRepository {
public:
    IProgressRepository() = default;
    virtual ~IProgressRepository() = default;
    IProgressRepository(const IProgressRepository&) = delete;
    IProgressRepository& operator=(const IProgressRepository&) = delete;
    IProgressRepository(IProgressRepository&&) = delete;
    IProgressRepository& operator=(IProgressRepository&&) = delete;

    [[nodiscard]] virtual StorageResult<domain::TourProgress> load(const domain::TourId& id) const = 0;
    [[nodiscard]] virtual StorageResult<void> save(const domain::TourProgress& progress) = 0;
};

}
