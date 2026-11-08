#pragma once

#include "Application/Ports/PortErrors.hpp"
#include "Domain/Tour/Step.hpp"
#include "Domain/Tour/StepId.hpp"
#include "Domain/Tour/Tour.hpp"

#include <functional>
#include <set>
#include <string>
#include <string_view>
#include <vector>

namespace sloptour::application {

class StepIdRegistry {
public:
    void reserve(const domain::Tour& tour);
    [[nodiscard]] ApplicationResult<std::vector<domain::Step>> claim(const std::vector<domain::Step>& steps,
                                                                     std::string_view prefix);

private:
    [[nodiscard]] ApplicationResult<domain::Step> claim_one(const domain::Step& step, std::string_view prefix);
    [[nodiscard]] std::string available(const std::string& base) const;

    std::set<std::string, std::less<>> m_taken;
};

}
