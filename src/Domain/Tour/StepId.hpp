#pragma once

#include "Domain/Tour/TourError.hpp"

#include <compare>
#include <string>
#include <string_view>
#include <utility>

namespace sloptour::domain {

class StepId {
public:
    [[nodiscard]] static TourResult<StepId> create(std::string_view value);

    [[nodiscard]] const std::string& value() const noexcept {
        return m_value;
    }

    [[nodiscard]] auto operator<=>(const StepId&) const = default;

private:
    explicit StepId(std::string value)
        : m_value(std::move(value)) {}

    std::string m_value;
};

}
