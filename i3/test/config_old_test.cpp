#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <strstream>
#include <sstream>
#include <tuple>

#include "config_applier_adapter.h"

import i3;
import i3_config_base;
import i3_config_old;

using namespace std::literals;

class MockedResourceDatabase : public BaseResourceDatabase {
    MOCK_METHOD(std::string, get_resource, (std::string_view const name, std::string_view const fallback), (override));
};

TEST(ConfigOldTest, test_mode_bindings_ok) {
    std::stringbuf output;
    std::ostream out{&output};
    ConfigApplierAdapter applier{out};
    MockedResourceDatabase resourceDatabase{};
    std::istringstream in{R""""(
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

    OldParser p("/tmp/foo", in, resourceDatabase, config_load_t::C_LOAD, applier);

    try {
        p.parse_file();
    } catch (std::exception &e) {
        FAIL() << "Error parsing configuration file" << e.what();
    }

    ASSERT_EQ(output.str(), expected);
}

TEST(ConfigOldTest, test_exec_okay) {
    std::stringbuf output;
    std::ostream out{&output};
    ConfigApplierAdapter applier{out};
    MockedResourceDatabase resourceDatabase{};
    std::istringstream in{R""""(
exec geeqie
exec --no-startup-id /tmp/foo.sh
exec_always firefox
exec_always --no-startup-id /tmp/bar.sh
)""""};

    std::string expected{R""""(cfg::exec(exec, (null), geeqie)
cfg::exec(exec, --no-startup-id, /tmp/foo.sh)
cfg::exec(exec_always, (null), firefox)
cfg::exec(exec_always, --no-startup-id, /tmp/bar.sh)
)""""};

    OldParser p("/tmp/foo", in, resourceDatabase, config_load_t::C_LOAD, applier);

    try {
        p.parse_file();
    } catch (std::exception &e) {
        FAIL() << "Error parsing configuration file" << e.what();
    }

    ASSERT_EQ(output.str(), expected);
}

TEST(ConfigOldTest, test_for_window_okay) {
    std::stringbuf output;
    std::ostream out{&output};
    ConfigApplierAdapter applier{out};
    MockedResourceDatabase resourceDatabase{};
    std::istringstream in{R""""(
for_window [] nop empty
for_window [class="^Chrome"] floating enable
for_window [class=^Chrome] floating enable
for_window [floating_from   = "auto" class= ==Class==  ] nop floating
for_window [tiling_from=auto class="==Class=="]nop floating
)""""};

    std::string expected{R""""(cfg::for_window(nop empty)
cfg::criteria_add(class, ^Chrome)
cfg::for_window(floating enable)
cfg::criteria_add(class, ^Chrome)
cfg::for_window(floating enable)
cfg::criteria_add(floating_from, auto)
cfg::criteria_add(class, ==Class==)
cfg::for_window(nop floating)
cfg::criteria_add(tiling_from, auto)
cfg::criteria_add(class, ==Class==)
cfg::for_window(nop floating)
)""""};

    OldParser p("/tmp/foo", in, resourceDatabase, config_load_t::C_LOAD, applier);

    try {
        p.parse_file();
    } catch (std::exception &e) {
        FAIL() << "Error parsing configuration file" << e.what();
    }

    ASSERT_EQ(output.str(), expected);
}

TEST(ConfigOldTest, test_for_window_errors_okay) {
    std::stringbuf output;
    std::ostream out{&output};
    ConfigApplierAdapter applier{out};
    MockedResourceDatabase resourceDatabase{};
    std::istringstream in{R""""(for_window [tiling_from=typo] nop typo
for_window [tiling_from="typo"] nop typo
)""""};

    std::string expected{R""""(CONFIG: Expected one of these tokens: '"', 'auto', 'user'
CONFIG: (in file /tmp/foo)
CONFIG: Line   1: for_window [tiling_from=typo] nop typo
CONFIG:                                   ^^^^^^^^^^^^^^
CONFIG: Line   2: for_window [tiling_from="typo"] nop typo
CONFIG: Expected one of these tokens: 'auto', 'user'
CONFIG: (in file /tmp/foo)
CONFIG: Line   1: for_window [tiling_from=typo] nop typo
CONFIG: Line   2: for_window [tiling_from="typo"] nop typo
CONFIG:                                    ^^^^^^^^^^^^^^^
)""""};

    OldParser p("/tmp/foo", in, resourceDatabase, config_load_t::C_LOAD, applier);

    try {
        p.parse_file();
        FAIL() << "Configuration should not have parsed successfully";
    } catch (std::exception &e) {
        ASSERT_EQ(e.what(), expected);
    }
}

TEST(ConfigOldTest, test_for_window_okay2) {
    std::stringbuf output;
    std::ostream out{&output};
    ConfigApplierAdapter applier{out};
    MockedResourceDatabase resourceDatabase{};
    std::istringstream in{R""""(
