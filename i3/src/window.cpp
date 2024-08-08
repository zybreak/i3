/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * window.c: Updates window attributes (X11 hints/properties).
 *
 */
module;
#include <cassert>
#include <xcb/xcb.h>
#include <xcb/xcb_icccm.h>

#include <cairo.h>

#include <fmt/printf.h>

#include <float.h>
module i3;

import log;
import utils;

/*
 * Frees an i3Window and all its members.
 *
 */
i3Window::~i3Window() {
    cairo_surface_destroy(this->icon);
}

/*
 * Updates the WM_CLASS (consisting of the class and instance) for the
 * given window.
 *
 */
void i3Window::window_update_class(xcb_get_property_reply_t *prop) {
    if (prop == nullptr || xcb_get_property_value_length(prop) == 0) {
        DLOG("WM_CLASS not set.\n");
        return;
    }

    /* We cannot use asprintf here since this property contains two
     * null-terminated strings (for compatibility reasons). Instead, we
     * use strdup() on both strings */
    const size_t prop_length = xcb_get_property_value_length(prop);
    char *new_class = (char*)xcb_get_property_value(prop);
    const size_t class_class_index = strnlen(new_class, prop_length) + 1;

    this->class_instance.clear();
    this->class_class.clear();

    this->class_instance.assign(new_class, prop_length);
    if (class_class_index < prop_length) {
        this->class_class.assign(new_class + class_class_index, prop_length - class_class_index);
    }
    LOG(fmt::sprintf("WM_CLASS changed to %s (instance), %s (class)\n",
        this->class_instance, this->class_class));
}

/*
 * Updates the name by using _NET_WM_NAME (encoded in UTF-8) for the given
 * window. Further updates using window_update_name_legacy will be ignored.
 *
 */
void i3Window::window_update_name(xcb_get_property_reply_t *prop) {
    if (prop == nullptr || xcb_get_property_value_length(prop) == 0) {
        DLOG("_NET_WM_NAME not specified, not changing\n");
        return;
    }

    /* Truncate the name at the first zero byte. See #3515. */
    const int len = xcb_get_property_value_length(prop);
    char *prop_name = sstrndup((const char *)xcb_get_property_value(prop), len);
    this->name = prop_name;
    free(prop_name);

    ConCon *con = con_by_window_id(id);
    if (con != nullptr && !con->title_format.empty()) {
        std::string title = con_parse_title_format(con);
        ewmh_update_visible_name(id, title.c_str());
    }
    name_x_changed = true;
    LOG(fmt::sprintf("_NET_WM_NAME changed to \"%s\"\n", this->name));

    uses_net_wm_name = true;
}

/*
 * Updates the name by using WM_NAME (encoded in COMPOUND_TEXT). We do not
 * touch what the client sends us but pass it to xcb_image_text_8. To get
 * proper unicode rendering, the application has to use _NET_WM_NAME (see
 * window_update_name()).
 *
 */
void i3Window::window_update_name_legacy(xcb_get_property_reply_t *prop) {
    if (prop == nullptr || xcb_get_property_value_length(prop) == 0) {
        DLOG("WM_NAME not set (_NET_WM_NAME is what you want anyways).\n");
        return;
    }

    /* ignore update when the window is known to already have a UTF-8 name */
    if (uses_net_wm_name) {
        return;
    }

    const int len = xcb_get_property_value_length(prop);
    char *prop_name = sstrndup((const char *)xcb_get_property_value(prop), len);
    this->name = prop_name;
    free(prop_name);

    ConCon *con = con_by_window_id(id);
    if (con != nullptr && !con->title_format.empty()) {
        std::string title = con_parse_title_format(con);
        ewmh_update_visible_name(id, title.c_str());
    }

    LOG(fmt::sprintf("WM_NAME changed to \"%s\"\n", this->name));
    LOG("Using legacy window title. Note that in order to get Unicode window "
        "titles in i3, the application has to set _NET_WM_NAME (UTF-8)\n");

    name_x_changed = true;
}

/*
 * Updates the CLIENT_LEADER (logical parent window).
 *
 */
