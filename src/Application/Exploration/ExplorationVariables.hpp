#pragma once

#include "Application/Exploration/ExplorationRequest.hpp"
#include "Application/Ports/PortErrors.hpp"
#include "Application/Ports/PromptVariables.hpp"

namespace sloptour::application {

[[nodiscard]] ApplicationResult<PromptVariables> exploration_variables(const ExplorationRequest& request);

}
