#pragma once

#include <chrono>
#include <filesystem>
#include <string>
#include <vector>

namespace sloptour::infrastructure {

struct ProcessCommand {
    std::vector<std::string> arguments;
    std::string input;
    std::filesystem::path working_directory;
    std::chrono::milliseconds timeout{};
};

}