assign [class="^Chrome"] 4
assign [class="^Chrome"] workspace number 3
assign [class="^Chrome"] named workspace
assign [class="^Chrome"] "quoted named workspace"
assign [class="^Chrome"] → "quoted named workspace"
)""""};

    std::string expected{R""""(cfg::criteria_add(class, ^Chrome)
cfg::assign(4, 0)
cfg::criteria_add(class, ^Chrome)
cfg::assign(3, 1)
cfg::criteria_add(class, ^Chrome)
cfg::assign(named workspace, 0)
cfg::criteria_add(class, ^Chrome)
cfg::assign(quoted named workspace, 0)
cfg::criteria_add(class, ^Chrome)
cfg::assign(quoted named workspace, 0)
)""""};

    OldParser p("/tmp/foo", in, resourceDatabase, config_load_t::C_LOAD, applier);

    try {
        p.parse_file();
    } catch (std::exception &e) {
        FAIL() << "Error parsing configuration file" << e.what();
    }

    ASSERT_EQ(output.str(), expected);
}

TEST(ConfigOldTest, test_floating_minimum_size_ok) {
    std::stringbuf output;
    std::ostream out{&output};
    ConfigApplierAdapter applier{out};
    MockedResourceDatabase resourceDatabase{};
    std::istringstream in{R""""(
floating_minimum_size 80x55
floating_minimum_size 80    x  55
floating_maximum_size 73 x 10
)""""};

    std::string expected{R""""(cfg::floating_minimum_size(80, 55)
cfg::floating_minimum_size(80, 55)
cfg::floating_maximum_size(73, 10)
)""""};

    OldParser p("/tmp/foo", in, resourceDatabase, config_load_t::C_LOAD, applier);

    try {
        p.parse_file();
    } catch (std::exception &e) {
        FAIL() << "Error parsing configuration file" << e.what();
    }

    ASSERT_EQ(output.str(), expected);
}

TEST(ConfigOldTest, test_popup_during_fullscreen_ok) {
    std::stringbuf output;
    std::ostream out{&output};
    ConfigApplierAdapter applier{out};
    MockedResourceDatabase resourceDatabase{};
    std::istringstream in{R""""(
popup_during_fullscreen ignore
popup_during_fullscreen leave_fullscreen
popup_during_fullscreen SMArt
)""""};

    std::string expected{R""""(cfg::popup_during_fullscreen(ignore)
cfg::popup_during_fullscreen(leave_fullscreen)
cfg::popup_during_fullscreen(smart)
)""""};

    OldParser p("/tmp/foo", in, resourceDatabase, config_load_t::C_LOAD, applier);

    try {
        p.parse_file();
    } catch (std::exception &e) {
        FAIL() << "Error parsing configuration file" << e.what();
    }

    ASSERT_EQ(output.str(), expected);
}

TEST(ConfigOldTest, test_floating_modifier_ok) {
    std::stringbuf output;
    std::ostream out{&output};
    ConfigApplierAdapter applier{out};
    MockedResourceDatabase resourceDatabase{};
    std::istringstream in{R""""(
floating_modifier Mod1
floating_modifier mOd1
)""""};

    std::string expected{R""""(cfg::floating_modifier(Mod1)
cfg::floating_modifier(Mod1)
)""""};

    OldParser p("/tmp/foo", in, resourceDatabase, config_load_t::C_LOAD, applier);

    try {
        p.parse_file();
    } catch (std::exception &e) {
        FAIL() << "Error parsing configuration file" << e.what();
    }

    ASSERT_EQ(output.str(), expected);
}

TEST(ConfigOldTest, test_default_orientation_ok) {
    std::stringbuf output;
    std::ostream out{&output};
    ConfigApplierAdapter applier{out};
    MockedResourceDatabase resourceDatabase{};
    std::istringstream in{R""""(
default_orientation horizontal
default_orientation vertical
default_orientation auto
)""""};

    std::string expected{R""""(cfg::default_orientation(horizontal)
cfg::default_orientation(vertical)
cfg::default_orientation(auto)
)""""};

    OldParser p("/tmp/foo", in, resourceDatabase, config_load_t::C_LOAD, applier);

    try {
        p.parse_file();
    } catch (std::exception &e) {
        FAIL() << "Error parsing configuration file" << e.what();
    }

    ASSERT_EQ(output.str(), expected);
}

TEST(ConfigOldTest, test_workspace_layout_ok) {
    std::stringbuf output;
    std::ostream out{&output};
    ConfigApplierAdapter applier{out};
    MockedResourceDatabase resourceDatabase{};
    std::istringstream in{R""""(
