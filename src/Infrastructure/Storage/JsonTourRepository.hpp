#pragma once

#include "Application/Ports/ITourRepository.hpp"

#include <filesystem>
#include <utility>

namespace sloptour::infrastructure {

class JsonTourRepository final : public application::ITourRepository {
public:
    explicit JsonTourRepository(std::filesystem::path directory)
        : m_directory(std::move(directory)) {}

    [[nodiscard]] application::StorageResult<void> save(const domain::Tour& tour) override;
    [[nodiscard]] application::StorageResult<domain::Tour> load(const domain::TourId& id) const override;
    [[nodiscard]] application::StorageResult<std::vector<domain::TourId>> list() const override;

private:
    [[nodiscard]] std::filesystem::path path_of(const domain::TourId& id) const;

    std::filesystem::path m_directory;
};

}
