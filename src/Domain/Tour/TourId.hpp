#pragma once

#include "Domain/Tour/TourError.hpp"

#include <compare>
#include <string>
#include <utility>

namespace sloptour::domain {

class TourId {
public:
    [[nodiscard]] static TourResult<TourId> create(std::string value);

    [[nodiscard]] const std::string& value() const noexcept {
        return m_value;
    }

    [[nodiscard]] auto operator<=>(const TourId&) const = default;

private:
    explicit TourId(std::string value)
        : m_value(std::move(value)) {}

    std::string m_value;
};

}
