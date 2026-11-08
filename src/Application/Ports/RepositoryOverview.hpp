#pragma once

#include <string>

namespace sloptour::application {

struct RepositoryOverview {
    std::string name;
    std::string root;
    bool versioned = false;
    std::string branch;
    std::string head;
};

}
