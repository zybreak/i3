#!/bin/bash
# check_config.sh <i3> <display number> <work directory>
#
# Checks i3 -C (check the configuration file and exit) on Xvfb: a configuration with workspace assignments,
# per-workspace gaps and for_window, assign and no_focus directives is valid (exit 0, no error), and an invalid one is
# reported (exit 1, an error). i3 -C runs without a WorkspaceManager or an AssignmentManager; those directives once
# crashed it. It needs an X server: the configuration loads fonts and colours on i3's X connection.
#
# Exits 0 if every check passes.
set -u
I3=$1
DPY=:$2
W=$3

rm -rf "$W"
mkdir -p "$W"

cat > "$W/valid" <<'EOF'
# i3 config file (v4)
font pango:monospace 8
workspace 1 output DP-1
workspace 2 output HDMI-1 DP-1
workspace 1 gaps inner 5
workspace 3 gaps outer 2
for_window [class="XTerm"] border pixel 1
assign [class="Firefox"] 2
assign [class="mpv"] output DP-1
no_focus [class="Dialog"]
EOF

cat > "$W/invalid" <<'EOF'
# i3 config file (v4)
font pango:monospace 8
workspace 1 output DP-1
invalid
EOF

failures=0
check() {  # check <description> <command…>
    local what=$1
    shift
    if "$@"; then echo "ok   $what"; else echo "FAIL $what"; failures=$((failures + 1)); fi
}
has_error() {  # has_error <i3 output>: i3's error log lines, or upstream's ERROR
    grep -qE ' error \[thread |ERROR' "$1"
}
no_error() {
    ! has_error "$1"
}

Xvfb "$DPY" -screen 0 1024x768x24 -nolisten tcp > "$W/xvfb.log" 2>&1 &
XVFB=$!
trap 'kill $XVFB 2>/dev/null; wait 2>/dev/null' EXIT
for _ in $(seq 50); do [ -e "/tmp/.X11-unix/X${DPY#:}" ] && break; sleep 0.1; done
export DISPLAY=$DPY

"$I3" -C -c "$W/valid" > "$W/valid.out" 2>&1
status=$?
check "a valid configuration exits 0 (exit $status)" test "$status" -eq 0
check "a valid configuration reports no error" no_error "$W/valid.out"

"$I3" -C -c "$W/invalid" > "$W/invalid.out" 2>&1
status=$?
check "an invalid configuration exits 1 (exit $status)" test "$status" -eq 1
check "an invalid configuration reports an error" has_error "$W/invalid.out"

if [ "$failures" -ne 0 ]; then
    tail -20 "$W/valid.out" "$W/invalid.out"
fi
exit $((failures != 0))
