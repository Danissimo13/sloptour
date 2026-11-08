#include "Core/Errors/Panic.hpp"

#include <cstdlib>
#include <print>

namespace sloptour::core {

void panic(std::string_view message) {
    std::println(stderr, "sloptour: fatal: {}", message);
    std::abort();
}

}
