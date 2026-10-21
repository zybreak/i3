#!/bin/sh
# ctest's test "complete-run" (label "it"): runs the Perl suite on its own Xvfb
# and passes when every test in baseline/passing.txt passes (baseline/check.pl).
# complete-run.pl's own exit status is ignored: the baseline has failing tests.
# Runs in the build tree's testcases directory. Extra arguments (test files)
# go to complete-run.pl.
set -u
srcdir=$(dirname "$0")
./complete-run.pl --parallel "${I3_TEST_PARALLEL:-8}" --timeout "${I3_TEST_TIMEOUT:-60}" --keep-going "$@"
exec perl "$srcdir/baseline/check.pl" latest/complete-run.log