workspace_layout default
workspace_layout stacked
workspace_layout stacking
workspace_layout tabbed
)""""};

    std::string expected{R""""(cfg::workspace_layout(default)
cfg::workspace_layout(stacked)
cfg::workspace_layout(stacking)
cfg::workspace_layout(tabbed)
)""""};

    OldParser p("/tmp/foo", in, resourceDatabase, config_load_t::C_LOAD, applier);

    try {
        p.parse_file();
    } catch (std::exception &e) {
        FAIL() << "Error parsing configuration file" << e.what();
    }

    ASSERT_EQ(output.str(), expected);
}

TEST(ConfigOldTest, test_workspace_assignment_ok) {
    std::stringbuf output;
    std::ostream out{&output};
    ConfigApplierAdapter applier{out};
    MockedResourceDatabase resourceDatabase{};
    std::istringstream in{R""""(
workspace "3" output DP-1
workspace "3" output     	VGA-1
)""""};

    std::string expected{R""""(cfg::workspace(3, DP-1)
cfg::workspace(3, VGA-1)
)""""};

    OldParser p("/tmp/foo", in, resourceDatabase, config_load_t::C_LOAD, applier);

    try {
        p.parse_file();
    } catch (std::exception &e) {
        FAIL() << "Error parsing configuration file" << e.what();
    }

    ASSERT_EQ(output.str(), expected);
}

TEST(ConfigOldTest, test_new_window_ok) {
    std::stringbuf output;
    std::ostream out{&output};
    ConfigApplierAdapter applier{out};
    MockedResourceDatabase resourceDatabase{};
    std::istringstream in{R""""(
new_window 1pixel
new_window normal
new_window none
default_border 1pixel
default_border normal
default_border none
new_float 1pixel
new_float normal
new_float none
default_floating_border 1pixel
default_floating_border normal
default_floating_border none
)""""};

    std::string expected{R""""(cfg::default_border(new_window, 1pixel, -1)
cfg::default_border(new_window, normal, 2)
cfg::default_border(new_window, none, -1)
cfg::default_border(default_border, 1pixel, -1)
cfg::default_border(default_border, normal, 2)
cfg::default_border(default_border, none, -1)
cfg::default_border(new_float, 1pixel, -1)
cfg::default_border(new_float, normal, 2)
cfg::default_border(new_float, none, -1)
cfg::default_border(default_floating_border, 1pixel, -1)
cfg::default_border(default_floating_border, normal, 2)
cfg::default_border(default_floating_border, none, -1)
)""""};

    OldParser p("/tmp/foo", in, resourceDatabase, config_load_t::C_LOAD, applier);

    try {
        p.parse_file();
    } catch (std::exception &e) {
        FAIL() << "Error parsing configuration file" << e.what();
    }

    ASSERT_EQ(output.str(), expected);
}

TEST(ConfigOldTest, test_hide_edge_borders_ok) {
    std::stringbuf output;
    std::ostream out{&output};
    ConfigApplierAdapter applier{out};
    MockedResourceDatabase resourceDatabase{};
    std::istringstream in{R""""(
hide_edge_borders none
hide_edge_borders vertical
hide_edge_borders horizontal
hide_edge_borders both
hide_edge_borders smart
)""""};

    std::string expected{R""""(cfg::hide_edge_borders(none)
cfg::hide_edge_borders(vertical)
cfg::hide_edge_borders(horizontal)
cfg::hide_edge_borders(both)
cfg::hide_edge_borders(smart)
)""""};

    OldParser p("/tmp/foo", in, resourceDatabase, config_load_t::C_LOAD, applier);

    try {
        p.parse_file();
    } catch (std::exception &e) {
        FAIL() << "Error parsing configuration file" << e.what();
    }

    ASSERT_EQ(output.str(), expected);
}

TEST(ConfigOldTest, test_focus_follows_mouse_ok) {
    std::stringbuf output;
    std::ostream out{&output};
    ConfigApplierAdapter applier{out};
    MockedResourceDatabase resourceDatabase{};
    std::istringstream in{R""""(
focus_follows_mouse yes
focus_follows_mouse no
)""""};

    std::string expected{R""""(cfg::focus_follows_mouse(yes)
