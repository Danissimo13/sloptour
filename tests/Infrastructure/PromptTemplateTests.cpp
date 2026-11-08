#include "Infrastructure/Prompts/PromptTemplate.hpp"

#include <gtest/gtest.h>

namespace sloptour::tests {

TEST(PromptTemplateTests, FillsEveryPlaceholder) {
    const auto filled = infrastructure::filled("Hi {{ name }}, see {{name}}.", {{"name", "Ann"}});
    EXPECT_EQ(filled.value(), "Hi Ann, see Ann.");
}

TEST(PromptTemplateTests, RejectsUnknownPlaceholders) {
    EXPECT_FALSE(infrastructure::filled("{{missing}}", {}).has_value());
}

TEST(PromptTemplateTests, DoesNotExpandPlaceholdersInsideValues) {
    const auto filled = infrastructure::filled("{{code}}", {{"code", "{{template}}"}});
    EXPECT_EQ(filled.value(), "{{template}}");
}

}
