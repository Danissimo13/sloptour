#pragma once

#include <string>
#include <vector>

namespace sloptour::terminal {

struct LanguageSyntax {
    std::vector<std::string> extensions;
    std::vector<std::string> aliases;
    std::string line_comment;
    std::vector<std::string> keywords;
};

}
