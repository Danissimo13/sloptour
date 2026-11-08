#pragma once

#include "Application/Ports/IProgressRepository.hpp"

#include <map>

namespace sloptour::tests {

class InMemoryProgressRepository final : public application::IProgressRepository {
public:
    [[nodiscard]] application::StorageResult<domain::TourProgress> load(const domain::TourId& id) const override {
        const auto found = m_progress.find(id);
        if (found == m_progress.end()) {
            return domain::TourProgress(id);
        }
        return found->second;
    }

    [[nodiscard]] application::StorageResult<void> save(const domain::TourProgress& progress) override {
        m_progress.insert_or_assign(progress.tour(), progress);
        return {};
    }

private:
    std::map<domain::TourId, domain::TourProgress> m_progress;
};

}
