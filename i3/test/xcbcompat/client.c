/* A test client on the system's libxcb, driving i3 through XTEST.
 *
 * client manage   : maps a window, waits until i3 reparents it, prints "managed <frame>", then clicks into it and
 *                   waits for the ButtonPress that i3 replays (grab_button + allow_events), printing "click ok".
 * client key <kc> : presses and releases keycode <kc> on the root window (exercises i3's key grabs).
 * client orphans  : prints how many top-level windows named "phase2-client" are children of the root
 *                   (after i3 exits, its save set puts them back there). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <xcb/xcb.h>
#include <xcb/xtest.h>

static xcb_window_t parent_of(xcb_connection_t *c, xcb_window_t w) {
    xcb_query_tree_reply_t *r = xcb_query_tree_reply(c, xcb_query_tree(c, w), NULL);
    if (!r) return XCB_NONE;
    xcb_window_t p = r->parent;
    free(r);
    return p;
}

static void fake(xcb_connection_t *c, uint8_t type, uint8_t detail, xcb_window_t root, int16_t x, int16_t y) {
    xcb_test_fake_input(c, type, detail, XCB_CURRENT_TIME, root, x, y, 0);
    xcb_flush(c);
    usleep(50000);
}

int main(int argc, char **argv) {
    int screen_num;
    xcb_connection_t *c = xcb_connect(NULL, &screen_num);
    if (xcb_connection_has_error(c)) { fprintf(stderr, "cannot connect\n"); return 2; }
    xcb_screen_iterator_t it = xcb_setup_roots_iterator(xcb_get_setup(c));
    for (int i = 0; i < screen_num; i++) xcb_screen_next(&it);
    xcb_screen_t *s = it.data;

    if (argc >= 3 && strcmp(argv[1], "key") == 0) {
        uint8_t kc = (uint8_t)atoi(argv[2]);
        fake(c, XCB_KEY_PRESS, kc, s->root, 0, 0);
        fake(c, XCB_KEY_RELEASE, kc, s->root, 0, 0);
        xcb_disconnect(c);
        return 0;
    }

    if (argc >= 2 && strcmp(argv[1], "orphans") == 0) {
        xcb_query_tree_reply_t *r = xcb_query_tree_reply(c, xcb_query_tree(c, s->root), NULL);
        int n = 0;
        xcb_window_t *kids = xcb_query_tree_children(r);
        for (int i = 0; i < xcb_query_tree_children_length(r); i++) {
            xcb_get_property_reply_t *p = xcb_get_property_reply(
                c, xcb_get_property(c, 0, kids[i], XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 0, 64), NULL);
            if (p && xcb_get_property_value_length(p) == 13 && memcmp(xcb_get_property_value(p), "phase2-client", 13) == 0) n++;
            free(p);
        }
        free(r);
        printf("orphans %d\n", n);
        xcb_disconnect(c);
        return 0;
    }

    /* manage */
    xcb_window_t w = xcb_generate_id(c);
    uint32_t values[] = {s->white_pixel, XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_STRUCTURE_NOTIFY};
    xcb_create_window(c, XCB_COPY_FROM_PARENT, w, s->root, 0, 0, 300, 200, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT,
                      s->root_visual, XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK, values);
    xcb_change_property(c, XCB_PROP_MODE_REPLACE, w, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, 13, "phase2-client");
    xcb_map_window(c, w);
    xcb_flush(c);

    xcb_window_t frame = XCB_NONE;
    for (int i = 0; i < 100 && frame == XCB_NONE; i++) {
        usleep(50000);
        xcb_window_t p = parent_of(c, w);
        if (p != XCB_NONE && p != s->root) frame = p;
    }
    if (frame == XCB_NONE) { printf("not managed\n"); return 1; }
    printf("managed 0x%x\n", frame);
    fflush(stdout);

    /* Click into the window: i3 grabs the buttons synchronously, then replays the click to the client. */
    xcb_translate_coordinates_reply_t *t =
        xcb_translate_coordinates_reply(c, xcb_translate_coordinates(c, w, s->root, 10, 10), NULL);
    fake(c, XCB_MOTION_NOTIFY, 0, s->root, t->dst_x, t->dst_y);
    free(t);
    fake(c, XCB_BUTTON_PRESS, 1, s->root, 0, 0);
    fake(c, XCB_BUTTON_RELEASE, 1, s->root, 0, 0);

    int clicked = 0;
    for (int i = 0; i < 40 && !clicked; i++) {
        xcb_generic_event_t *e;
        while ((e = xcb_poll_for_event(c)) != NULL) {
            if ((e->response_type & 0x7f) == XCB_BUTTON_PRESS) clicked = 1;
            free(e);
        }
        usleep(50000);
    }
    printf(clicked ? "click ok\n" : "click lost\n");
    fflush(stdout);

    /* Stay alive (and keep the window) until killed, so that the save set can be checked. */
    pause();
    return 0;
}
