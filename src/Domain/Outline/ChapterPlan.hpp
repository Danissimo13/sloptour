#pragma once

#include <string>
#include <vector>

namespace sloptour::domain {

struct ChapterPlan {
    std::string id;
    std::string title;
    std::string goal;
    std::vector<std::string> focus_paths;
};

}
