#include <string>
#include "config_parser.h"
#include "config_adapter.h"
#undef None
#undef Bool
#include <gtest/gtest.h>

static std::string parse(const std::string &s) {
    std::stringbuf output;

    ConfigApplierAdapter applier{&output};

    std::stringstream input;

    input.str(s);

    ResourceDatabase resourceDatabase{nullptr};

    NewParser parser{resourceDatabase, &input, config_load_t::C_LOAD, applier};
    parser.parse_file();

    return output.str();
}

TEST(ConfigParserNewTest, A){

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

  std::string expected =
R"(cfg::enter_mode((null), "meh")
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

  auto result = parse(config);

  EXPECT_EQ(
      expected,
      result
  );
}