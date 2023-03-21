#include <gtest/gtest.h>
#include "../src/include/configuration.h"
#include "../src/config/old/config_parser.h"

bool parse_config(struct parser_ctx &ctx, const std::string &input, const char *filename);

TEST(FooTest, ModeBindingsOk){

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

  std::string expected = R"(
cfg::enter_mode((null), meh)
cfg::mode_binding(bindsym, Mod1,Shift, x, (null), (null), (null), (null), resize grow)
cfg::mode_binding(bindcode, Mod1, 44, (null), (null), (null), (null), resize shrink)
cfg::mode_binding(bindsym, Mod1, x, --release, (null), (null), (null), exec foo)
cfg::mode_binding(bindsym, (null), button3, (null), (null), --whole-window, (null), nop)
cfg::mode_binding(bindsym, (null), button3, --release, (null), --whole-window, (null), nop)
cfg::mode_binding(bindsym, (null), button3, (null), --border, (null), (null), nop)
cfg::mode_binding(bindsym, (null), button3, --release, --border, (null), (null), nop)
cfg::mode_binding(bindsym, (null), button3, (null), (null), (null), --exclude-titlebar, nop)
cfg::mode_binding(bindsym, (null), button3, (null), --border, --whole-window, --exclude-titlebar, nop)
)";

  struct parser_ctx ctx = {
      .load_type = config_load_t::C_LOAD,
  };

  parse_config(ctx, config, "<stdin>");

  //EXPECT_EQ(
  //    config,
  //    expected
  //);
}