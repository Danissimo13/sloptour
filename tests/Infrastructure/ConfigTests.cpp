#include "Infrastructure/Configuration/ConfigLoader.hpp"

#include <gtest/gtest.h>

namespace sloptour::tests {

TEST(ConfigTests, DefaultsDescribeEveryAgentAndSetting) {
    const auto config = infrastructure::load_config({});
    ASSERT_TRUE(config.has_value()) << config.error().message();
    EXPECT_FALSE(config->agents.empty());
    ASSERT_TRUE(config->generation.depth("").has_value());
    EXPECT_TRUE(config->generation.depth("high").has_value());
    EXPECT_TRUE(config->generation.detail("light").has_value());
    EXPECT_FALSE(config->agents.front().models.empty());
    EXPECT_FALSE(config->editor.default_arguments.empty());
}

TEST(ConfigTests, ReportsMissingSections) {
    EXPECT_FALSE(infrastructure::config_from_document(infrastructure::Json::object()).has_value());
}

}
