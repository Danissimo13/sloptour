#pragma once

#include "Application/Ports/IClock.hpp"

namespace sloptour::tests {

class FixedClock final : public application::IClock {
public:
    [[nodiscard]] std::string timestamp() const override {
        return "2026-11-06 12:00";
    }

    [[nodiscard]] std::string compact_stamp() const override {
        return "20261106-120000";
    }
};

}
