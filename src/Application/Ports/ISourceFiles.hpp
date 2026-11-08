#pragma once

#include "Application/Ports/PortErrors.hpp"

#include <string>
#include <string_view>
#include <vector>

namespace sloptour::application {

class ISourceFiles {
public:
    ISourceFiles() = default;
    virtual ~ISourceFiles() = default;
    ISourceFiles(const ISourceFiles&) = delete;
    ISourceFiles& operator=(const ISourceFiles&) = delete;
    ISourceFiles(ISourceFiles&&) = delete;
    ISourceFiles& operator=(ISourceFiles&&) = delete;

    [[nodiscard]] virtual StorageResult<std::vector<std::string>> read_lines(std::string_view path) const = 0;
};

}
