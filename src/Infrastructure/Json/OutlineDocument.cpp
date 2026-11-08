#include "Infrastructure/Json/OutlineDocument.hpp"

#include "Core/Text/Text.hpp"
#include "Infrastructure/Json/DocumentKeys.hpp"

namespace sloptour::infrastructure {

namespace {

[[nodiscard]] domain::ChapterPlan plan_of(const Json& document) {
    auto title = optional_string(document, keys::TITLE);
    auto id = core::slugify(optional_string(document, keys::ID));
    return {.id = id.empty() ? core::slugify(title) : std::move(id),
            .title = std::move(title),
            .goal = optional_string(document, keys::GOAL),
            .focus_paths = string_list(document, keys::FOCUS_PATHS)};
}

}

application::DecodeResult<domain::TourOutline> outline_from_document(const Json& document) {
    std::vector<domain::ChapterPlan> plans;
    for (const auto& item : object_list(document, keys::CHAPTERS)) {
        plans.push_back(plan_of(item));
    }
    auto outline = domain::TourOutline::create(optional_string(document, keys::TITLE),
                                               optional_string(document, keys::SUMMARY), std::move(plans));
    if (!outline) {
        return std::unexpected(application::DecodeError(outline.error()));
    }
    return std::move(*outline);
}

}
