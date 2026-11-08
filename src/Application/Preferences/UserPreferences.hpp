#pragma once

#include "Application/Ports/IPreferencesStore.hpp"
#include "Application/Ports/IStorageSwitch.hpp"
#include "Application/Ports/PortErrors.hpp"
#include "Application/Preferences/Preferences.hpp"

#include <memory>
#include <mutex>
#include <string>

namespace sloptour::application {

class UserPreferences {
public:
    struct Dependencies {
        std::shared_ptr<IPreferencesStore> store;
        std::shared_ptr<IStorageSwitch> storage;
        Preferences initial;
        bool persistent = true;
    };

    explicit UserPreferences(Dependencies dependencies);

    [[nodiscard]] Preferences current() const;
    [[nodiscard]] std::string storage_location(StorageMode mode) const;
    [[nodiscard]] ApplicationResult<void> choose_language(std::string language);
    [[nodiscard]] ApplicationResult<void> choose_storage(StorageMode mode);

private:
    [[nodiscard]] ApplicationResult<void> persist(const Preferences& preferences);

    Dependencies m_dependencies;
    mutable std::mutex m_mutex;
    Preferences m_current;
};

}
