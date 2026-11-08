#include "Domain/Outline/TourOutline.hpp"

#include "Core/Text/Text.hpp"

#include <set>

namespace sloptour::domain {

namespace {

[[nodiscard]] TourResult<void> validate(const ChapterPlan& plan, std::set<std::string>& seen) {
    if (core::is_blank(plan.id) || core::is_blank(plan.title)) {
        return std::unexpected(TourError("outline chapter needs an id and a title"));
    }
    if (!seen.insert(plan.id).second) {
        return std::unexpected(TourError("duplicate outline chapter id '" + plan.id + "'"));
    }
    return {};
}

}

TourResult<TourOutline> TourOutline::create(std::string title, std::string summary, std::vector<ChapterPlan> chapters) {
    if (core::is_blank(title)) {
        return std::unexpected(TourError("outline has no title"));
    }
    if (chapters.empty()) {
        return std::unexpected(TourError("outline has no chapters"));
    }
    std::set<std::string> seen;
    for (const auto& plan : chapters) {
        if (auto valid = validate(plan, seen); !valid) {
            return std::unexpected(valid.error());
        }
    }
    return TourOutline(std::move(title), std::move(summary), std::move(chapters));
}

}
