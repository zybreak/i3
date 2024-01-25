/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 */
module;
#include <xcb/xcb_xrm.h>
#include <cstdlib>
#include <memory>
#include <fmt/printf.h>
module i3;

import log;

static double dpi;

static double init_dpi_fallback(xcb_screen_t *root_screen) {
    return (double)root_screen->height_in_pixels * 25.4 / (double)root_screen->height_in_millimeters;
}

struct database_free
{
    void operator()(xcb_xrm_database_t* database) {
        xcb_xrm_database_free(database);
    }
};

struct ptr_free
{
    void operator()(void* p) {
        free(p);
    }
};

static double _init_dpi(xcb_connection_t *conn) {
    if (conn == nullptr) {
        return 0;
    }

    std::unique_ptr<xcb_xrm_database_t, database_free> database{xcb_xrm_database_from_default(conn)};
    if (database == nullptr) {
        ELOG("Failed to open the resource database.\n");

        return 0;
    }

    char *p = nullptr;
    xcb_xrm_resource_get_string(database.get(), "Xft.dpi", nullptr, &p);
    if (p == nullptr) {
        DLOG("Resource Xft.dpi not specified, skipping.\n");

        return 0;
    }
    std::unique_ptr<char, ptr_free> resource{p};

    char *endptr;
    double in_dpi = strtod(resource.get(), &endptr);
    if (in_dpi == HUGE_VAL || dpi < 0 || *endptr != '\0' || endptr == resource.get()) {
         ELOG(fmt::sprintf("Xft.dpi = %s is an invalid number and couldn't be parsed.\n",  *resource));
        return 0;
    }
    double _dpi = std::round(in_dpi);

     DLOG(fmt::sprintf("Found Xft.dpi = %f.\n",  _dpi));

    return _dpi;
}

/*
 * Initialize the DPI setting.
 * This will use the 'Xft.dpi' X resource if available and fall back to
 * guessing the correct value otherwise.
 */
void init_dpi(xcb_connection_t *conn, xcb_screen_t *root_screen) {
    dpi = _init_dpi(conn);

    if (dpi == 0) {
        DLOG("Using fallback for calculating DPI.\n");
        dpi = init_dpi_fallback(root_screen);
         DLOG(fmt::sprintf("Using dpi = %f\n",  dpi));
    }
}

/*
 * This function returns the value of the DPI setting.
 *
 */
double get_dpi_value() {
    return dpi;
}

/*
 * Convert a logical amount of pixels (e.g. 2 pixels on a “standard” 96 DPI
 * screen) to a corresponding amount of physical pixels on a standard or retina
 * screen, e.g. 5 pixels on a 227 DPI MacBook Pro 13" Retina screen.
 *
 */
long logical_px(xcb_screen_t *root_screen, const long logical) {
    if (root_screen == nullptr) {
        /* Dpi info may not be available when parsing a config without an X
         * server, such as for config file validation. */
        return logical;
    }

    /* There are many misconfigurations out there, i.e. systems with screens
     * whose dpi is in fact higher than 96 dpi, but not significantly higher,
     * so software was never adapted. We could tell people to reconfigure their
     * systems to 96 dpi in order to get the behavior they expect/are used to,
     * but since we can easily detect this case in code, let’s do it for them.
     */
    if ((dpi / 96.0) < 1.25) {
        return logical;
    }
    return ceil((dpi / 96.0) * logical);
}