cfg::focus_follows_mouse(no)
)""""};

    OldParser p("/tmp/foo", in, resourceDatabase, config_load_t::C_LOAD, applier);

    try {
        p.parse_file();
    } catch (std::exception &e) {
        FAIL() << "Error parsing configuration file" << e.what();
    }

    ASSERT_EQ(output.str(), expected);
}

TEST(ConfigOldTest, test_mouse_warping_ok) {
    std::stringbuf output;
    std::ostream out{&output};
    ConfigApplierAdapter applier{out};
    MockedResourceDatabase resourceDatabase{};
    std::istringstream in{R""""(
mouse_warping output
mouse_warping none
)""""};

    std::string expected{R""""(cfg::mouse_warping(output)
cfg::mouse_warping(none)
)""""};

    OldParser p("/tmp/foo", in, resourceDatabase, config_load_t::C_LOAD, applier);

    try {
        p.parse_file();
    } catch (std::exception &e) {
        FAIL() << "Error parsing configuration file" << e.what();
    }

    ASSERT_EQ(output.str(), expected);
}

static std::string parser_calls(std::string str) {
    std::stringbuf output;
    std::ostream out{&output};
    ConfigApplierAdapter applier{out};
    MockedResourceDatabase resourceDatabase{};
    std::istringstream in{str};
    
    OldParser p("/tmp/foo", in, resourceDatabase, config_load_t::C_LOAD, applier);
    
    try {
        p.parse_file();
    } catch (std::exception &e) {
        return e.what();
    }

    return output.str();
}

TEST(ConfigOldTest, test_force_display_urgency_hint_300) {
    using namespace std::literals;
    
    std::stringbuf output;
    std::ostream out{&output};
    ConfigApplierAdapter applier{out};
    MockedResourceDatabase resourceDatabase{};
    
    ASSERT_EQ(parser_calls("force_display_urgency_hint 300"s), "cfg::force_display_urgency_hint(300)\n"s);

    ASSERT_EQ(parser_calls("force_display_urgency_hint 500 ms"s), "cfg::force_display_urgency_hint(500)\n"s);

    ASSERT_EQ(parser_calls("force_display_urgency_hint 700ms"s), "cfg::force_display_urgency_hint(700)\n"s);

    std::istringstream in{R""""(
force_display_urgency_hint 300
force_display_urgency_hint 500 ms
force_display_urgency_hint 700ms
force_display_urgency_hint 700
)""""};

    std::string expected{R"""(cfg::force_display_urgency_hint(300)
cfg::force_display_urgency_hint(500)
cfg::force_display_urgency_hint(700)
cfg::force_display_urgency_hint(700)
)"""};

    OldParser p("/tmp/foo", in, resourceDatabase, config_load_t::C_LOAD, applier);
    
    try {
        p.parse_file();
    } catch (std::exception &e) {
        FAIL() << "Error parsing configuration file" << e.what();
    }

    ASSERT_EQ(output.str(), expected);
}

TEST(ConfigOldTest, test_workspace_ok) {
    std::stringbuf output;
    std::ostream out{&output};
    ConfigApplierAdapter applier{out};
    MockedResourceDatabase resourceDatabase{};
    std::istringstream in{R""""(
workspace 3 output VGA-1
workspace "4: output" output VGA-2
workspace bleh output LVDS1/I_1
# See #3646
workspace foo output a b c "a b c"
)""""};

    std::string expected{R""""(cfg::workspace(3, VGA-1)
cfg::workspace(4: output, VGA-2)
cfg::workspace(bleh, LVDS1/I_1)
cfg::workspace(foo, a)
cfg::workspace((null), b)
cfg::workspace((null), c)
cfg::workspace((null), a b c)
)""""};

    OldParser p("/tmp/foo", in, resourceDatabase, config_load_t::C_LOAD, applier);

    try {
        p.parse_file();
    } catch (std::exception &e) {
        FAIL() << "Error parsing configuration file" << e.what();
    }

    ASSERT_EQ(output.str(), expected);
}

TEST(ConfigOldTest, test_ipc_socket_ok) {
    std::stringbuf output;
    std::ostream out{&output};
    ConfigApplierAdapter applier{out};
    MockedResourceDatabase resourceDatabase{};
    std::istringstream in{R""""(
ipc-socket /tmp/i3.sock
ipc_socket ~/.i3/i3.sock
)""""};

    std::string expected{R""""(cfg::ipc_socket(/tmp/i3.sock)
cfg::ipc_socket(~/.i3/i3.sock)
)""""};

    OldParser p("/tmp/foo", in, resourceDatabase, config_load_t::C_LOAD, applier);

    try {
        p.parse_file();
    } catch (std::exception &e) {
        FAIL() << "Error parsing configuration file" << e.what();
    }

    ASSERT_EQ(output.str(), expected);
}

TEST(ConfigOldTest, test_colors_ok) {
    std::stringbuf output;
    std::ostream out{&output};
    ConfigApplierAdapter applier{out};
    MockedResourceDatabase resourceDatabase{};
    std::istringstream in{R""""(
