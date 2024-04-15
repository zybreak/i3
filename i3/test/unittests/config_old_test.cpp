#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <strstream>

#include "config_applier_adapter.h"

import i3;
import i3_config_base;
import i3_config_old;

using namespace std::literals;

class MockedResourceDatabase : public BaseResourceDatabase {
    MOCK_METHOD(char*, get_resource, (char *name, const char *fallback), (override));
};

TEST(ConfigOldTest, test_mode_bindings_ok) {
    std::stringbuf output;
    std::ostream out{&output};
    ConfigApplierAdapter applier{out};
    MockedResourceDatabase resourceDatabase{};
    std::istrstream in{R""""(
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
)""""};

    OldParser p("/tmp/foo", in, resourceDatabase, config_load_t::C_LOAD, applier);

    try {
        p.parse_file();
    } catch (std::exception &e) {
        FAIL() << "Error parsing configuration file" << e.what();
    }

    std::string expected{R""""(cfg::enter_mode((null), meh)
cfg::mode_binding(bindsym, Mod1,Shift, x, (null), (null), (null), (null), resize grow)
cfg::mode_binding(bindcode, Mod1, 44, (null), (null), (null), (null), resize shrink)
cfg::mode_binding(bindsym, Mod1, x, --release, (null), (null), (null), exec foo)
cfg::mode_binding(bindsym, (null), button3, (null), (null), --whole-window, (null), nop)
cfg::mode_binding(bindsym, (null), button3, --release, (null), --whole-window, (null), nop)
cfg::mode_binding(bindsym, (null), button3, (null), --border, (null), (null), nop)
cfg::mode_binding(bindsym, (null), button3, --release, --border, (null), (null), nop)
cfg::mode_binding(bindsym, (null), button3, (null), (null), (null), --exclude-titlebar, nop)
cfg::mode_binding(bindsym, (null), button3, (null), --border, --whole-window, --exclude-titlebar, nop)
)""""};

    ASSERT_EQ(output.str(), expected);
}
