#include "Infrastructure/Time/SystemClock.hpp"

#include "Infrastructure/Platform/Platform.hpp"

#include <array>
#include <ctime>
#include <string_view>

namespace sloptour::infrastructure {

namespace {

constexpr std::string_view TIMESTAMP_FORMAT = "%Y-%m-%d %H:%M";
constexpr std::string_view COMPACT_FORMAT = "%Y%m%d-%H%M%S";
constexpr std::size_t BUFFER_SIZE = 32;

[[nodiscard]] std::string formatted_now(std::string_view format) {
    const auto now = std::time(nullptr);
    const auto local = platform::local_time(now);
    std::array<char, BUFFER_SIZE> buffer{};
    const auto size = std::strftime(buffer.data(), buffer.size(), std::string(format).c_str(), &local);
    return {buffer.data(), size};
}

}

std::string SystemClock::timestamp() const {
    return formatted_now(TIMESTAMP_FORMAT);
}

std::string SystemClock::compact_stamp() const {
    return formatted_now(COMPACT_FORMAT);
}

}
