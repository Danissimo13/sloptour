#pragma once

#include "Infrastructure/Process/LineListener.hpp"

namespace sloptour::infrastructure {

struct ProcessListeners {
    LineListener output;
    LineListener errors;
};

}
