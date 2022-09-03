# -*- mode: meson -*-

# We cannot use configure_file for complete-run.pl.in and i3test.pm.in
# because configure_file strips the backslash in e.g. \@display,
# resulting in @display, breaking our Perl code:
# https://github.com/mesonbuild/meson/issues/7165
bash = find_program('bash')
replace_dirs = [
  bash, '-c',  # Use bash to capture output and mark as executable
  'sed -e \'s,@abs_top_builddir@,'
  + meson.current_build_dir()
  + ',g;s,@abs_top_srcdir@,'
  + meson.current_source_dir()+',g\''
  # Only mark files ending in .pl as executables
  + ' "$0" > "$1" && { [[ "${1##*.}" == pl ]] && chmod +x "$1" || true; }',
  '@INPUT0@',   # $0
  '@OUTPUT0@',  # $1
]
complete_run = custom_target(
  'complete-run',
  input: ['complete-run.pl.in'],
  output: ['complete-run.pl'],
  command: replace_dirs,
  # build this target when running e.g. ninja or ninja test.
  # This is required for older meson versions (< 0.46.0).
  build_by_default: true,
)
i3test_pm = custom_target(
  'i3test-pm',
  input: ['lib/i3test.pm.in'],
  output: ['i3test.pm'],
  command: replace_dirs,
  # build this target when running e.g. ninja or ninja test.
  # This is required for older meson versions (< 0.46.0).
  build_by_default: true,
)

if get_option('docs')
  i3_pod2html = find_program('../docs/i3-pod2html')

  custom_target(
    'lib-i3test.html',
    input: i3test_pm,
    output: 'lib-i3test.html',
    command: [
      i3_pod2html,
      '@INPUT@',
      '@OUTPUT@',
    ],
    install: true,
    install_dir: docdir,
  )

  custom_target(
    'lib-i3test-test.html',
    input: 'lib/i3test/Test.pm',
    output: 'lib-i3test-test.html',
    command: [
      i3_pod2html,
      '@INPUT@',
      '@OUTPUT@',
    ],
    install: true,
    install_dir: docdir,
  )
endif

executable(
  'test.inject_randr15',
  'inject_randr1.5.cpp',
  include_directories: [ '../libi3/include', config_h_dir ],
  dependencies: common_deps,
  link_with: libi3,
)

executable(
  'test.commands_parser',
  [
    'commands_parser.cpp',
    '../i3/src/commands_parser.cpp',
    '../i3/src/parser_stack.cpp',
    '../i3/src/global.cpp',
    'mock.cpp',
    command_parser,
  ],
  include_directories: [ '../lib', '../i3/src/include', '../i3/src/config/old', '../libi3/include', '../lib/i3_ipc/include', config_h_dir ],
  cpp_args: '-DTEST_PARSER',
  dependencies: common_deps,
  link_with: libi3,
)

executable(
  'test.config_parser',
  [
    'config_parser.cpp',
    '../i3/src/config/old/config_parser.cpp',
    '../i3/src/parser_stack.cpp',
    '../i3/src/global.cpp',
    'mock.cpp',
    config_parser,
  ],
  include_directories: [ '../lib', '../i3/src/include', '../i3/src/config/old', '../libi3/include', '../lib/i3_ipc/include', config_h_dir ],
  cpp_args: '-DTEST_PARSER',
  dependencies: common_deps,
  link_with: libi3,
)

parser_deps = common_deps
parser_deps += antlr_dep

executable(
  'test.config_parser_new',
  [
    'config_parser_new.cpp',
    '../i3/src/config/new/config_parser.cpp',
    '../i3/src/config/new/parser-specs/configBaseListener.cpp',
    '../i3/src/config/new/parser-specs/configLexer.cpp',
    '../i3/src/config/new/parser-specs/configParser.cpp',
    '../i3/src/config/new/parser-specs/configListener.cpp',
    '../i3/src/global.cpp',
    'mock_new.cpp',
  ],
  include_directories: [ '../lib', '../i3/src/include', '../i3/src/', '../i3/src/config/new', '../libi3/include', '../lib/i3_ipc/include', config_h_dir, '/usr/include/antlr4-runtime' ],
  cpp_args: '-DTEST_PARSER',
  dependencies: parser_deps,
  link_with: libi3,
)


test(
  'complete-run',
  perl,
  args: [complete_run],
  depends: [
    anyevent_i3,
    i3test_pm,
  ],
  timeout: 120,  # Default of 30 seconds can cause timeouts on slower machines
)
