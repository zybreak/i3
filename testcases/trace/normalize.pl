#!/usr/bin/env perl
# vim:ts=4:sw=4:expandtab
#
# Normalises an X11 protocol trace (xtrace -n, as complete-run.pl --xtrace
# writes it) so that two runs of the same test can be diffed. A test that
# starts i3 several times gets one xtrace per i3 instance appended to the same
# log, each numbering its connections from 000; every connection (from its
# "am lsb-first" line to the next one with the same number) is a session here.
#
# Per line:
# - sequence numbers and lengths are dropped;
# - atoms become their names: 0x17("RESOURCE_MANAGER") -> ("RESOURCE_MANAGER");
# - timestamps (fields whose name contains "time", and the ones xtrace leaves
#   in raw bytes: the MANAGER client message, GetMonitors' reply) become T;
# - values that differ per run (i3's PID and its temporary paths in root
#   properties, the X server's release, the connection's resource base in the
#   setup reply, which is its X client slot, the PID, host name and time in
#   startup-notification IDs, in the messages and in _NET_STARTUP_ID) are
#   dropped, and so are the random values the tests choose (I3_SYNC data,
#   workspace names);
# - a last line without a newline (xtrace stopped while writing it, when the
#   test ended) is dropped;
# - the data of 32-bit client messages, which xtrace prints as bytes, is
#   printed as 32-bit words, so that the window IDs in it (the MANAGER
#   message's selection owner: i3's first window, whose value depends on i3's
#   X client slot) are numbered like any other XID.
#
# Per session:
# - the requests are printed before the server's replies, events and errors:
#   the order within each direction is kept, but how the two interleave
#   depends on timing;
# - QueryExtension requests (and their replies) are taken out of the stream
#   and printed as a sorted block after the requests: i3 on libxcb queries
#   XKEYBOARD, BIG-REQUESTS and RANDR right after connecting, on xcbpp just
#   before first use. The block keeps every query, so an extension queried
#   once more, once less or with a different answer still shows; only its
#   position is lost, and a client can't use an extension before the reply.
#
# XIDs (8-digit hex values from 0x00200000, the first client resource base,
# to 0x1fffffff: an XID's top three bits are zero; not keysyms, which reach
# into that range) become X1, X2, … in the
# order they first appear in the requests, over all sessions in the order
# printed, with one table for the whole log (a window that outlives an i3
# instance keeps its name). XIDs that appear only in replies, events or
# errors become Y1, Y2, … in the order they first appear there. So a reply
# or event, or a request of another connection that interleaves differently,
# can't shift the numbering of the requests; replies and events use the same
# table, so the full output benefits too. Renaming is consistent: a window ID
# swapped for another everywhere is invisible, swapped in one place it shows.
#
# With TRACE_REQUESTS_ONLY=1, only the requests (and the QueryExtension
# block's requests) are printed: some replies carry X server state that
# differs between runs (XKB device, keymap and indicator state, the startup
# QueryTree, Xephyr's RandR output and monitor info).
#
# Left alone on purpose, because a rule that hid them could hide a real
# change (all of them differ between two runs of the same build):
# - the order of requests within a session: when i3 handles an event (an
#   UnmapNotify, the test's I3_SYNC message) relative to its other work moves
#   requests or whole request groups. diff.sh reports a test whose lines only
#   moved as "reordered" instead of hiding it: an event-order bug looks the
#   same;
# - the number of idle redraws (WM_NAME '' and SetInputFocus with nothing
#   focused): collapsing repeated groups would also hide requests sent twice;
# - CurrentTime instead of a timestamp, when i3 sets the focus before it has
#   seen an event with a time;
# - the content of properties i3 writes: restore_layout's placeholder windows
#   get WM_HINTS from an uninitialised struct (an i3 bug), so their words
#   differ per run.
# XIDs are named by value, so a value that a later client reuses keeps the
# first object's name.
#
# Usage: normalize.pl <xtrace log>  (or on stdin)
use strict;
use warnings;
use v5.10;

my $requests_only = $ENV{TRACE_REQUESTS_ONLY};

# Atom names by number, from every 0x..("NAME") xtrace prints: to name the
# atoms that xtrace leaves in raw bytes (the MANAGER message's selection). The
# atoms are the X server's, so one table serves every session of the log.
my %atom_names;

