#pragma once

#include "Application/Ports/TokenUsage.hpp"

#include <chrono>
#include <deque>
#include <string>

namespace sloptour::terminal {

struct RunningTask {
    std::string label;
    std::chrono::steady_clock::time_point started;
    std::deque<std::string> activity;
    application::TokenUsage usage;
    bool in_chat = false;
    std::string question;
};

}
