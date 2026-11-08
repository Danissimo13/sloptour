#include "Domain/Tour/CodeLocation.hpp"
#include "Domain/Tour/LineRange.hpp"
#include "Domain/Tour/Step.hpp"

#include <gtest/gtest.h>

namespace sloptour::tests {

TEST(StepTests, DropsCheckpointWithoutAnswer) {
    const auto step = domain::Step::create({.id = domain::StepId::create("a").value(),
                                            .title = "Title",
                                            .body = "Body",
                                            .locations = {},
                                            .commands = {"make", "  "},
                                            .checkpoint = domain::Checkpoint{.question = "Why?", .answer = ""}});
    ASSERT_TRUE(step.has_value());
    EXPECT_FALSE(step->checkpoint().has_value());
    EXPECT_EQ(step->commands().size(), 1U);
}

TEST(StepTests, RequiresBody) {
    EXPECT_FALSE(domain::Step::create({.id = domain::StepId::create("a").value(),
                                       .title = "Title",
                                       .body = " ",
                                       .locations = {},
                                       .commands = {},
                                       .checkpoint = std::nullopt})
                     .has_value());
}

TEST(StepTests, LineRangeRejectsZeroAndReversedRanges) {
    EXPECT_FALSE(domain::LineRange::create(0, 3).has_value());
    EXPECT_FALSE(domain::LineRange::create(5, 3).has_value());
    EXPECT_TRUE(domain::LineRange::create(3, 3).has_value());
}

TEST(StepTests, CodeLocationStaysInsideTheRepository) {
    EXPECT_FALSE(domain::CodeLocation::create("../secret.txt", std::nullopt, "").has_value());
    EXPECT_FALSE(domain::CodeLocation::create("/etc/passwd", std::nullopt, "").has_value());
}

TEST(StepTests, CodeLocationDropsLeadingCurrentDirectory) {
    EXPECT_EQ(domain::CodeLocation::create("./src/main.cpp", std::nullopt, "").value().path(), "src/main.cpp");
}

}
