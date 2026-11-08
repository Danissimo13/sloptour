#pragma once

#include <optional>
#include <string>

namespace sloptour::application {

struct ChangeScope {
    std::optional<std::string> base_revision;
    bool include_working_tree = true;
};

}
