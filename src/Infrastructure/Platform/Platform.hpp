#pragma once

#include <ctime>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace sloptour::infrastructure::platform {

[[nodiscard]] bool standard_streams_are_terminal() noexcept;
void ignore_broken_pipes() noexcept;
[[nodiscard]] std::tm local_time(std::time_t moment) noexcept;
[[nodiscard]] char path_list_separator() noexcept;
[[nodiscard]] std::vector<std::string> executable_suffixes();
[[nodiscard]] bool is_launchable(const std::filesystem::path& file);
[[nodiscard]] std::vector<std::string> launch_prefix(const std::filesystem::path& executable);
[[nodiscard]] std::optional<std::filesystem::path> user_config_directory();
[[nodiscard]] std::optional<std::filesystem::path> user_data_directory();
[[nodiscard]] std::optional<std::string> default_editor();
[[nodiscard]] std::optional<std::string> environment_variable(std::string_view name);
[[nodiscard]] std::string display_path(const std::filesystem::path& path);

}
