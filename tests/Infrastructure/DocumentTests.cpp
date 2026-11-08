#include "Infrastructure/Json/ProgressDocument.hpp"
#include "Infrastructure/Json/TourDocument.hpp"
#include "Infrastructure/Json/TourKindNames.hpp"
#include "tests/Fakes/TourBuilder.hpp"

#include <gtest/gtest.h>

namespace sloptour::tests {

TEST(DocumentTests, TourSurvivesARoundTrip) {
    const auto tour = two_chapter_tour();
    const auto restored = infrastructure::tour_from_document(infrastructure::tour_document(tour));
    ASSERT_TRUE(restored.has_value()) << restored.error().message();
    EXPECT_EQ(restored->step_count(), tour.step_count());
    EXPECT_EQ(restored->metadata().scope, tour.metadata().scope);
    EXPECT_EQ(restored->chapters().at(1).steps().at(0).id().value(), "c");
}

TEST(DocumentTests, QuizAndLevelsSurviveARoundTrip) {
    auto tour = two_chapter_tour();
    tour.attach_quiz(domain::Quiz::create({domain::QuizQuestion::create("Q", {"a", "b"}, 1, "why").value()}).value());
    const auto restored = infrastructure::tour_from_document(infrastructure::tour_document(tour)).value();
    EXPECT_EQ(restored.quiz().transform([](const domain::Quiz& quiz) { return quiz.questions().front().correct(); }),
              std::optional<std::size_t>(1));
    EXPECT_EQ(restored.metadata().depth, tour.metadata().depth);
    EXPECT_EQ(restored.metadata().language, tour.metadata().language);
}

TEST(DocumentTests, EveryTourKindKeepsItsName) {
    for (const auto kind : {domain::TourKind::Onboarding, domain::TourKind::Recap, domain::TourKind::Functional}) {
        EXPECT_EQ(infrastructure::kind_named(infrastructure::name_of(kind)), std::optional(kind));
    }
}

TEST(DocumentTests, ProgressSurvivesARoundTrip) {
    domain::TourProgress progress(domain::TourId::create("demo").value());
    const auto step = domain::StepId::create("a").value();
    progress.visit(step);
    progress.reveal(step);
    progress.record(step, {.question = "Why?", .answer = "Because."});
    progress.answer(2, 1);
    const auto restored =
        infrastructure::progress_from_document(progress.tour(), infrastructure::progress_document(progress));
    EXPECT_TRUE(restored.is_visited(step));
    EXPECT_TRUE(restored.is_revealed(step));
    EXPECT_EQ(restored.exchanges(step).front().answer, "Because.");
    EXPECT_EQ(restored.current(), step);
    EXPECT_EQ(restored.quiz_answer(2), std::optional<std::size_t>(1));
}

}
