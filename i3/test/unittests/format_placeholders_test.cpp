#include <gtest/gtest.h>

import format_placeholders;

TEST(FormatPlaceholdersTest, foo) {
    std::vector<placeholder_t> placeholders{{.name = "%title", .value = "foo"}};
    std::string result = format_placeholders("TITLE: %title", placeholders);
    ASSERT_EQ(result, "TITLE: foo");
}
