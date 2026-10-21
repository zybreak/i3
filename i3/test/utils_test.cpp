#include <gtest/gtest.h>
import utils;

using namespace std::string_literals;

TEST(UtilsTest, ParseQuotedStringNew){
    std::string s{"\"foo a \\\" x\" bar"};
    auto itr = s.cbegin();
    auto result = utils::parse_string(itr, true);
    ASSERT_EQ(*result, "foo a \" x");
    
    std::string s2{itr, s.cend()};
    ASSERT_EQ(s2, "\" bar");
}

TEST(UtilsTest, ParseStringNew){
    std::vector<std::string> word_input = {"foo bar", "foo\tbar", "foo,bar", "foo;bar", "foo\rbar", "foo\nbar", "foo\0bar"s};
    std::vector<std::string> nonword_input = {"foo,bar", "foo;bar", "foo\rbar", "foo\nbar", "foo\0bar"s};
    
    for (auto &i : nonword_input){
        auto itr = i.cbegin();
        auto result = utils::parse_string(itr, false);
        std::string remainder{itr+1, i.cend()};
        ASSERT_EQ(*result, "foo");
        ASSERT_EQ(remainder, "bar");
    }
    
    for (auto &i : word_input){
        auto itr = i.cbegin();
        auto result = utils::parse_string(itr, true);
        std::string remainder{itr+1, i.cend()};
        ASSERT_EQ(*result, "foo");
        ASSERT_EQ(remainder, "bar");
    }
}

TEST(UtilsTest, IsNullNew) {
    std::vector<std::string> word_input = {"", " ", "\t", "]", ",", ";", "\r", "\n", "\0"s};
    std::vector<std::string> nonword_input = {"", ";", ",", "\r", "\n", "\0"s};
    std::string brace_input = "]";
   
    for (auto &i : word_input){
        auto itr = i.cbegin();
        auto result = utils::parse_string(itr, true);
        ASSERT_FALSE(result);
    }

    {
        auto itr = brace_input.cbegin();
        auto result = utils::parse_string(itr, false);
        ASSERT_TRUE(result);
    }
    
    for (auto &i : nonword_input){
        auto itr = i.cbegin();
        auto result = utils::parse_string(itr, false);
        ASSERT_FALSE(result);
    }
}

TEST(UtilsTest, ParseLong) {
    long out = 0;

    EXPECT_TRUE(utils::parse_long("42", out));
    EXPECT_EQ(out, 42);
    EXPECT_TRUE(utils::parse_long("-42", out));
    EXPECT_EQ(out, -42);
    EXPECT_FALSE(utils::parse_long("", out));
    EXPECT_FALSE(utils::parse_long("42a", out));
    EXPECT_FALSE(utils::parse_long(" 42", out));
    EXPECT_FALSE(utils::parse_long("0x2a", out));

    int small = 0;
    EXPECT_FALSE(utils::parse_long("4294967296", small));
}

TEST(UtilsTest, ParseLongDetectsTheBase) {
    long out = 0;

    EXPECT_TRUE(utils::parse_long("94371851", out, 0));
    EXPECT_EQ(out, 94371851);
    EXPECT_TRUE(utils::parse_long("-12", out, 0));
    EXPECT_EQ(out, -12);
    EXPECT_TRUE(utils::parse_long("0", out, 0));
    EXPECT_EQ(out, 0);
    EXPECT_TRUE(utils::parse_long("0x5a0000b", out, 0));
    EXPECT_EQ(out, 0x5a0000b);
    EXPECT_TRUE(utils::parse_long("0X2A", out, 0));
    EXPECT_EQ(out, 42);
    EXPECT_TRUE(utils::parse_long("052", out, 0));
    EXPECT_EQ(out, 42);

    EXPECT_FALSE(utils::parse_long("", out, 0));
    EXPECT_FALSE(utils::parse_long("0x", out, 0));
    EXPECT_FALSE(utils::parse_long("0x-5", out, 0));
    EXPECT_FALSE(utils::parse_long("08", out, 0));
    EXPECT_FALSE(utils::parse_long("foobar", out, 0));
}
