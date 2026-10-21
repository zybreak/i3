#!/usr/bin/env perl
# vim:ts=4:sw=4:expandtab
#
# Normalises an X11 protocol trace (xtrace -n, as complete-run.pl --xtrace
# writes it) so that two runs of the same test can be diffed:
# - sequence numbers are dropped;
# - XIDs (any 8-digit hex value at or above 0x00200000, the first client
#   resource base) become X1, X2, … in order of first appearance;
# - atoms become their names: 0x17("RESOURCE_MANAGER") -> "RESOURCE_MANAGER";
# - timestamps (fields whose name contains "time", and the ones xtrace leaves
#   in raw bytes: the MANAGER client message, GetMonitors' reply) become T;
# - values that differ per run (i3's PID and its temporary paths in root
#   properties, the X server's release) are dropped;
# - each connection's requests are printed before the server's replies, events
#   and errors: the order within each direction is kept, but how the two
#   interleave depends on timing.
#
# With TRACE_REQUESTS_ONLY=1, only the requests are printed: some replies carry
# X server state that differs between runs (XKB device and indicator state,
# the startup QueryTree, Xephyr's RandR output info).
#
# Usage: normalize.pl <xtrace log>  (or on stdin)
use strict;
use warnings;
use v5.10;

my %xids;
my (@connections, %lines);
while (my $line = <>) {
    next if $line =~ /^\d+:[<>]: Warning: buffer filled!$/;
    # 000:<:0003: 20: Request(55): ... -> 000:<: Request(55): ... (the length
    # changes with the values dropped below)
    $line =~ s/^(\d+:[<>]):[0-9a-f]+:(?: *\d+:)?/$1:/;
    $line =~ s/0x[0-9a-f]+(\("[^"]*"\))/$1/g;
    # The random value of the i3 sync protocol, and the workspace names, which
    # the tests choose at random (fresh_workspace).
    $line =~ s/(type=\("I3_SYNC"\) data=).*/${1}…/;
    $line =~ s/(property=\("_NET_DESKTOP_NAMES"\) type=\(\S+\) data=).*/${1}…/;
    $line =~ s/\b([\w-]*time[\w-]*)=0x[0-9a-f]+/$1=T/g;
    $line =~ s/(type=\("MANAGER"\) data=)(?:0x[0-9a-f]{2},){4}/${1}T,/;
    $line =~ s/(Reply to GetMonitors: .*unparsed-data=)(?:0x[0-9a-f]{2},){4}/${1}T,/;
    $line =~ s/(property=\("I3_(?:PID|CONFIG_PATH|SOCKET_PATH|SHMLOG_PATH|LOG_STREAM_SOCKET_PATH)"\) type=\(\S+\) data=).*/${1}…/;
    $line =~ s/\brelease=\d+/release=R/;
    $line =~ s{\b0x([0-9a-f]{8})\b}{
        hex($1) >= 0x00200000 ? ($xids{$1} //= 'X' . (keys(%xids) + 1)) : "0x$1"
    }ge;

    my ($connection, $direction) = $line =~ /^(\d+):([<>])/;
    ($connection, $direction) = ('', '<') unless defined($connection);
    push @connections, $connection unless exists($lines{$connection});
    push @{$lines{$connection}{$direction}}, $line;
}

for my $connection (@connections) {
    print @{$lines{$connection}{'<'} // []};
    print @{$lines{$connection}{'>'} // []} unless $ENV{TRACE_REQUESTS_ONLY};
}
