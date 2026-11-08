#include "Domain/Tour/CodeLocation.hpp"

#include "Core/Text/Text.hpp"

#include <filesystem>

namespace sloptour::domain {

namespace {

constexpr std::string_view PARENT_DIRECTORY = "..";
constexpr std::string_view CURRENT_DIRECTORY_PREFIX = "./";

[[nodiscard]] bool escapes_repository(const std::filesystem::path& path) {
    for (const auto& part : path) {
        if (part == PARENT_DIRECTORY) {
            return true;
        }
    }
    return path.is_absolute();
}

[[nodiscard]] std::string normalized(std::string_view path) {
    auto trimmed = core::trim(path);
    while (trimmed.starts_with(CURRENT_DIRECTORY_PREFIX)) {
        trimmed.remove_prefix(CURRENT_DIRECTORY_PREFIX.size());
    }
    return std::string(trimmed);
}

}

TourResult<CodeLocation> CodeLocation::create(std::string_view path, std::optional<LineRange> lines, std::string note) {
    auto relative = normalized(path);
    if (relative.empty()) {
        return std::unexpected(TourError("code location path is empty"));
    }
    if (escapes_repository(relative)) {
        return std::unexpected(TourError("code location must stay inside the repository: " + relative));
    }
    return CodeLocation(std::move(relative), lines, std::move(note));
}

}
