# -*- mode: meson -*-

libi3srcs = [
  'src/dpi.cpp',
  'src/draw_util.cpp',
  'src/font.cpp',
  'src/format_placeholders.cpp',
  'src/get_colorpixel.cpp',
  'src/get_config_path.cpp',
  'src/get_mod_mask.cpp',
  'src/get_process_filename.cpp',
  'src/get_visualtype.cpp',
  'src/log.cpp',
  'src/resolve_tilde.cpp',
  'src/root_atom_contents.cpp',
  'src/safewrappers.cpp',
  'src/string.cpp',
  'src/ucs2_conversion.cpp',
  'src/mkdirp.cpp',
]

libi3 = static_library(
  'i3',
  libi3srcs,
  include_directories: [ '../lib', 'include', config_h_dir ],
  dependencies: [
    pangocairo_dep,
  ],
)
