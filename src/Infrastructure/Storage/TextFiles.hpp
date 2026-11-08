#pragma once

#include "Application/Ports/PortErrors.hpp"

#include <filesystem>
#include <string>
#include <string_view>

namespace sloptour::infrastructure {

[[nodiscard]] application::StorageResult<std::string> read_text(const std::filesystem::path& path);
[[nodiscard]] application::StorageResult<void> write_text(const std::filesystem::path& path, std::string_view text);
[[nodiscard]] bool is_file(const std::filesystem::path& path) noexcept;

}
