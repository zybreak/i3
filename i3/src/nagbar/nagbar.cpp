module;
#include <err.h>
#include <fcntl.h>

#include <sys/stat.h>
#include <sys/types.h>

#include "i3.h"
#include "atoms.h"
//#include "config_parser.h"

#include <libgen.h>
#include <sys/wait.h>
#include <fmt/core.h>
#include <fmt/printf.h>
#include <getopt.h>
#include <paths.h>

#include <xcb/randr.h>
#include <xcb/xcb.h>
#include <xcb/xcb_aux.h>
#include <xcb/xcb_cursor.h>
#include <xcb/xproto.h>

#if defined(__OpenBSD__)
#include <sys/cdefs.h>
#endif
module i3;

import std;
import utils;
import log;

static const button_t *get_button_at(const std::vector<button_t> &buttons, int16_t x, int16_t y) {
    for (const auto &button : buttons) {
        if (x >= (button.x) && x <= (button.x + button.width)) {
            return &button;
        }
    }

    return nullptr;
}


class Nagbar {
   private:
    xcb_connection_t *conn;
    surface_t bar;
    button_t btn_close;
    xcb_screen_t *root_screen;
    i3Font *font;
    std::string prompt;
    std::vector<button_t> buttons;
    bool running{true};
    xcb_window_t win;

    long MSG_PADDING;
    long BTN_PADDING;
    long BTN_BORDER;
    long BTN_GAP;
    long CLOSE_BTN_GAP;
    long BAR_BORDER;

    /* Result of get_colorpixel() for the various colors. */
    color_t color_background;        /* background of the bar */
    color_t color_button_background; /* background for buttons */
    color_t color_border;            /* color of the button border */
    color_t color_border_bottom;     /* color of the bottom border */
    color_t color_text;              /* color of the text */

    xcb_atom_t A_ATOM;
    xcb_atom_t A_CARDINAL;
    xcb_intern_atom_cookie_t ATOM_cookie;
    xcb_intern_atom_cookie_t CARDINAL_cookie;

    void window_kill() {
        xcb_destroy_window(conn, win);
        xcb_flush(conn);
    }


    void nagbar_handle_button_press(xcb_button_press_event_t *event) {
        LOG(fmt::sprintf("button pressed on x = %d, y = %d\n",
            event->event_x, event->event_y));
        /* TODO: set a flag for the button, re-render */
    }

    /*
     * Called when the user releases the mouse button. Checks whether the
     * coordinates are over a button and executes the appropriate action.
     *
     */
    void handle_button_release(xcb_button_release_event_t *event) {
        LOG(fmt::sprintf("button released on x = %d, y = %d\n",
            event->event_x, event->event_y));
        /* If the user hits the close button, we exit(0) */
        if (event->event_x >= btn_close.x && event->event_x < btn_close.x + btn_close.width) {
            //running = false;
            window_kill();
            return;
        }
        const button_t *button = get_button_at(buttons, event->event_x, event->event_y);
        if (!button) {
            return;
        }

        start_application(button->action, true);

        /* TODO: unset flag, re-render */
    }

