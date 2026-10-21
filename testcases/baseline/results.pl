#!/usr/bin/env perl
# vim:ts=4:sw=4:expandtab
#
# Prints "<test> pass|fail" for each test in a complete-run.log, sorted by test
# name. A test fails if it bailed out, or if its TAP output has a failed test, a
# parse error (such as a missing plan, when the test died or timed out) or no
# output at all.
#
# Usage: results.pl <testsuite-dir>/complete-run.log
use strict;
use warnings;
use v5.10;
use File::Basename qw(basename);
use TAP::Parser;

my $logfile = shift or die "usage: $0 <complete-run.log>\n";
open(my $fh, '<', $logfile) or die "$logfile: $!\n";

my (%started, %bailed_out, %tap, $current);
while (my $line = <$fh>) {
    if ($line =~ m,^\[:\d+\] (\S+\.t(?:-DISABLED)?): starting$,) {
        $started{basename($1)} = 1;
        next;
    }
    # complete-run.pl --keep-going
    if ($line =~ m,^\[:\d+\] (\S+\.t(?:-DISABLED)?): BAILOUT$,) {
        $bailed_out{basename($1)} = 1;
        next;
    }
    if ($line =~ m,^output for (\S+\.t(?:-DISABLED)?):$,) {
        $current = basename($1);
        $tap{$current} = '';
        next;
    }
    # The summary that follows the last test's output.
    if ($line =~ m,^(All tests successful\.|Test Summary Report|Files=\d+),) {
        $current = undef;
        next;
    }
    $tap{$current} .= $line if defined($current);
}

for my $test (sort keys %started) {
    my $status = 'fail';
    if (!$bailed_out{$test} && defined($tap{$test}) && length($tap{$test}) > 0) {
        my $parser = TAP::Parser->new({ tap => $tap{$test} });
        while (defined($parser->next)) { }
        $status = 'pass' unless $parser->has_problems;
    }
    say "$test $status";
}
