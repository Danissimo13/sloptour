#pragma once

#include "Application/Ports/ITourRepository.hpp"
#include "Infrastructure/Storage/StorageRouter.hpp"

#include <memory>
#include <utility>

namespace sloptour::infrastructure {

class RoutedTourRepository final : public application::ITourRepository {
public:
    explicit RoutedTourRepository(std::shared_ptr<const StorageRouter> router)
        : m_router(std::move(router)) {}

    [[nodiscard]] application::StorageResult<void> save(const domain::Tour& tour) override {
        return m_router->active().tours->save(tour);
    }

    [[nodiscard]] application::StorageResult<domain::Tour> load(const domain::TourId& id) const override {
        return m_router->active().tours->load(id);
    }

    [[nodiscard]] application::StorageResult<std::vector<domain::TourId>> list() const override {
        return m_router->active().tours->list();
    }

private:
    std::shared_ptr<const StorageRouter> m_router;
};

}