    /**
     * Tries to position the rectangle on the output with input focus.
     * If unsuccessful, try to position on primary output.
     */
    void set_window_position_focus(xcb_rectangle_t *result) {
        bool success = false;
        xcb_get_input_focus_reply_t *input_focus = nullptr;
        xcb_get_geometry_reply_t *geometry = nullptr;
        xcb_translate_coordinates_reply_t *coordinates = nullptr;

        /* To avoid the input window disappearing while determining its position */
        xcb_grab_server(conn);

        input_focus = xcb_get_input_focus_reply(conn, xcb_get_input_focus(conn), nullptr);
        if (input_focus == nullptr || input_focus->focus == XCB_NONE) {
            LOG("Failed to receive the current input focus or no window has the input focus right now.\n");
            goto free_resources;
        }

        geometry = xcb_get_geometry_reply(conn, xcb_get_geometry(conn, input_focus->focus), nullptr);
        if (geometry == nullptr) {
            LOG("Failed to received window geometry.\n");
            goto free_resources;
        }

        coordinates = xcb_translate_coordinates_reply(
            conn, xcb_translate_coordinates(conn, input_focus->focus, global.x->root, geometry->x, geometry->y), nullptr);
        if (coordinates == nullptr) {
            LOG("Failed to translate coordinates.\n");
            goto free_resources;
        }

        LOG(fmt::sprintf("Found current focus at x = %i / y = %i.\n", coordinates->dst_x, coordinates->dst_y));
        result->x = coordinates->dst_x;
        result->y = coordinates->dst_y;
        success = true;

    free_resources:
        xcb_ungrab_server(conn);
        free(input_focus);
        free(coordinates);
        free(geometry);
        if (!success) {
            LOG(fmt::sprintf("Could not position on focused output, trying to position on primary output.\n"));
            set_window_position_primary(result);
        }
    }

    /*
     * Draws a button and returns its width
     *
     */
    int button_draw(button_t *button, int position) {
        int text_width = predict_text_width(font, conn, root_screen, button->label);
        button->width = text_width + 2 * BTN_PADDING + 2 * BTN_BORDER;
        button->x = position - button->width;

        /* draw border */
        draw_util_rectangle(&bar, color_border,
                            position - button->width,
                            MSG_PADDING - BTN_PADDING - BTN_BORDER,
                            button->width,
                            font->height + 2 * BTN_PADDING + 2 * BTN_BORDER);
        /* draw background */
        draw_util_rectangle(&bar, color_button_background,
                            position - button->width + BTN_BORDER,
                            MSG_PADDING - BTN_PADDING,
                            text_width + 2 * BTN_PADDING,
                            font->height + 2 * BTN_PADDING);
        /* draw label */
        draw_util_text(conn, font, button->label, &bar, color_text, color_button_background,
                       position - button->width + BTN_BORDER + BTN_PADDING,
                       MSG_PADDING,
                       200);
        return button->width;
    }

    /**
     * Tries to position the rectangle on the primary output.
     */
    void set_window_position_primary(xcb_rectangle_t *result) {
        xcb_randr_get_screen_resources_current_cookie_t rcookie = xcb_randr_get_screen_resources_current(conn, global.x->root);
        xcb_randr_get_output_primary_cookie_t pcookie = xcb_randr_get_output_primary(conn, global.x->root);

        xcb_randr_get_output_primary_reply_t *primary = nullptr;
        xcb_randr_get_screen_resources_current_reply_t *res = nullptr;

        if ((primary = xcb_randr_get_output_primary_reply(conn, pcookie, nullptr)) == nullptr) {
            LOG("Could not determine the primary output.\n");
            free(res);
            free(primary);
            return;
        }

        if ((res = xcb_randr_get_screen_resources_current_reply(conn, rcookie, nullptr)) == nullptr) {
            LOG("Could not query screen resources.\n");
            free(res);
            free(primary);
            return;
        }

        xcb_randr_get_output_info_reply_t *output =
            xcb_randr_get_output_info_reply(conn,
                                            xcb_randr_get_output_info(conn, primary->output, res->config_timestamp),
                                            nullptr);
        if (output == nullptr || output->crtc == XCB_NONE) {
            LOG("Could not query primary screen.\n");
            free(res);
            free(primary);
            return;
        }

        xcb_randr_get_crtc_info_reply_t *crtc =
            xcb_randr_get_crtc_info_reply(conn,
                                          xcb_randr_get_crtc_info(conn, output->crtc, res->config_timestamp),
                                          nullptr);
        if (crtc == nullptr) {
            LOG("Could not get CRTC.\n");
            free(res);
            free(primary);
            return;
        }

        LOG(fmt::sprintf("Found primary output on position x = %i / y = %i / w = %i / h = %i.\n",
            crtc->x, crtc->y, crtc->width, crtc->height));
        if (crtc->width == 0 || crtc->height == 0) {
            LOG("Primary output is not active, ignoring it.\n");
            free(res);
            free(primary);
            return;
        }

        result->x = crtc->x;
        result->y = crtc->y;
        free(res);
        free(primary);
    }


