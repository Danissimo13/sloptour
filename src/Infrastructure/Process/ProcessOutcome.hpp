#pragma once

#include <string>

namespace sloptour::infrastructure {

struct ProcessOutcome {
    int status = 0;
    std::string output;
    std::string errors;
};

}
