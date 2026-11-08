#pragma once

#include <cstddef>
#include <string>

namespace sloptour::application {

struct DepthLevel {
    std::string name;
    std::size_t min_chapters = 1;
    std::size_t max_chapters = 1;
    std::size_t max_steps = 1;
    std::size_t deepen_steps = 1;
    std::size_t quiz_questions = 0;
    std::string guidance;
};

}
