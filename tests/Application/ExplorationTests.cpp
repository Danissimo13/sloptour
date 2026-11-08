#include "Application/Exploration/AskAboutStep.hpp"
#include "Application/Exploration/DeepenStep.hpp"
#include "Application/Library/TourSession.hpp"
#include "Infrastructure/Agents/JsonReplyDecoder.hpp"
#include "tests/Fakes/FakeAgent.hpp"
#include "tests/Fakes/FakeAgentProvider.hpp"
#include "tests/Fakes/FakePromptLibrary.hpp"
#include "tests/Fakes/GenerationSettingsBuilder.hpp"
#include "tests/Fakes/InMemoryProgressRepository.hpp"
#include "tests/Fakes/InMemoryTourRepository.hpp"
#include "tests/Fakes/TourBuilder.hpp"

#include <gtest/gtest.h>

namespace sloptour::tests {

namespace {

[[nodiscard]] std::shared_ptr<FakeAgentProvider> answering(std::string reply) {
    return std::make_shared<FakeAgentProvider>(std::make_shared<FakeAgent>(
        [reply = std::move(reply)](const application::AgentTask& /*task*/) { return reply; }));
}

[[nodiscard]] application::ExplorationRequest about_a(std::string question) {
    return {.tour = two_chapter_tour(),
            .step = domain::StepId::create("a").value(),
            .agent = {.name = "fake", .model = ""},
            .question = std::move(question),
            .history = {}};
}

}

TEST(ExplorationTests, AnswersAQuestionWithTheTrimmedReply) {
    const application::AskAboutStep asker(answering("  It parses the config.  \n"),
                                          std::make_shared<FakePromptLibrary>());
    const auto exchange = asker.execute(about_a("What does it do?"), application::AgentListener{}, std::stop_token{});
    ASSERT_TRUE(exchange.has_value());
    EXPECT_EQ(exchange->answer, "It parses the config.");
}

TEST(ExplorationTests, RefusesBlankQuestions) {
    const application::AskAboutStep asker(answering("x"), std::make_shared<FakePromptLibrary>());
    EXPECT_FALSE(asker.execute(about_a("  "), application::AgentListener{}, std::stop_token{}).has_value());
}

TEST(ExplorationTests, DeeperStepsAreNamedAfterTheirParentAndInsertable) {
    const application::DeepenStep deepener(
        {.agents = answering(R"({"steps": [{"id": "b", "title": "Edge cases", "body": "More."}]})"),
         .prompts = std::make_shared<FakePromptLibrary>(),
         .decoder = std::make_shared<infrastructure::JsonReplyDecoder>(),
         .settings = test_settings()});
    auto steps = deepener.execute(about_a(""), application::AgentListener{}, std::stop_token{});
    ASSERT_TRUE(steps.has_value());
    EXPECT_EQ(steps->front().id().value(), "a/b");
    application::TourSession session({.tours = std::make_shared<InMemoryTourRepository>(),
                                      .progress = std::make_shared<InMemoryProgressRepository>()},
                                     two_chapter_tour(), domain::TourProgress(domain::TourId::create("demo").value()));
    EXPECT_TRUE(session.insert_after(domain::StepId::create("a").value(), std::move(*steps)).has_value());
    EXPECT_EQ(session.tour().chapters().at(0).steps().at(1).id().value(), "a/b");
}

TEST(ExplorationTests, SessionAttachesAQuizAndScoresAnswers) {
    application::TourSession session({.tours = std::make_shared<InMemoryTourRepository>(),
                                      .progress = std::make_shared<InMemoryProgressRepository>()},
                                     two_chapter_tour(), domain::TourProgress(domain::TourId::create("demo").value()));
    auto question = domain::QuizQuestion::create("Where?", {"here", "there"}, 1, "There.").value();
    ASSERT_TRUE(session.attach_quiz(domain::Quiz::create({question, question}).value()).has_value());
    ASSERT_TRUE(session.answer(0, 1).has_value());
    ASSERT_TRUE(session.answer(1, 0).has_value());
    const auto& answers = session.progress().snapshot().quiz_answers;
    EXPECT_EQ(session.tour().quiz().transform([&answers](const domain::Quiz& quiz) { return quiz.score(answers); }),
              std::optional<std::size_t>(1));
    ASSERT_TRUE(session.restart_quiz().has_value());
    EXPECT_TRUE(session.progress().snapshot().quiz_answers.empty());
}

TEST(ExplorationTests, SessionResumesAtTheStartWhenTheCurrentStepIsGone) {
    domain::ProgressSnapshot snapshot;
    snapshot.current = domain::StepId::create("gone").value();
    const application::TourSession session({.tours = std::make_shared<InMemoryTourRepository>(),
                                            .progress = std::make_shared<InMemoryProgressRepository>()},
                                           two_chapter_tour(),
                                           domain::TourProgress(domain::TourId::create("demo").value(), snapshot));
    EXPECT_EQ(session.resume_position(), domain::StepPosition{});
}

}
