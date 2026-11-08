#pragma once

#include "Application/Ports/PortErrors.hpp"
#include "Application/Preferences/StorageMode.hpp"
#include "Terminal/Settings/TerminalSettings.hpp"
#include "Terminal/Shell/TerminalServices.hpp"

#include <filesystem>
#include <optional>
#include <string>

namespace sloptour::app {

struct Composition {
    std::filesystem::path root;
    std::filesystem::path storage;
    terminal::TerminalServices services;
    terminal::TerminalSettings terminal;
};

struct CompositionOverrides {
    std::optional<std::string> language;
    std::optional<application::StorageMode> storage;
};

[[nodiscard]] application::DecodeResult<Composition> compose(const std::filesystem::path& start,
                                                             const CompositionOverrides& overrides);

}
