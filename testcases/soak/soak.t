#!perl
# vim:ts=4:sw=4:expandtab
#
# Soak test (MIGRATION-PLAN task 0.8), not part of the normal suite. Opens and
# closes SOAK_WINDOWS windows (default 10000), renaming each a few times, and
# records i3's RSS and open-fd count every SOAK_SAMPLE windows (default 500) in
# $OUTDIR/soak.csv. Run it on its own, without the suite's timeout:
#
#   SOAK_WINDOWS=10000 ./complete-run.pl --parallel 1 soak/soak.t
#
# It only records; the numbers are compared by hand (or by a later gate).
use i3test i3_autostart => 0;
use List::Util qw(first);

my $windows = $ENV{SOAK_WINDOWS} // 10000;
my $sample_every = $ENV{SOAK_SAMPLE} // 500;

my $config = <<EOT;
# i3 config file (v4)
font -misc-fixed-medium-r-normal--13-120-75-75-C-70-iso10646-1
EOT

my $pid = launch_with_config($config);

sub sample {
    my ($opened) = @_;
    open(my $fh, '<', "/proc/$pid/status") or die "/proc/$pid/status: $!";
    my $rss = first { defined } map { /^VmRSS:\s+(\d+) kB/ ? $1 : undef } <$fh>;
    close($fh);
    my $fds = () = glob("/proc/$pid/fd/*");
    return [ $opened, $rss, $fds ];
}

fresh_workspace;

my @samples = (sample(0));
for my $i (1 .. $windows) {
    my $window = open_window(name => "soak $i");
    $window->name("soak $i renamed");
    $window->name("soak $i renamed again");
    $window->destroy;
    if ($i % $sample_every == 0) {
        sync_with_i3;
        push @samples, sample($i);
        diag("windows=$samples[-1][0] rss_kb=$samples[-1][1] fds=$samples[-1][2]");
    }
}

my $csv = "$ENV{OUTDIR}/soak.csv";
open(my $out, '>', $csv) or die "$csv: $!";
print $out "windows,rss_kb,fds\n";
print $out join(',', @$_) . "\n" for @samples;
close($out);

ok(-s $csv, "samples written to $csv");

exit_gracefully($pid);

done_testing;
