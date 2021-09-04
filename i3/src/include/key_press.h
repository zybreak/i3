/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * key_press.c: key press handler
 *
 */
#pragma once

#include <config.h>

/**
 * There was a key press. We compare this key code with our bindings table and pass
 * the bound action to parse_command().
 *
 */
void handle_key_press(xcb_key_press_event_t *event);
