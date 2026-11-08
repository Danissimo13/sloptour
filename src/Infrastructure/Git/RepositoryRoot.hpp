#pragma once

#include "Infrastructure/Process/ProcessRunner.hpp"

#include <filesystem>

namespace sloptour::infrastructure {

[[nodiscard]] std::filesystem::path repository_root(const std::filesystem::path& start, const ProcessRunner& runner);

}
