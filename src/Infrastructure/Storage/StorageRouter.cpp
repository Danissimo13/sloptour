#include "Infrastructure/Storage/StorageRouter.hpp"

#include "Core/Errors/Panic.hpp"

namespace sloptour::infrastructure {

StorageRouter::StorageRouter(std::map<application::StorageMode, StorageBackend> backends)
    : m_backends(std::move(backends)) {
    if (m_backends.empty()) {
        core::panic("storage router needs at least one backend");
    }
    m_mode = m_backends.begin()->first;
}

void StorageRouter::use(application::StorageMode mode) noexcept {
    if (m_backends.contains(mode)) {
        m_mode = mode;
    }
}

application::StorageMode StorageRouter::current() const noexcept {
    return m_mode;
}

std::string StorageRouter::location(application::StorageMode mode) const {
    const auto found = m_backends.find(mode);
    return found == m_backends.end() ? std::string() : found->second.location;
}

const StorageBackend& StorageRouter::active() const {
    return m_backends.at(m_mode);
}

}
