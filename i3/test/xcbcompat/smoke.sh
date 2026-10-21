#!/bin/bash
# smoke.sh <i3> <display number> <work directory> [depth]
#
# Runs i3 on Xvfb and checks that it runs on xcbpp's libxcb compatibility layer, manages a window, replays a click
# to it (GrabButton and AllowEvents), runs a key binding (GrabKey), exits on request, and that its save set gives the
# window back to the root window. The test client (client.c) runs on the system's libxcb, as X clients do; it is
# compiled here with pkg-config, because i3's own build links the layer in libxcb's place.
#
# Exits 0 if every check passes. With depth 8, i3 allocates its colors on the server (AllocColor).
set -u
I3=$1
DPY=:$2
W=$3
DEPTH=${4:-24}
SRC=$(cd "$(dirname "$0")" && pwd)
LAYER_RE='/xcbpp/libxcb\.so'

rm -rf "$W"
mkdir -p "$W"
cc -O1 -o "$W/client" "$SRC/client.c" $(pkg-config --cflags --libs xcb xcb-xtest) || exit 2

cat > "$W/config" <<EOF
# i3 config file (v4)
font pango:monospace 8
ipc-socket $W/ipc
bindcode 75 exec touch $W/key-marker
EOF

failures=0
check() {  # check <description> <command…>
    local what=$1
    shift
    if "$@"; then echo "ok   $what"; else echo "FAIL $what"; failures=$((failures + 1)); fi
}

Xvfb "$DPY" -screen 0 1024x768x"$DEPTH" -nolisten tcp > "$W/xvfb.log" 2>&1 &
XVFB=$!
CLIENT=
I3PID=
trap 'kill $CLIENT $I3PID $XVFB 2>/dev/null; wait 2>/dev/null' EXIT
for _ in $(seq 50); do [ -e "/tmp/.X11-unix/X${DPY#:}" ] && break; sleep 0.1; done
export DISPLAY=$DPY

"$I3" -c "$W/config" -V > "$W/i3.log" 2>&1 &
I3PID=$!
sleep 1.5
if ! kill -0 "$I3PID" 2>/dev/null; then
    echo "FAIL i3 died at startup"
    tail -20 "$W/i3.log"
    exit 1
fi

check "i3's libxcb.so.1 is the layer" grep -qE "$LAYER_RE" "/proc/$I3PID/maps"
check "no other libxcb.so.1 is loaded" bash -c "! grep -E 'libxcb\\.so\\.1' /proc/$I3PID/maps | grep -vqE '$LAYER_RE'"

"$W/client" manage > "$W/client.out" 2>&1 &
CLIENT=$!
for _ in $(seq 60); do grep -qE 'click (ok|lost)|not managed' "$W/client.out" && break; sleep 0.1; done
check "the window is managed" grep -q '^managed' "$W/client.out"
check "the click is replayed to the window" grep -q '^click ok' "$W/client.out"

"$W/client" key 75
for _ in $(seq 20); do [ -e "$W/key-marker" ] && break; sleep 0.1; done
check "the key binding runs" test -e "$W/key-marker"

i3-msg -s "$W/ipc" exit > /dev/null 2>&1
for _ in $(seq 30); do kill -0 "$I3PID" 2>/dev/null || break; sleep 0.1; done
check "i3 exits" bash -c "! kill -0 $I3PID 2>/dev/null"
wait "$I3PID"
check "i3's exit status is 0" test $? -eq 0
I3PID=
check "the save set gives the window back to the root" grep -q '^orphans 1$' <("$W/client" orphans)

exit $((failures > 0))
