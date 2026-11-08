#pragma once

#include "Core/Errors/Error.hpp"

#include <expected>
#include <string_view>
#include <utility>

namespace sloptour::core {

[[noreturn]] void panic(std::string_view message);

template <typename TValue, typename TContext>
[[nodiscard]] TValue must(std::expected<TValue, Error<TContext>> result) {
    if (!result.has_value()) {
        panic(result.error().message());
    }
    return std::move(*result);
}

template <typename TContext>
void must(const std::expected<void, Error<TContext>>& result) {
    if (!result.has_value()) {
        panic(result.error().message());
    }
}

}
