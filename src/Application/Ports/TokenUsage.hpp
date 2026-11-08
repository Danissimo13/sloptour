#pragma once

#include <cstdint>

namespace sloptour::application {

struct TokenUsage {
    std::int64_t input = 0;
    std::int64_t output = 0;
    std::int64_t cache_read = 0;
    std::int64_t cache_write = 0;
    double cost_usd = 0.0;
    bool estimated = false;

    [[nodiscard]] std::int64_t prompt_side() const noexcept {
        return input + cache_read + cache_write;
    }

    [[nodiscard]] std::int64_t total() const noexcept {
        return prompt_side() + output;
    }

    TokenUsage& operator+=(const TokenUsage& other) noexcept {
        input += other.input;
        output += other.output;
        cache_read += other.cache_read;
        cache_write += other.cache_write;
        cost_usd += other.cost_usd;
        estimated = estimated || other.estimated;
        return *this;
    }

    [[nodiscard]] TokenUsage minus(const TokenUsage& earlier) const noexcept {
        return {.input = input - earlier.input,
                .output = output - earlier.output,
                .cache_read = cache_read - earlier.cache_read,
                .cache_write = cache_write - earlier.cache_write,
                .cost_usd = cost_usd - earlier.cost_usd,
                .estimated = estimated};
    }
};

}
