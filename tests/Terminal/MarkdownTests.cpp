#include "Terminal/Generation/GenerationProgress.hpp"
#include "Terminal/Markdown/InlineParser.hpp"
#include "Terminal/Theme/ThemeFactory.hpp"
#include "Terminal/Widgets/Chrome.hpp"

#include <gtest/gtest.h>

namespace sloptour::tests {

TEST(MarkdownTests, SplitsStrongCodeAndEmphasis) {
    const auto spans = terminal::inline_spans("a **bold** `code` _soft_ end");
    ASSERT_EQ(spans.size(), 7U);
    EXPECT_TRUE(spans.at(1).strong);
    EXPECT_TRUE(spans.at(3).code);
    EXPECT_EQ(spans.at(3).text, "code");
    EXPECT_TRUE(spans.at(5).emphasis);
}

TEST(MarkdownTests, KeepsUnderscoresInsideIdentifiers) {
    const auto spans = terminal::inline_spans("call snake_case_name now");
    ASSERT_EQ(spans.size(), 1U);
    EXPECT_EQ(spans.front().text, "call snake_case_name now");
}

TEST(ChromeTests, ShortensByCharactersNotBytes) {
    EXPECT_EQ(terminal::shortened("Удаление задачи", 5), "Удал…");
    EXPECT_EQ(terminal::shortened("Удаление", 8), "Удаление");
    EXPECT_EQ(terminal::shortened("abcdef", 4), "abc…");
}

TEST(ThemeTests, AcceptsTerminalPaletteNamesAndHex) {
    const auto theme =
        terminal::theme_from({{"accent", "blue"}, {"text", "default"}, {"danger", "#ff0000"}, {"muted", "nope"}});
    EXPECT_EQ(theme.accent, ftxui::Color(ftxui::Color::Blue));
    EXPECT_EQ(theme.text, ftxui::Color(ftxui::Color::Default));
    EXPECT_EQ(theme.danger, ftxui::Color::RGB(255, 0, 0));
    EXPECT_EQ(theme.muted, ftxui::Color(ftxui::Color::Default));
}

TEST(GenerationProgressTests, SumsTokensAcrossLanes) {
    terminal::GenerationProgress progress(2);
    progress.apply(application::OutlineReady{.title = "T", .chapters = {"One"}});
    const application::TokenUsage spent{
        .input = 5, .output = 1, .cache_read = 0, .cache_write = 0, .cost_usd = 0.0, .estimated = false};
    progress.apply(application::TokensUsed{.chapter = 0, .usage = spent});
    progress.apply(application::TokensUsed{.chapter = std::nullopt, .usage = spent});
    EXPECT_EQ(progress.usage().total(), 12);
    EXPECT_EQ(progress.chapters().front().usage.total(), 6);
}

TEST(GenerationProgressTests, TracksChaptersAndBoundsActivity) {
    terminal::GenerationProgress progress(2);
    progress.apply(application::OutlineReady{.title = "T", .chapters = {"One", "Two"}});
    progress.apply(application::ChapterStarted{.index = 0});
    progress.apply(application::ChapterReady{.index = 0, .steps = 2});
    for (const auto* line : {"a", "b", "c"}) {
        progress.apply(application::AgentOutput{.chapter = std::nullopt, .line = line});
    }
    EXPECT_EQ(progress.finished_chapters(), 1U);
    EXPECT_EQ(progress.activity().size(), 2U);
    EXPECT_EQ(progress.activity().back(), "c");
}

}
