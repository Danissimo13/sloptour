#include "Infrastructure/Json/StorageModeNames.hpp"

#include <array>
#include <utility>

namespace sloptour::infrastructure {

namespace {

constexpr std::array MODE_NAMES{
    std::pair{application::StorageMode::Repository, std::string_view("repository")},
    std::pair{application::StorageMode::Global, std::string_view("global")},
    std::pair{application::StorageMode::Memory, std::string_view("memory")},
};

}

std::string_view name_of(application::StorageMode mode) noexcept {
    for (const auto& [candidate, name] : MODE_NAMES) {
        if (candidate == mode) {
            return name;
        }
    }
    return MODE_NAMES.front().second;
}

std::optional<application::StorageMode> storage_named(std::string_view name) noexcept {
    for (const auto& [mode, candidate] : MODE_NAMES) {
        if (candidate == name) {
            return mode;
        }
    }
    return std::nullopt;
}

}
