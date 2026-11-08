#pragma once

#include "Application/Generation/TourFocus.hpp"
#include "Application/Ports/AgentChoice.hpp"
#include "Application/Preferences/ReplyLanguage.hpp"

#include <optional>
#include <string>

namespace sloptour::application {

struct GenerationRequest {
    TourFocus focus;
    AgentChoice agent;
    std::string hint;
    std::optional<std::string> name;
    std::string depth;
    std::string detail;
    ReplyLanguage language;
};

}