    /*
     * Handles expose events (redraws of the window) and rendering in general. Will
     * be called from the code with event == NULL or from X with event != NULL.
     *
     */
    int handle_expose(xcb_expose_event_t *event) {
        /* draw background */
        draw_util_clear_surface(&bar, color_background);
        /* draw message */
        draw_util_text(conn, font, prompt, &bar, color_text, color_background,
                       MSG_PADDING, MSG_PADDING,
                       bar.width - 2 * MSG_PADDING);

        int position = bar.width - (MSG_PADDING - BTN_BORDER - BTN_PADDING);

        /* render close button */
        position -= button_draw(&btn_close, position);
        position -= CLOSE_BTN_GAP;

        /* render custom buttons */
        for (int i = 0; i < buttons.size(); i++) {
            position -= BTN_GAP;
            position -= button_draw(&buttons[i], position);
        }

        /* border line at the bottom */
        draw_util_rectangle(&bar, color_border_bottom, 0, bar.height - BAR_BORDER, bar.width, BAR_BORDER);

        xcb_flush(conn);
        return 1;
    }


   public:

    Nagbar() = delete;

    Nagbar(bar_type_t bar_type, std::string prompt, std::string pattern, std::vector<button_t> &buttons) : prompt(prompt), buttons(buttons) {
        btn_close.label = "X";

        int screens;
        if ((conn = xcb_connect(nullptr, &screens)) == nullptr ||
            xcb_connection_has_error(conn)) {
            errx(EXIT_FAILURE, "Cannot open display");
        }

        /* Init startup notification. */
        //SnDisplay *sndisplay = sn_xcb_display_new(conn, nullptr, nullptr);
        //SnLauncheeContext *sncontext = sn_launchee_context_new_from_environment(sndisplay, screens);
        //sn_display_unref(sndisplay);

        root_screen = xcb_aux_get_screen(conn, screens);
        //global.x->root = root_screen->root;

        if (bar_type == bar_type_t::TYPE_ERROR) {
            /* Red theme for error messages */
            color_button_background = draw_util_hex_to_color(conn, root_screen, "#680a0a");
            color_background = draw_util_hex_to_color(conn, root_screen, "#900000");
            color_text = draw_util_hex_to_color(conn, root_screen, "#ffffff");
            color_border = draw_util_hex_to_color(conn, root_screen, "#d92424");
            color_border_bottom = draw_util_hex_to_color(conn, root_screen, "#470909");
        } else {
            /* Yellowish theme for warnings */
            color_button_background = draw_util_hex_to_color(conn, root_screen, "#ffc100");
            color_background = draw_util_hex_to_color(conn, root_screen, "#ffa8000");
            color_text = draw_util_hex_to_color(conn, root_screen, "#000000");
            color_border = draw_util_hex_to_color(conn, root_screen, "#ab7100");
            color_border_bottom = draw_util_hex_to_color(conn, root_screen, "#ab7100");
        }

        init_dpi(conn, root_screen);
        font = load_font(conn, root_screen, pattern.c_str(), true);

        /* Place requests for the atoms we need as soon as possible */
        ATOM_cookie = xcb_intern_atom(conn, 0, strlen("ATOM"), "ATOM");
        CARDINAL_cookie = xcb_intern_atom(conn, 0, strlen("CARDINAL"), "CARDINAL");

        MSG_PADDING = logical_px(root_screen, 8);
        BTN_PADDING = logical_px(root_screen, 3);
        BTN_BORDER = logical_px(root_screen, 3);
        BTN_GAP = logical_px(root_screen, 20);
        CLOSE_BTN_GAP = logical_px(root_screen, 15);
        BAR_BORDER = logical_px(root_screen, 2);
    }

