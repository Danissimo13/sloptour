#pragma once

#include "Application/Generation/FocusBriefing.hpp"
#include "Application/Generation/TourFocus.hpp"
#include "Application/Ports/IRepositoryInspector.hpp"
#include "Application/Ports/PortErrors.hpp"

#include <memory>
#include <utility>

namespace sloptour::application {

class FocusBriefer {
public:
    explicit FocusBriefer(std::shared_ptr<const IRepositoryInspector> inspector)
        : m_inspector(std::move(inspector)) {}

    [[nodiscard]] ApplicationResult<FocusBriefing> brief(const TourFocus& focus) const;
    [[nodiscard]] ApplicationResult<FocusBriefing> operator()(const OnboardingFocus& focus) const;
    [[nodiscard]] ApplicationResult<FocusBriefing> operator()(const RecapFocus& focus) const;
    [[nodiscard]] ApplicationResult<FocusBriefing> operator()(const FunctionalFocus& focus) const;

private:
    std::shared_ptr<const IRepositoryInspector> m_inspector;
};

}
