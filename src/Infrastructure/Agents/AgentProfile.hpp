#pragma once

#include <chrono>
#include <string>
#include <vector>

namespace sloptour::infrastructure {

struct AgentProfile {
    std::string name;
    std::string description;
    std::vector<std::string> command;
    std::string output;
    std::vector<std::string> models;
    std::vector<std::string> model_arguments;
    std::chrono::seconds timeout{};
};

}
