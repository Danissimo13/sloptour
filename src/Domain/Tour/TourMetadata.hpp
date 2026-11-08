#pragma once

#include "Domain/Tour/TourKind.hpp"

#include <string>

namespace sloptour::domain {

struct TourMetadata {
    std::string title;
    std::string summary;
    TourKind kind = TourKind::Onboarding;
    std::string repository;
    std::string scope;
    std::string agent;
    std::string model;
    std::string depth;
    std::string detail;
    std::string language;
    std::string created_at;
};

}
