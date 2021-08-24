# -*- mode: meson -*-

executable(
  'i3-nagbar',
  [
    'i3-nagbar-atoms.xmacro.h',
    'main.c',
  ],
  install: true,
  include_directories: [ '../include', '../libi3/include', config_h_dir ],
  dependencies: common_deps,
  link_with: libi3,
)
