#pragma once

#include "Domain/Progress/Exchange.hpp"
#include "Domain/Quiz/Quiz.hpp"
#include "Domain/Tour/StepId.hpp"

#include <map>
#include <optional>
#include <set>
#include <vector>

namespace sloptour::domain {

struct ProgressSnapshot {
    std::optional<StepId> current;
    std::set<StepId> visited;
    std::set<StepId> revealed;
    std::map<StepId, std::vector<Exchange>> exchanges;
    QuizAnswers quiz_answers;
};

}
