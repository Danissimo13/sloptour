#include "Infrastructure/Assets/EmbeddedAssets.hpp"
#include "Infrastructure/Json/JsonAccess.hpp"
#include "Terminal/Localization/Localizer.hpp"

#include <gtest/gtest.h>

#include <set>

namespace sloptour::tests {

namespace {

[[nodiscard]] terminal::Localizer localizer() {
    const terminal::LanguagePack english{.code = "en",
                                         .label = "EN",
                                         .agent_name = "English",
                                         .plural_rule = "one-many",
                                         .texts = {{"greeting", "Hi {1}, {2}"},
                                                   {"only.english", "fallback"},
                                                   {"plural.step.one", "{1} step"},
                                                   {"plural.step.many", "{1} steps"}}};
    const terminal::LanguagePack russian{.code = "ru",
                                         .label = "RU",
                                         .agent_name = "Russian",
                                         .plural_rule = "east-slavic",
                                         .texts = {{"greeting", "Привет {1}, {2}"},
                                                   {"plural.step.one", "{1} шаг"},
                                                   {"plural.step.few", "{1} шага"},
                                                   {"plural.step.many", "{1} шагов"}}};
    return terminal::Localizer({english, russian});
}

[[nodiscard]] std::set<std::string> keys_of(std::string_view path) {
    std::set<std::string> keys;
    for (const auto& asset : infrastructure::embedded_assets()) {
        if (asset.path == path) {
            const auto document = infrastructure::parsed_json(asset.content).value();
            for (const auto& [key, value] : document.at("texts").items()) {
                if (!key.contains(".few")) {
                    keys.insert(key);
                }
            }
        }
    }
    return keys;
}

}

TEST(LocalizationTests, FormatsPlaceholdersInOrder) {
    EXPECT_EQ(localizer().format("greeting", {"Ann", "welcome"}), "Hi Ann, welcome");
}

TEST(LocalizationTests, FallsBackToTheFirstLanguageThenTheKey) {
    auto text = localizer();
    ASSERT_TRUE(text.use("ru"));
    EXPECT_EQ(text.text("only.english"), "fallback");
    EXPECT_EQ(text.text("missing.key"), "missing.key");
}

TEST(LocalizationTests, UsesTheEastSlavicPluralForms) {
    auto text = localizer();
    ASSERT_TRUE(text.use("ru"));
    EXPECT_EQ(text.count(1, "step"), "1 шаг");
    EXPECT_EQ(text.count(3, "step"), "3 шага");
    EXPECT_EQ(text.count(11, "step"), "11 шагов");
    EXPECT_EQ(text.count(21, "step"), "21 шаг");
    EXPECT_EQ(text.count(25, "step"), "25 шагов");
}

TEST(LocalizationTests, UsesEnglishPluralForms) {
    EXPECT_EQ(localizer().count(1, "step"), "1 step");
    EXPECT_EQ(localizer().count(0, "step"), "0 steps");
}

TEST(LocalizationTests, RefusesUnknownLanguages) {
    auto text = localizer();
    EXPECT_FALSE(text.use("de"));
    EXPECT_EQ(text.reply_language().code, "en");
}

TEST(LocalizationTests, EmbeddedCatalogsHaveTheSameKeys) {
    EXPECT_EQ(keys_of("i18n/en.json"), keys_of("i18n/ru.json"));
    EXPECT_FALSE(keys_of("i18n/en.json").empty());
}

}
