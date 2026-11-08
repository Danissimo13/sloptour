#pragma once

#include "Application/Ports/IClock.hpp"

namespace sloptour::infrastructure {

class SystemClock final : public application::IClock {
public:
    [[nodiscard]] std::string timestamp() const override;
    [[nodiscard]] std::string compact_stamp() const override;
};

}
