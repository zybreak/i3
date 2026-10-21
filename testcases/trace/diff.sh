#!/bin/sh
# Diffs the X11 traces of two suite runs test by test (MIGRATION-PLAN task 0.5).
# Both runs must have been made with complete-run.pl --xtrace, with the X11
# protocol tracer ahead of glibc's xtrace on PATH:
#
#   PATH=$srcdir/build/tools/bin:$PATH ./complete-run.pl --xtrace ...
#
# Usage: diff.sh <run dir A> <run dir B> [test name...]
# TRACE_REQUESTS_ONLY=1 compares only the requests (see normalize.pl).
# Writes <test>.diff into the current directory for each test whose normalised
# traces differ, and prints a summary. Exits 1 if any trace differs.
set -u
here=$(cd "$(dirname "$0")" && pwd)
a=$1 b=$2
shift 2

tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT

if [ $# -eq 0 ]; then
    set -- $(cd "$a" && ls xtrace-for-*.log 2>/dev/null | sed 's/^xtrace-for-//; s/\.log$//')
fi

same=0 differ=0 missing=0
for test in "$@"; do
    fa=$a/xtrace-for-$test.log fb=$b/xtrace-for-$test.log
    if [ ! -e "$fa" ] || [ ! -e "$fb" ]; then
        echo "missing: $test"
        missing=$((missing + 1))
        continue
    fi
    perl "$here/normalize.pl" "$fa" > "$tmp/a"
    perl "$here/normalize.pl" "$fb" > "$tmp/b"
    if diff -u "$tmp/a" "$tmp/b" > "$test.diff"; then
        rm -f "$test.diff"
        same=$((same + 1))
    else
        echo "differs: $test ($(grep -c '^[-+][^-+]' "$test.diff") lines, $test.diff)"
        differ=$((differ + 1))
    fi
done
echo "identical: $same, different: $differ, missing: $missing"
[ "$differ" -eq 0 ]
