#pragma once

#include "Application/Ports/IProgressRepository.hpp"

#include <map>
#include <mutex>

namespace sloptour::infrastructure {

class MemoryProgressRepository final : public application::IProgressRepository {
public:
    [[nodiscard]] application::StorageResult<domain::TourProgress> load(const domain::TourId& id) const override;
    [[nodiscard]] application::StorageResult<void> save(const domain::TourProgress& progress) override;

private:
    mutable std::mutex m_mutex;
    std::map<domain::TourId, domain::TourProgress> m_progress;
};

}
