#include "Infrastructure/Storage/TextFiles.hpp"

#include <fstream>
#include <iterator>
#include <system_error>

namespace sloptour::infrastructure {

namespace {

constexpr std::string_view TEMPORARY_SUFFIX = ".tmp";

[[nodiscard]] application::StorageError failure(std::string_view action, const std::filesystem::path& path,
                                                const std::error_code& error) {
    return application::StorageError("cannot " + std::string(action) + " " + path.string() + ": " + error.message());
}

}

application::StorageResult<std::string> read_text(const std::filesystem::path& path) {
    std::ifstream stream(path, std::ios::binary);
    if (!stream) {
        return std::unexpected(application::StorageError("cannot read " + path.string()));
    }
    return std::string(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>());
}

application::StorageResult<void> write_text(const std::filesystem::path& path, std::string_view text) {
    std::error_code error;
    std::filesystem::create_directories(path.parent_path(), error);
    if (error) {
        return std::unexpected(failure("create directory for", path, error));
    }
    auto temporary = path;
    temporary += TEMPORARY_SUFFIX;
    {
        std::ofstream stream(temporary, std::ios::binary | std::ios::trunc);
        stream << text;
        if (!stream) {
            return std::unexpected(application::StorageError("cannot write " + temporary.string()));
        }
    }
    std::filesystem::rename(temporary, path, error);
    if (error) {
        return std::unexpected(failure("replace", path, error));
    }
    return {};
}

bool is_file(const std::filesystem::path& path) noexcept {
    std::error_code error;
    return std::filesystem::is_regular_file(path, error);
}

}
