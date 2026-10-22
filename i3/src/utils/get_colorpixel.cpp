/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 */
module;
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <xcb/xcb.h>
module utils;

import std;
import xcb;
import xcb.xcbcompat;

struct Colorpixel {
    char hex[8];
    uint32_t pixel;
};

/* A pixel belongs to the connection that allocated it, which the server frees
 * with it, so each thread has its own cache: a thread allocates colors on one
 * connection (a nagbar's thread on its own, i3's main thread on i3's), and the
 * cache of a nagbar's thread ends with the thread, after its connection. */
static thread_local std::vector<std::unique_ptr<Colorpixel>> colorpixels{};

/*
 * Returns the colorpixel to use for the given hex color (think of HTML).
 *
 * The hex_color has to start with #, for example #FF00FF.
 *
 * NOTE that get_colorpixel() does _NOT_ check the given color code for validity.
 * This has to be done by the caller.
 *
 */
uint32_t get_colorpixel(xcb_connection_t *conn, xcb_screen_t *root_screen, char const *hex) {
    char alpha[2];
    if (strlen(hex) == strlen("#rrggbbaa")) {
        alpha[0] = hex[7];
        alpha[1] = hex[8];
    } else {
        alpha[0] = alpha[1] = 'F';
    }

    char strgroups[4][3] = {
        {hex[1], hex[2], '\0'},
        {hex[3], hex[4], '\0'},
        {hex[5], hex[6], '\0'},
        {alpha[0], alpha[1], '\0'}};
    uint8_t r = strtol(strgroups[0], nullptr, 16);
    uint8_t g = strtol(strgroups[1], nullptr, 16);
    uint8_t b = strtol(strgroups[2], nullptr, 16);
    uint8_t a = strtol(strgroups[3], nullptr, 16);

    /* Shortcut: if our screen is true color, no need to do a roundtrip to X11 */
    if (root_screen == nullptr || root_screen->root_depth == 24 || root_screen->root_depth == 32) {
        return (a << 24) | (r << 16 | g << 8 | b);
    }

    /* Lookup this colorpixel in the cache */
    for (auto &colorpixel : colorpixels) {
        if (strcmp(colorpixel->hex, hex) == 0) {
            return colorpixel->pixel;
        }
    }

#define RGB_8_TO_16(i) (65535 * ((i) & 0xFF) / 255)
    int r16 = RGB_8_TO_16(r);
    int g16 = RGB_8_TO_16(g);
    int b16 = RGB_8_TO_16(b);

    /* conn is a handle of the libxcb compatibility layer, on an xcbpp connection. */
    auto reply = xcb::xproto::alloc_color(xcb::xcbcompat::connection(conn), root_screen->default_colormap, r16, g16, b16).get();

    if (!reply) {
        // LOG("Could not allocate color\n");
        exit(1);
    }

    uint32_t pixel = reply->pixel;

    /* Store the result in the cache */
    auto cache_pixel = std::make_unique<Colorpixel>();

    strncpy(cache_pixel->hex, hex, 7);
    cache_pixel->hex[7] = '\0';

    cache_pixel->pixel = pixel;

    colorpixels.emplace_back(std::move(cache_pixel));

    return pixel;
}
