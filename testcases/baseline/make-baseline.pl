#!/usr/bin/env perl
# vim:ts=4:sw=4:expandtab
#
# Writes passing.txt, failing.txt and flaky.txt next to this script from the
# complete-run.log of several runs: a test that passed in every run is passing,
# one that failed in every run is failing, and the rest are flaky.
#
# Usage: make-baseline.pl <complete-run.log>...
use strict;
use warnings;
use v5.10;
use File::Basename qw(dirname);

die "usage: $0 <complete-run.log>...\n" unless @ARGV;
my $dir = dirname(__FILE__);
my $results = "$dir/results.pl";

my %passes;
my %runs;
for my $log (@ARGV) {
    for (qx($^X $results $log)) {
        die "$results $log failed\n" if $?;
        my ($test, $status) = split;
        $runs{$test}++;
        $passes{$test}++ if $status eq 'pass';
    }
}

my %lists = (passing => [], failing => [], flaky => []);
for my $test (sort keys %runs) {
    my $passed = $passes{$test} // 0;
    # A test missing from a run counts as a failure in that run.
    my $list = $passed == @ARGV ? 'passing' : $passed == 0 ? 'failing' : 'flaky';
    push @{$lists{$list}}, $test;
}

for my $list (sort keys %lists) {
    open(my $fh, '>', "$dir/$list.txt") or die "$dir/$list.txt: $!\n";
    say $fh $_ for @{$lists{$list}};
    close($fh);
    say "$list: " . scalar @{$lists{$list}};
}