    ~Nagbar() {
        draw_util_surface_free(conn, &bar);
        xcb_disconnect(conn);
    }

    void create_window(bool position_on_primary) {
        /* Default values if we cannot determine the preferred window position. */
        xcb_rectangle_t win_pos = (xcb_rectangle_t){50, 50, 500, static_cast<uint16_t>(font->height + 2 * MSG_PADDING + BAR_BORDER)};
        if (position_on_primary) {
            set_window_position_primary(&win_pos);
        } else {
            set_window_position_focus(&win_pos);
        }

        xcb_cursor_context_t *cursor_ctx;
        if (xcb_cursor_context_new(conn, root_screen, &cursor_ctx) < 0) {
            errx(EXIT_FAILURE, "Cannot allocate xcursor context");
        }
        xcb_cursor_t cursor = xcb_cursor_load_cursor(cursor_ctx, "left_ptr");
        xcb_cursor_context_free(cursor_ctx);

        /* Open an input window */
        win = xcb_generate_id(conn);

        uint32_t mask, mask_list[3];

        mask_list[0] = 0;
        mask_list[1] = XCB_EVENT_MASK_EXPOSURE |
                       XCB_EVENT_MASK_STRUCTURE_NOTIFY |
                       XCB_EVENT_MASK_BUTTON_PRESS |
                       XCB_EVENT_MASK_BUTTON_RELEASE;
        mask_list[2] = cursor;

        mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK | XCB_CW_CURSOR;

        xcb_create_window(
            conn,
            XCB_COPY_FROM_PARENT,
            win,                                                 /* the window id */
            global.x->root,                                                /* parent == root */
            win_pos.x, win_pos.y, win_pos.width, win_pos.height, /* dimensions */
            0,                                                   /* x11 border = 0, we draw our own */
            XCB_WINDOW_CLASS_INPUT_OUTPUT,
            XCB_WINDOW_CLASS_COPY_FROM_PARENT, /* copy visual from parent */
            mask,
            mask_list);
        //if (sncontext) {
        //    sn_launchee_context_setup_window(sncontext, win);
        //}

        /* Map the window (make it visible) */
        xcb_map_window(conn, win);

        /* Setup NetWM atoms */
        xcb_intern_atom_reply_t *reply;
        reply = xcb_intern_atom_reply(conn, ATOM_cookie, nullptr);
        if (!reply) {
            errx(EXIT_FAILURE, "Could not get atom ATOM");
        }

        A_ATOM = reply->atom;
        free(reply);

        reply = xcb_intern_atom_reply(conn, CARDINAL_cookie, nullptr);
        if (!reply) {
            errx(EXIT_FAILURE, "Could not get atom CARDINAL");
        }

        A_CARDINAL = reply->atom;
        free(reply);

        /* Set dock mode */
        xcb_change_property(conn,
                            XCB_PROP_MODE_REPLACE,
                            win,
                            A__NET_WM_WINDOW_TYPE,
                            A_ATOM,
                            32,
                            1,
                            (unsigned char *)&A__NET_WM_WINDOW_TYPE_DOCK);

        /* Reserve some space at the top of the screen */
        struct {
            uint32_t left;
            uint32_t right;
            uint32_t top;
            uint32_t bottom;
            uint32_t left_start_y;
            uint32_t left_end_y;
            uint32_t right_start_y;
            uint32_t right_end_y;
            uint32_t top_start_x;
            uint32_t top_end_x;
            uint32_t bottom_start_x;
            uint32_t bottom_end_x;
        } __attribute__((__packed__)) strut_partial{};
        memset(&strut_partial, 0, sizeof(strut_partial));

        strut_partial.top = font->height + logical_px(root_screen, 6);
        strut_partial.top_start_x = 0;
        strut_partial.top_end_x = 800;

        xcb_change_property(conn,
                            XCB_PROP_MODE_REPLACE,
                            win,
                            A__NET_WM_STRUT_PARTIAL,
                            A_CARDINAL,
                            32,
                            12,
                            &strut_partial);

        /* Initialize the drawable bar */
        draw_util_surface_init(conn, &bar, win, get_visualtype(root_screen), win_pos.width, win_pos.height);

        /* Startup complete. */
        //if (sncontext) {
        //    sn_launchee_context_complete(sncontext);
        //    sn_launchee_context_unref(sncontext);
        //}
    }

