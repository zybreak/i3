# -*- mode: meson -*-

libi3srcs = [
  'src/boolstr.c',
  'src/create_socket.c',
  'src/dpi.c',
  'src/draw_util.c',
  'src/fake_configure_notify.c',
  'src/font.c',
  'src/format_placeholders.c',
  'src/get_colorpixel.c',
  'src/get_config_path.c',
  'src/get_exe_path.c',
  'src/get_mod_mask.c',
  'src/get_process_filename.c',
  'src/get_visualtype.c',
  'src/g_utf8_make_valid.c',
  'src/ipc_connect.c',
  'src/ipc_recv_message.c',
  'src/ipc_send_message.c',
  'src/is_debug_build.c',
  'src/path_exists.c',
  'src/resolve_tilde.c',
  'src/root_atom_contents.c',
  'src/safewrappers.c',
  'src/string.c',
  'src/ucs2_conversion.c',
  'src/nonblock.c',
  'src/screenshot_wallpaper.c',
  'src/is_background_set.c',
]

if not cdata.get('HAVE_STRNDUP')
  libi3srcs += 'src/strndup.c'
endif

if not cdata.get('HAVE_MKDIRP')
  libi3srcs += 'src/mkdirp.c'
endif

libi3 = static_library(
  'i3',
  libi3srcs,
  include_directories: '../include',
  dependencies: [
    pangocairo_dep,
    config_h,
  ],
)
