#include "Infrastructure/Storage/GlobalStorage.hpp"

#include "Core/Text/Text.hpp"
#include "Infrastructure/Platform/Platform.hpp"

#include <cstdint>
#include <format>
#include <system_error>

namespace sloptour::infrastructure {

namespace {

constexpr std::string_view REPOSITORIES = "repositories";
constexpr std::uint32_t FNV_OFFSET = 2166136261U;
constexpr std::uint32_t FNV_PRIME = 16777619U;

[[nodiscard]] std::uint32_t fingerprint(std::string_view text) {
    std::uint32_t hash = FNV_OFFSET;
    for (const char character : text) {
        hash ^= static_cast<std::uint8_t>(character);
        hash *= FNV_PRIME;
    }
    return hash;
}

}

std::optional<std::filesystem::path> global_storage_directory(const std::filesystem::path& repository) {
    const auto data = platform::user_data_directory();
    if (!data) {
        return std::nullopt;
    }
    std::error_code error;
    const auto canonical = std::filesystem::weakly_canonical(repository, error);
    const auto identity = (error ? repository : canonical).generic_string();
    const auto folder = std::format("{}-{:08x}", core::slugify(repository.filename().string()), fingerprint(identity));
    return *data / REPOSITORIES / folder;
}

}
