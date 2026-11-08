#pragma once

#include "Application/Generation/FunctionalFocus.hpp"
#include "Application/Generation/OnboardingFocus.hpp"
#include "Application/Generation/RecapFocus.hpp"

#include <variant>

namespace sloptour::application {

using TourFocus = std::variant<OnboardingFocus, RecapFocus, FunctionalFocus>;

}
