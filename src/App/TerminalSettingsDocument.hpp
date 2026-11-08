#pragma once

#include "Application/Ports/PortErrors.hpp"
#include "Infrastructure/Json/JsonAccess.hpp"
#include "Terminal/Settings/TerminalSettings.hpp"

#include <cstddef>

namespace sloptour::app {

[[nodiscard]] application::DecodeResult<terminal::TerminalSettings>
terminal_settings(const infrastructure::Json& document, std::size_t recent_commits);

}
