#include <gtest/gtest.h>
import utils;

using namespace std::string_literals;

TEST(UtilsTest, ParseQuotedString){
    const char *s = "\"foo a \\\" x\" bar";
    char *result = utils::parse_string(&s, true);
    ASSERT_TRUE(strcmp(result, "foo a \" x") == 0);
    ASSERT_TRUE(strcmp(s, "\" bar") == 0);
}

TEST(UtilsTest, ParseString){
    const char *word_input[] = {"foo bar", "foo\tbar", "foo,bar", "foo;bar", "foo\rbar", "foo\nbar", "foo\0bar"};
    const char *nonword_input[] = {"foo,bar", "foo;bar", "foo\rbar", "foo\nbar", "foo\0bar"};
    
    for (auto &i : nonword_input){
        char *result = utils::parse_string(&i, false);
        ASSERT_TRUE(strcmp(result, "foo") == 0);
        ASSERT_TRUE(strcmp(i+1, "bar") == 0);
    }
    
    for (auto &i : word_input){
        char *result = utils::parse_string(&i, true);
        ASSERT_TRUE(strcmp(result, "foo") == 0);
        ASSERT_TRUE(strcmp(i+1, "bar") == 0);
    }
}

TEST(UtilsTest, IsNull) {
    const char *word_input[] = {"", " ", "\t", "]", ",", ";", "\r", "\n", "\0"};
    const char *nonword_input[] = {"", ";", ",", "\r", "\n", "\0"};
    const char *brace_input = "]";
   
    for (auto &i : word_input){
        char *result = utils::parse_string(&i, true);
        ASSERT_EQ(result, nullptr);
    }

    {
        char *result = utils::parse_string(&brace_input, false);
        ASSERT_NE(result, nullptr);
    }
    
    for (auto &i : nonword_input){
        char *result = utils::parse_string(&i, false);
        ASSERT_EQ(result, nullptr);
    }
}

// ------------------------------



TEST(UtilsTest, ParseQuotedStringNew){
    std::string s{"\"foo a \\\" x\" bar"};
    auto itr = s.cbegin();
    auto result = utils::parse_string_new(itr, true);
    ASSERT_EQ(*result, "foo a \" x");
    
    std::string s2{itr, s.cend()};
    ASSERT_EQ(s2, "\" bar");
}

TEST(UtilsTest, ParseStringNew){
    std::vector<std::string> word_input = {"foo bar", "foo\tbar", "foo,bar", "foo;bar", "foo\rbar", "foo\nbar", "foo\0bar"s};
    std::vector<std::string> nonword_input = {"foo,bar", "foo;bar", "foo\rbar", "foo\nbar", "foo\0bar"s};
    
    for (auto &i : nonword_input){
        auto itr = i.cbegin();
        auto result = utils::parse_string_new(itr, false);
        std::string remainder{itr+1, i.cend()};
        ASSERT_EQ(*result, "foo");
        ASSERT_EQ(remainder, "bar");
    }
    
    for (auto &i : word_input){
        auto itr = i.cbegin();
        auto result = utils::parse_string_new(itr, true);
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
        auto result = utils::parse_string_new(itr, true);
        ASSERT_FALSE(result);
    }

    {
        auto itr = brace_input.cbegin();
        auto result = utils::parse_string_new(itr, false);
        ASSERT_TRUE(result);
    }
    
    for (auto &i : nonword_input){
        auto itr = i.cbegin();
        auto result = utils::parse_string_new(itr, false);
        ASSERT_FALSE(result);
    }
}
