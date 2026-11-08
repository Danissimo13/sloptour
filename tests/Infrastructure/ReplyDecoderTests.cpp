#include "Infrastructure/Agents/JsonReplyDecoder.hpp"

#include <gtest/gtest.h>

namespace sloptour::tests {

TEST(ReplyDecoderTests, ReadsOutlineFromAFencedBlock) {
    const infrastructure::JsonReplyDecoder decoder;
    const auto outline = decoder.decode_outline(
        "Sure!\n```json\n{\"title\": \"T\", \"summary\": \"S\", \"chapters\": [{\"title\": \"Big Picture\"}]}\n```\n");
    ASSERT_TRUE(outline.has_value()) << outline.error().message();
    EXPECT_EQ(outline->chapters().front().id, "big-picture");
}

TEST(ReplyDecoderTests, ReadsStepsFromABareArray) {
    const infrastructure::JsonReplyDecoder decoder;
    const auto steps = decoder.decode_steps(R"([{"title": "One", "body": "B"}])");
    ASSERT_TRUE(steps.has_value());
    EXPECT_EQ(steps->front().id().value(), "one");
}

TEST(ReplyDecoderTests, SkipsUnusableStepsAndLocations) {
    const infrastructure::JsonReplyDecoder decoder;
    const auto steps = decoder.decode_steps(R"({"steps": [
        {"title": "No body"},
        {"title": "Good", "body": "B", "locations": [
            {"path": "../outside", "start_line": 1},
            {"path": "src/a.cpp", "start_line": 9, "end_line": 3}]}]})");
    ASSERT_TRUE(steps.has_value());
    ASSERT_EQ(steps->size(), 1U);
    ASSERT_EQ(steps->front().locations().size(), 1U);
    EXPECT_EQ(steps->front().locations().front().lines(), domain::LineRange::create(9, 9).value());
}

TEST(ReplyDecoderTests, ReadsQuizAndSkipsBrokenQuestions) {
    const infrastructure::JsonReplyDecoder decoder;
    const auto quiz = decoder.decode_quiz(R"({"questions": [
        {"question": "Good?", "options": ["yes", "no"], "answer": 0, "explanation": "Yes."},
        {"question": "Broken", "options": ["one"], "answer": 3}]})");
    ASSERT_TRUE(quiz.has_value());
    EXPECT_EQ(quiz->questions().size(), 1U);
}

TEST(ReplyDecoderTests, ReportsRepliesWithoutJson) {
    const infrastructure::JsonReplyDecoder decoder;
    EXPECT_FALSE(decoder.decode_steps("I am sorry, I cannot help").has_value());
}

}
