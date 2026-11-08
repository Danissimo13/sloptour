#include "Infrastructure/Agents/AgentCommand.hpp"
#include "Infrastructure/Agents/ClaudeStreamOutput.hpp"
#include "Infrastructure/Agents/UsageMeter.hpp"

#include <gtest/gtest.h>

namespace sloptour::tests {

namespace {

constexpr std::int64_t PROMPT_TOKENS = 100;
constexpr std::int64_t FINAL_OUTPUT_TOKENS = 40;

[[nodiscard]] infrastructure::AgentProfile profile(std::vector<std::string> command) {
    return {.name = "claude",
            .description = "",
            .command = std::move(command),
            .output = "text",
            .models = {"opus"},
            .model_arguments = {"--model", "{model}"},
            .timeout = std::chrono::seconds(1)};
}

[[nodiscard]] application::TokenUsage tokens(std::int64_t input, std::int64_t output) {
    return {.input = input, .output = output, .cache_read = 0, .cache_write = 0, .cost_usd = 0.0, .estimated = false};
}

}

TEST(AgentTests, InsertsModelArgumentsIntoTheSlot) {
    const auto command = infrastructure::agent_command(profile({"claude", "{model_arguments}", "-p"}), "opus");
    EXPECT_EQ(command.value(), (std::vector<std::string>{"claude", "--model", "opus", "-p"}));
}

TEST(AgentTests, DropsTheSlotWithoutAModel) {
    const auto command = infrastructure::agent_command(profile({"claude", "{model_arguments}", "-p"}), "");
    EXPECT_EQ(command.value(), (std::vector<std::string>{"claude", "-p"}));
}

TEST(AgentTests, RefusesAModelWhenTheCommandHasNoSlot) {
    EXPECT_FALSE(infrastructure::agent_command(profile({"claude", "-p"}), "opus").has_value());
}

TEST(AgentTests, UsageMeterCountsRepeatedMessagesOnce) {
    infrastructure::UsageMeter meter;
    EXPECT_EQ(meter.record({.key = "a", .usage = tokens(100, 1), .authoritative = false}).total(), 101);
    EXPECT_EQ(meter.record({.key = "a", .usage = tokens(100, 5), .authoritative = false}).output, 4);
    EXPECT_EQ(meter.record({.key = "b", .usage = tokens(50, 0), .authoritative = false}).input, 50);
}

TEST(AgentTests, UsageMeterLetsTheFinalTotalReplaceTheEstimate) {
    infrastructure::UsageMeter meter;
    std::ignore = meter.record({.key = "a", .usage = tokens(PROMPT_TOKENS, 1), .authoritative = false});
    const auto correction =
        meter.record({.key = "result", .usage = tokens(PROMPT_TOKENS, FINAL_OUTPUT_TOKENS), .authoritative = true});
    EXPECT_EQ(correction.input, 0);
    EXPECT_EQ(correction.output, FINAL_OUTPUT_TOKENS - 1);
}

TEST(AgentTests, ClaudeStreamReportsMessageUsageAndCost) {
    const infrastructure::ClaudeStreamOutput output("/repo");
    const auto message = output.parse(
        R"({"type":"assistant","message":{"id":"m1","content":[],"usage":{"input_tokens":3,"output_tokens":7,"cache_read_input_tokens":20}}})");
    const auto usage = message.usage.value_or(infrastructure::UsageSample{}).usage;
    EXPECT_EQ(usage.prompt_side(), 23);
    EXPECT_EQ(usage.output, 7);
    const auto result = output.parse(
        R"({"type":"result","result":"x","total_cost_usd":0.25,"usage":{"input_tokens":4,"output_tokens":90}})");
    const auto total = result.usage.value_or(infrastructure::UsageSample{});
    EXPECT_TRUE(total.authoritative);
    EXPECT_EQ(total.usage.output, 90);
    EXPECT_DOUBLE_EQ(total.usage.cost_usd, 0.25);
}

}