client.focused          #4c7899 #285577 #ffffff #2e9ef4 #b34d4c
client.focused_inactive #333333 #5f676a #ffffff #484e50
client.unfocused        #333333 #222222 #888888 #292d2e
client.urgent           #2f343a #900000 #ffffff #900000 #c00000
client.placeholder      #000000 #0c0c0c #ffffff #000000
)""""};

    std::string expected{R""""(cfg::color(client.focused, #4c7899, #285577, #ffffff, #2e9ef4, #b34d4c)
cfg::color(client.focused_inactive, #333333, #5f676a, #ffffff, #484e50, (null))
cfg::color(client.unfocused, #333333, #222222, #888888, #292d2e, (null))
cfg::color(client.urgent, #2f343a, #900000, #ffffff, #900000, #c00000)
cfg::color(client.placeholder, #000000, #0c0c0c, #ffffff, #000000, (null))
)""""};

    OldParser p("/tmp/foo", in, resourceDatabase, config_load_t::C_LOAD, applier);

    try {
        p.parse_file();
    } catch (std::exception &e) {
        FAIL() << "Error parsing configuration file" << e.what();
    }

    ASSERT_EQ(output.str(), expected);
}

TEST(ConfigOldTest, test_errors_dont_harm_subsequent_statements) {
    std::stringbuf output;
    std::ostream out{&output};
    ConfigApplierAdapter applier{out};
    MockedResourceDatabase resourceDatabase{};
    std::istringstream in{R""""(hide_edge_border both
client.focused          #4c7899 #285577 #ffffff #2e9ef4
)""""};

    std::string expected{R""""(CONFIG: Expected one of these tokens: <end>, '#', 'set ', 'set	', 'set_from_resource', 'include', 'bindsym', 'bindcode', 'bind', 'bar', 'font', 'mode', 'gaps', 'smart_borders', 'smart_gaps', 'floating_minimum_size', 'floating_maximum_size', 'floating_modifier', 'default_orientation', 'workspace_layout', 'default_border', 'new_window', 'default_floating_border', 'new_float', 'hide_edge_borders', 'for_window', 'assign', 'no_focus', 'focus_follows_mouse', 'mouse_warping', 'focus_wrapping', 'force_focus_wrapping', 'workspace_auto_back_and_forth', 'force_display_urgency_hint', 'focus_on_window_activation', 'title_align', 'workspace', 'ipc_socket', 'ipc-socket', 'ipc_kill_timeout', 'restart_state', 'popup_during_fullscreen', 'tiling_drag', 'exec_always', 'exec', 'client.background', 'client.focused_inactive', 'client.focused_tab_title', 'client.focused', 'client.unfocused', 'client.urgent', 'client.placeholder'
CONFIG: (in file /tmp/foo)
CONFIG: Line   1: hide_edge_border both
CONFIG:           ^^^^^^^^^^^^^^^^^^^^^
CONFIG: Line   2: client.focused          #4c7899 #285577 #ffffff #2e9ef4
)""""};

    OldParser p("/tmp/foo", in, resourceDatabase, config_load_t::C_LOAD, applier);

    try {
        p.parse_file();

        FAIL() << "Configuration should not have parsed successfully";
    } catch (std::exception &e) {
        ASSERT_EQ(e.what(), expected);
        ASSERT_EQ(output.str(), "cfg::color(client.focused, #4c7899, #285577, #ffffff, #2e9ef4, (null))\n");
    }
}

TEST(ConfigOldTest, test_errors_dont_harm_subsequent_statements2) {
    std::stringbuf output;
    std::ostream out{&output};
    ConfigApplierAdapter applier{out};
    MockedResourceDatabase resourceDatabase{};
    std::istringstream in{R""""(hide_edge_borders FOOBAR
client.focused          #4c7899 #285577 #ffffff #2e9ef4
)""""};

    std::string expected{R""""(CONFIG: Expected one of these tokens: 'none', 'vertical', 'horizontal', 'both', 'smart_no_gaps', 'smart', '1', 'yes', 'true', 'on', 'enable', 'active'
