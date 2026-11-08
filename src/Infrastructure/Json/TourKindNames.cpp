#include "Infrastructure/Json/TourKindNames.hpp"

#include <array>
#include <utility>

namespace sloptour::infrastructure {

namespace {

constexpr std::array KIND_NAMES{
    std::pair{domain::TourKind::Onboarding, std::string_view("onboarding")},
    std::pair{domain::TourKind::Recap, std::string_view("recap")},
    std::pair{domain::TourKind::Functional, std::string_view("functional")},
};

}

std::string_view name_of(domain::TourKind kind) noexcept {
    for (const auto& [candidate, name] : KIND_NAMES) {
        if (candidate == kind) {
            return name;
        }
    }
    return KIND_NAMES.front().second;
}

std::optional<domain::TourKind> kind_named(std::string_view name) noexcept {
    for (const auto& [kind, candidate] : KIND_NAMES) {
        if (candidate == name) {
            return kind;
        }
    }
    return std::nullopt;
}

}