void i3Window::window_update_leader(xcb_get_property_reply_t *prop) {
    if (prop == nullptr || xcb_get_property_value_length(prop) == 0) {
        DLOG(fmt::sprintf("CLIENT_LEADER not set on window 0x%08x.\n",  this->id));
        this->leader = XCB_NONE;
        return;
    }

    auto *leader_prop = (xcb_window_t*)xcb_get_property_value(prop);
    if (leader_prop == nullptr) {
        return;
    }

    DLOG(fmt::sprintf("Client leader changed to %08x\n",  *leader_prop));

    this->leader = *leader_prop;
}

/*
 * Updates the TRANSIENT_FOR (logical parent window).
 *
 */
void i3Window::window_update_transient_for(xcb_get_property_reply_t *prop) {
    if (prop == nullptr || xcb_get_property_value_length(prop) == 0) {
        DLOG(fmt::sprintf("TRANSIENT_FOR not set on window 0x%08x.\n",  this->id));
        this->transient_for = XCB_NONE;
        return;
    }

    xcb_window_t transient_for;
    if (!xcb_icccm_get_wm_transient_for_from_reply(&transient_for, prop)) {
        return;
    }

    DLOG(fmt::sprintf("Transient for changed to 0x%08x (window 0x%08x)\n",  transient_for, this->id));

    this->transient_for = transient_for;
}

/*
 * Updates the _NET_WM_STRUT_PARTIAL (reserved pixels at the screen edges)
 *
 */
void i3Window::window_update_strut_partial(xcb_get_property_reply_t *prop) {
    if (prop == nullptr || xcb_get_property_value_length(prop) == 0) {
        DLOG("_NET_WM_STRUT_PARTIAL not set.\n");
        return;
    }

    uint32_t *strut;
    if (!(strut = (uint32_t*)xcb_get_property_value(prop))) {
        return;
    }

    DLOG(fmt::sprintf("Reserved pixels changed to: left = %d, right = %d, top = %d, bottom = %d\n",
         strut[0], strut[1], strut[2], strut[3]));

    this->reserved = (struct reservedpx){strut[0], strut[1], strut[2], strut[3]};
}

/*
 * Updates the WM_WINDOW_ROLE
 *
 */
void i3Window::window_update_role(xcb_get_property_reply_t *prop) {
    if (prop == nullptr || xcb_get_property_value_length(prop) == 0) {
        DLOG("WM_WINDOW_ROLE not set.\n");
        return;
    }

    this->role.assign((char *)xcb_get_property_value(prop), xcb_get_property_value_length(prop));
     LOG(fmt::sprintf("WM_WINDOW_ROLE changed to \"%s\"\n", this->role));
}

/*
 * Updates the _NET_WM_WINDOW_TYPE property.
 *
 */
bool i3Window::window_update_type(xcb_get_property_reply_t *reply) {
    xcb_atom_t new_type = xcb_get_preferred_window_type(reply);
    if (new_type == XCB_NONE) {
        DLOG("cannot read _NET_WM_WINDOW_TYPE from window.\n");
        return false;
    }

    this->window_type = new_type;
    LOG(fmt::sprintf("_NET_WM_WINDOW_TYPE changed to %i.\n",  this->window_type));

    return true;
}

/*
 * Updates the WM_NORMAL_HINTS
 *
 */
