#include "Domain/Outline/TourOutline.hpp"
#include "Domain/Progress/TourProgress.hpp"
#include "tests/Fakes/TourBuilder.hpp"

#include <gtest/gtest.h>

namespace sloptour::tests {

TEST(OutlineTests, RejectsDuplicateChapterIds) {
    const auto outline =
        domain::TourOutline::create("Tour", "summary",
                                    {{.id = "intro", .title = "Intro", .goal = "", .focus_paths = {}},
                                     {.id = "intro", .title = "Again", .goal = "", .focus_paths = {}}});
    EXPECT_FALSE(outline.has_value());
}

TEST(ProgressTests, CountsOnlyVisitedStepsThatStillExist) {
    const auto tour = two_chapter_tour();
    domain::TourProgress progress(tour.id());
    progress.visit(domain::StepId::create("a").value());
    progress.visit(domain::StepId::create("removed").value());
    EXPECT_EQ(progress.visited_count_in(tour), 1U);
}

TEST(ProgressTests, VisitMovesTheCurrentStep) {
    domain::TourProgress progress(domain::TourId::create("demo").value());
    progress.visit(domain::StepId::create("a").value());
    progress.visit(domain::StepId::create("b").value());
    EXPECT_EQ(progress.current(), domain::StepId::create("b").value());
}

}
