#include "Application/Generation/GenerateTour.hpp"
#include "Infrastructure/Agents/JsonReplyDecoder.hpp"
#include "tests/Fakes/FakeAgent.hpp"
#include "tests/Fakes/FakeAgentProvider.hpp"
#include "tests/Fakes/FakePromptLibrary.hpp"
#include "tests/Fakes/FakeRepositoryInspector.hpp"
#include "tests/Fakes/FixedClock.hpp"
#include "tests/Fakes/GenerationSettingsBuilder.hpp"
#include "tests/Fakes/InMemoryTourRepository.hpp"
#include "tests/Fakes/RecordingObserver.hpp"

#include <gtest/gtest.h>

namespace sloptour::tests {

namespace {

constexpr std::string_view OUTLINE = R"({"title": "Demo", "summary": "A demo.", "chapters": [
    {"id": "intro", "title": "Intro", "goal": "Know it"},
    {"id": "build", "title": "Build", "goal": "Build it"},
    {"id": "extra", "title": "Extra", "goal": "Beyond the limit"}]})";
constexpr std::string_view STEPS = R"(Here you go: {"steps": [{"title": "First look", "body": "Look."},
    {"title": "Second look", "body": "Look again."}]})";
constexpr std::string_view QUIZ = R"({"questions": [
    {"question": "Where?", "options": ["here", "there"], "answer": 1, "explanation": "There."}]})";

struct Harness {
    std::shared_ptr<FakeAgent> agent;
    std::size_t quiz_questions = TEST_QUIZ_QUESTIONS;
    std::shared_ptr<InMemoryTourRepository> tours = std::make_shared<InMemoryTourRepository>();
    RecordingObserver observer;

    [[nodiscard]] application::GenerateTour generator() const {
        const auto provider = std::make_shared<FakeAgentProvider>(agent);
        const auto prompts = std::make_shared<FakePromptLibrary>();
        const auto decoder = std::make_shared<infrastructure::JsonReplyDecoder>();
        return application::GenerateTour(
            {.agents = provider,
             .prompts = prompts,
             .decoder = decoder,
             .tours = tours,
             .inspector = std::make_shared<FakeRepositoryInspector>(),
             .clock = std::make_shared<FixedClock>(),
             .quiz = std::make_shared<application::ComposeQuiz>(
                 application::ComposeQuiz::Dependencies{.agents = provider,
                                                        .prompts = prompts,
                                                        .decoder = decoder,
                                                        .settings = test_settings(quiz_questions)}),
             .settings = test_settings(quiz_questions)});
    }

    [[nodiscard]] domain::Tour run(const application::GenerationRequest& request) {
        const auto id = generator().execute(request, observer, std::stop_token{});
        EXPECT_TRUE(id.has_value());
        return tours->load(id.value()).value();
    }
};

[[nodiscard]] application::GenerationRequest onboarding() {
    return {.focus = application::OnboardingFocus{},
            .agent = {.name = "fake", .model = ""},
            .hint = "",
            .name = std::nullopt,
            .depth = "",
            .detail = ""};
}

[[nodiscard]] FakeAgent::Responder scripted(std::string_view failing) {
    return [failing](const application::AgentTask& task) -> application::AgentResult<std::string> {
        if (task.prompt.starts_with("onboarding_outline") || task.prompt.starts_with("functional_outline")) {
            return std::string(OUTLINE);
        }
        const bool fails = task.prompt.contains(failing) || task.prompt.starts_with("repair");
        if (!failing.empty() && fails) {
            return std::string("I could not do it");
        }
        if (task.prompt.starts_with("quiz")) {
            return std::string(QUIZ);
        }
        return std::string(STEPS);
    };
}

}

TEST(GenerateTourTests, BuildsAndSavesTheTourFromOutlineAndChapters) {
    Harness harness{.agent = std::make_shared<FakeAgent>(scripted(""))};
    const auto tour = harness.run(onboarding());
    ASSERT_EQ(tour.chapters().size(), 2U);
    EXPECT_EQ(tour.chapters().at(0).title(), "Intro");
    EXPECT_EQ(tour.chapters().at(1).steps().at(0).id().value(), "build/first-look");
    EXPECT_EQ(tour.id().value(), "onboarding-20261106-120000");
}

TEST(GenerateTourTests, AppliesTheChosenDepthAndRecordsTheChoice) {
    Harness harness{.agent = std::make_shared<FakeAgent>(scripted(""))};
    auto request = onboarding();
    request.depth = "light";
    request.agent.model = "big";
    const auto tour = harness.run(request);
    EXPECT_EQ(tour.chapters().size(), 1U);
    EXPECT_EQ(tour.chapters().front().steps().size(), 1U);
    EXPECT_EQ(tour.metadata().depth, "light");
    EXPECT_EQ(tour.metadata().detail, "medium");
    EXPECT_EQ(tour.metadata().model, "big");
}

