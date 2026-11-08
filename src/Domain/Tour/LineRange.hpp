#pragma once

#include "Domain/Tour/TourError.hpp"

#include <compare>
#include <cstddef>

namespace sloptour::domain {

class LineRange {
public:
    [[nodiscard]] static TourResult<LineRange> create(std::size_t first, std::size_t last);

    [[nodiscard]] std::size_t first() const noexcept {
        return m_first;
    }

    [[nodiscard]] std::size_t last() const noexcept {
        return m_last;
    }

    [[nodiscard]] bool contains(std::size_t line) const noexcept {
        return line >= m_first && line <= m_last;
    }

    [[nodiscard]] auto operator<=>(const LineRange&) const = default;

private:
    LineRange(std::size_t first, std::size_t last)
        : m_first(first)
        , m_last(last) {}

    std::size_t m_first;
    std::size_t m_last;
};

}
