#pragma once

#include "Application/Ports/PortErrors.hpp"

#include <cstddef>
#include <filesystem>

namespace sloptour::infrastructure {

[[nodiscard]] application::StorageResult<std::size_t> eject_prompts(const std::filesystem::path& directory);

}
