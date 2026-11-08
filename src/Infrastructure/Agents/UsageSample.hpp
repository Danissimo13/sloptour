#pragma once

#include "Application/Ports/TokenUsage.hpp"

#include <string>

namespace sloptour::infrastructure {

struct UsageSample {
    std::string key;
    application::TokenUsage usage;
    bool authoritative = false;
};

}
