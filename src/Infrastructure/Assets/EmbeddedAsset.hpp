#pragma once

#include <string_view>

namespace sloptour::infrastructure {

struct EmbeddedAsset {
    std::string_view path;
    std::string_view content;
};

}
