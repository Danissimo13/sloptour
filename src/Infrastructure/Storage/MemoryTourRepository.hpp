#pragma once

#include "Application/Ports/ITourRepository.hpp"

#include <map>
#include <mutex>

namespace sloptour::infrastructure {

class MemoryTourRepository final : public application::ITourRepository {
public:
    [[nodiscard]] application::StorageResult<void> save(const domain::Tour& tour) override;
    [[nodiscard]] application::StorageResult<domain::Tour> load(const domain::TourId& id) const override;
    [[nodiscard]] application::StorageResult<std::vector<domain::TourId>> list() const override;

private:
    mutable std::mutex m_mutex;
    std::map<domain::TourId, domain::Tour> m_tours;
};

}
