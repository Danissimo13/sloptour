#pragma once

#include <functional>
#include <map>
#include <string>

namespace sloptour::terminal {

using Catalog = std::map<std::string, std::string, std::less<>>;

struct LanguagePack {
    std::string code;
    std::string label;
    std::string agent_name;
    std::string plural_rule;
    Catalog texts;
};

}
