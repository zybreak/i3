anyevent_i3 = custom_target(
  'anyevent-i3',
  # Should be AnyEvent-I3/blib/lib/AnyEvent/I3.pm,
  # but see https://github.com/mesonbuild/meson/issues/2320
  output: 'AnyEvent-I3.stamp',
  command: [
    'sh',
    '-c',
    'cp -r @0@ . && cd AnyEvent-I3 && perl Makefile.PL && make && touch ../AnyEvent-I3.stamp'.format(meson.current_source_dir()),
  ],
)