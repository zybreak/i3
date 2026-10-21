#!/bin/sh
# The extension matrix (MIGRATION-PLAN task 0.7): runs the suite once per X
# extension with that extension disabled in the Xephyr servers, and writes the
# per-test results to extension-<name>.txt next to this script.
#
# Usage, from the build tree's testcases directory:
#   extension-matrix.sh [extension...] [-- test files...]
# The extensions default to RANDR and RENDER. Xephyr can't disable XKEYBOARD or
# SHAPE (only the extensions listed by "Xephyr -help" can be toggled).
set -u
here=$(cd "$(dirname "$0")" && pwd)

extensions=
while [ $# -gt 0 ] && [ "$1" != "--" ]; do
    extensions="$extensions $1"
    shift
done
[ $# -gt 0 ] && shift
extensions=${extensions:-RANDR RENDER}

for ext in $extensions; do
    I3_TEST_XSERVER_ARGS="-extension $ext" ./complete-run.pl --parallel "${I3_TEST_PARALLEL:-8}" \
        --timeout "${I3_TEST_TIMEOUT:-60}" --keep-going "$@" > "extension-$ext.out" 2>&1
    perl "$here/results.pl" latest/complete-run.log > "$here/extension-$ext.txt"
    echo "$ext: $(grep -c ' pass$' "$here/extension-$ext.txt") pass," \
         "$(grep -c ' fail$' "$here/extension-$ext.txt") fail ($(readlink -f latest))"
done
