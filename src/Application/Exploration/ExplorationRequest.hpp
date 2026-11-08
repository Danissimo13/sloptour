#pragma once

#include "Application/Ports/AgentChoice.hpp"
#include "Application/Preferences/ReplyLanguage.hpp"
#include "Domain/Progress/Exchange.hpp"
#include "Domain/Tour/StepId.hpp"
#include "Domain/Tour/Tour.hpp"

#include <string>
#include <vector>

namespace sloptour::application {

struct ExplorationRequest {
    domain::Tour tour;
    domain::StepId step;
    AgentChoice agent;
    std::string question;
    std::vector<domain::Exchange> history;
    ReplyLanguage language;
};

}
