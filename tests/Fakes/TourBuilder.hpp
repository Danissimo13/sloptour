#pragma once

#include "Domain/Tour/Tour.hpp"

#include <string>
#include <vector>

namespace sloptour::tests {

[[nodiscard]] inline domain::Step step_named(const std::string& id) {
    return domain::Step::create({.id = domain::StepId::create(id).value(),
                                 .title = "Title of " + id,
                                 .body = "Body of " + id,
                                 .locations = {},
                                 .commands = {},
                                 .checkpoint = std::nullopt})
        .value();
}

[[nodiscard]] inline domain::Chapter chapter_of(const std::string& title, const std::vector<std::string>& ids) {
    std::vector<domain::Step> steps;
    steps.reserve(ids.size());
    for (const auto& id : ids) {
        steps.push_back(step_named(id));
    }
    return domain::Chapter::create(title, "summary", std::move(steps)).value();
}

[[nodiscard]] inline domain::Tour tour_of(std::vector<domain::Chapter> chapters) {
    return domain::Tour::create(domain::TourId::create("demo").value(),
                                {.title = "Demo tour",
                                 .summary = "summary",
                                 .kind = domain::TourKind::Onboarding,
                                 .repository = "demo",
                                 .scope = "whole repository",
                                 .agent = "fake",
                                 .created_at = "2026-11-06 12:00",
                                 .language = "ru"},
                                std::move(chapters))
        .value();
}

[[nodiscard]] inline domain::Tour two_chapter_tour() {
    return tour_of({chapter_of("First", {"a", "b"}), chapter_of("Second", {"c"})});
}

}
