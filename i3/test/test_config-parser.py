#!/usr/bin/python
# vim:ts=4:sw=4:expandtab
#
# Please read the following documents before working on tests:
# • https://build.i3wm.org/docs/testsuite.html
#   (or docs/testsuite)
#
# • https://build.i3wm.org/docs/lib-i3test.html
#   (alternatively: perldoc ./testcases/lib/i3test.pm)
#
# • https://build.i3wm.org/docs/ipc.html
#   (or docs/ipc)
#
# • http://onyxneon.com/books/modern_perl/modern_perl_a4.pdf
#   (unless you are already familiar with Perl)
#
# Tests the standalone parser binary to see if it calls the right code when
# confronted with various commands, if it prints proper error messages for
# wrong commands and if it terminates in every case.
#
import os
import subprocess

print("CWD: " + os.getcwd())

def parser_calls(command):
    process = subprocess.Popen(["./test.config_parser"], stdin=subprocess.PIPE, stderr=subprocess.PIPE,
                               stdout=subprocess.PIPE)
    stdout, stderr = process.communicate(input=command.encode('utf-8').strip())

    return "".join([x for x in stderr.decode('utf-8').splitlines(True) if
                    not x.startswith("cfg::criteria_init") and not x.startswith(
                        "cfg::criteria_pop_state") and not x.startswith("#")])


def assert_is(actual, expected, name):
    expected = "".join([s for s in expected.splitlines(True) if s.strip("\r\n").strip()])
    actual = "".join([s for s in actual.splitlines(True) if s.strip("\r\n").strip()])

    if expected == actual:
        print(f"OK {name}")
    else:
        print(f"FAILED {name}")
        print(" expected:\n -> " + " -> ".join([s for s in expected.splitlines(True)]))
        print(" actual:\n -> " + " -> ".join([s for s in actual.splitlines(True)]))
        exit(1)

config = """
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
"""

expected = """
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
"""

assert_is(parser_calls(config), expected, 'mode bindings ok')

################################################################################
# exec and exec_always
################################################################################

config = """
exec geeqie
exec --no-startup-id /tmp/foo.sh
exec_always firefox
exec_always --no-startup-id /tmp/bar.sh
"""

expected = """
cfg::exec(exec, (null), geeqie)
cfg::exec(exec, --no-startup-id, /tmp/foo.sh)
cfg::exec(exec_always, (null), firefox)
cfg::exec(exec_always, --no-startup-id, /tmp/bar.sh)
"""

assert_is(parser_calls(config), expected, 'exec okay')

################################################################################
# for_window
################################################################################

config = """
for_window [] nop empty
for_window [class="^Chrome"] floating enable
for_window [class=^Chrome] floating enable
for_window [floating_from   = "auto" class= ==Class==  ] nop floating
for_window [tiling_from=auto class="==Class=="]nop floating
"""

expected = """
cfg::for_window(nop empty)
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
"""

assert_is(parser_calls(config), expected, 'for_window okay')

config = """
for_window [tiling_from=typo] nop typo
for_window [tiling_from="typo"] nop typo
"""

expected = """
ERROR: CONFIG: Expected one of these tokens: '"', 'auto', 'user'
ERROR: CONFIG: (in file <stdin>)
ERROR: CONFIG: Line   1: for_window [tiling_from=typo] nop typo
ERROR: CONFIG:                                   ^^^^^^^^^^^^^^
ERROR: CONFIG: Line   2: for_window [tiling_from="typo"] nop typo
ERROR: CONFIG: Expected one of these tokens: 'auto', 'user'
ERROR: CONFIG: (in file <stdin>)
ERROR: CONFIG: Line   1: for_window [tiling_from=typo] nop typo
ERROR: CONFIG: Line   2: for_window [tiling_from="typo"] nop typo
ERROR: CONFIG:                                    ^^^^^^^^^^^^^^^
"""

assert_is(parser_calls(config), expected, 'for_window errors okay')

################################################################################
# assign
################################################################################

config = """
assign [class="^Chrome"] 4
assign [class="^Chrome"] workspace number 3
assign [class="^Chrome"] named workspace
assign [class="^Chrome"] "quoted named workspace"
assign [class="^Chrome"] → "quoted named workspace"
"""

expected = """
cfg::criteria_add(class, ^Chrome)
cfg::assign(4, 0)
cfg::criteria_add(class, ^Chrome)
cfg::assign(3, 1)
cfg::criteria_add(class, ^Chrome)
cfg::assign(named workspace, 0)
cfg::criteria_add(class, ^Chrome)
cfg::assign(quoted named workspace, 0)
cfg::criteria_add(class, ^Chrome)
cfg::assign(quoted named workspace, 0)
"""

assert_is(parser_calls(config), expected, 'for_window okay')

