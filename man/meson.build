# -*- mode: meson -*-

################################################################################
# manpages
################################################################################

man1 = join_paths(get_option('mandir'), 'man1')

if get_option('mans')
  asciidoc = find_program('asciidoc')

  asciidoc_conf = configure_file(
    input: 'asciidoc.conf.in',
    output: 'asciidoc.conf',
    configuration: {
        'PACKAGE_VERSION': meson.project_version()
    },
  )

  xmlto = find_program('xmlto')

  pod2man = find_program('pod2man')

  man_inputs = [
    'i3.man',
    'i3bar.man',
    'i3-nagbar.man',
    'i3-config-wizard.man',
    'i3-sensible-terminal.man',
    'i3-dump-log.man',
  ]

  foreach m : man_inputs
    xml = custom_target(
      m.underscorify()+'_asciidoc',
      input: m,
      output: '@BASENAME@.xml',
      command: [
        asciidoc,
        '-d', 'manpage',
        '-b', 'docbook',
        '-f', asciidoc_conf,
        '-o', '@OUTPUT@',
        '@INPUT@',
      ],
    )

    custom_target(
      m.underscorify()+'_xmlto',
      input: xml,
      output: '@BASENAME@.1',
      command: [
        xmlto,
        'man',
        '-o',
        '@OUTDIR@',
        '@INPUT@',
      ],
      # We should use install and install_dir instead of install_man as per:
      # https://github.com/mesonbuild/meson/issues/4981#issuecomment-467084867
      # https://github.com/mesonbuild/meson/issues/1550#issuecomment-370164307
      install: true,
      install_dir: man1,
    )
  endforeach

  pod2man_inputs = [
    '../utils/i3-save-tree',
  ]
  foreach m : pod2man_inputs
    custom_target(
      m.underscorify()+'_pod2man',
      input: m,
      output: '@BASENAME@.1',
      command: [
        pod2man,
        '--utf8',
        '@INPUT@',
        '@OUTPUT@',
      ],
      # We should use install and install_dir instead of install_man as per:
      # https://github.com/mesonbuild/meson/issues/4981#issuecomment-467084867
      # https://github.com/mesonbuild/meson/issues/1550#issuecomment-370164307
      install: true,
      install_dir: man1,
    )
  endforeach

else
  if run_command('[', '-f', 'i3.1', ']').returncode() == 0
    install_data(
      [
	'i3.1',
	'i3bar.1',
	'i3-nagbar.1',
	'i3-config-wizard.1',
	'i3-sensible-terminal.1',
	'i3-dump-log.1',
	'i3-save-tree.1',
      ],
      install_dir: man1,
    )
  endif
endif

summary('build manpages (-Dmans)', get_option('mans'))
