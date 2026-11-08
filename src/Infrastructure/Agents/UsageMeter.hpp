#pragma once

#include "Application/Ports/TokenUsage.hpp"
#include "Infrastructure/Agents/UsageSample.hpp"

#include <functional>
#include <map>
#include <string>

namespace sloptour::infrastructure {

class UsageMeter {
public:
    [[nodiscard]] application::TokenUsage record(const UsageSample& sample);

    [[nodiscard]] bool empty() const noexcept {
        return m_latest.empty();
    }

private:
    std::map<std::string, application::TokenUsage, std::less<>> m_latest;
    application::TokenUsage m_total;
};

}
