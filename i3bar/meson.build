# -*- mode: meson -*-

executable(
  'i3bar',
  [
    'src/child.c',
    'src/config.c',
    'src/ipc.c',
    'src/main.c',
    'src/mode.c',
    'src/outputs.c',
    'src/parse_json_header.c',
    'src/workspaces.c',
    'src/xcb.c',
  ],
  install: true,
  include_directories: [ '../include', '../i3/include', '../libi3/include', 'src/include', config_h_dir ],
  dependencies: common_deps,
  link_with: libi3,
)