CONFIG: (in file /tmp/foo)
CONFIG: Line   1: hide_edge_borders FOOBAR
CONFIG:                             ^^^^^^
CONFIG: Line   2: client.focused          #4c7899 #285577 #ffffff #2e9ef4
)""""};

    OldParser p("/tmp/foo", in, resourceDatabase, config_load_t::C_LOAD, applier);

    try {
        p.parse_file();
        FAIL() << "Configuration should not have parsed successfully";
    } catch (std::exception &e) {
        ASSERT_EQ(e.what(), expected);
        ASSERT_EQ(output.str(), "cfg::color(client.focused, #4c7899, #285577, #ffffff, #2e9ef4, (null))\n");
    }
}

TEST(ConfigOldTest, test_semicolon_does_not_end_a_comment_line) {
    std::stringbuf output;
    std::ostream out{&output};
    ConfigApplierAdapter applier{out};
    MockedResourceDatabase resourceDatabase{};
    std::istringstream in{R""""(
# "foo" client.focused          #4c7899 #285577 #ffffff #2e9ef4
)""""};

    std::string expected{""};

    OldParser p("/tmp/foo", in, resourceDatabase, config_load_t::C_LOAD, applier);

    try {
        p.parse_file();
    } catch (std::exception &e) {
        FAIL() << "Error parsing configuration file" << e.what();
    }

    ASSERT_EQ(output.str(), expected);
}

TEST(ConfigOldTest, test_error_message_2_2_context_ok) {
    std::stringbuf output;
    std::ostream out{&output};
    ConfigApplierAdapter applier{out};
    MockedResourceDatabase resourceDatabase{};
    std::istringstream in{R""""(# i3 config file (v4)

font foobar

unknown qux

# yay
# this should not show up
)""""};

    std::string expected{R""""(CONFIG: Expected one of these tokens: <end>, '#', 'set ', 'set	', 'set_from_resource', 'include', 'bindsym', 'bindcode', 'bind', 'bar', 'font', 'mode', 'gaps', 'smart_borders', 'smart_gaps', 'floating_minimum_size', 'floating_maximum_size', 'floating_modifier', 'default_orientation', 'workspace_layout', 'default_border', 'new_window', 'default_floating_border', 'new_float', 'hide_edge_borders', 'for_window', 'assign', 'no_focus', 'focus_follows_mouse', 'mouse_warping', 'focus_wrapping', 'force_focus_wrapping', 'workspace_auto_back_and_forth', 'force_display_urgency_hint', 'focus_on_window_activation', 'title_align', 'workspace', 'ipc_socket', 'ipc-socket', 'ipc_kill_timeout', 'restart_state', 'popup_during_fullscreen', 'tiling_drag', 'exec_always', 'exec', 'client.background', 'client.focused_inactive', 'client.focused_tab_title', 'client.focused', 'client.unfocused', 'client.urgent', 'client.placeholder'
CONFIG: (in file /tmp/foo)
CONFIG: Line   3: font foobar
CONFIG: Line   4: 
CONFIG: Line   5: unknown qux
CONFIG:           ^^^^^^^^^^^
CONFIG: Line   6: 
CONFIG: Line   7: # yay
)""""};

    OldParser p("/tmp/foo", in, resourceDatabase, config_load_t::C_LOAD, applier);

    try {
        p.parse_file();
        FAIL() << "Configuration should not have parsed successfully";
    } catch (std::exception &e) {
        ASSERT_EQ(output.str(), "cfg::font(foobar)\n");
        ASSERT_EQ(e.what(), expected);
    }
}

TEST(ConfigOldTest, test_error_message_0_0_context_ok) {
    std::stringbuf output;
    std::ostream out{&output};
    ConfigApplierAdapter applier{out};
    MockedResourceDatabase resourceDatabase{};
    std::istringstream in{R""""(unknown qux
)""""};

    std::string expected{R""""(CONFIG: Expected one of these tokens: <end>, '#', 'set ', 'set	', 'set_from_resource', 'include', 'bindsym', 'bindcode', 'bind', 'bar', 'font', 'mode', 'gaps', 'smart_borders', 'smart_gaps', 'floating_minimum_size', 'floating_maximum_size', 'floating_modifier', 'default_orientation', 'workspace_layout', 'default_border', 'new_window', 'default_floating_border', 'new_float', 'hide_edge_borders', 'for_window', 'assign', 'no_focus', 'focus_follows_mouse', 'mouse_warping', 'focus_wrapping', 'force_focus_wrapping', 'workspace_auto_back_and_forth', 'force_display_urgency_hint', 'focus_on_window_activation', 'title_align', 'workspace', 'ipc_socket', 'ipc-socket', 'ipc_kill_timeout', 'restart_state', 'popup_during_fullscreen', 'tiling_drag', 'exec_always', 'exec', 'client.background', 'client.focused_inactive', 'client.focused_tab_title', 'client.focused', 'client.unfocused', 'client.urgent', 'client.placeholder'
