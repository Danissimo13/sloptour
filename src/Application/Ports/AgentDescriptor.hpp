#pragma once

#include <string>
#include <vector>

namespace sloptour::application {

struct AgentDescriptor {
    std::string name;
    std::string description;
    std::string executable;
    std::vector<std::string> models;
    bool installed = false;
};

}
