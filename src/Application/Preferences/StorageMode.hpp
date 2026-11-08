#pragma once

#include <cstdint>

namespace sloptour::application {

enum class StorageMode : std::uint8_t {
    Repository,
    Global,
    Memory,
};

}
