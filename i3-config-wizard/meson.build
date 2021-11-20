# -*- mode: meson -*-

executable(
  'i3-config-wizard',
  [
    'i3-config-wizard-atoms.xmacro.h',
    'main.cpp',
    'xcb.h',
    config_parser,
  ],
  install: true,
  include_directories: [ '../include', '../libi3/include', config_h_dir ],
  dependencies: common_deps,
  link_with: libi3,
)
