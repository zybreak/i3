# -*- mode: meson -*-

executable(
  'i3-dump-log',
  'i3-dump-log.c',
  install: true,
  include_directories: [ '../include', '../libi3/include', '../i3/include' ],
  dependencies: common_deps,
  link_with: libi3,
)

install_data(
  [
    'i3-save-tree',
    'i3-sensible-terminal',
  ],
  install_dir: 'bin',
)