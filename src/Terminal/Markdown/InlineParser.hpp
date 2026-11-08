#pragma once

#include "Terminal/Markdown/InlineSpan.hpp"

#include <string_view>
#include <vector>

namespace sloptour::terminal {

[[nodiscard]] std::vector<InlineSpan> inline_spans(std::string_view text);

}
