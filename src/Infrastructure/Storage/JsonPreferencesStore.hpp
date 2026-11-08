#pragma once

#include "Application/Ports/IPreferencesStore.hpp"

#include <filesystem>
#include <utility>

namespace sloptour::infrastructure {

class JsonPreferencesStore final : public application::IPreferencesStore {
public:
    explicit JsonPreferencesStore(std::filesystem::path file)
        : m_file(std::move(file)) {}

    [[nodiscard]] application::Preferences load(const application::Preferences& fallback) const override;
    [[nodiscard]] application::StorageResult<void> save(const application::Preferences& preferences) override;

private:
    std::filesystem::path m_file;
};

}
