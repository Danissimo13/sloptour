#include "tests/Fakes/TourBuilder.hpp"

#include <gtest/gtest.h>

namespace sloptour::tests {

TEST(TourTests, RejectsDuplicateStepIdsAcrossChapters) {
    auto tour = domain::Tour::create(domain::TourId::create("demo").value(), {.title = "Demo"},
                                     {chapter_of("First", {"a"}), chapter_of("Second", {"a"})});
    EXPECT_FALSE(tour.has_value());
}

TEST(TourTests, RejectsTourWithoutChapters) {
    EXPECT_FALSE(domain::Tour::create(domain::TourId::create("demo").value(), {.title = "Demo"}, {}).has_value());
}

TEST(TourTests, NextCrossesIntoTheFollowingChapter) {
    const auto tour = two_chapter_tour();
    EXPECT_EQ(tour.next({.chapter = 0, .step = 1}), std::optional(domain::StepPosition{.chapter = 1, .step = 0}));
}

TEST(TourTests, NextAfterTheLastStepIsEmpty) {
    EXPECT_FALSE(two_chapter_tour().next({.chapter = 1, .step = 0}).has_value());
}

TEST(TourTests, PreviousReturnsToTheLastStepOfThePreviousChapter) {
    EXPECT_EQ(two_chapter_tour().previous({.chapter = 1, .step = 0}),
              std::optional(domain::StepPosition{.chapter = 0, .step = 1}));
}

TEST(TourTests, InsertsStepsRightAfterTheAnchor) {
    auto tour = two_chapter_tour();
    ASSERT_TRUE(tour.insert_steps_after(domain::StepId::create("a").value(), {step_named("a1"), step_named("a2")}));
    const auto& steps = tour.chapters().at(0).steps();
    ASSERT_EQ(steps.size(), 4U);
    EXPECT_EQ(steps.at(1).id().value(), "a1");
    EXPECT_EQ(steps.at(2).id().value(), "a2");
    EXPECT_EQ(steps.at(3).id().value(), "b");
}

TEST(TourTests, RefusesToInsertStepsWithTakenIds) {
    auto tour = two_chapter_tour();
    EXPECT_FALSE(tour.insert_steps_after(domain::StepId::create("a").value(), {step_named("c")}).has_value());
}

TEST(TourTests, RefusesToInsertAfterAnUnknownStep) {
    auto tour = two_chapter_tour();
    EXPECT_FALSE(tour.insert_steps_after(domain::StepId::create("zzz").value(), {step_named("x")}).has_value());
}

TEST(TourTests, OrdinalCountsStepsOfEarlierChapters) {
    EXPECT_EQ(two_chapter_tour().ordinal_of({.chapter = 1, .step = 0}), 2U);
}

}
