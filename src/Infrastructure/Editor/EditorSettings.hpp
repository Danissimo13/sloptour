#pragma once

#include <functional>
#include <map>
#include <string>
#include <vector>

namespace sloptour::infrastructure {

struct EditorSettings {
    std::string fallback;
    std::vector<std::string> default_arguments;
    std::map<std::string, std::vector<std::string>, std::less<>> arguments;
};

}