    void start_event_loop() {
        /* Grab the keyboard to get all input */
        xcb_flush(conn);

        xcb_generic_event_t *event;
        while (running && (event = xcb_wait_for_event(conn)) != nullptr) {
            if (event->response_type == 0) {
                ELOG(fmt::sprintf("X11 Error received! sequence %x\n", event->sequence));
                continue;
            }

            /* Strip off the highest bit (set if the event is generated) */
            int type = (event->response_type & 0x7F);

            switch (type) {
                case XCB_EXPOSE:
                    if (((xcb_expose_event_t *)event)->count == 0) {
                        handle_expose((xcb_expose_event_t *)event);
                    }

                    break;

                case XCB_BUTTON_PRESS:
                    nagbar_handle_button_press((xcb_button_press_event_t *)event);
                    break;

                case XCB_BUTTON_RELEASE:
                    handle_button_release((xcb_button_release_event_t *)event);
                    break;

                case XCB_CONFIGURE_NOTIFY: {
                    auto *configure_notify = (xcb_configure_notify_event_t *)event;
                    if (configure_notify->width > 0 && configure_notify->height > 0) {
                        draw_util_surface_set_size(&bar, configure_notify->width, configure_notify->height);
                    }
                    break;
                }
                case XCB_DESTROY_NOTIFY:
                    running = false;
                    LOG(fmt::sprintf("DESTROY\n"));
                    break;
                case XCB_UNMAP_NOTIFY:
                    LOG(fmt::sprintf("UNMAP\n"));
                    break;
                default:
                    LOG(fmt::sprintf("Unhandled event of type %d\n", type));
                    break;
            }

            free(event);
        }
    }
};

static void draw_nagbar(std::string prompt,
                        std::vector<button_t> buttons,
                        bar_type_t bar_type,
                        bool position_on_primary,
                        std::string pattern) {

    Nagbar nagbar{bar_type, prompt, pattern, buttons};

    nagbar.create_window(position_on_primary);

    nagbar.start_event_loop();
}

/*
 * Starts an i3-nagbar instance with the given parameters. Takes care of
 * handling SIGCHLD and killing i3-nagbar when i3 exits.
 *
 * The resulting PID will be stored in *nagbar_pid and can be used with
 * kill_nagbar() to kill the bar later on.
 *
 */
void start_nagbar(pid_t *nagbar_pid,
                  std::vector<button_t> &buttons,
                  std::string &prompt,
                  std::string &pattern,
                  bar_type_t bar_type,
                  bool position_on_primary) {
    auto nagbar_thread = std::thread(draw_nagbar, prompt, buttons, bar_type, position_on_primary, pattern);
    nagbar_thread.detach();
}

/*
 * Kills the i3-nagbar process, if nagbar_pid != -1.
 *
 * If wait_for_it is set (restarting i3), this function will waitpid(),
 * otherwise, ev is assumed to handle it (reloading).
 *
 */
void kill_nagbar(pid_t nagbar_pid, bool wait_for_it) {
    if (nagbar_pid == -1) {
        return;
    }

    if (kill(nagbar_pid, SIGTERM) == -1) {
        warn("kill(configerror_nagbar) failed");
    }

    if (!wait_for_it) {
        return;
    }

    /* When restarting, we don’t enter the ev main loop anymore and after the
     * exec(), our old pid is no longer watched. So, ev won’t handle SIGCHLD
     * for us and we would end up with a <defunct> process. Therefore we
     * waitpid() here. */
    waitpid(nagbar_pid, nullptr, 0);
}
