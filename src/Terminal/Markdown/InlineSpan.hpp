#pragma once

#include <string>

namespace sloptour::terminal {

struct InlineSpan {
    std::string text;
    bool strong = false;
    bool emphasis = false;
    bool code = false;
};

}