################################################################################
# floating_minimum_size / floating_maximum_size
################################################################################

config = """
floating_minimum_size 80x55
floating_minimum_size 80    x  55
floating_maximum_size 73 x 10
"""

expected = """
cfg::floating_minimum_size(80, 55)
cfg::floating_minimum_size(80, 55)
cfg::floating_maximum_size(73, 10)
"""

assert_is(parser_calls(config), expected, 'floating_minimum_size ok')

################################################################################
# popup_during_fullscreen
################################################################################

config = """
popup_during_fullscreen ignore
popup_during_fullscreen leave_fullscreen
popup_during_fullscreen SMArt
"""

expected = """
cfg::popup_during_fullscreen(ignore)
cfg::popup_during_fullscreen(leave_fullscreen)
cfg::popup_during_fullscreen(smart)
"""

assert_is(parser_calls(config), expected, 'popup_during_fullscreen ok')

################################################################################
# floating_modifier
################################################################################

config = """
floating_modifier Mod1
floating_modifier mOd1
"""

expected = """
cfg::floating_modifier(Mod1)
cfg::floating_modifier(Mod1)
"""

assert_is(parser_calls(config), expected, 'floating_modifier ok')

################################################################################
# default_orientation
################################################################################

config = """
default_orientation horizontal
default_orientation vertical
default_orientation auto
"""

expected = """
cfg::default_orientation(horizontal)
cfg::default_orientation(vertical)
cfg::default_orientation(auto)
"""

assert_is(parser_calls(config), expected, 'default_orientation ok')

################################################################################
# workspace_layout
################################################################################

config = """
workspace_layout default
workspace_layout stacked
workspace_layout stacking
workspace_layout tabbed
"""

expected = """
cfg::workspace_layout(default)
cfg::workspace_layout(stacked)
cfg::workspace_layout(stacking)
cfg::workspace_layout(tabbed)
"""

assert_is(parser_calls(config), expected, 'workspace_layout ok')

################################################################################
# workspace assignments, with trailing whitespace (ticket #921)
################################################################################

config = """
workspace "3" output DP-1
workspace "3" output     	VGA-1
"""

expected = """
cfg::workspace(3, DP-1)
cfg::workspace(3, VGA-1)
"""

assert_is(parser_calls(config), expected, 'workspace assignment ok')

################################################################################
# new_window
################################################################################

config = """
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
"""

expected = """
cfg::default_border(new_window, 1pixel, -1)
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
"""

# TODO: are there no tests for "border pixel 1" etc?

assert_is(parser_calls(config), expected, 'new_window ok')

################################################################################
# hide_edge_borders
################################################################################

config = """
hide_edge_borders none
hide_edge_borders vertical
hide_edge_borders horizontal
hide_edge_borders both
hide_edge_borders smart
"""

expected = """
cfg::hide_edge_borders(none)
cfg::hide_edge_borders(vertical)
cfg::hide_edge_borders(horizontal)
cfg::hide_edge_borders(both)
cfg::hide_edge_borders(smart)
"""

assert_is(parser_calls(config), expected, 'hide_edge_borders ok')

################################################################################
# focus_follows_mouse
################################################################################

config = """
focus_follows_mouse yes
focus_follows_mouse no
"""

expected = """
cfg::focus_follows_mouse(yes)
cfg::focus_follows_mouse(no)
"""

assert_is(parser_calls(config), expected, 'focus_follows_mouse ok')

################################################################################
# mouse_warping
################################################################################

config = """
mouse_warping output
mouse_warping none
"""

expected = """
cfg::mouse_warping(output)
cfg::mouse_warping(none)
"""

assert_is(parser_calls(config), expected, 'mouse_warping ok')

################################################################################
# force_display_urgency_hint
################################################################################

assert_is(parser_calls('force_display_urgency_hint 300'), "cfg::force_display_urgency_hint(300)\n",
          'force_display_urgency_hint ok')

assert_is(parser_calls('force_display_urgency_hint 500 ms'), "cfg::force_display_urgency_hint(500)\n",
          'force_display_urgency_hint ok')

assert_is(parser_calls('force_display_urgency_hint 700ms'), "cfg::force_display_urgency_hint(700)\n",
          'force_display_urgency_hint ok')

config = """
force_display_urgency_hint 300
force_display_urgency_hint 500 ms
force_display_urgency_hint 700ms
force_display_urgency_hint 700
"""

expected = """
cfg::force_display_urgency_hint(300)
cfg::force_display_urgency_hint(500)
cfg::force_display_urgency_hint(700)
cfg::force_display_urgency_hint(700)
"""

assert_is(parser_calls(config), expected, 'force_display_urgency_hint ok')

################################################################################
# workspace
################################################################################

