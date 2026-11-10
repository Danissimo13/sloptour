#include "Infrastructure/Agents/ClaudeStreamOutput.hpp"
#include "Infrastructure/Process/ExecutableLocator.hpp"
#include "Infrastructure/Process/LineSplitter.hpp"
#include "Infrastructure/Process/ProcessRunner.hpp"

#include <gtest/gtest.h>

#include <chrono>
#include <filesystem>
#include <thread>
#include <vector>

namespace sloptour::tests {

namespace {

constexpr std::chrono::milliseconds POLL{10};
constexpr std::chrono::seconds GENEROUS{20};
constexpr std::chrono::milliseconds CANCEL_AFTER{100};
constexpr std::chrono::seconds CANCEL_DEADLINE{10};

[[nodiscard]] infrastructure::ProcessListeners quiet() {
    return {.output = [](std::string_view) {}, .errors = [](std::string_view) {}};
}

[[nodiscard]] infrastructure::ProcessCommand shell(std::string script, std::string input = {}) {
    return {.arguments = {"/bin/sh", "-c", std::move(script)},
            .input = std::move(input),
            .working_directory = std::filesystem::temp_directory_path(),
            .timeout = GENEROUS};
}

}

TEST(ProcessTests, PassesInputAndCollectsOutput) {
    const infrastructure::ProcessRunner runner(POLL);
    const auto outcome = runner.run(shell("cat", "hello"), quiet(), std::stop_token{});
    ASSERT_TRUE(outcome.has_value()) << outcome.error().message();
    EXPECT_EQ(outcome->output, "hello");
    EXPECT_EQ(outcome->status, 0);
}

TEST(ProcessTests, GivesInputAsAFileThatAgentsOnWindowsCanRead) {
    const infrastructure::ProcessRunner runner(POLL);
    const auto outcome = runner.run(shell("test -f /dev/stdin && cat", "hello"), quiet(), std::stop_token{});
    ASSERT_TRUE(outcome.has_value()) << outcome.error().message();
    EXPECT_EQ(outcome->output, "hello");
    EXPECT_EQ(outcome->status, 0);
}

TEST(ProcessTests, ReportsExitStatusAndErrors) {
    const infrastructure::ProcessRunner runner(POLL);
    const auto outcome = runner.run(shell("echo broken >&2; exit 3"), quiet(), std::stop_token{});
    ASSERT_TRUE(outcome.has_value());
    EXPECT_EQ(outcome->status, 3);
    EXPECT_EQ(outcome->errors, "broken\n");
}

TEST(ProcessTests, StopsWhenCancelled) {
    const infrastructure::ProcessRunner runner(POLL);
    std::stop_source source;
    const std::jthread canceller([&source] {
        std::this_thread::sleep_for(CANCEL_AFTER);
        source.request_stop();
    });
    const auto started = std::chrono::steady_clock::now();
    const auto outcome = runner.run(shell("sleep 30"), quiet(), source.get_token());
    EXPECT_FALSE(outcome.has_value());
    EXPECT_LT(std::chrono::steady_clock::now() - started, CANCEL_DEADLINE);
}

TEST(ProcessTests, StreamsLinesToListeners) {
    const infrastructure::ProcessRunner runner(POLL);
    std::vector<std::string> lines;
    const infrastructure::ProcessListeners listeners{
        .output = [&lines](std::string_view line) { lines.emplace_back(line); }, .errors = [](std::string_view) {}};
    ASSERT_TRUE(runner.run(shell("printf 'one\\ntwo'"), listeners, std::stop_token{}).has_value());
    EXPECT_EQ(lines, (std::vector<std::string>{"one", "two"}));
}

TEST(ProcessTests, LineSplitterJoinsLinesSplitAcrossChunks) {
    infrastructure::LineSplitter splitter;
    std::vector<std::string> lines;
    const auto listener = [&lines](std::string_view line) { lines.emplace_back(line); };
    splitter.feed("hel", listener);
    splitter.feed("lo\nwor", listener);
    splitter.flush(listener);
    EXPECT_EQ(lines, (std::vector<std::string>{"hello", "wor"}));
}

TEST(ProcessTests, ClaudeStreamShowsToolUseRelativeToTheRepository) {
    const infrastructure::ClaudeStreamOutput output("/repo");
    const auto activity = output.parse(
        R"({"type":"assistant","message":{"content":[{"type":"tool_use","name":"Read","input":{"file_path":"/repo/src/a.py"}}]}})");
    EXPECT_EQ(activity.activity.value_or(""), "Read src/a.py");
}

TEST(ProcessTests, ClaudeStreamReplyIsTheResultEvent) {
    const infrastructure::ClaudeStreamOutput output("/repo");
    const auto reply =
        output.reply("{\"type\":\"system\"}\n{\"type\":\"result\",\"is_error\":false,\"result\":\"done\"}\n");
    EXPECT_EQ(reply.value(), "done");
}

TEST(ProcessTests, LaunchableResolvesTheExecutableOnPath) {
    const auto arguments = infrastructure::launchable({"sh", "-c", "true"});
    ASSERT_EQ(arguments.size(), 3U);
    EXPECT_TRUE(std::filesystem::path(arguments.front()).is_absolute());
    EXPECT_EQ(arguments.back(), "true");
}

TEST(ProcessTests, LaunchableKeepsUnknownCommands) {
    const std::vector<std::string> arguments{"no-such-sloptour-tool", "x"};
    EXPECT_EQ(infrastructure::launchable(arguments), arguments);
}

}
