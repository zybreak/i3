module;
#include <xcb/xcb.h>
#include <xcb/xcb_icccm.h>
#include <cairo.h>
#include <fmt/printf.h>
module i3;

import std;
import log;

namespace handle_property {

/*
 * Updates the WM_CLASS (consisting of the class and instance) for the
 * given window.
 *
 */
std::optional<window_class> window_update_class(xcb_get_property_reply_t *prop) {
    if (prop == nullptr || xcb_get_property_value_length(prop) == 0) {
        DLOG("WM_CLASS not set.\n");
        return std::nullopt;
    }

    /* We cannot use asprintf here since this property contains two
     * null-terminated strings (for compatibility reasons). Instead, we
     * use strdup() on both strings */
    const size_t prop_length = xcb_get_property_value_length(prop);
    char *new_class = (char *)xcb_get_property_value(prop);
    const size_t class_class_index = strnlen(new_class, prop_length) + 1;

    std::string class_instance{};
    std::string class_class{};

    class_instance.assign(new_class, prop_length);
    if (class_class_index < prop_length) {
        class_class.assign(new_class + class_class_index, prop_length - class_class_index);
    }

    return window_class{
        .class_name = class_class,
        .class_instance = class_instance};
}

/*
 * Updates the name by using _NET_WM_NAME (encoded in UTF-8) for the given
 * window. Further updates using window_update_name_legacy will be ignored.
 *
 */
std::optional<std::string> window_update_name(xcb_get_property_reply_t *prop) {
    if (prop == nullptr || xcb_get_property_value_length(prop) == 0) {
        DLOG("_NET_WM_NAME not specified, not changing\n");
        return std::nullopt;
    }

    /* Truncate the name at the first zero byte. See #3515. */
    auto len = static_cast<std::string::size_type>(xcb_get_property_value_length(prop));
    std::string name{(const char *)xcb_get_property_value(prop), len};

    return name;
}

/*
 * Updates the name by using WM_NAME (encoded in COMPOUND_TEXT). We do not
 * touch what the client sends us but pass it to xcb_image_text_8. To get
 * proper unicode rendering, the application has to use _NET_WM_NAME (see
 * window_update_name()).
 *
 */
std::optional<std::string> window_update_name_legacy(xcb_get_property_reply_t *prop) {
    if (prop == nullptr || xcb_get_property_value_length(prop) == 0) {
        DLOG("WM_NAME not set (_NET_WM_NAME is what you want anyways).\n");
        return std::nullopt;
    }

    const auto len = static_cast<std::string::size_type>(xcb_get_property_value_length(prop));
    std::string name{(const char *)xcb_get_property_value(prop), len};

    return name;
}

/*
 * Updates the CLIENT_LEADER (logical parent window).
 *
 */
std::optional<xcb_window_t> window_update_leader(xcb_get_property_reply_t *prop) {
    if (prop == nullptr || xcb_get_property_value_length(prop) == 0) {
        DLOG(fmt::sprintf("CLIENT_LEADER not set.\n"));
        return std::nullopt;
    }

    auto *leader_prop = (xcb_window_t *)xcb_get_property_value(prop);
    if (leader_prop == nullptr) {
        return std::nullopt;
    }

    return *leader_prop;
}

/*
 * Updates the TRANSIENT_FOR (logical parent window).
 *
 */
std::optional<xcb_window_t> window_update_transient_for(xcb_get_property_reply_t *prop) {
    if (prop == nullptr || xcb_get_property_value_length(prop) == 0) {
        return std::nullopt;
    }

    xcb_window_t transient_for;
    if (!xcb_icccm_get_wm_transient_for_from_reply(&transient_for, prop)) {
        return std::nullopt;
    }

    return transient_for;
}

/*
 * Updates the _NET_WM_STRUT_PARTIAL (reserved pixels at the screen edges)
 *
 */
std::optional<reservedpx> window_update_strut_partial(xcb_get_property_reply_t *prop) {
    if (prop == nullptr || xcb_get_property_value_length(prop) == 0) {
        return std::nullopt;
    }

    uint32_t *strut;
    if (!(strut = (uint32_t *)xcb_get_property_value(prop))) {
        return std::nullopt;
    }

    return reservedpx{
        .left = strut[0],
        .right = strut[1],
        .top = strut[2],
        .bottom = strut[3]};
}

/*
 * Updates the WM_WINDOW_ROLE
 *
 */
std::optional<std::string> window_update_role(xcb_get_property_reply_t *prop) {
    if (prop == nullptr || xcb_get_property_value_length(prop) == 0) {
        return std::nullopt;
    }

    std::string role{(char *)xcb_get_property_value(prop), static_cast<std::string::size_type>(xcb_get_property_value_length(prop))};
    return role;
}

/*
 * Updates the _NET_WM_WINDOW_TYPE property.
 *
 */
std::optional<xcb_atom_t> window_update_type(xcb_get_property_reply_t *reply) {
    xcb_atom_t new_type = xcb_get_preferred_window_type(reply);
    if (new_type == XCB_NONE) {
        return std::nullopt;
    }

    return new_type;
}

/*
 * Updates the WM_HINTS (we only care about the input focus handling part).
 *
 */
std::optional<window_hints> window_update_hints(xcb_get_property_reply_t *prop) {
    if (prop == nullptr || xcb_get_property_value_length(prop) == 0) {
        DLOG("WM_HINTS not set.\n");
        return std::nullopt;
    }

    xcb_icccm_wm_hints_t hints;
    bool doesnt_accept_focus = false;

    if (!xcb_icccm_get_wm_hints_from_reply(&hints, prop)) {
        DLOG("Could not get WM_HINTS\n");
        return std::nullopt;
    }

    if (hints.flags & XCB_ICCCM_WM_HINT_INPUT) {
        doesnt_accept_focus = !hints.input;
        LOG(fmt::sprintf("WM_HINTS.input changed to \"%d\"\n", hints.input));
    }

    bool urgency_hint = (xcb_icccm_wm_hints_get_urgency(&hints) != 0);

    return window_hints{
        .doesnt_accept_focus = doesnt_accept_focus,
        .urgency_hint = urgency_hint};
}

/* See `man VendorShell' for more info, `XmNmwmDecorations' section:
 * https://linux.die.net/man/3/vendorshell
 * The following constants are adapted from <Xm/MwmUtil.h>.
 */
static const unsigned int MWM_HINTS_FLAGS_FIELD = 0;
static const unsigned int MWM_HINTS_DECORATIONS_FIELD = 2;

static const unsigned int MWM_HINTS_DECORATIONS = (1 << 1);
static const unsigned int MWM_DECOR_ALL = (1 << 0);
static const unsigned int MWM_DECOR_BORDER = (1 << 1);
static const unsigned int MWM_DECOR_TITLE = (1 << 3);

static border_style_t border_style_from_motif_value(uint32_t value) {
    if (value & MWM_DECOR_ALL) {
        /* If this value is set, all other flags set are exclusive:
         * MWM_DECOR_ALL
         * All decorations except those specified by other flag bits that are set
         *
         * We support these cases:
         * - No exceptions -> BS_NORMAL
         * - Title and no border (ignored) -> BS_NORMAL
         * - No title and no border -> BS_NONE
         * - No title and border -> BS_PIXEL
         * */

        if (value & MWM_DECOR_TITLE) {
            if (value & MWM_DECOR_BORDER) {
                return border_style_t::BS_NONE;
            }

            return border_style_t::BS_PIXEL;
        }

        return border_style_t::BS_NORMAL;
    } else if (value & MWM_DECOR_TITLE) {
        return border_style_t::BS_NORMAL;
    } else if (value & MWM_DECOR_BORDER) {
        return border_style_t::BS_PIXEL;
    } else {
        return border_style_t::BS_NONE;
    }
}

/*
 * Updates the MOTIF_WM_HINTS. The container's border style should be set to
 * `motif_border_style' if border style is not BS_NORMAL.
 *
 * i3 only uses this hint when it specifies a window should have no
 * title bar, or no decorations at all, which is how most window managers
 * handle it.
 *
 * The EWMH spec intended to replace Motif hints with _NET_WM_WINDOW_TYPE, but
 * it is still in use by popular widget toolkits such as GTK+ and Java AWT.
 *
 */
std::optional<uint32_t> window_update_motif_hints(xcb_get_property_reply_t *prop) {
    if (prop == nullptr) {
        return std::nullopt;
    }

    if (xcb_get_property_value_length(prop) == 0) {
        return std::nullopt;
    }

    /* The property consists of an array of 5 uint32_t's. The first value is a
     * bit mask of what properties the hint will specify. We are only interested
     * in MWM_HINTS_DECORATIONS because it indicates that the third value of the
     * array tells us which decorations the window should have, each flag being
     * a particular decoration. Notice that X11 (Xlib) often mentions 32-bit
     * fields which in reality are implemented using unsigned long variables
     * (64-bits long on amd64 for example). On the other hand,
     * xcb_get_property_value() behaves strictly according to documentation,
     * i.e. returns 32-bit data fields. */
    uint32_t *motif_hints = reinterpret_cast<uint32_t *>(xcb_get_property_value(prop));

    if (motif_hints[MWM_HINTS_FLAGS_FIELD] & MWM_HINTS_DECORATIONS) {
        return std::to_underlying(border_style_from_motif_value(motif_hints[MWM_HINTS_DECORATIONS_FIELD]));
    }
    return std::nullopt;
}

/*
 * Updates the WM_CLIENT_MACHINE
 *
 */
std::optional<std::string> window_update_machine(xcb_get_property_reply_t *prop) {
    if (prop == nullptr || xcb_get_property_value_length(prop) == 0) {
        DLOG("WM_CLIENT_MACHINE not set.\n");
        return std::nullopt;
    }

    std::string machine{(char *)xcb_get_property_value(prop), static_cast<std::string::size_type>(xcb_get_property_value_length(prop))};

    return machine;
}

#if 0
// TODO: FIXME
void window_update_icon(xcb_get_property_reply_t *prop) {
    uint32_t *data = nullptr;
    uint32_t width = 0, height = 0;
    uint64_t len = 0;
    const auto pref_size = static_cast<uint32_t>(render_deco_height() - logical_px(global.x->root_screen, 2));

    if (!prop || prop->type != XCB_ATOM_CARDINAL || prop->format != 32) {
        DLOG("_NET_WM_ICON is not set\n");
        return;
    }

    auto prop_value_len = static_cast<uint32_t>(xcb_get_property_value_length(prop));
    auto *prop_value = static_cast<uint32_t*>(xcb_get_property_value(prop));

    /* Find an icon matching the preferred size.
     * If there is no such icon, take the smallest icon having at least
     * the preferred size.
     * If all icons are smaller than the preferred size, chose the largest.
     */
    while (prop_value_len > (sizeof(uint32_t) * 2) && prop_value &&
           prop_value[0] && prop_value[1]) {
        const uint32_t cur_width = prop_value[0];
        const uint32_t cur_height = prop_value[1];
        /* Check that the property is as long as it should be (in bytes),
           handling integer overflow. "+2" to handle the width and height
           fields. */
        const uint64_t cur_len = cur_width * static_cast<uint64_t>(cur_height);
        const uint64_t expected_len = (cur_len + 2) * 4;

        if (expected_len > prop_value_len) {
            break;
        }

        DLOG(fmt::sprintf("Found _NET_WM_ICON of size: (%d,%d)\n",  cur_width, cur_height));

        const bool at_least_preferred_size = (cur_width >= pref_size &&
                                              cur_height >= pref_size);
        const bool smaller_than_current = (cur_width < width ||
                                           cur_height < height);
        const bool larger_than_current = (cur_width > width ||
                                          cur_height > height);
        const bool not_yet_at_preferred = (width < pref_size ||
                                           height < pref_size);
        if (len == 0 ||
            (at_least_preferred_size &&
             (smaller_than_current || not_yet_at_preferred)) ||
            (!at_least_preferred_size &&
             not_yet_at_preferred &&
             larger_than_current)) {
            len = cur_len;
            width = cur_width;
            height = cur_height;
            data = prop_value;
        }

        if (width == pref_size && height == pref_size) {
            break;
        }

        /* Find pointer to next icon in the reply. */
        prop_value_len -= expected_len;
        prop_value = (uint32_t *)(((uint8_t *)prop_value) + expected_len);
    }

    if (!data) {
        DLOG("Could not get _NET_WM_ICON\n");
        return;
    }

    DLOG(fmt::sprintf("Using icon of size (%d,%d) (preferred size: %d)\n",
                      width, height, pref_size));

    this->name_x_changed = true; /* trigger a redraw */

    auto *icon = (uint32_t*)smalloc(len * 4);

    for (uint64_t i = 0; i < len; i++) {
        uint8_t r, g, b, a;
        const uint32_t pixel = data[2 + i];
        a = (pixel >> 24) & 0xff;
        r = (pixel >> 16) & 0xff;
        g = (pixel >> 8) & 0xff;
        b = (pixel >> 0) & 0xff;

        /* Cairo uses premultiplied alpha */
        r = (r * a) / 0xff;
        g = (g * a) / 0xff;
        b = (b * a) / 0xff;

        icon[i] = (static_cast<uint32_t>(a) << 24) | (r << 16) | (g << 8) | b;
    }

    if (this->icon != nullptr) {
        cairo_surface_destroy(this->icon);
    }
    this->icon = cairo_image_surface_create_for_data(
        (unsigned char *)icon,
        CAIRO_FORMAT_ARGB32,
        width,
        height,
        width * 4);
    static cairo_user_data_key_t free_data;
    cairo_surface_set_user_data(this->icon, &free_data, icon, free);
}
#endif

}