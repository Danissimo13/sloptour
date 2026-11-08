#pragma once

#include "Application/Ports/IStorageSwitch.hpp"
#include "Infrastructure/Storage/StorageBackend.hpp"

#include <atomic>
#include <map>

namespace sloptour::infrastructure {

class StorageRouter final : public application::IStorageSwitch {
public:
    explicit StorageRouter(std::map<application::StorageMode, StorageBackend> backends);

    void use(application::StorageMode mode) noexcept override;
    [[nodiscard]] application::StorageMode current() const noexcept override;
    [[nodiscard]] std::string location(application::StorageMode mode) const override;
    [[nodiscard]] const StorageBackend& active() const;

private:
    std::map<application::StorageMode, StorageBackend> m_backends;
    std::atomic<application::StorageMode> m_mode{application::StorageMode::Repository};
};

}
