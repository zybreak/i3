#include <gtest/gtest.h>

TEST(FooTest, ReferenceCheck){

  const int NumberOfElements = 10;


  EXPECT_EQ(
      10,
      NumberOfElements
    );
}

TEST(FooTest, ReferenceCheck2){

  std::string config = R"(
mode "meh" {
    bindsym Mod1 + Shift +   x resize grow
    bindcode Mod1+44 resize shrink
    bindsym --release Mod1+x exec foo
    bindsym --whole-window button3 nop
    bindsym --release --whole-window button3 nop
    bindsym --border button3 nop
    bindsym --release --border button3 nop
    bindsym --exclude-titlebar button3 nop
    bindsym --whole-window --border --exclude-titlebar button3 nop
}
)";

  std::equal(config.begin(), config.end(), "hej");

  EXPECT_EQ(
      config,
      "hej"
  );
}