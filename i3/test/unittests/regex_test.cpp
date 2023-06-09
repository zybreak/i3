#include <gtest/gtest.h>
#include "i3_regex.h"

TEST(RegexTest, New){

  auto *r = new Regex{".*"};

  ASSERT_NE(
      r,
      nullptr
  );

  ASSERT_TRUE(r->valid);
  ASSERT_TRUE(r->regex_matches("foo"));
}

TEST(RegexTest, Invalid){

  auto *r = new Regex{"*"};

  ASSERT_NE(
      r,
      nullptr
  );

  ASSERT_FALSE(r->valid);
}