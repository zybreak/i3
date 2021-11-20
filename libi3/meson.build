# -*- mode: meson -*-

libi3srcs = [
  'src/boolstr.cpp',
  'src/dpi.cpp',
  'src/draw_util.cpp',
  'src/fake_configure_notify.cpp',
  'src/font.cpp',
  'src/format_placeholders.cpp',
  'src/get_colorpixel.cpp',
  'src/get_config_path.cpp',
  'src/get_exe_path.cpp',
  'src/get_mod_mask.cpp',
  'src/get_process_filename.cpp',
  'src/get_visualtype.cpp',
  'src/g_utf8_make_valid.cpp',
  'src/ipc_connect.cpp',
  'src/ipc_recv_message.cpp',
  'src/ipc_send_message.cpp',
  'src/is_debug_build.cpp',
  'src/path_exists.cpp',
  'src/resolve_tilde.cpp',
  'src/root_atom_contents.cpp',
  'src/safewrappers.cpp',
  'src/string.cpp',
  'src/ucs2_conversion.cpp',
  'src/nonblock.cpp',
  'src/screenshot_wallpaper.cpp',
  'src/is_background_set.cpp',
]

if not cdata.get('HAVE_STRNDUP')
  libi3srcs += 'src/strndup.cpp'
endif

if not cdata.get('HAVE_MKDIRP')
  libi3srcs += 'src/mkdirp.cpp'
endif

libi3 = static_library(
  'i3',
  libi3srcs,
  include_directories: [ '../include', '../i3/include', 'include', config_h_dir ],
  dependencies: [
    pangocairo_dep,
  ],
)
