# -*- mode: meson -*-

executable(
  'i3-config-wizard',
  [
    'i3-config-wizard-atoms.xmacro.h',
    'main.c',
    'xcb.h',
    config_parser,
  ],
  install: true,
  include_directories: include_directories('../include'),
  dependencies: common_deps,
  link_with: libi3,
)
