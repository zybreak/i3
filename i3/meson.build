# -*- mode: meson -*-

i3_srcs = [
  'src/assignments.cpp',
  'src/bindings.cpp',
  'src/click.cpp',
  'src/commands.cpp',
  'src/commands_parser.cpp',
  'src/con.cpp',
  'src/config.cpp',
  'src/config_directives.cpp',
  'src/config_parser.cpp',
  'src/display_version.cpp',
  'src/drag.cpp',
  'src/ewmh.cpp',
  'src/fake_outputs.cpp',
  'src/floating.cpp',
  'src/handlers.cpp',
  'src/ipc.cpp',
  'src/key_press.cpp',
  'src/load_layout.cpp',
  'src/log.cpp',
  'src/main.cpp',
  'src/manage.cpp',
  'src/match.cpp',
  'src/move.cpp',
  'src/output.cpp',
  'src/randr.cpp',
  'src/regex.cpp',
  'src/render.cpp',
  'src/resize.cpp',
  'src/restore_layout.cpp',
  'src/scratchpad.cpp',
  'src/sighandler.cpp',
  'src/startup.cpp',
  'src/sync.cpp',
  'src/tree.cpp',
  'src/util.cpp',
  'src/version.cpp',
  'src/window.cpp',
  'src/workspace.cpp',
  'src/x.cpp',
  'src/xcb.cpp',
  'src/xcursor.cpp',
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
