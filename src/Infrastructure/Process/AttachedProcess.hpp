#pragma once

#include "Infrastructure/Process/ProcessError.hpp"

#include <filesystem>
#include <string>
#include <vector>

namespace sloptour::infrastructure {

[[nodiscard]] ProcessResult<int> run_attached(const std::vector<std::string>& arguments,
                                              const std::filesystem::path& working_directory);

}
