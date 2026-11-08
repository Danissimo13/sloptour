#pragma once

#include <string>

namespace sloptour::application {

class IClock {
public:
    IClock() = default;
    virtual ~IClock() = default;
    IClock(const IClock&) = delete;
    IClock& operator=(const IClock&) = delete;
    IClock(IClock&&) = delete;
    IClock& operator=(IClock&&) = delete;

    [[nodiscard]] virtual std::string timestamp() const = 0;
    [[nodiscard]] virtual std::string compact_stamp() const = 0;
};

}
