#pragma once

#include "Application/Ports/IProgressRepository.hpp"
#include "Infrastructure/Storage/StorageRouter.hpp"

#include <memory>
#include <utility>

namespace sloptour::infrastructure {

class RoutedProgressRepository final : public application::IProgressRepository {
public:
    explicit RoutedProgressRepository(std::shared_ptr<const StorageRouter> router)
        : m_router(std::move(router)) {}

    [[nodiscard]] application::StorageResult<domain::TourProgress> load(const domain::TourId& id) const override {
        return m_router->active().progress->load(id);
    }

    [[nodiscard]] application::StorageResult<void> save(const domain::TourProgress& progress) override {
        return m_router->active().progress->save(progress);
    }

private:
    std::shared_ptr<const StorageRouter> m_router;
};

}
