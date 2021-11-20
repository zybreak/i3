# -*- mode: meson -*-

# Style objective: be consistent with what mesonbuild.com documents/uses, and/or
# the meson book: https://meson-manual.com/

project(
  'i3',
  'cpp',
  version: '4.19.1',
  default_options: [
    'cpp_std=c++20',
    'warning_level=1',  # enable all warnings (-Wall)
  ],
  meson_version: '>=0.53.0',
)

cpp = meson.get_compiler('cpp')
add_project_arguments(cpp.get_supported_arguments(['-Wunused-value']), language: 'cpp')

# https://github.com/mesonbuild/meson/issues/2166#issuecomment-629696911
# meson.add_dist_script('meson/meson-dist-script')

################################################################################
# Version handling
################################################################################

cdata = configuration_data()

version_array = meson.project_version().split('.')
cdata.set('MAJOR_VERSION', version_array[0].to_int())
cdata.set('MINOR_VERSION', version_array[1].to_int())
if version_array.length() > 2
  cdata.set('PATCH_VERSION', version_array[2].to_int())
else
  cdata.set('PATCH_VERSION', 0)
endif
cdata.set_quoted('I3_VERSION', meson.project_version())
cdata.set_quoted('SYSCONFDIR', join_paths(get_option('prefix'), get_option('sysconfdir')))

if get_option('b_sanitize').split(',').contains('address')
  cdata.set('I3_ASAN_ENABLED', 1)
endif

cdata.set('HAVE_STRNDUP', cpp.has_function('strndup'))
cdata.set('HAVE_MKDIRP', cpp.has_function('mkdirp'))

configure_file(
  output: 'config.h',
  configuration: cdata,
)

config_h_dir = include_directories('.')

# Required for e.g. struct ucred to be defined as per unix(7).
#add_project_arguments('-D_GNU_SOURCE', language: 'c')

# https://mesonbuild.com/howtox.html#add-math-library-lm-portably
m_dep = cpp.find_library('m', required: false)
rt_dep = cpp.find_library('rt', required: false)
iconv_dep = cpp.find_library('iconv', required: false)

libsn_dep = dependency('libstartup-notification-1.0', method: 'pkg-config')
xcb_dep = dependency('xcb', method: 'pkg-config')
xcb_xkb_dep = dependency('xcb-xkb', method: 'pkg-config')
xcb_randr_dep = dependency('xcb-randr', method: 'pkg-config')
xcb_shape_dep = dependency('xcb-shape', method: 'pkg-config')
xcb_util_dep = dependency('xcb-util', method: 'pkg-config')
xcb_util_cursor_dep = dependency('xcb-cursor', method: 'pkg-config')
xcb_util_keysyms_dep = dependency('xcb-keysyms', method: 'pkg-config')
xcb_util_wm_dep = dependency('xcb-icccm', method: 'pkg-config')
xcb_util_xrm_dep = dependency('xcb-xrm', method: 'pkg-config')
xkbcommon_dep = dependency('xkbcommon', method: 'pkg-config')
xkbcommon_x11_dep = dependency('xkbcommon-x11', method: 'pkg-config')
yajl_dep = dependency('yajl', method: 'pkg-config')
libpcre_dep = dependency('libpcre', version: '>=8.10', method: 'pkg-config')
cairo_dep = dependency('cairo', version: '>=1.14.4', method: 'pkg-config')
pangocairo_dep = dependency('pangocairo', method: 'pkg-config')
glib_dep = dependency('glib-2.0', method: 'pkg-config')
gobject_dep = dependency('gobject-2.0', method: 'pkg-config')

ev_dep = cpp.find_library('ev')

# src/log.c uses threading primitives for synchronization
thread_dep = dependency('threads')

common_deps = [
  thread_dep,
  m_dep,
  iconv_dep,
  rt_dep,
  libsn_dep,
  xcb_dep,
  xcb_xkb_dep,
  xcb_randr_dep,
  xcb_shape_dep,
  xcb_util_dep,
  xcb_util_cursor_dep,
  xcb_util_keysyms_dep,
  xcb_util_wm_dep,
  xcb_util_xrm_dep,
  xkbcommon_dep,
  xkbcommon_x11_dep,
  yajl_dep,
  libpcre_dep,
  cairo_dep,
  pangocairo_dep,
  glib_dep,
  gobject_dep,
  ev_dep,
]

# Verify the perl interpreter is present for running parser_gen,
# ensuring a good error message when it isn’t:
perl = find_program('perl')
parser_gen = find_program('generate-command-parser.pl')

command_parser = custom_target(
  'command_parser',
  input: 'parser-specs/commands.spec',
  output: [
    'GENERATED_command_enums.h',
    'GENERATED_command_tokens.h',
    'GENERATED_command_call.h',
  ],
  command: [perl, parser_gen, '--input=@INPUT@', '--prefix=command'],
)

config_parser = custom_target(
  'config_parser',
  input: 'parser-specs/config.spec',
  output: [
    'GENERATED_config_enums.h',
    'GENERATED_config_tokens.h',
    'GENERATED_config_call.h',
  ],
  command: [parser_gen, '--input=@INPUT@', '--prefix=config'],
)

subdir('libi3')
subdir('i3')
subdir('i3bar')
subdir('i3-config-wizard')
subdir('i3-nagbar')
subdir('utils')
subdir('docs')
subdir('man')
#subdir('testcases')

install_subdir(
  'etc',
  strip_directory: true,
  install_dir: join_paths(get_option('sysconfdir'), 'i3'),
)

install_subdir(
  'share/',
  strip_directory: true,
  install_dir: get_option('datadir'),
)

