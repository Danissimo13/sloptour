#pragma once

#include "Application/Ports/IProgressRepository.hpp"

#include <filesystem>
#include <utility>

namespace sloptour::infrastructure {

class JsonProgressRepository final : public application::IProgressRepository {
public:
    explicit JsonProgressRepository(std::filesystem::path directory)
        : m_directory(std::move(directory)) {}

    [[nodiscard]] application::StorageResult<domain::TourProgress> load(const domain::TourId& id) const override;
    [[nodiscard]] application::StorageResult<void> save(const domain::TourProgress& progress) override;

private:
    [[nodiscard]] std::filesystem::path path_of(const domain::TourId& id) const;

    std::filesystem::path m_directory;
};

}
