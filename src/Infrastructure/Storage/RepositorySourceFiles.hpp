#pragma once

#include "Application/Ports/ISourceFiles.hpp"

#include <filesystem>
#include <utility>

namespace sloptour::infrastructure {

class RepositorySourceFiles final : public application::ISourceFiles {
public:
    explicit RepositorySourceFiles(std::filesystem::path root)
        : m_root(std::move(root)) {}

    [[nodiscard]] application::StorageResult<std::vector<std::string>> read_lines(std::string_view path) const override;

private:
    std::filesystem::path m_root;
};

}
