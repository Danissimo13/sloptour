#pragma once

#include "Domain/Tour/Checkpoint.hpp"
#include "Domain/Tour/CodeLocation.hpp"
#include "Domain/Tour/StepId.hpp"

#include <optional>
#include <string>
#include <vector>

namespace sloptour::domain {

struct StepDraft {
    StepId id;
    std::string title;
    std::string body;
    std::vector<CodeLocation> locations;
    std::vector<std::string> commands;
    std::optional<Checkpoint> checkpoint;
};

}
