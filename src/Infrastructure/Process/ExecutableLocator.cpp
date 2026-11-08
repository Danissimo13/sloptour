#include "Infrastructure/Process/ExecutableLocator.hpp"

#include "Infrastructure/Platform/Platform.hpp"

namespace sloptour::infrastructure {

namespace {

constexpr std::string_view PATH_VARIABLE = "PATH";

[[nodiscard]] std::optional<std::filesystem::path> with_suffix(const std::filesystem::path& candidate) {
    for (const auto& suffix : platform::executable_suffixes()) {
        auto file = candidate;
        file += suffix;
        if (platform::is_launchable(file)) {
            return file;
        }
    }
    return std::nullopt;
}

[[nodiscard]] bool has_directory(std::string_view name) {
    return std::filesystem::path(name).has_parent_path();
}

}

std::optional<std::filesystem::path> locate_executable(std::string_view name) {
    if (name.empty()) {
        return std::nullopt;
    }
    if (has_directory(name)) {
        return with_suffix(name);
    }
    const auto directories = platform::environment_variable(PATH_VARIABLE).value_or("");
    const auto separator = platform::path_list_separator();
    std::size_t start = 0;
    while (start <= directories.size()) {
        auto end = directories.find(separator, start);
        end = end == std::string::npos ? directories.size() : end;
        if (end > start) {
            if (auto found = with_suffix(std::filesystem::path(directories.substr(start, end - start)) / name)) {
                return found;
            }
        }
        start = end + 1;
    }
    return std::nullopt;
}

std::vector<std::string> launchable(const std::vector<std::string>& arguments) {
    if (arguments.empty()) {
        return arguments;
    }
    const auto executable = locate_executable(arguments.front());
    if (!executable) {
        return arguments;
    }
    auto resolved = platform::launch_prefix(*executable);
    resolved.push_back(executable->string());
    resolved.insert(resolved.end(), std::next(arguments.begin()), arguments.end());
    return resolved;
}

}
