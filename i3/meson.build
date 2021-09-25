# -*- mode: meson -*-

i3_srcs = [
  'src/assignments.c',
  'src/bindings.c',
  'src/click.c',
  'src/commands.c',
  'src/commands_parser.c',
  'src/con.c',
  'src/config.c',
  'src/config_directives.c',
  'src/config_parser.c',
  'src/display_version.c',
  'src/drag.c',
  'src/ewmh.c',
  'src/fake_outputs.c',
  'src/floating.c',
  'src/handlers.c',
  'src/ipc.c',
  'src/key_press.c',
  'src/load_layout.c',
  'src/log.c',
  'src/main.c',
  'src/manage.c',
  'src/match.c',
  'src/move.c',
  'src/output.c',
  'src/randr.c',
  'src/regex.c',
  'src/render.c',
  'src/resize.c',
  'src/restore_layout.c',
  'src/scratchpad.c',
  'src/sighandler.c',
  'src/startup.c',
  'src/sync.c',
  'src/tree.c',
  'src/util.c',
  'src/version.c',
  'src/window.c',
  'src/workspace.c',
  'src/x.c',
  'src/xcb.c',
  'src/xcursor.c',
]

i3_srcs += command_parser
i3_srcs += config_parser

i3 = executable(
  'i3',
  i3_srcs,
  install: true,
  include_directories: [ '../include', '../libi3/include', 'include', 'src/include', config_h_dir ],
  dependencies: common_deps,
  link_with: libi3,
)

install_headers(
  'include/i3/ipc.h',
  subdir: 'i3',
)
