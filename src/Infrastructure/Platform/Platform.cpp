#include "Infrastructure/Platform/Platform.hpp"

#include <algorithm>
#include <cctype>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <system_error>
#include <tuple>

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

namespace sloptour::infrastructure::platform {

namespace {

constexpr std::string_view CONFIG_DIRECTORY = ".config";
constexpr std::string_view APPLICATION = "sloptour";
#ifdef _WIN32
constexpr std::string_view HOME_VARIABLE = "USERPROFILE";
#else
constexpr std::string_view HOME_VARIABLE = "HOME";
#endif

}

std::optional<std::string> environment_variable(std::string_view name) {
    const auto* const value = std::getenv(std::string(name).c_str());
    if (value == nullptr || *value == '\0') {
        return std::nullopt;
    }
    return std::string(value);
}

std::string display_path(const std::filesystem::path& path) {
    auto text = path.string();
    const auto home = environment_variable(HOME_VARIABLE);
    if (!home || !text.starts_with(*home)) {
        return text;
    }
    return "~" + text.substr(home->size());
}

#ifdef _WIN32

namespace {

constexpr std::string_view SCRIPT_SHELL = "cmd.exe";
constexpr std::string_view SCRIPT_SHELL_FLAG = "/c";
constexpr std::string_view PATH_EXTENSIONS = "PATHEXT";
constexpr std::string_view DEFAULT_PATH_EXTENSIONS = ".COM;.EXE;.BAT;.CMD";
constexpr std::string_view APPLICATION_DATA = "APPDATA";
constexpr std::string_view USER_PROFILE = "USERPROFILE";
constexpr std::string_view WINDOWS_EDITOR = "notepad";
constexpr std::string_view LOCAL_APPLICATION_DATA = "LOCALAPPDATA";

[[nodiscard]] std::string lowered(std::string text) {
    std::ranges::transform(text, text.begin(), [](char character) {
        return static_cast<char>(std::tolower(static_cast<unsigned char>(character)));
    });
    return text;
}

[[nodiscard]] std::vector<std::string> split(std::string_view text, char separator) {
    std::vector<std::string> parts;
    std::size_t start = 0;
    while (start <= text.size()) {
        auto end = text.find(separator, start);
        end = end == std::string_view::npos ? text.size() : end;
        if (end > start) {
            parts.emplace_back(text.substr(start, end - start));
        }
        start = end + 1;
    }
    return parts;
}

[[nodiscard]] bool is_script(const std::filesystem::path& executable) {
    const auto extension = lowered(executable.extension().string());
    return extension == ".cmd" || extension == ".bat";
}

}

bool standard_streams_are_terminal() noexcept {
    return _isatty(_fileno(stdin)) != 0 && _isatty(_fileno(stdout)) != 0;
}

void ignore_broken_pipes() noexcept {}

std::tm local_time(std::time_t moment) noexcept {
    std::tm local{};
    localtime_s(&local, &moment);
    return local;
}

char path_list_separator() noexcept {
    return ';';
}

std::vector<std::string> executable_suffixes() {
    auto suffixes = split(environment_variable(PATH_EXTENSIONS).value_or(std::string(DEFAULT_PATH_EXTENSIONS)), ';');
    suffixes.insert(suffixes.begin(), std::string());
    return suffixes;
}

bool is_launchable(const std::filesystem::path& file) {
    std::error_code error;
    return std::filesystem::is_regular_file(file, error);
}

std::vector<std::string> launch_prefix(const std::filesystem::path& executable) {
    if (is_script(executable)) {
        return {std::string(SCRIPT_SHELL), std::string(SCRIPT_SHELL_FLAG)};
    }
    return {};
}

std::optional<std::filesystem::path> user_config_directory() {
    if (const auto roaming = environment_variable(APPLICATION_DATA)) {
        return std::filesystem::path(*roaming) / APPLICATION;
    }
    if (const auto profile = environment_variable(USER_PROFILE)) {
        return std::filesystem::path(*profile) / CONFIG_DIRECTORY / APPLICATION;
    }
    return std::nullopt;
}

std::optional<std::string> default_editor() {
    return std::string(WINDOWS_EDITOR);
}

std::optional<std::filesystem::path> user_data_directory() {
    if (const auto local = environment_variable(LOCAL_APPLICATION_DATA)) {
        return std::filesystem::path(*local) / APPLICATION;
    }
    return user_config_directory();
}

#else

namespace {

constexpr std::string_view XDG_CONFIG_HOME = "XDG_CONFIG_HOME";
constexpr std::string_view XDG_DATA_HOME = "XDG_DATA_HOME";
#ifdef __APPLE__
constexpr std::string_view DATA_HOME = "Library/Application Support";
#else
constexpr std::string_view DATA_HOME = ".local/share";
#endif
constexpr std::string_view HOME = "HOME";

}

bool standard_streams_are_terminal() noexcept {
    return isatty(STDIN_FILENO) != 0 && isatty(STDOUT_FILENO) != 0;
}

void ignore_broken_pipes() noexcept {
    std::ignore = std::signal(SIGPIPE, SIG_IGN);
}

std::tm local_time(std::time_t moment) noexcept {
    std::tm local{};
    localtime_r(&moment, &local);
    return local;
}

char path_list_separator() noexcept {
    return ':';
}

std::vector<std::string> executable_suffixes() {
    return {std::string()};
}

bool is_launchable(const std::filesystem::path& file) {
    std::error_code error;
    const auto status = std::filesystem::status(file, error);
    if (error || !std::filesystem::is_regular_file(status)) {
        return false;
    }
    return (status.permissions() & std::filesystem::perms::owner_exec) != std::filesystem::perms::none;
}

std::vector<std::string> launch_prefix(const std::filesystem::path& /*executable*/) {
    return {};
}

std::optional<std::filesystem::path> user_config_directory() {
    if (const auto configured = environment_variable(XDG_CONFIG_HOME)) {
        return std::filesystem::path(*configured) / APPLICATION;
    }
    if (const auto home = environment_variable(HOME)) {
        return std::filesystem::path(*home) / CONFIG_DIRECTORY / APPLICATION;
    }
    return std::nullopt;
}

std::optional<std::string> default_editor() {
    return std::nullopt;
}

std::optional<std::filesystem::path> user_data_directory() {
    if (const auto configured = environment_variable(XDG_DATA_HOME)) {
        return std::filesystem::path(*configured) / APPLICATION;
    }
    const auto home = environment_variable(HOME);
    if (!home) {
        return std::nullopt;
    }
    return std::filesystem::path(*home) / DATA_HOME / APPLICATION;
}

#endif

}
