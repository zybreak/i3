package SocketActivation;
# vim:ts=4:sw=4:expandtab

use strict;
use warnings;
use IO::Socket::UNIX; # core
use Cwd qw(abs_path); # core
use POSIX qw(:fcntl_h); # core
use AnyEvent::Handle; # not core
use AnyEvent::Util; # not core
use AnyEvent; # not core
use Exporter 'import';
use v5.10;

our @EXPORT = qw(activate_i3);

#
# Starts i3 using socket activation. Creates a listening socket (with bind +
# listen) which is then passed to i3, who in turn calls accept and handles the
# requests.
#
# Since the kernel buffers the connect, the parent process can connect to the
# socket immediately after forking. It then sends a request and waits until it
# gets an answer. Obviously, i3 has to be initialized to actually answer the
# request.
#
# This way, we can wait *precisely* the amount of time which i3 waits to get
# ready, which is a *HUGE* speed gain (and a lot more robust) in comparison to
# using sleep() with a fixed amount of time.
#
# unix_socket_path: Location of the socket to use for the activation
# display: X11 $ENV{DISPLAY}
# configfile: path to the configuration file to use
# logpath: path to the logfile to which i3 will append
# cv: an AnyEvent->condvar which will be triggered once i3 is ready
#
sub activate_i3 {
    my %args = @_;

    my $pid = fork;
    if (!defined($pid)) {
        die "could not fork()";
    }
    if ($pid == 0) {
        # Start a process group so that in the parent, we can kill the entire
        # process group and immediately kill i3bar and any other child
        # processes.
        setpgrp;

        delete $ENV{DESKTOP_STARTUP_ID};
        delete $ENV{I3SOCK};
        # $SHELL could be set to fish, which will horribly break running shell
        # commands via i3’s exec feature. This happened e.g. when having
        # “set-option -g default-shell "/usr/bin/fish"” in ~/.tmux.conf
        delete $ENV{SHELL};
        unless ($args{dont_create_temp_dir}) {
            $ENV{XDG_RUNTIME_DIR} = '/tmp/i3-testsuite/';
            mkdir $ENV{XDG_RUNTIME_DIR};
        }
        $ENV{DISPLAY} = $args{display};

        # Make sure no file descriptors are open. Strangely, I got an open file
        # descriptor pointing to AnyEvent/Impl/EV.pm when testing.
        AnyEvent::Util::close_all_fds_except(0, 1, 2);

        # Construct the command to launch i3. Use maximum debug level, disable
        # the interactive signalhandler to make it crash immediately instead.
        # Also disable logging to SHM since we redirect the logs anyways.
        # Force Xinerama because we use Xdmx for multi-monitor tests.
        my $i3cmd = q|i3 --disable-signalhandler|;
        if (!$args{validate_config}) {
            # We only set logging if i3 is actually started, but not if we only
            # validate the config file. This is to keep logging to a minimum as
            # such a test will likely want to inspect the log file.
            $i3cmd .= q| -V -d all|;
        }

        # For convenience:
        my $outdir = $args{outdir};
        my $test = $args{testname};

        if ($args{restart}) {
            $i3cmd .= ' -L ' . abs_path('restart-state.golden');
        }

        if ($args{validate_config}) {
            $i3cmd .= ' -C';
        }

        if ($args{valgrind}) {
            $i3cmd =
                qq|valgrind --log-file="$outdir/valgrind-for-$test.log" | .
                qq|--suppressions="./valgrind.supp" | .
                qq|--leak-check=full --track-origins=yes --num-callers=20 | .
                qq|--tool=memcheck -- $i3cmd|;
        }

        my $logfile = "$outdir/i3-log-for-$test";
        # Append to $logfile instead of overwriting because i3 might be
        # run multiple times in one testcase.
        my $cmd = "exec $i3cmd -c $args{configfile} >>$logfile 2>&1";
        
        if ($args{strace}) {
            my $out = "$outdir/strace-for-$test.log";

            # We overwrite LISTEN_PID with the correct process ID to make
            # socket activation work (LISTEN_PID has to match getpid(),
            # otherwise the LISTEN_FDS will be treated as a left-over).
            $cmd = qq|strace -fvy -s2048 -o "$out" -- | .
                     'sh -c "export LISTEN_PID=\$\$; ' . $cmd . '"';
        }

        if ($args{xtrace}) {
            my $out = "$outdir/xtrace-for-$test.log";

            # See comment in $args{strace} branch.
            $cmd = qq|xtrace -n -o "$out" -- | .
                     'sh -c "export LISTEN_PID=\$\$; ' . $cmd . '"';
        }

        # We need to use the shell due to using output redirections.
        exec '/bin/sh', '-c', $cmd;

        # if we are still here, i3 could not be found or exec failed. bail out.
        exit 1;
    }

    # close the socket, the child process should be the only one which keeps a file
    # descriptor on the listening socket.

    if ($args{validate_config}) {
        $args{cv}->send(1);
        return $pid;
    }
    my $w;
    $w = AnyEvent->timer(after => .5, cb => sub {
        $args{cv}->send(0);
        undef $w;
    });

    return $pid;
}
