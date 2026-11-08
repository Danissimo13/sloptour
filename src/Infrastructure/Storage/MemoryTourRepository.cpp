#include "Infrastructure/Storage/MemoryTourRepository.hpp"

namespace sloptour::infrastructure {

application::StorageResult<void> MemoryTourRepository::save(const domain::Tour& tour) {
    const std::scoped_lock lock(m_mutex);
    m_tours.insert_or_assign(tour.id(), tour);
    return {};
}

application::StorageResult<domain::Tour> MemoryTourRepository::load(const domain::TourId& id) const {
    const std::scoped_lock lock(m_mutex);
    const auto found = m_tours.find(id);
    if (found == m_tours.end()) {
        return std::unexpected(application::StorageError("tour '" + id.value() + "' is not in memory"));
    }
    return found->second;
}

application::StorageResult<std::vector<domain::TourId>> MemoryTourRepository::list() const {
    const std::scoped_lock lock(m_mutex);
    std::vector<domain::TourId> ids;
    ids.reserve(m_tours.size());
    for (const auto& [id, tour] : m_tours) {
        ids.push_back(id);
    }
    return ids;
}

}
