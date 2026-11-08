#pragma once

#include "Application/Ports/PortErrors.hpp"
#include "Application/Preferences/Preferences.hpp"

namespace sloptour::application {

class IPreferencesStore {
public:
    IPreferencesStore() = default;
    virtual ~IPreferencesStore() = default;
    IPreferencesStore(const IPreferencesStore&) = delete;
    IPreferencesStore& operator=(const IPreferencesStore&) = delete;
    IPreferencesStore(IPreferencesStore&&) = delete;
    IPreferencesStore& operator=(IPreferencesStore&&) = delete;

    [[nodiscard]] virtual Preferences load(const Preferences& fallback) const = 0;
    [[nodiscard]] virtual StorageResult<void> save(const Preferences& preferences) = 0;
};

}
