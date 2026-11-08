#pragma once

#include <string_view>

namespace sloptour::terminal {

struct KeyHint {
    std::string_view key;
    std::string_view label;
};

}
