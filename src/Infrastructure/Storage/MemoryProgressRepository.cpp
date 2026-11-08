#include "Infrastructure/Storage/MemoryProgressRepository.hpp"

namespace sloptour::infrastructure {

application::StorageResult<domain::TourProgress> MemoryProgressRepository::load(const domain::TourId& id) const {
    const std::scoped_lock lock(m_mutex);
    const auto found = m_progress.find(id);
    if (found == m_progress.end()) {
        return domain::TourProgress(id);
    }
    return found->second;
}

application::StorageResult<void> MemoryProgressRepository::save(const domain::TourProgress& progress) {
    const std::scoped_lock lock(m_mutex);
    m_progress.insert_or_assign(progress.tour(), progress);
    return {};
}

}
