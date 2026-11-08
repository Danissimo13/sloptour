#pragma once

#include "Infrastructure/Assets/EmbeddedAsset.hpp"

#include <span>

namespace sloptour::infrastructure {

[[nodiscard]] std::span<const EmbeddedAsset> embedded_assets() noexcept;

}
