# -*- mode: meson -*-

i3_srcs = [
  'src/assignments.cpp',
  'src/atoms.cpp',
  'src/autostarts.cpp',
  'src/bindings.cpp',
  'src/boolstr.cpp',
  'src/click.cpp',
  'src/commands.cpp',
  'src/commands_parser.cpp',
  'src/con.cpp',
  'src/config/old/config_directives.cpp',
  'src/config/old/config_parser.cpp',
  'src/config/new/config_parser.cpp',
  'src/config/new/config_applier.cpp',
  'src/config/new/parser-specs/configBaseListener.cpp',
  'src/config/new/parser-specs/configLexer.cpp',
  'src/config/new/parser-specs/configParser.cpp',
  'src/config/new/parser-specs/configListener.cpp',
  'src/configuration.cpp',
  'src/drag.cpp',
  'src/ewmh.cpp',
  'src/floating.cpp',
  'src/global.cpp',
  'src/handlers.cpp',
  'src/ipc.cpp',
  'src/is_background_set.cpp',
  'src/key_press.cpp',
  'src/load_layout.cpp',
  'src/nagbar.cpp',
  'src/main.cpp',
  'src/manage.cpp',
  'src/match.cpp',
  'src/move.cpp',
  'src/nonblock.cpp',
  'src/output.cpp',
  'src/parser_stack.cpp',
  'src/program_arguments.cpp',
  'src/randr.cpp',
  'src/rect.cpp',
  'src/regex.cpp',
  'src/render.cpp',
  'src/resize.cpp',
  'src/restore_layout.cpp',
  'src/scratchpad.cpp',
  'src/sighandler.cpp',
  'src/startup.cpp',
  'src/sync.cpp',
  'src/tree.cpp',
  'src/util.cpp',
  'src/version.cpp',
  'src/window.cpp',
  'src/workspace.cpp',
  'src/xcb.cpp',
  'src/x.cpp',
  'src/xcursor.cpp',
]

i3_srcs += command_parser
i3_srcs += config_parser

i3_deps = common_deps
i3_deps += antlr_dep

i3 = executable(
  'i3',
  i3_srcs,
  install: true,
  include_directories: [ '../lib', '../libi3/include', '../lib/i3_ipc/include', 'src', 'src/include', 'src/config/old', config_h_dir, '/usr/include/antlr4-runtime' ],
  dependencies: i3_deps,
  link_with: [ libi3, libi3ipc ],
)

#parser_input = '../parser-specs/config.g4'
#parser_outputs = [
#  'configBaseListener.cpp',
#  'configLexer.cpp',
#  'configParser.cpp',
#  'configListener.cpp'
#]

#antlr4 = find_program('antlr4')
#parser = custom_target('gen-parser',
#        input: parser_input,
#        output: parser_outputs,
#        command: [antlr4,
#                '@INPUT@',
#                '-o', 'parser',
#                '-Dlanguage=Cpp'])

#parser_srcs = [
#  'src/config/new/main.cpp',
#  'src/config/new/parser.cpp',
#  'src/config/new/parser-specs/configBaseListener.cpp',
#  'src/config/new/parser-specs/configLexer.cpp',
#  'src/config/new/parser-specs/configParser.cpp',
#  'src/config/new/parser-specs/configListener.cpp',
#  'src/match.cpp',
#  'src/regex.cpp',
#  'src/tree.cpp',
#  'src/con.cpp',
#  'src/configuration.cpp',
#  'src/atoms.cpp',
#  'src/global.cpp',
#  'src/rect.cpp',
#  'src/util.cpp',
#  'src/x.cpp',
#  'src/load_layout.cpp',
#  'src/restore_layout.cpp',
#  'src/workspace.cpp',
#  'src/handlers.cpp',
#  'src/bindings.cpp',
#  'src/ipc.cpp',
#]
#
#parser_deps = []
#
#parser_deps += common_deps
#parser_deps += antlr_dep
#
#
#parser = executable(
#  'parser',
#  parser_srcs,
#  install: true,
#  include_directories: [ '../lib', '../libi3/include', '../lib/i3_ipc/include', 'src/include', 'src/config/new', config_h_dir, '/usr/include/antlr4-runtime' ],
#  dependencies: parser_deps,
#  link_with: [ libi3, libi3ipc ],
#)
#