CONFIG: (in file /tmp/foo)
CONFIG: Line   1: unknown qux
CONFIG:           ^^^^^^^^^^^
)""""};

    OldParser p("/tmp/foo", in, resourceDatabase, config_load_t::C_LOAD, applier);

    try {
        p.parse_file();

        FAIL() << "Configuration should not have parsed successfully";
    } catch (std::exception &e) {
        ASSERT_EQ(e.what(), expected);
    }
}

TEST(ConfigOldTest, test_error_message_1_0_context_ok) {
    std::stringbuf output;
    std::ostream out{&output};
    ConfigApplierAdapter applier{out};
    MockedResourceDatabase resourceDatabase{};
    std::istringstream in{R""""(# context before
unknown qux
)""""};

    std::string expected{R""""(CONFIG: Expected one of these tokens: <end>, '#', 'set ', 'set	', 'set_from_resource', 'include', 'bindsym', 'bindcode', 'bind', 'bar', 'font', 'mode', 'gaps', 'smart_borders', 'smart_gaps', 'floating_minimum_size', 'floating_maximum_size', 'floating_modifier', 'default_orientation', 'workspace_layout', 'default_border', 'new_window', 'default_floating_border', 'new_float', 'hide_edge_borders', 'for_window', 'assign', 'no_focus', 'focus_follows_mouse', 'mouse_warping', 'focus_wrapping', 'force_focus_wrapping', 'workspace_auto_back_and_forth', 'force_display_urgency_hint', 'focus_on_window_activation', 'title_align', 'workspace', 'ipc_socket', 'ipc-socket', 'ipc_kill_timeout', 'restart_state', 'popup_during_fullscreen', 'tiling_drag', 'exec_always', 'exec', 'client.background', 'client.focused_inactive', 'client.focused_tab_title', 'client.focused', 'client.unfocused', 'client.urgent', 'client.placeholder'
CONFIG: (in file /tmp/foo)
CONFIG: Line   1: # context before
CONFIG: Line   2: unknown qux
CONFIG:           ^^^^^^^^^^^
)""""};

    OldParser p("/tmp/foo", in, resourceDatabase, config_load_t::C_LOAD, applier);

    try {
        p.parse_file();

        FAIL() << "Configuration should not have parsed successfully";
    } catch (std::exception &e) {
        ASSERT_EQ(e.what(), expected);
    }
}

TEST(ConfigOldTest, test_error_message_0_1_context_ok) {
    std::stringbuf output;
    std::ostream out{&output};
    ConfigApplierAdapter applier{out};
    MockedResourceDatabase resourceDatabase{};
    std::istringstream in{R""""(unknown qux
# context after
)""""};

    std::string expected{R""""(CONFIG: Expected one of these tokens: <end>, '#', 'set ', 'set	', 'set_from_resource', 'include', 'bindsym', 'bindcode', 'bind', 'bar', 'font', 'mode', 'gaps', 'smart_borders', 'smart_gaps', 'floating_minimum_size', 'floating_maximum_size', 'floating_modifier', 'default_orientation', 'workspace_layout', 'default_border', 'new_window', 'default_floating_border', 'new_float', 'hide_edge_borders', 'for_window', 'assign', 'no_focus', 'focus_follows_mouse', 'mouse_warping', 'focus_wrapping', 'force_focus_wrapping', 'workspace_auto_back_and_forth', 'force_display_urgency_hint', 'focus_on_window_activation', 'title_align', 'workspace', 'ipc_socket', 'ipc-socket', 'ipc_kill_timeout', 'restart_state', 'popup_during_fullscreen', 'tiling_drag', 'exec_always', 'exec', 'client.background', 'client.focused_inactive', 'client.focused_tab_title', 'client.focused', 'client.unfocused', 'client.urgent', 'client.placeholder'
CONFIG: (in file /tmp/foo)
CONFIG: Line   1: unknown qux
CONFIG:           ^^^^^^^^^^^
CONFIG: Line   2: # context after
)""""};

    OldParser p("/tmp/foo", in, resourceDatabase, config_load_t::C_LOAD, applier);

    try {
        p.parse_file();

        FAIL() << "Configuration should not have parsed successfully";
    } catch (std::exception &e) {
        ASSERT_EQ(e.what(), expected);
    }
}

TEST(ConfigOldTest, test_error_message_0_2_context_ok) {
    std::stringbuf output;
    std::ostream out{&output};
    ConfigApplierAdapter applier{out};
    MockedResourceDatabase resourceDatabase{};
    std::istringstream in{R""""(unknown qux
