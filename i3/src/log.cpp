/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * log.c: Logging functions.
 *
 */
#include "log.h"

static bool debug_logging = false;
static bool verbose = false;

bool get_verbosity() {
    return verbose;
}
/*
 * Set verbosity of i3. If verbose is set to true, informative messages will
 * be printed to stdout. If verbose is set to false, only errors will be
 * printed.
 *
 */
void set_verbosity(bool _verbose) {
    verbose = _verbose;
}

/*
 * Get debug logging.
 *
 */
bool get_debug_logging() {
    return debug_logging;
}

/*
 * Set debug logging.
 *
 */
void set_debug_logging(const bool _debug_logging) {
    debug_logging = _debug_logging;
}