config = """
workspace 3 output VGA-1
workspace "4: output" output VGA-2
workspace bleh output LVDS1/I_1
# See #3646
workspace foo output a b c "a b c"
"""

expected = """
cfg::workspace(3, VGA-1)
cfg::workspace(4: output, VGA-2)
cfg::workspace(bleh, LVDS1/I_1)
cfg::workspace(foo, a)
cfg::workspace((null), b)
cfg::workspace((null), c)
cfg::workspace((null), a b c)
"""

assert_is(parser_calls(config), expected, 'workspace ok')

################################################################################
# ipc-socket
################################################################################

config = """
ipc-socket /tmp/i3.sock
ipc_socket ~/.i3/i3.sock
"""

expected = """
cfg::ipc_socket(/tmp/i3.sock)
cfg::ipc_socket(~/.i3/i3.sock)
"""

assert_is(parser_calls(config), expected, 'ipc-socket ok')

################################################################################
# colors
################################################################################

config = """
client.focused          #4c7899 #285577 #ffffff #2e9ef4 #b34d4c
client.focused_inactive #333333 #5f676a #ffffff #484e50
client.unfocused        #333333 #222222 #888888 #292d2e
client.urgent           #2f343a #900000 #ffffff #900000 #c00000
client.placeholder      #000000 #0c0c0c #ffffff #000000
"""

expected = """
cfg::color(client.focused, #4c7899, #285577, #ffffff, #2e9ef4, #b34d4c)
cfg::color(client.focused_inactive, #333333, #5f676a, #ffffff, #484e50, NULL)
cfg::color(client.unfocused, #333333, #222222, #888888, #292d2e, NULL)
cfg::color(client.urgent, #2f343a, #900000, #ffffff, #900000, #c00000)
cfg::color(client.placeholder, #000000, #0c0c0c, #ffffff, #000000, NULL)
"""

assert_is(parser_calls(config), expected, 'colors ok')

################################################################################
# Verify that errors don’t harm subsequent valid statements
################################################################################

config = """
hide_edge_border both
client.focused          #4c7899 #285577 #ffffff #2e9ef4
"""

expected_all_tokens = "ERROR: CONFIG: Expected one of these tokens: <end>, '#', 'set ', 'set	', 'set_from_resource', 'include', 'bindsym', 'bindcode', 'bind', 'bar', 'font', 'mode', 'floating_minimum_size', 'floating_maximum_size', 'floating_modifier', 'default_orientation', 'workspace_layout', 'default_border', 'new_window', 'default_floating_border', 'new_float', 'hide_edge_borders', 'for_window', 'assign', 'no_focus', 'focus_follows_mouse', 'mouse_warping', 'focus_wrapping', 'force_focus_wrapping', 'workspace_auto_back_and_forth', 'force_display_urgency_hint', 'focus_on_window_activation', 'title_align', 'workspace', 'ipc_socket', 'ipc-socket', 'ipc_kill_timeout', 'restart_state', 'popup_during_fullscreen', 'exec_always', 'exec', 'client.background', 'client.focused_inactive', 'client.focused', 'client.unfocused', 'client.urgent', 'client.placeholder'"

expected_end = """
ERROR: CONFIG: (in file <stdin>)
ERROR: CONFIG: Line   1: hide_edge_border both
ERROR: CONFIG:           ^^^^^^^^^^^^^^^^^^^^^
ERROR: CONFIG: Line   2: client.focused          #4c7899 #285577 #ffffff #2e9ef4
cfg::color(client.focused, #4c7899, #285577, #ffffff, #2e9ef4, NULL)
"""

expected = f"{expected_all_tokens}{expected_end}"

assert_is(parser_calls(config), expected, 'errors dont harm subsequent statements')

config = """
hide_edge_borders FOOBAR
client.focused          #4c7899 #285577 #ffffff #2e9ef4
"""

expected = """
ERROR: CONFIG: Expected one of these tokens: 'none', 'vertical', 'horizontal', 'both', 'smart', '1', 'yes', 'true', 'on', 'enable', 'active'
ERROR: CONFIG: (in file <stdin>)
ERROR: CONFIG: Line   1: hide_edge_borders FOOBAR
ERROR: CONFIG:                             ^^^^^^
ERROR: CONFIG: Line   2: client.focused          #4c7899 #285577 #ffffff #2e9ef4
cfg::color(client.focused, #4c7899, #285577, #ffffff, #2e9ef4, NULL)
"""

assert_is(parser_calls(config), expected, 'errors dont harm subsequent statements')

################################################################################
# Regression: semicolons end comments, but shouldn’t
################################################################################

config = """
# "foo" client.focused          #4c7899 #285577 #ffffff #2e9ef4
"""

expected = """

"""

assert_is(parser_calls(config), expected, 'semicolon does not end a comment line')

################################################################################
# Error message with 2+2 lines of context
################################################################################

