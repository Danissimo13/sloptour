#include "Infrastructure/Storage/RepositorySourceFiles.hpp"

#include "Core/Text/Text.hpp"
#include "Infrastructure/Storage/TextFiles.hpp"

#include <system_error>

namespace sloptour::infrastructure {

namespace {

constexpr std::uintmax_t MAX_SOURCE_BYTES = std::uintmax_t{4} * 1024 * 1024;

[[nodiscard]] bool is_inside(const std::filesystem::path& root, const std::filesystem::path& candidate) {
    const auto relative = candidate.lexically_relative(root);
    return !relative.empty() && *relative.begin() != "..";
}

}

application::StorageResult<std::vector<std::string>> RepositorySourceFiles::read_lines(std::string_view path) const {
    std::error_code error;
    const auto root = std::filesystem::weakly_canonical(m_root, error);
    const auto file = std::filesystem::weakly_canonical(m_root / path, error);
    if (error || !is_inside(root, file)) {
        return std::unexpected(application::StorageError("file is outside of the repository: " + std::string(path)));
    }
    if (!is_file(file)) {
        return std::unexpected(application::StorageError("file not found: " + std::string(path)));
    }
    if (std::filesystem::file_size(file, error) > MAX_SOURCE_BYTES) {
        return std::unexpected(application::StorageError("file is too large to show: " + std::string(path)));
    }
    auto text = read_text(file);
    if (!text) {
        return std::unexpected(text.error());
    }
    std::vector<std::string> lines;
    for (const auto line : core::split_lines(*text)) {
        lines.emplace_back(line);
    }
    if (!lines.empty() && lines.back().empty()) {
        lines.pop_back();
    }
    return lines;
}

}
