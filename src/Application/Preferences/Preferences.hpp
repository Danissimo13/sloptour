#pragma once

#include "Application/Preferences/StorageMode.hpp"

#include <string>

namespace sloptour::application {

struct Preferences {
    std::string language;
    StorageMode storage = StorageMode::Repository;
};

}
