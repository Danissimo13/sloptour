#pragma once

#include "Application/Ports/PortErrors.hpp"
#include "Application/Ports/PromptVariables.hpp"

#include <string>
#include <string_view>

namespace sloptour::infrastructure {

[[nodiscard]] application::PromptResult<std::string> filled(std::string_view text,
                                                            const application::PromptVariables& variables);

}