TEST(GenerateTourTests, RejectsUnknownLevels) {
    Harness harness{.agent = std::make_shared<FakeAgent>(scripted(""))};
    auto request = onboarding();
    request.detail = "extreme";
    EXPECT_FALSE(harness.generator().execute(request, harness.observer, std::stop_token{}).has_value());
}

TEST(GenerateTourTests, EndsTheTourWithAQuiz) {
    Harness harness{.agent = std::make_shared<FakeAgent>(scripted(""))};
    const auto tour = harness.run(onboarding());
    ASSERT_TRUE(tour.quiz().has_value());
    EXPECT_EQ(harness.observer.count<application::QuizReady>(), 1U);
}

TEST(GenerateTourTests, KeepsTheTourWhenTheQuizFails) {
    Harness harness{.agent = std::make_shared<FakeAgent>(scripted("quiz"))};
    const auto tour = harness.run(onboarding());
    EXPECT_FALSE(tour.quiz().has_value());
    EXPECT_EQ(harness.observer.count<application::QuizFailed>(), 1U);
}

TEST(GenerateTourTests, SkipsTheQuizWhenTheDepthAsksForNone) {
    Harness harness{.agent = std::make_shared<FakeAgent>(scripted("")), .quiz_questions = 0};
    EXPECT_FALSE(harness.run(onboarding()).quiz().has_value());
    EXPECT_EQ(harness.observer.count<application::QuizStarted>(), 0U);
}

TEST(GenerateTourTests, ReportsTokensForEveryAgentRun) {
    Harness harness{.agent = std::make_shared<FakeAgent>(scripted(""))};
    std::ignore = harness.run(onboarding());
    EXPECT_EQ(harness.observer.count<application::TokensUsed>(), harness.agent->prompts().size());
}

TEST(GenerateTourTests, KeepsTheOtherChaptersWhenOneFails) {
    Harness harness{.agent = std::make_shared<FakeAgent>(scripted("chapter_id=intro"))};
    const auto tour = harness.run(onboarding());
    EXPECT_EQ(tour.chapters().size(), 1U);
    EXPECT_EQ(harness.observer.count<application::ChapterFailed>(), 1U);
}

TEST(GenerateTourTests, AsksTheAgentToRepairAnUnreadableReply) {
    Harness harness{.agent = std::make_shared<FakeAgent>(scripted("chapter_id=intro"))};
    std::ignore = harness.run(onboarding());
    const auto prompts = harness.agent->prompts();
    EXPECT_TRUE(std::ranges::any_of(prompts, [](const std::string& prompt) { return prompt.starts_with("repair"); }));
}

TEST(GenerateTourTests, FailsWhenNoChapterCanBeWritten) {
    Harness harness{.agent = std::make_shared<FakeAgent>(scripted("onboarding_chapter"))};
    EXPECT_FALSE(harness.generator().execute(onboarding(), harness.observer, std::stop_token{}).has_value());
}

TEST(GenerateTourTests, UsesTheRequestedName) {
    Harness harness{.agent = std::make_shared<FakeAgent>(scripted(""))};
    auto request = onboarding();
    request.name = "My First Tour";
    EXPECT_EQ(harness.run(request).id().value(), "my-first-tour");
}

TEST(GenerateTourTests, FunctionalTourFollowsTheDescribedFeature) {
    Harness harness{.agent = std::make_shared<FakeAgent>(scripted(""))};
    auto request = onboarding();
    request.focus = application::FunctionalFocus{.functionality = "  checkout and payment  "};
    const auto tour = harness.run(request);
    EXPECT_EQ(tour.metadata().kind, domain::TourKind::Functional);
    EXPECT_EQ(tour.metadata().scope, "checkout and payment");
    EXPECT_TRUE(tour.id().value().starts_with("functional-"));
    const auto prompts = harness.agent->prompts();
    EXPECT_TRUE(std::ranges::any_of(
        prompts, [](const std::string& prompt) { return prompt.starts_with("functional_chapter"); }));
}

TEST(GenerateTourTests, FunctionalTourNeedsADescription) {
    Harness harness{.agent = std::make_shared<FakeAgent>(scripted(""))};
    auto request = onboarding();
    request.focus = application::FunctionalFocus{.functionality = "   "};
    EXPECT_FALSE(harness.generator().execute(request, harness.observer, std::stop_token{}).has_value());
}

TEST(GenerateTourTests, RejectsUnknownAgents) {
    Harness harness{.agent = std::make_shared<FakeAgent>(scripted(""))};
    auto request = onboarding();
    request.agent.name = "nobody";
    EXPECT_FALSE(harness.generator().execute(request, harness.observer, std::stop_token{}).has_value());
}

}
