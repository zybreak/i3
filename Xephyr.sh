#!/bin/sh

_XDG_SEAT=$XDG_SEAT
unset XDG_SEAT

DISP=${1:-":2"}

Xephyr -ac -br -v +extension RANDR -screen 1280x800 $DISP

export XDG_SEAT=$_XDG_SEAT

