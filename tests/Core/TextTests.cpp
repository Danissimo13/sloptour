#include "Core/Text/Text.hpp"

#include <gtest/gtest.h>

namespace sloptour::tests {

TEST(TextTests, SlugifyLowercasesAndCollapsesSeparators) {
    EXPECT_EQ(core::slugify("  Hello,  World! / API "), "hello-world-api");
}

TEST(TextTests, SlugifyNeverEndsWithSeparatorAfterTruncation) {
    const auto slug = core::slugify(std::string(47, 'a') + " b");
    EXPECT_FALSE(slug.ends_with('-'));
}

TEST(TextTests, SplitLinesDropsCarriageReturns) {
    const auto lines = core::split_lines("one\r\ntwo\nthree");
    ASSERT_EQ(lines.size(), 3U);
    EXPECT_EQ(lines.at(0), "one");
    EXPECT_EQ(lines.at(2), "three");
}

}
