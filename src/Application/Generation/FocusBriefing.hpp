#pragma once

#include "Application/Ports/PromptVariables.hpp"
#include "Domain/Tour/TourKind.hpp"

#include <string>
#include <string_view>

namespace sloptour::application {

struct FocusBriefing {
    domain::TourKind kind = domain::TourKind::Onboarding;
    std::string_view label;
    std::string_view outline_prompt;
    std::string_view chapter_prompt;
    std::string scope;
    PromptVariables variables;
};

}
