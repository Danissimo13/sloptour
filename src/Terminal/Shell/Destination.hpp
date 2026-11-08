#pragma once

#include "Application/Generation/GenerationRequest.hpp"
#include "Domain/Tour/TourId.hpp"
#include "Domain/Tour/TourKind.hpp"

#include <optional>
#include <string>
#include <variant>

namespace sloptour::terminal {

struct GoHome {
    std::optional<std::string> notice;
};

struct GoWizard {
    domain::TourKind kind = domain::TourKind::Onboarding;
};

struct GoGenerate {
    application::GenerationRequest request;
};

struct GoPlay {
    domain::TourId tour;
};

struct GoExit {};

using Destination = std::variant<GoHome, GoWizard, GoGenerate, GoPlay, GoExit>;

}
