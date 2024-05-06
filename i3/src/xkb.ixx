module;
#define explicit dont_use_cxx_explicit
#include <xcb/xkb.h>
#undef explicit
#include "xpp/proto/bigreq.hpp"
#include "xpp/proto/xkb.hpp"
#include "xpp/xpp.hpp"
export module i3:xkb;

import :x;
import log;

export class Xkb {
   public:
    bool xkb_supported{true};
    int xkb_base{-1};

    Xkb() = delete;
    explicit Xkb(X &x) {
        x_connection *conn = &*x;
        auto xkb_ext = conn->extension<xpp::xkb::extension>();
        xkb_supported = xkb_ext->present;
        if (!xkb_supported) {
            DLOG("xkb is not present on this server");
        } else {
            DLOG("initializing xcb-xkb");
            conn->xkb().use_extension(XCB_XKB_MAJOR_VERSION, XCB_XKB_MINOR_VERSION);
            conn->xkb().select_events(
                XCB_XKB_ID_USE_CORE_KBD,
                XCB_XKB_EVENT_TYPE_STATE_NOTIFY | XCB_XKB_EVENT_TYPE_MAP_NOTIFY | XCB_XKB_EVENT_TYPE_NEW_KEYBOARD_NOTIFY,
                0,
                XCB_XKB_EVENT_TYPE_STATE_NOTIFY | XCB_XKB_EVENT_TYPE_MAP_NOTIFY | XCB_XKB_EVENT_TYPE_NEW_KEYBOARD_NOTIFY,
                0xff,
                0xff,
                nullptr);

            /* Setting both, XCB_XKB_PER_CLIENT_FLAG_GRABS_USE_XKB_STATE and
             * XCB_XKB_PER_CLIENT_FLAG_LOOKUP_STATE_WHEN_GRABBED, will lead to the
             * X server sending us the full XKB state in KeyPress and KeyRelease:
             * https://cgit.freedesktop.org/xorg/xserver/tree/xkb/xkbEvents.c?h=xorg-server-1.20.0#n927
             *
             * XCB_XKB_PER_CLIENT_FLAG_DETECTABLE_AUTO_REPEAT enable detectable autorepeat:
             * https://www.x.org/releases/current/doc/kbproto/xkbproto.html#Detectable_Autorepeat
             * This affects bindings using the --release flag: instead of getting multiple KeyRelease
             * events we get only one event when the key is physically released by the user.
             */
            const uint32_t mask = XCB_XKB_PER_CLIENT_FLAG_GRABS_USE_XKB_STATE |
                                  XCB_XKB_PER_CLIENT_FLAG_LOOKUP_STATE_WHEN_GRABBED |
                                  XCB_XKB_PER_CLIENT_FLAG_DETECTABLE_AUTO_REPEAT;
            /* The last three parameters are unset because they are only relevant
             * when using a feature called “automatic reset of boolean controls”:
             * https://www.x.org/releases/X11R7.7/doc/kbproto/xkbproto.html#Automatic_Reset_of_Boolean_Controls
             * */
            auto pcf_reply = conn->xkb().per_client_flags(XCB_XKB_ID_USE_CORE_KBD, mask, mask, 0, 0, 0);

            if (pcf_reply.get() == nullptr || !(pcf_reply->value & (XCB_XKB_PER_CLIENT_FLAG_GRABS_USE_XKB_STATE))) {
                 ELOG("Could not set XCB_XKB_PER_CLIENT_FLAG_GRABS_USE_XKB_STATE");
            }
            if (pcf_reply.get() == nullptr || !(pcf_reply->value & (XCB_XKB_PER_CLIENT_FLAG_LOOKUP_STATE_WHEN_GRABBED))) {
                ELOG("Could not set XCB_XKB_PER_CLIENT_FLAG_LOOKUP_STATE_WHEN_GRABBED");
            }
            if (pcf_reply.get() == nullptr || !(pcf_reply->value & (XCB_XKB_PER_CLIENT_FLAG_DETECTABLE_AUTO_REPEAT))) {
                ELOG("Could not set XCB_XKB_PER_CLIENT_FLAG_DETECTABLE_AUTO_REPEAT");
            }

            xkb_base = xkb_ext->first_event;
        }
    }
};
