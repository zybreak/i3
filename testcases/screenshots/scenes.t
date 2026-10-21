#!perl
# vim:ts=4:sw=4:expandtab
#
# Screenshot scenes (MIGRATION-PLAN task 0.6), not part of the normal suite.
# Each scene is set up on a fresh workspace, and the root window is captured
# with xwd and compared with golden/depth<depth>/<scene>.png on pixel data only
# (an XWD file's header and padding vary between runs). Run it on its own:
#
#   ./complete-run.pl --parallel 1 screenshots/scenes.t
#   I3_TEST_SCREEN=1280x800x32 ./complete-run.pl --parallel 1 screenshots/scenes.t
#
# SCREENSHOT_UPDATE=1 writes the goldens instead of comparing with them. The
# captures (and, for differences, compare's diff images) are in $OUTDIR.
#
# Open (2026-10-21): every capture is black, so no goldens are committed. xwd's
# pixel data is all zero even for a root set to red with xsetroot on a plain
# Xvfb, so the problem is below i3 (xwd or the X server's GetImage), not in
# this harness's scenes.
#
# Text is drawn with the X server's misc-fixed core font and with the bundled
# DejaVu Sans Mono through a FONTCONFIG_FILE that knows no other font.
use i3test i3_autostart => 0;
use X11::XCB qw(PROP_MODE_REPLACE);
use File::Basename qw(dirname);
use File::Path qw(make_path);
use Cwd qw(abs_path);

my $dir = dirname(abs_path(__FILE__));
my ($depth) = ($ENV{I3_TEST_SCREEN} // '') =~ /x(\d+)$/;
$depth //= 24;
my $golden_dir = "$dir/golden/depth$depth";
my $update = $ENV{SCREENSHOT_UPDATE};
make_path($golden_dir) if $update;

# i3's Conan-built fontconfig finds no configuration of its own.
my $fontconfig = "$ENV{OUTDIR}/screenshots-fonts.conf";
open(my $fc, '>', $fontconfig) or die "$fontconfig: $!";
print $fc <<EOT;
<?xml version="1.0"?>
<!DOCTYPE fontconfig SYSTEM "urn:fontconfig:fonts.dtd">
<fontconfig>
  <dir>$dir/fonts</dir>
  <cachedir>$ENV{OUTDIR}/fontconfig-cache</cachedir>
  <match target="font">
    <edit name="antialias" mode="assign"><bool>true</bool></edit>
    <edit name="hinting" mode="assign"><bool>false</bool></edit>
    <edit name="rgba" mode="assign"><const>none</const></edit>
  </match>
</fontconfig>
EOT
close($fc);
$ENV{FONTCONFIG_FILE} = $fontconfig;

sub capture {
    my ($scene) = @_;
    sync_with_i3;
    # i3test's SIGCHLD handler would report xwd, magick and compare exiting.
    local $SIG{CHLD} = 'DEFAULT';
    my $xwd = "$ENV{OUTDIR}/screenshot-$scene-depth$depth.xwd";
    my $png = "$ENV{OUTDIR}/screenshot-$scene-depth$depth.png";
    system('xwd', '-root', '-silent', '-display', $ENV{DISPLAY}, '-out', $xwd) == 0
        or die "xwd failed";
    system('magick', "xwd:$xwd", "png:$png") == 0 or die "magick failed";
    unlink($xwd);

    my $golden = "$golden_dir/$scene.png";
    if ($update) {
        system('cp', $png, $golden) == 0 or die "cp failed";
        pass("$scene: golden written");
        return;
    }
    if (!-e $golden) {
        fail("$scene: no golden $golden");
        return;
    }
    # The number of differing pixels; compare writes it to stderr.
    my $diff = "$ENV{OUTDIR}/screenshot-$scene-depth$depth-diff.png";
    my $ae = qx(compare -metric AE "$golden" "$png" "$diff" 2>&1);
    chomp($ae);
    ($ae) = $ae =~ /^(\d+)/;
    is($ae, 0, "$scene: pixels identical to the golden");
    unlink($diff) if defined($ae) && $ae == 0;
}

sub set_icon {
    my ($window) = @_;
    # A 16x16 icon: an orange square with a dark border, ARGB.
    my @pixels;
    for my $y (0 .. 15) {
        for my $x (0 .. 15) {
            my $edge = $x == 0 || $y == 0 || $x == 15 || $y == 15;
            push @pixels, $edge ? 0xff202020 : 0xffff8000;
        }
    }
    $x->change_property(
        PROP_MODE_REPLACE,
        $window->id,
        $x->atom(name => '_NET_WM_ICON')->id,
        $x->atom(name => 'CARDINAL')->id,
        32,
        2 + @pixels,
        pack('L*', 16, 16, @pixels),
    );
}

my $common = <<EOT;
# i3 config file (v4)
focus_follows_mouse no
mouse_warping none
EOT

#####################################################################
# Core font: layouts, borders and urgency.
#####################################################################

my $pid = launch_with_config($common . <<EOT);
font -misc-fixed-medium-r-normal--13-120-75-75-C-70-iso10646-1
EOT

fresh_workspace;
open_window(name => "left");
open_window(name => "middle");
open_window(name => "right");
capture('split-h');

fresh_workspace;
open_window(name => "top");
cmd 'split v';
open_window(name => "center");
open_window(name => "bottom");
capture('split-v');

fresh_workspace;
open_window(name => "tab one");
open_window(name => "tab two");
open_window(name => "tab three");
cmd 'layout tabbed';
capture('tabbed');

fresh_workspace;
open_window(name => "stack one");
open_window(name => "stack two");
open_window(name => "stack three");
cmd 'layout stacking';
capture('stacked');

fresh_workspace;
open_window(name => "pixel border");
cmd 'border pixel 5';
open_window(name => "no border");
cmd 'border none';
open_window(name => "normal border");
capture('borders');

fresh_workspace;
my $urgent = open_window(name => "urgent");
open_window(name => "focused");
cmd 'layout tabbed';
$urgent->add_hint('urgency');
capture('urgency');

exit_gracefully($pid);

#####################################################################
# Pango with the bundled font: markup and title icons.
#####################################################################

$pid = launch_with_config($common . <<EOT);
font pango:DejaVu Sans Mono 9
EOT

fresh_workspace;
open_window(name => "plain title");
open_window(name => "markup title");
cmd q|title_format "<span foreground='#ff4040'><b>%title</b></span> <i>italic</i>"|;
capture('markup');

fresh_workspace;
open_window(name => "with icon", before_map => \&set_icon);
cmd 'title_window_icon on';
open_window(name => "without icon");
capture('icon');

exit_gracefully($pid);

done_testing;
