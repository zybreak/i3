#!/bin/sh
# Diffs the X11 traces of two suite runs test by test (MIGRATION-PLAN task 0.5).
# Both runs must have been made with complete-run.pl --xtrace, with the X11
# protocol tracer ahead of glibc's xtrace on PATH:
#
#   PATH=$srcdir/build/tools/bin:$PATH ./complete-run.pl --xtrace ...
#
# Usage: diff.sh <run dir A> <run dir B> [test name...]
# TRACE_REQUESTS_ONLY=1 compares only the requests (see normalize.pl).
# Without test names, compares every test that has a trace in either run.
# Writes <test>.diff into the current directory for each test whose normalised
# traces differ, prints a line for each such test and a summary. A test is
#   identical  if its normalised traces are the same;
#   reordered  if they have the same lines, but in a different order: usually
#              timing (i3 handled an event or the test's next message earlier
#              or later), but a change in event or request order looks the
#              same, so read the diff;
#   different  if lines were added, removed or changed;
#   missing    if only one run has its trace.
# Exits 0 if every test is identical, 2 if the others are only reordered, and
# 1 if any test is different or missing.
set -u
here=$(cd "$(dirname "$0")" && pwd)
a=$1 b=$2
shift 2

tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT

if [ $# -eq 0 ]; then
    # shellcheck disable=SC2046 # test names have no spaces
    set -- $( { (cd "$a" && ls xtrace-for-*.log); (cd "$b" && ls xtrace-for-*.log); } 2>/dev/null |
        sed 's/^xtrace-for-//; s/\.log$//' | sort -u)
fi

same=0 reordered=0 differ=0 missing=0
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
        continue
    fi
    lines=$(grep -c '^[-+][^-+]' "$test.diff")
    sort "$tmp/a" > "$tmp/a.sorted"
    sort "$tmp/b" > "$tmp/b.sorted"
    if cmp -s "$tmp/a.sorted" "$tmp/b.sorted"; then
        echo "reordered: $test ($lines lines, $test.diff)"
        reordered=$((reordered + 1))
    else
        echo "differs: $test ($lines lines, $test.diff)"
        differ=$((differ + 1))
    fi
done
echo "identical: $same, reordered: $reordered, different: $differ, missing: $missing"
if [ $((differ + missing)) -gt 0 ]; then
    exit 1
elif [ "$reordered" -gt 0 ]; then
    exit 2
fi