# context after
# context 2 after
)""""};

    std::string expected{R""""(CONFIG: Expected one of these tokens: <end>, '#', 'set ', 'set	', 'set_from_resource', 'include', 'bindsym', 'bindcode', 'bind', 'bar', 'font', 'mode', 'gaps', 'smart_borders', 'smart_gaps', 'floating_minimum_size', 'floating_maximum_size', 'floating_modifier', 'default_orientation', 'workspace_layout', 'default_border', 'new_window', 'default_floating_border', 'new_float', 'hide_edge_borders', 'for_window', 'assign', 'no_focus', 'focus_follows_mouse', 'mouse_warping', 'focus_wrapping', 'force_focus_wrapping', 'workspace_auto_back_and_forth', 'force_display_urgency_hint', 'focus_on_window_activation', 'title_align', 'workspace', 'ipc_socket', 'ipc-socket', 'ipc_kill_timeout', 'restart_state', 'popup_during_fullscreen', 'tiling_drag', 'exec_always', 'exec', 'client.background', 'client.focused_inactive', 'client.focused_tab_title', 'client.focused', 'client.unfocused', 'client.urgent', 'client.placeholder'
CONFIG: (in file /tmp/foo)
CONFIG: Line   1: unknown qux
CONFIG:           ^^^^^^^^^^^
CONFIG: Line   2: # context after
CONFIG: Line   3: # context 2 after
)""""};

    OldParser p("/tmp/foo", in, resourceDatabase, config_load_t::C_LOAD, applier);

    try {
        p.parse_file();

        FAIL() << "Configuration should not have parsed successfully";
    } catch (std::exception &e) {
        ASSERT_EQ(e.what(), expected);
    }
}

TEST(ConfigOldTest, test_error_message_mode_block_ok) {
    std::stringbuf output;
    std::ostream out{&output};
    ConfigApplierAdapter applier{out};
    MockedResourceDatabase resourceDatabase{};
    std::istringstream in{R""""(mode "yo" {
    bindsym x resize shrink left
    unknown qux
}
)""""};

    std::string expected{R""""(CONFIG: Expected one of these tokens: <end>, '#', 'set', 'bindsym', 'bindcode', 'bind', '}'
CONFIG: (in file /tmp/foo)
CONFIG: Line   1: mode "yo" {
CONFIG: Line   2:     bindsym x resize shrink left
CONFIG: Line   3:     unknown qux
CONFIG:               ^^^^^^^^^^^
CONFIG: Line   4: }
)""""};

    OldParser p("/tmp/foo", in, resourceDatabase, config_load_t::C_LOAD, applier);

    try {
        p.parse_file();

        FAIL() << "Configuration should not have parsed successfully";
    } catch (std::exception &e) {
        ASSERT_EQ(e.what(), expected);
        ASSERT_EQ(output.str(), "cfg::enter_mode((null), yo)\ncfg::mode_binding(bindsym, (null), x, (null), (null), (null), (null), resize shrink left)\n");
    }
}

TEST(ConfigOldTest, test_error_message_bar_block_ok) {
    std::stringbuf output;
    std::ostream out{&output};
    ConfigApplierAdapter applier{out};
    MockedResourceDatabase resourceDatabase{};
    std::istringstream in{R""""(bar {
    output LVDS-1
    unknown qux
}
)""""};

    std::string expected{R""""(CONFIG: Expected one of these tokens: <end>, '#', 'set', 'i3bar_command', 'status_command', 'workspace_command', 'socket_path', 'mode', 'hidden_state', 'id', 'modifier', 'wheel_up_cmd', 'wheel_down_cmd', 'bindsym', 'position', 'output', 'tray_output', 'tray_padding', 'font', 'separator_symbol', 'binding_mode_indicator', 'workspace_buttons', 'workspace_min_width', 'strip_workspace_numbers', 'strip_workspace_name', 'verbose', 'height', 'padding', 'colors', '}'
CONFIG: (in file /tmp/foo)
CONFIG: Line   1: bar {
CONFIG: Line   2:     output LVDS-1
CONFIG: Line   3:     unknown qux
CONFIG:               ^^^^^^^^^^^
CONFIG: Line   4: }
)""""};

    OldParser p("/tmp/foo", in, resourceDatabase, config_load_t::C_LOAD, applier);

    try {
        p.parse_file();

        FAIL() << "Configuration should not have parsed successfully";
    } catch (std::exception &e) {
        ASSERT_EQ(e.what(), expected);
        //ASSERT_EQ(output.str(), "cfg::bar_start()\ncfg::bar_output(LVDS-1)\ncfg::bar_finish()\n");
    }
}
