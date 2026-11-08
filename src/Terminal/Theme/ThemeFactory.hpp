#pragma once

#include "Terminal/Theme/Theme.hpp"
#include "Terminal/Theme/ThemePalette.hpp"

namespace sloptour::terminal {

[[nodiscard]] Theme theme_from(const ThemePalette& palette);

}
