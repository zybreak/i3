module;
#include <xcb/xcb.h>
export module i3:dpi;

export {
    /**
     * Initialize the DPI setting.
     * This will use the 'Xft.dpi' X resource if available and fall back to
     * guessing the correct value otherwise.
     */
    void init_dpi(xcb_connection_t * conn, xcb_screen_t * root_screen);

    /**
     * This function returns the value of the DPI setting.
     *
     */
    double get_dpi_value();

    /**
     * Convert a logical amount of pixels (e.g. 2 pixels on a “standard” 96 DPI
     * screen) to a corresponding amount of physical pixels on a standard or retina
     * screen, e.g. 5 pixels on a 227 DPI MacBook Pro 13" Retina screen.
     *
     */
    long logical_px(xcb_screen_t * root_screen, long logical);
}