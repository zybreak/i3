#!/bin/sh
# Installs the Perl suite's CPAN modules into a prefix, without root:
#   testcases/install-perl-deps.sh [prefix]
# The prefix defaults to build/perl5 in the source tree, which is CMake's default
# I3_TEST_PERL_PREFIX; the ctest test "complete-run" puts its lib/perl5 on PERL5LIB.
# Modules the system already has are skipped. X11::XCB builds against the
# system's libxcb and xcb-proto (pkg-config xcb, xcb-proto).
set -eu
srcdir=$(cd "$(dirname "$0")/.." && pwd)
prefix=${1:-$srcdir/build/perl5}
exec cpanm --notest -L "$prefix" \
    AnyEvent AnyEvent::I3 ExtUtils::PkgConfig Inline Inline::C IPC::Run JSON::XS \
    X11::XCB
