# -*- mode: meson -*-

executable(
  'i3-dump-log',
  'main.c',
  install: true,
  include_directories: [ '../include', '../libi3/include', '../i3/include' ],
  dependencies: common_deps,
  link_with: libi3,
)
