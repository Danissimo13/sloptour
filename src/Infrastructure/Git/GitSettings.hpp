#pragma once

#include <cstddef>

namespace sloptour::infrastructure {

struct GitSettings {
    std::size_t recent_commits = 0;
    std::size_t max_digest_characters = 0;
};

}