bool i3Window::window_update_normal_hints(xcb_get_property_reply_t *reply, xcb_get_geometry_reply_t *geom) {
    bool changed = false;
    xcb_size_hints_t size_hints;

    /* If the hints were already in this event, use them, if not, request them */
    bool success;
    if (reply != nullptr) {
        success = xcb_icccm_get_wm_size_hints_from_reply(&size_hints, reply);
    } else {
        success = xcb_icccm_get_wm_normal_hints_reply(**global.x, xcb_icccm_get_wm_normal_hints_unchecked(**global.x, this->id), &size_hints, nullptr);
    }
    if (!success) {
        DLOG("Could not get WM_NORMAL_HINTS\n");
        return false;
    }

    if ((size_hints.flags & XCB_ICCCM_SIZE_HINT_P_MIN_SIZE)) {
        DLOG(fmt::sprintf("Minimum size: %d (width) x %d (height)\n",  size_hints.min_width, size_hints.min_height));

        if (this->min_width != size_hints.min_width) {
            this->min_width = size_hints.min_width;
            changed = true;
        }
        if (this->min_height != size_hints.min_height) {
            this->min_height = size_hints.min_height;
            changed = true;
        }
    }

    if ((size_hints.flags & XCB_ICCCM_SIZE_HINT_P_MAX_SIZE)) {
        DLOG(fmt::sprintf("Maximum size: %d (width) x %d (height)\n",  size_hints.max_width, size_hints.max_height));

        uint32_t max_width = std::max(0, size_hints.max_width);
        uint32_t max_height = std::max(0, size_hints.max_height);

        if (this->max_width != max_width) {
            this->max_width = max_width;
            changed = true;
        }
        if (this->max_height != max_height) {
            this->max_height = max_height;
            changed = true;
        }
    } else {
        DLOG("Clearing maximum size\n");

        if (this->max_width != 0) {
            this->max_width = 0;
            changed = true;
        }
        if (this->max_height != 0) {
            this->max_height = 0;
            changed = true;
        }
    }

    if ((size_hints.flags & XCB_ICCCM_SIZE_HINT_P_RESIZE_INC)) {
        DLOG(fmt::sprintf("Size increments: %d (width) x %d (height)\n",  size_hints.width_inc, size_hints.height_inc));

        if (size_hints.width_inc > 0 && size_hints.width_inc < 0xFFFF) {
            if (this->width_increment != size_hints.width_inc) {
                this->width_increment = size_hints.width_inc;
                changed = true;
            }
        } else {
            if (this->width_increment != 0) {
                this->width_increment = 0;
                changed = true;
            }
        }

        if (size_hints.height_inc > 0 && size_hints.height_inc < 0xFFFF) {
            if (this->height_increment != size_hints.height_inc) {
                this->height_increment = size_hints.height_inc;
                changed = true;
            }
        } else {
            if (this->height_increment != 0) {
                this->height_increment = 0;
                changed = true;
            }
        }
    } else {
        DLOG("Clearing size increments\n");

        if (this->width_increment != 0) {
            this->width_increment = 0;
            changed = true;
        }
        if (this->height_increment != 0) {
            this->height_increment = 0;
            changed = true;
        }
    }

    /* The base width / height is the desired size of the window. */
    if (size_hints.flags & XCB_ICCCM_SIZE_HINT_BASE_SIZE &&
        (this->base_width >= 0) && (this->base_height >= 0)) {
        DLOG(fmt::sprintf("Base size: %d (width) x %d (height)\n",  size_hints.base_width, size_hints.base_height));

        if (this->base_width != size_hints.base_width) {
            this->base_width = size_hints.base_width;
            changed = true;
        }
        if (this->base_height != size_hints.base_height) {
            this->base_height = size_hints.base_height;
            changed = true;
        }
    } else {
        DLOG("Clearing base size\n");

        if (this->base_width != 0) {
            this->base_width = 0;
            changed = true;
        }
        if (this->base_height != 0) {
            this->base_height = 0;
            changed = true;
        }
    }

    if (geom != nullptr &&
        (size_hints.flags & XCB_ICCCM_SIZE_HINT_US_POSITION || size_hints.flags & XCB_ICCCM_SIZE_HINT_P_POSITION) &&
        (size_hints.flags & XCB_ICCCM_SIZE_HINT_US_SIZE || size_hints.flags & XCB_ICCCM_SIZE_HINT_P_SIZE)) {
        DLOG(fmt::sprintf("Setting geometry x=%d y=%d w=%d h=%d\n",  size_hints.x, size_hints.y, size_hints.width, size_hints.height));
        geom->x = size_hints.x;
        geom->y = size_hints.y;
        geom->width = size_hints.width;
        geom->height = size_hints.height;
    }

    /* If no aspect ratio was set or if it was invalid, we ignore the hints */
    if (size_hints.flags & XCB_ICCCM_SIZE_HINT_P_ASPECT &&
        (size_hints.min_aspect_num >= 0) && (size_hints.min_aspect_den > 0) &&
        (size_hints.max_aspect_num >= 0) && (size_hints.max_aspect_den > 0)) {
        /* Convert numerator/denominator to a double */
        double min_aspect = static_cast<double>(size_hints.min_aspect_num) / size_hints.min_aspect_den;
        double max_aspect = static_cast<double>(size_hints.max_aspect_num) / size_hints.max_aspect_den;
        DLOG(fmt::sprintf("Aspect ratio set: minimum %f, maximum %f\n",  min_aspect, max_aspect));
        if (fabs(this->min_aspect_ratio - min_aspect) > DBL_EPSILON) {
            this->min_aspect_ratio = min_aspect;
            changed = true;
        }
        if (fabs(this->max_aspect_ratio - max_aspect) > DBL_EPSILON) {
            this->max_aspect_ratio = max_aspect;
            changed = true;
        }
    } else {
        DLOG("Clearing aspect ratios\n");

        if (this->min_aspect_ratio != 0.0) {
            this->min_aspect_ratio = 0.0;
            changed = true;
        }
        if (this->max_aspect_ratio != 0.0) {
            this->max_aspect_ratio = 0.0;
            changed = true;
        }
    }

    return changed;
}

