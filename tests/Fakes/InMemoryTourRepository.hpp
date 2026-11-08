#pragma once

#include "Application/Ports/ITourRepository.hpp"

#include <map>

namespace sloptour::tests {

class InMemoryTourRepository final : public application::ITourRepository {
public:
    [[nodiscard]] application::StorageResult<void> save(const domain::Tour& tour) override {
        m_tours.insert_or_assign(tour.id(), tour);
        return {};
    }

    [[nodiscard]] application::StorageResult<domain::Tour> load(const domain::TourId& id) const override {
        const auto found = m_tours.find(id);
        if (found == m_tours.end()) {
            return std::unexpected(application::StorageError("missing tour"));
        }
        return found->second;
    }

    [[nodiscard]] application::StorageResult<std::vector<domain::TourId>> list() const override {
        std::vector<domain::TourId> ids;
        ids.reserve(m_tours.size());
        for (const auto& [id, tour] : m_tours) {
            ids.push_back(id);
        }
        return ids;
    }

private:
    std::map<domain::TourId, domain::Tour> m_tours;
};

}
