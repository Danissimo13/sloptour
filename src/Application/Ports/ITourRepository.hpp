#pragma once

#include "Application/Ports/PortErrors.hpp"
#include "Domain/Tour/Tour.hpp"
#include "Domain/Tour/TourId.hpp"

#include <vector>

namespace sloptour::application {

class ITourRepository {
public:
    ITourRepository() = default;
    virtual ~ITourRepository() = default;
    ITourRepository(const ITourRepository&) = delete;
    ITourRepository& operator=(const ITourRepository&) = delete;
    ITourRepository(ITourRepository&&) = delete;
    ITourRepository& operator=(ITourRepository&&) = delete;

    [[nodiscard]] virtual StorageResult<void> save(const domain::Tour& tour) = 0;
    [[nodiscard]] virtual StorageResult<domain::Tour> load(const domain::TourId& id) const = 0;
    [[nodiscard]] virtual StorageResult<std::vector<domain::TourId>> list() const = 0;
};

}
