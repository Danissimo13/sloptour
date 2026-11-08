#pragma once

#include "Application/Ports/PortErrors.hpp"
#include "Infrastructure/Configuration/AppConfig.hpp"

#include <filesystem>
#include <vector>

namespace sloptour::infrastructure {

[[nodiscard]] application::DecodeResult<AppConfig> load_config(const std::vector<std::filesystem::path>& overrides);
[[nodiscard]] application::DecodeResult<AppConfig> config_from_document(const Json& document);

}
