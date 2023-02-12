# -*- mode: meson -*-

libi3ipcsrcs = [
  'src/ipc_connect.cpp',
  'src/ipc_recv_message.cpp',
  'src/ipc_send_message.cpp',
]

libi3ipc = static_library(
  'i3ipc',
  libi3ipcsrcs,
  include_directories: [ 'include', config_h_dir ],
  dependencies: [
  ],
)

install_headers(
  'include/i3-ipc.h',
  subdir: 'i3',
)
