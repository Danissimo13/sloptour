#pragma once

#include <chrono>
#include <cstddef>
#include <string>
#include <string_view>
#include <variant>

namespace sloptour::terminal {

struct ChapterQueued {};

struct ChapterWriting {
    std::chrono::steady_clock::time_point started;
};

struct ChapterWritten {
    std::size_t steps = 0;
    std::string_view unit = "step";
    std::chrono::steady_clock::duration took{};
};

struct ChapterBroken {
    std::string reason;
};

using ChapterStatus = std::variant<ChapterQueued, ChapterWriting, ChapterWritten, ChapterBroken>;

}
