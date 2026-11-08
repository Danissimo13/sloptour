#include "Application/Preferences/UserPreferences.hpp"

#include <utility>

namespace sloptour::application {

UserPreferences::UserPreferences(Dependencies dependencies)
    : m_dependencies(std::move(dependencies))
    , m_current(m_dependencies.initial) {
    m_dependencies.storage->use(m_current.storage);
}

Preferences UserPreferences::current() const {
    const std::scoped_lock lock(m_mutex);
    return m_current;
}

std::string UserPreferences::storage_location(StorageMode mode) const {
    return m_dependencies.storage->location(mode);
}

ApplicationResult<void> UserPreferences::choose_language(std::string language) {
    Preferences updated;
    {
        const std::scoped_lock lock(m_mutex);
        m_current.language = std::move(language);
        updated = m_current;
    }
    return persist(updated);
}

ApplicationResult<void> UserPreferences::choose_storage(StorageMode mode) {
    Preferences updated;
    {
        const std::scoped_lock lock(m_mutex);
        m_current.storage = mode;
        updated = m_current;
    }
    m_dependencies.storage->use(mode);
    return persist(updated);
}

ApplicationResult<void> UserPreferences::persist(const Preferences& preferences) {
    if (!m_dependencies.persistent) {
        return {};
    }
    return lifted(m_dependencies.store->save(preferences));
}

}
