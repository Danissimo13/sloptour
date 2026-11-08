#pragma once

#include "Application/Ports/TokenUsage.hpp"

#include <functional>
#include <string_view>

namespace sloptour::application {

struct AgentListener {
    std::function<void(std::string_view line)> activity;
    std::function<void(const TokenUsage& spent)> usage;

    void report(std::string_view line) const {
        if (activity) {
            activity(line);
        }
    }

    void spend(const TokenUsage& spent) const {
        if (usage) {
            usage(spent);
        }
    }
};

}