config = """
# i3 config file (v4)

font foobar

unknown qux

# yay
# this should not show up
"""

expected_head = """
cfg::font(foobar)
"""

expected_tail = """
ERROR: CONFIG: (in file <stdin>)
ERROR: CONFIG: Line   3: font foobar
ERROR: CONFIG: Line   4: 
ERROR: CONFIG: Line   5: unknown qux
ERROR: CONFIG:           ^^^^^^^^^^^
ERROR: CONFIG: Line   6: 
ERROR: CONFIG: Line   7: # yay
"""

expected = f"{expected_head}{expected_all_tokens}{expected_tail}"

assert_is(parser_calls(config), expected, 'error message (2+2 context) ok')

################################################################################
# Error message with 0+0 lines of context
################################################################################

config = """
unknown qux
"""

expected_tail = """
ERROR: CONFIG: (in file <stdin>)
ERROR: CONFIG: Line   1: unknown qux
ERROR: CONFIG:           ^^^^^^^^^^^
"""

expected = f"{expected_all_tokens}{expected_tail}"

assert_is(parser_calls(config), expected, 'error message (0+0 context) ok')

################################################################################
# Error message with 1+0 lines of context
################################################################################

config = """
# context before
unknown qux
"""

expected_tail = """
ERROR: CONFIG: (in file <stdin>)
ERROR: CONFIG: Line   1: # context before
ERROR: CONFIG: Line   2: unknown qux
ERROR: CONFIG:           ^^^^^^^^^^^
"""

expected = f"{expected_all_tokens}{expected_tail}"

assert_is(parser_calls(config), expected, 'error message (1+0 context) ok')

################################################################################
# Error message with 0+1 lines of context
################################################################################

config = """
unknown qux
# context after
"""

expected_tail = """
ERROR: CONFIG: (in file <stdin>)
ERROR: CONFIG: Line   1: unknown qux
ERROR: CONFIG:           ^^^^^^^^^^^
ERROR: CONFIG: Line   2: # context after
"""

expected = f"{expected_all_tokens}{expected_tail}"

assert_is(parser_calls(config), expected, 'error message (0+1 context) ok')

################################################################################
# Error message with 0+2 lines of context
################################################################################

config = """
unknown qux
# context after
# context 2 after
"""

expected_tail = """
ERROR: CONFIG: (in file <stdin>)
ERROR: CONFIG: Line   1: unknown qux
ERROR: CONFIG:           ^^^^^^^^^^^
ERROR: CONFIG: Line   2: # context after
ERROR: CONFIG: Line   3: # context 2 after
"""

expected = f"{expected_all_tokens}{expected_tail}"

assert_is(parser_calls(config), expected, 'error message (0+2 context) ok')

################################################################################
# Error message within mode blocks
################################################################################

config = """
mode "yo" {
    bindsym x resize shrink left
    unknown qux
}
"""

expected = """
cfg::enter_mode((null), yo)
cfg::mode_binding(bindsym, (null), x, (null), (null), (null), (null), resize shrink left)
ERROR: CONFIG: Expected one of these tokens: <end>, '#', 'set', 'bindsym', 'bindcode', 'bind', '}'
ERROR: CONFIG: (in file <stdin>)
ERROR: CONFIG: Line   1: mode "yo" {
ERROR: CONFIG: Line   2:     bindsym x resize shrink left
ERROR: CONFIG: Line   3:     unknown qux
ERROR: CONFIG:               ^^^^^^^^^^^
ERROR: CONFIG: Line   4: }
"""

assert_is(parser_calls(config), expected, 'error message (mode block) ok')

################################################################################
# Error message within bar blocks
################################################################################

config = """
bar {
    output LVDS-1
    unknown qux
}
"""

expected = """
cfg::bar_start()
cfg::bar_output(LVDS-1)
ERROR: CONFIG: Expected one of these tokens: <end>, '#', 'set', 'i3bar_command', 'status_command', 'socket_path', 'mode', 'hidden_state', 'id', 'modifier', 'wheel_up_cmd', 'wheel_down_cmd', 'bindsym', 'position', 'output', 'tray_output', 'tray_padding', 'font', 'separator_symbol', 'binding_mode_indicator', 'workspace_buttons', 'workspace_min_width', 'strip_workspace_numbers', 'strip_workspace_name', 'verbose', 'colors', '}'
ERROR: CONFIG: (in file <stdin>)
ERROR: CONFIG: Line   1: bar {
ERROR: CONFIG: Line   2:     output LVDS-1
ERROR: CONFIG: Line   3:     unknown qux
ERROR: CONFIG:               ^^^^^^^^^^^
ERROR: CONFIG: Line   4: }
cfg::bar_finish()
"""

assert_is(parser_calls(config), expected, 'error message (bar block) ok')
