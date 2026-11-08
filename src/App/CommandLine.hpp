#pragma once

#include "App/Invocation.hpp"

#include <variant>

namespace sloptour::app {

struct ExitRequested {
    int code = 0;
};

using ParsedCommandLine = std::variant<Invocation, ExitRequested>;

[[nodiscard]] ParsedCommandLine parse_command_line(int argc, const char* const* argv);

}
