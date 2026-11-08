#pragma once

#include "Application/Ports/PortErrors.hpp"

#include <cstddef>
#include <string_view>

namespace sloptour::application {

class IEditor {
public:
    IEditor() = default;
    virtual ~IEditor() = default;
    IEditor(const IEditor&) = delete;
    IEditor& operator=(const IEditor&) = delete;
    IEditor(IEditor&&) = delete;
    IEditor& operator=(IEditor&&) = delete;

    [[nodiscard]] virtual StorageResult<void> open(std::string_view path, std::size_t line) const = 0;
};

}
