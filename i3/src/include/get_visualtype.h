#pragma once

#include <xcb/xcb.h>

/**
 * Returns the visual type associated with the given screen.
 *
 */
xcb_visualtype_t *get_visualtype(xcb_screen_t *screen);
