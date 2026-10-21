# vim:ts=4:sw=4:sts=4:expandtab
package TestWorker;
use strict; use warnings;
use v5.10;

use Socket qw(AF_UNIX SOCK_DGRAM PF_UNSPEC);
use IO::Handle; # for ->autoflush

use POSIX ();

use Errno qw(EAGAIN);

use Exporter 'import';
our @EXPORT = qw(worker worker_next);

use File::Basename qw(basename);
my @x;
my $options;

our $EOF = "# end of file\n";

# Test pid => pid of its watchdog (--timeout).
our %watchdogs;

sub worker {
    my ($display, $x, $outdir, $optref) = @_;

    # make sure $x hangs around
    push @x, $x;

    # store the options hashref
    $options = $optref;

    socketpair(my $ipc_child, my $ipc, AF_UNIX, SOCK_DGRAM, PF_UNSPEC)
        or die "socketpair: $!";

    $ipc->autoflush(1);
    $ipc_child->autoflush(1);

    my $worker = {
        display => $display,
        ipc => $ipc,
    };

    my $pid = fork // die "could not fork: $!";

    if ($pid == 0) {
        close $ipc;
        undef @complete_run::CLEANUP;
        # reap dead test children
        $SIG{CHLD} = sub {
            while ((my $child = waitpid(-1, POSIX::WNOHANG)) > 0) {
                my $status = $?;
                my $watchdog = delete $watchdogs{$child};
                next unless defined($watchdog);
                kill('KILL', $watchdog);
                # A test killed by a signal (by its watchdog, or by a crash in
                # an XS module) wrote no EOF marker; write it for the test.
                if (POSIX::WIFSIGNALED($status)) {
                    syswrite($ipc_child, "not ok 999 - test process killed by signal "
                             . POSIX::WTERMSIG($status) . "\n$EOF");
                }
            }
        };

        $worker->{ipc} = $ipc_child;

        # Preload the i3test module: reduces user CPU from 25s to 18s
        require i3test;

        worker_wait($worker, $outdir);
        exit 23;

    }

    close $ipc_child;
    push @complete_run::CLEANUP, sub {
        # signal via empty line to exit itself
        syswrite($ipc, "\n") or kill('TERM', $pid);
        waitpid $pid, 0;
    };

    return $worker;

}

# Kills a test that is still running 30 seconds after its --timeout, when its
# alarm couldn't end it, along with the process groups of its children (the
# i3 it started leads its own).
sub start_watchdog {
    my ($test_pid) = @_;
    my $pid = fork // die "could not fork: $!";
    return $pid if $pid != 0;

    $SIG{CHLD} = 'DEFAULT';
    sleep($options->{timeout} + 30);
    my @children;
    for my $stat (glob('/proc/[0-9]*/stat')) {
        open(my $fh, '<', $stat) or next;
        my ($child, $ppid) = (<$fh> // '') =~ /^(\d+) \(.*\) \S (\d+)/;
        push @children, $child if defined($ppid) && $ppid == $test_pid;
    }
    # The test first: killing its i3 first would let the test react to the
    # SIGCHLD (i3test bails out) and write output after its end.
    kill('KILL', $test_pid);
    kill('KILL', -$_) for @children;
    POSIX::_exit(0);
}

sub worker_wait {
    my ($self, $outdir) = @_;

    my $ipc = $self->{ipc};
    my $ipc_fd = fileno($ipc);

    while (1) {
        my $file = $ipc->getline;
        if (!defined($file)) {
            next if $! == EAGAIN;
            last;
        }
        chomp $file;

        exit unless $file;

        die "tried to launch nonexistent testfile $file: $!\n"
            unless -e $file;

        # start a new and self contained process:
        # whatever happens in the testfile should *NOT* affect us.

        # SIGCHLD stays blocked until the test's watchdog is registered, so
        # that a test which exits at once is reaped with its watchdog known.
        my $chld = POSIX::SigSet->new(POSIX::SIGCHLD);
        POSIX::sigprocmask(POSIX::SIG_BLOCK, $chld);
        my $pid = fork // die "could not fork: $!";
        if ($pid != 0) {
            $watchdogs{$pid} = start_watchdog($pid) if $options->{timeout};
            POSIX::sigprocmask(POSIX::SIG_UNBLOCK, $chld);
        }
        if ($pid == 0) {
            POSIX::sigprocmask(POSIX::SIG_UNBLOCK, $chld);
            undef @complete_run::CLEANUP;
            local $SIG{CHLD};

            $0 = $file;

            # Re-seed rand() so that File::Temp’s tempnam produces different
            # results, making a TOCTOU between e.g. t/175-startup-notification.t
            # and t/180-fd-leaks.t less likely.
            srand(time ^ $$);

            POSIX::dup2($ipc_fd, 0);
            POSIX::dup2($ipc_fd, 1);
            POSIX::dup2(1, 2);

            # get Test::Builder singleton
            my $test = Test::Builder->new;

            # Test::Builder dups stdout/stderr while loading.
            # we need to reset them here to point to $ipc
            $test->output(\*STDOUT);
            $test->failure_output(\*STDERR);
            $test->todo_output(\*STDOUT);

            @ENV{qw(HOME DISPLAY TESTNAME OUTDIR VALGRIND STRACE XTRACE COVERAGE RESTART)}
                = ($outdir,
                   $self->{display},
                   basename($file),
                   $outdir,
                   $options->{valgrind},
                   $options->{strace},
                   $options->{xtrace},
                   $options->{coverage},
                   $options->{restart});

            # A test that hangs (waiting for an IPC reply or an event that
            # never comes) fails after --timeout seconds instead of blocking
            # its worker for the rest of the run. exit runs i3test's END
            # block, which kills the test's i3. sigaction installs the
            # handler undeferred, so that it also runs while the test blocks
            # inside an XS call. A test that sets its own alarm (298) replaces
            # it; the watchdog still applies.
            if ($options->{timeout}) {
                POSIX::sigaction(POSIX::SIGALRM, POSIX::SigAction->new(sub {
                    $test->ok(0, "timed out after $options->{timeout} seconds");
                    print $EOF;
                    exit 0;
                }));
                alarm($options->{timeout});
            }

            package main;
            local $@;
            do $file;
            $test->ok(undef, "$@") if $@;

            # XXX hack, we need to trigger the read watcher once more
            # to signal eof to TAP::Parser
            print $EOF;

            exit 0;
        }
    }
}

sub worker_next {
    my ($self, $file) = @_;

    my $ipc = $self->{ipc};
    syswrite $ipc, "$file\n" or die "syswrite: $!";
}

__PACKAGE__ __END__
