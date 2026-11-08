#include "Domain/Progress/TourProgress.hpp"
#include "Domain/Quiz/Quiz.hpp"

#include <gtest/gtest.h>

namespace sloptour::tests {

namespace {

[[nodiscard]] domain::QuizQuestion question(std::size_t correct) {
    return domain::QuizQuestion::create("Q", {"a", "b", "c"}, correct, "because").value();
}

}

TEST(QuizTests, QuestionNeedsTwoOptionsAndAnAnswerAmongThem) {
    EXPECT_FALSE(domain::QuizQuestion::create("Q", {"only"}, 0, "").has_value());
    EXPECT_FALSE(domain::QuizQuestion::create("Q", {"a", "b"}, 2, "").has_value());
    EXPECT_FALSE(domain::QuizQuestion::create("Q", {"a", " "}, 0, "").has_value());
}

TEST(QuizTests, QuizNeedsQuestions) {
    EXPECT_FALSE(domain::Quiz::create({}).has_value());
}

TEST(QuizTests, ScoresOnlyCorrectAnswersToExistingQuestions) {
    const auto quiz = domain::Quiz::create({question(0), question(2)}).value();
    EXPECT_EQ(quiz.score({{0, 0}, {1, 1}, {7, 0}}), 1U);
}

TEST(QuizTests, ProgressRemembersAndResetsAnswers) {
    domain::TourProgress progress(domain::TourId::create("demo").value());
    progress.answer(1, 2);
    EXPECT_EQ(progress.quiz_answer(1), std::optional<std::size_t>(2));
    progress.reset_quiz();
    EXPECT_FALSE(progress.quiz_answer(1).has_value());
}

}
