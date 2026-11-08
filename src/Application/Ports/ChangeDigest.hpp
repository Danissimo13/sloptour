#pragma once

#include <string>

namespace sloptour::application {

struct ChangeDigest {
    std::string range;
    std::string commits;
    std::string diff_stat;
    std::string working_tree;
};

}
