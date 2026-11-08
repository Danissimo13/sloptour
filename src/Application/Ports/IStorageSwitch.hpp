#pragma once

#include "Application/Preferences/StorageMode.hpp"

#include <string>

namespace sloptour::application {

class IStorageSwitch {
public:
    IStorageSwitch() = default;
    virtual ~IStorageSwitch() = default;
    IStorageSwitch(const IStorageSwitch&) = delete;
    IStorageSwitch& operator=(const IStorageSwitch&) = delete;
    IStorageSwitch(IStorageSwitch&&) = delete;
    IStorageSwitch& operator=(IStorageSwitch&&) = delete;

    virtual void use(StorageMode mode) noexcept = 0;
    [[nodiscard]] virtual StorageMode current() const noexcept = 0;
    [[nodiscard]] virtual std::string location(StorageMode mode) const = 0;
};

}
