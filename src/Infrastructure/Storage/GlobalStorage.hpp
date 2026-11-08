#pragma once

#include <filesystem>
#include <optional>

namespace sloptour::infrastructure {

[[nodiscard]] std::optional<std::filesystem::path> global_storage_directory(const std::filesystem::path& repository);

}
