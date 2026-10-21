#!/usr/bin/env perl
# vim:ts=4:sw=4:expandtab
#
# The suite's gate: fails if a test in passing.txt didn't pass in the given run.
# Tests that pass but aren't in passing.txt are listed, so that the baseline can
# be updated.
#
# Usage: check.pl <complete-run.log>
use strict;
use warnings;
use v5.10;
use File::Basename qw(dirname);

my $log = shift or die "usage: $0 <complete-run.log>\n";
my $dir = dirname(__FILE__);

my %status;
for (qx($^X $dir/results.pl $log)) {
    my ($test, $status) = split;
    $status{$test} = $status;
}
die "$dir/results.pl $log failed\n" if $?;

open(my $fh, '<', "$dir/passing.txt") or die "$dir/passing.txt: $!\n";
chomp(my @passing = <$fh>);
my %in_baseline = map { $_ => 1 } @passing;

my @regressions = grep { ($status{$_} // 'missing') ne 'pass' } @passing;
my @new_passes = grep { $status{$_} eq 'pass' && !$in_baseline{$_} } sort keys %status;

say "Passing in the baseline but not in this run: @regressions" if @regressions;
say "Passing now but not in passing.txt: @new_passes" if @new_passes;
say scalar(@passing) - scalar(@regressions) . " of " . scalar(@passing) . " baseline-passing tests pass";
exit(@regressions > 0);