my @sessions;    # in the order of their first line
my %open;        # connection number -> its current session

sub new_session {
    my $session = {
        lsb_first => 1,
        requests => [],    # request lines (QueryExtension excluded)
        others => [],      # setup reply, replies, events, errors
        queries => [],     # QueryExtension: [request line, reply line]
        pending => {},     # sequence number -> its QueryExtension
        # direction -> window -> startup-notification message being read
        startup => {q{<} => {}, q{>} => {}},
    };
    push @sessions, $session;
    return $session;
}

# The data of a 32-bit client message as words in the client's byte order.
sub client_message_words {
    my ($session, $bytes) = @_;
    my @bytes = map { hex } split(/,/, $bytes);
    return $bytes unless @bytes == 20;
    my @words;
    while (my @word = splice(@bytes, 0, 4)) {
        @word = reverse(@word) if $session->{lsb_first};
        push @words, sprintf('0x%02x%02x%02x%02x', @word);
    }
    return join(',', @words);
}

# A startup-notification ID (libstartup-notification's <launcher>/<binary>/
# <PID>-<sequence>-<host>_TIME<time>) with the PID, host name and time
# replaced by P, H and T; the launch sequence number is kept.
my $startup_id = qr{\b([^/\s'=]+/[^/\s]+/)\d+-(\d+)-\S+?_TIME\d+};
sub mask_startup_id {
    my ($text) = @_;
    $text =~ s/$startup_id/${1}P-${2}-H_TIMET/g;
    return $text;
}

# A property that is just a startup-notification ID (_NET_STARTUP_ID, which
# xtrace prints as bytes) is printed as text, masked.
sub startup_id_property {
    my ($bytes) = @_;
    my $text = join('', map { chr(hex) } split(/,/, $bytes));
    return "data=$bytes;" unless $text =~ /\A$startup_id\z/;
    return 'data=\'' . mask_startup_id($text) . "';";
}

# Startup-notification messages (_NET_STARTUP_INFO_BEGIN, then
# _NET_STARTUP_INFO, 20 bytes each, until a 0 byte) are joined into one line
# with the text decoded and the ID masked, in place of the first. The bytes
# after the terminating 0 are the rest of the sender's buffer, which the
# specification says to ignore. A message that never ends is left as it was.
sub startup_notification {
    my ($pending, $lines, $line) = @_;
    my ($head, $window, $type, $bytes) =
        $line =~ /^(.*ClientMessage\(33\) format=0x08 window=(\S+) type=\("(_NET_STARTUP_INFO(?:_BEGIN)?)"\) data=)([0-9a-fx,]+);$/
        or return 0;
    my $message = $pending->{$window};
    if ($type eq '_NET_STARTUP_INFO_BEGIN') {
        $message = $pending->{$window} = {indices => [], head => $head, text => ''};
    } elsif (!defined($message)) {
        return 0;
    }
    push @$lines, $line;
    push @{$message->{indices}}, $#$lines;
    $message->{text} .= join('', map { chr(hex) } split(/,/, $bytes));
    my $end = index($message->{text}, "\0");
    return 1 if $end < 0;

    my $text = mask_startup_id(substr($message->{text}, 0, $end));
    my ($first, @rest) = @{$message->{indices}};
    $lines->[$first] = "$message->{head}'$text';\n";
    $lines->[$_] = undef for @rest;    # dropped when printed
    delete $pending->{$window};
    return 1;
}

while (my $line = <>) {
    # A trace ends where xtrace was stopped with the test, which may be in
    # the middle of a line.
    next unless $line =~ /\n\z/;
    next if $line =~ /^\d+:[<>]: Warning: buffer filled!$/;
    my ($connection, $direction, $sequence) = $line =~ /^(\d+):([<>]):(?:([0-9a-f]+):)?/;
    ($connection, $direction) = ('', '<') unless defined($connection);

    my $session = $open{$connection};
    if (!defined($session) || $line =~ /^\d+:<: am [lm]sb-first/) {
        $session = $open{$connection} = new_session();
        $session->{lsb_first} = 0 if $line =~ /^\d+:<: am msb-first/;
    }

    # 000:<:0003: 20: Request(55): ... -> 000:<: Request(55): ... (the length
    # changes with the values dropped below)
    $line =~ s/^(\d+:[<>]):[0-9a-f]+:(?: *\d+:)?/$1:/;
    while ($line =~ /0x([0-9a-f]+)\("([^"]*)"\)/g) {
        $atom_names{hex($1)} = $2;
    }
    $line =~ s/0x[0-9a-f]+(\("[^"]*"\))/$1/g;
    # The random value of the i3 sync protocol, and the workspace names, which
    # the tests choose at random (fresh_workspace).
    $line =~ s/(type=\("I3_SYNC"\) data=).*/${1}…/;
    $line =~ s/(property=\("_NET_DESKTOP_NAMES"\) type=\(\S+\) data=).*/${1}…/;
    $line =~ s/\b([\w-]*time[\w-]*)=0x[0-9a-f]+/$1=T/g;
    $line =~ s/(ClientMessage\(33\) format=0x20 .* data=)([0-9a-fx,]+)/$1 . client_message_words($session, $2)/e;
    # MANAGER: timestamp, selection, owner (ICCCM 2.8).
    $line =~ s{(type=\("MANAGER"\) data=)0x[0-9a-f]{8},0x([0-9a-f]{8})}{
        "${1}T," . (exists($atom_names{hex($2)}) ? "(\"$atom_names{hex($2)}\")" : "0x$2")
    }e;
    $line =~ s/(Reply to GetMonitors: .*unparsed-data=)(?:0x[0-9a-f]{2},){4}/${1}T,/;
    $line =~ s/(property=\("I3_(?:PID|CONFIG_PATH|SOCKET_PATH|SHMLOG_PATH|LOG_STREAM_SOCKET_PATH)"\) type=\(\S+\) data=).*/${1}…/;
    $line =~ s/\bdata=((?:0x[0-9a-f]{2},)+0x[0-9a-f]{2});/startup_id_property($1)/e
        if $line =~ /: (?:Request\(18\): ChangeProperty|Reply to GetProperty:) /;
    $line =~ s/\brelease=\d+/release=R/;
    # The connection's resource base: its X client slot.
    $line =~ s/^(\d+:>: Success, .*\bresource-id=)0x[0-9a-f]+/${1}B/;

    my $lines = $direction eq '<' ? $session->{requests} : $session->{others};
    next if startup_notification($session->{startup}{$direction}, $lines, $line);
    if (defined($sequence) && $line =~ /^\d+:<: Request\(98\): QueryExtension /) {
        push @{$session->{queries}}, ($session->{pending}{$sequence} = [$line]);
    } elsif (defined($sequence) && $line =~ /^\d+:>: Reply to QueryExtension: / && $session->{pending}{$sequence}) {
        push @{delete $session->{pending}{$sequence}}, $line;
    } else {
        push @$lines, $line;
    }
}

# Each session's lines in the order printed: requests, QueryExtension block,
# replies/events/errors. Requests only: the QueryExtension block's replies
# are left out with the rest.
my @printed;
for my $session (@sessions) {
    my @queries = sort { $a->[0] cmp $b->[0] || ($a->[1] // '') cmp ($b->[1] // '') } @{$session->{queries}};
    my @block = map { $requests_only ? $_->[0] : grep { defined } @$_ } @queries;
    push @printed, [@{$session->{requests}}, @block];
    push @printed, $session->{others} unless $requests_only;
}

# An 8-digit hex value, unless it's in a list of keysyms (GetKeyboardMapping's
# reply: vendor and Unicode keysyms are in the XID range).
my $xid = qr/(\bkeysyms=[^ ;]*)|\b0x([0-9a-f]{8})\b/;

my (%xids, $requests_xids, $others_xids);
sub number_xids {
    my ($prefix, $count, $text) = @_;
    return unless defined($text);
    while ($text =~ /$xid/g) {
        next if defined($1);
        my $value = hex($2);
        next if $value < 0x00200000 || $value > 0x1fffffff;
        $xids{$2} //= $prefix . ++$$count;
    }
}
for my $session (@sessions) {
    number_xids('X', \$requests_xids, $_) for @{$session->{requests}};
}
for my $session (@sessions) {
    number_xids('Y', \$others_xids, $_) for @{$session->{others}}, map { $_->[1] // () } @{$session->{queries}};
}

for my $lines (@printed) {
    for my $line (grep { defined } @$lines) {
        $line =~ s{$xid}{$1 // $xids{$2} // "0x$2"}ge;
        print $line;
    }
}