/*
 * Updates the WM_HINTS (we only care about the input focus handling part).
 *
 */
void i3Window::window_update_hints(xcb_get_property_reply_t *prop, bool *urgency_hint) {
    if (urgency_hint != nullptr) {
        *urgency_hint = false;
    }

    if (prop == nullptr || xcb_get_property_value_length(prop) == 0) {
        DLOG("WM_HINTS not set.\n");
        return;
    }

    xcb_icccm_wm_hints_t hints;

    if (!xcb_icccm_get_wm_hints_from_reply(&hints, prop)) {
        DLOG("Could not get WM_HINTS\n");
        return;
    }

    if (hints.flags & XCB_ICCCM_WM_HINT_INPUT) {
        this->doesnt_accept_focus = !hints.input;
         LOG(fmt::sprintf("WM_HINTS.input changed to \"%d\"\n", hints.input));
    }

    if (urgency_hint != nullptr) {
        *urgency_hint = (xcb_icccm_wm_hints_get_urgency(&hints) != 0);
    }
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
                return BS_NONE;
            }

            return BS_PIXEL;
        }

        return BS_NORMAL;
    } else if (value & MWM_DECOR_TITLE) {
        return BS_NORMAL;
    } else if (value & MWM_DECOR_BORDER) {
        return BS_PIXEL;
    } else {
        return BS_NONE;
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
bool i3Window::window_update_motif_hints(xcb_get_property_reply_t *prop, border_style_t *motif_border_style) {
    if (prop == nullptr) {
        return false;
    }
    assert(motif_border_style != nullptr);

    if (xcb_get_property_value_length(prop) == 0) {
        return false;
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
    uint32_t *motif_hints = (uint32_t *)xcb_get_property_value(prop);

    if (motif_hints[MWM_HINTS_FLAGS_FIELD] & MWM_HINTS_DECORATIONS) {
        *motif_border_style = border_style_from_motif_value(motif_hints[MWM_HINTS_DECORATIONS_FIELD]);
        return true;
    }
    return false;
}

/*
 * Updates the WM_CLIENT_MACHINE
 *
 */
void i3Window::window_update_machine(const std::string &machine) {
    this->machine = machine;
    LOG(fmt::sprintf("WM_CLIENT_MACHINE changed to \"%s\"\n", this->machine));
}

/*
 * Updates the WM_CLIENT_MACHINE
 *
 */
void i3Window::window_update_machine(xcb_get_property_reply_t *prop) {
    if (prop == nullptr || xcb_get_property_value_length(prop) == 0) {
        DLOG("WM_CLIENT_MACHINE not set.\n");
        return;
    }

    this->machine.assign((char *)xcb_get_property_value(prop), xcb_get_property_value_length(prop));
     LOG(fmt::sprintf("WM_CLIENT_MACHINE changed to \"%s\"\n", this->machine));
}

void i3Window::window_update_icon(xcb_get_property_reply_t *prop) {
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

    //int stride = cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, width); stride not really working for chromium?

    auto *icon_data = new uint32_t[len*4];

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

        icon_data[i] = (static_cast<uint32_t>(a) << 24) | (r << 16) | (g << 8) | b;
    }

    if (this->icon != nullptr) {
        cairo_surface_destroy(this->icon);
    }
    this->icon = cairo_image_surface_create_for_data(
        reinterpret_cast<unsigned char *>(icon_data),
        CAIRO_FORMAT_ARGB32,
        width,
        height,
        width * 4);
    static cairo_user_data_key_t free_data;
    cairo_surface_set_user_data(this->icon, &free_data, icon_data, [](void *icon_data) {
        delete[] reinterpret_cast<uint32_t*>(icon_data);
    });
}
