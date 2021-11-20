# -*- mode: meson -*-

executable(
  'i3bar',
  [
    'src/child.cpp',
    'src/config.cpp',
    'src/ipc.cpp',
    'src/main.cpp',
    'src/mode.cpp',
    'src/outputs.cpp',
    'src/parse_json_header.cpp',
    'src/workspaces.cpp',
    'src/xcb.cpp',
  ],
  install: true,
  include_directories: [ '../include', '../i3/include', '../libi3/include', 'src/include', config_h_dir ],
  dependencies: common_deps,
  link_with: libi3,
)
