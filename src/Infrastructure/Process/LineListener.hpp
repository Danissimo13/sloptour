#pragma once

#include <functional>
#include <string_view>

namespace sloptour::infrastructure {

using LineListener = std::function<void(std::string_view line)>;

}
