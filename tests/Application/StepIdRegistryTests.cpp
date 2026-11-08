#include "Application/Generation/StepIdRegistry.hpp"
#include "tests/Fakes/TourBuilder.hpp"

#include <gtest/gtest.h>

namespace sloptour::tests {

TEST(StepIdRegistryTests, PrefixesIdsWithTheChapter) {
    application::StepIdRegistry registry;
    const auto steps = registry.claim({step_named("Overview")}, "intro").value();
    EXPECT_EQ(steps.front().id().value(), "intro/overview");
}

TEST(StepIdRegistryTests, KeepsAnExistingPrefix) {
    application::StepIdRegistry registry;
    const auto steps = registry.claim({step_named("intro/overview")}, "intro").value();
    EXPECT_EQ(steps.front().id().value(), "intro/overview");
}

TEST(StepIdRegistryTests, SuffixesTakenIds) {
    application::StepIdRegistry registry;
    const auto steps = registry.claim({step_named("same"), step_named("same")}, "intro").value();
    EXPECT_EQ(steps.at(0).id().value(), "intro/same");
    EXPECT_EQ(steps.at(1).id().value(), "intro/same-2");
}

TEST(StepIdRegistryTests, AvoidsIdsAlreadyInTheTour) {
    application::StepIdRegistry registry;
    registry.reserve(tour_of({chapter_of("First", {"a/b"})}));
    const auto steps = registry.claim({step_named("b")}, "a").value();
    EXPECT_EQ(steps.front().id().value(), "a/b-2");
}

}
