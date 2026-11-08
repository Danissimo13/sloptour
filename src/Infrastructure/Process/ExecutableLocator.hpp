#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace sloptour::infrastructure {

[[nodiscard]] std::optional<std::filesystem::path> locate_executable(std::string_view name);
[[nodiscard]] std::vector<std::string> launchable(const std::vector<std::string>& arguments);

}
