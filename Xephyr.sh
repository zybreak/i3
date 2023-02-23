#!/bin/sh

_XDG_SEAT=$XDG_SEAT
unset XDG_SEAT

Xephyr -ac -br -v +extension RANDR -screen 1280x800 :2

export XDG_SEAT=$_XDG_SEAT

