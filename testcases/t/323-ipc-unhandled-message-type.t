#!perl
# vim:ts=4:sw=4:expandtab
#
# Please read the following documents before working on tests:
# • https://build.i3wm.org/docs/testsuite.html
#   (or docs/testsuite)
#
# • https://build.i3wm.org/docs/lib-i3test.html
#   (alternatively: perldoc ./testcases/lib/i3test.pm)
#
# • https://build.i3wm.org/docs/ipc.html
#   (or docs/ipc)
#
# • https://i3wm.org/downloads/modern_perl_a4.pdf
#   (unless you are already familiar with Perl)
#
# Verifies that i3 survives IPC messages it has no handler for: GET_MARKS (5),
# whose feature was removed, and a type beyond the known ones. i3 sends no
# reply for them, so the test doesn't wait for one.
use i3test;

my $i3 = i3(get_socket_path());
$i3->connect->recv;

$i3->message(5, "");
$i3->message(1000, "");

# i3 handles a connection's messages in order, so this reply comes after i3
# handled the two above.
my $reply = $i3->message(0, "nop")->recv;
ok($reply->[0]->{success}, 'a command on the same connection still gets a reply');

does_i3_live;

done_testing;
