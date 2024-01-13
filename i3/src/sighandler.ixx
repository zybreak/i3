/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 */
module;

#include <config.h>

export module i3:sighandler;

export {

/**
 * Configured a signal handler to gracefully handle crashes and allow the user
 * to generate a backtrace and rescue their session.
 *
 */
void setup_signal_handler();
}