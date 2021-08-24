# -*- mode: meson -*-

################################################################################
# docs generation
################################################################################

docdir = get_option('docdir')
if docdir == ''
  docdir = join_paths(get_option('datadir'), 'doc', 'i3')
endif

if get_option('docs')
  asciidoc = find_program('asciidoc')
  doc_toc_inputs = [
    'hacking-howto',
    'userguide',
    'ipc',
    'wsbar',
    'testsuite',
    'i3bar-protocol',
    'layout-saving',
  ]
  foreach m : doc_toc_inputs
    custom_target(
      m.underscorify()+'_asciidoc',
      input: m,
      output: '@BASENAME@.html',
      command: [
        asciidoc,
        '-a', 'toc',
        '-n',
        '-o', '@OUTPUT@',
        '@INPUT@',
      ],
      install: true,
      install_dir: docdir,
    )
  endforeach

  doc_notoc_inputs = [
    'debugging',
  ]
  foreach m : doc_notoc_inputs
    custom_target(
      m.underscorify()+'_asciidoc',
      input: m,
      output: '@BASENAME@.html',
      command: [
        asciidoc,
        '-n',
        '-o', '@OUTPUT@',
        '@INPUT@',
      ],
      install: true,
      install_dir: docdir,
    )
  endforeach

else
  if run_command('[', '-f', 'hacking-howto.html', ']').returncode() == 0
    install_data(
      [
	'hacking-howto.html',
	'userguide.html',
	'ipc.html',
	'wsbar.html',
	'testsuite.html',
	'i3bar-protocol.html',
	'layout-saving.html',
	'debugging.html',
      ],
      install_dir: docdir,
    )
  endif
endif

install_data(
  [
    'bigpicture.png',
    'single_terminal.png',
    'snapping.png',
    'two_columns.png',
    'two_terminals.png',
    'modes.png',
    'wsbar.png',
    'keyboard-layer1.png',
    'keyboard-layer2.png',
    'i3-sync-working.png',
    'i3-sync.png',
    'tree-layout1.png',
    'tree-layout2.png',
    'tree-shot1.png',
    'tree-shot2.png',
    'tree-shot3.png',
    'tree-shot4.png',
    'refcard.html',
    'refcard_style.css',
    'logo-30.png',
    'layout-saving-1.png',
  ],
  install_dir: docdir,
)

summary('build docs (-Ddocs)', get_option('docs'))
