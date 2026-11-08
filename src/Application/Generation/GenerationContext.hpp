#pragma once

#include "Application/Generation/DepthLevel.hpp"
#include "Application/Generation/DetailLevel.hpp"
#include "Application/Generation/FocusBriefing.hpp"
#include "Application/Ports/PromptVariables.hpp"
#include "Application/Ports/RepositoryOverview.hpp"

namespace sloptour::application {

struct GenerationContext {
    RepositoryOverview overview;
    FocusBriefing briefing;
    DepthLevel depth;
    DetailLevel detail;
    PromptVariables variables;
};

}
