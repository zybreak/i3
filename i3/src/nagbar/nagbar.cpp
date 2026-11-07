module;
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>

#include <sys/eventfd.h>
#include <sys/stat.h>
#include <sys/types.h>

// #include "config_parser.h"

#include <libgen.h>
#include <fmt/core.h>
#include <fmt/printf.h>
#include <getopt.h>

#include <xcb/xcb.h>
#include <xcb/xcb_cursor.h>

#if defined(__OpenBSD__)
#include <sys/cdefs.h>
#endif
module i3;

import std;
import utils;
import log;
import xcb.proto.randr;

static button_t const *get_button_at(std::vector<button_t> const &buttons, int16_t x, int16_t y) {
    for (auto const &button : buttons) {
        if (x >= (button.x) && x <= (button.x + button.width)) {
            return &button;
        }
    }

    return nullptr;
}

/*
 * A nagbar runs on a thread of its own (start_nagbar()), with a connection of
 * its own. It uses only that connection, its screen and what it was given by
 * value, and leaves i3's state (global) alone: the one exception is
 * handle_button_release(), see there.
 *
 * The members are destroyed in reverse order: the surface and the font before
 * the drawing state, and all of them before the connection is closed.
 */
class Nagbar {
  private:
    /* The connection, which the nagbar owns: it is closed last. The nagbar's requests go through xconn; the drawing
     * code and the C libraries (cairo, pango, xcb-cursor) take its libxcb handle, conn, and its screen as libxcb's
     * xcb_screen_t. */
    xcb::xcbcompat::unique_handle handle{};
    xcb_connection_t *conn;
    xcb::Connection *xconn;
    xcb_screen_t *root_screen;
    std::optional<DrawContext> drawing{};
    std::unique_ptr<surface_t> bar{};
    button_t btn_close;
    std::unique_ptr<i3Font> font{};
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
    std::future<std::expected<xcb::xproto::InternAtomReplyObj, xcb::generic_error_t>> ATOM_cookie;
    std::future<std::expected<xcb::xproto::InternAtomReplyObj, xcb::generic_error_t>> CARDINAL_cookie;

    void window_kill() {
        xcb::xproto::destroy_window(xconn, win);
        xconn->flush();
    }

    void nagbar_handle_button_press(xcb::xproto::button_press_event_t *event) {
        LOG(fmt::sprintf("button pressed on x = %d, y = %d\n",
                         event->event_x, event->event_y));
        /* TODO: set a flag for the button, re-render */
    }

    /*
     * Called when the user releases the mouse button. Checks whether the
     * coordinates are over a button and executes the appropriate action.
     *
     */
    void handle_button_release(xcb::xproto::button_release_event_t *event) {
        LOG(fmt::sprintf("button released on x = %d, y = %d\n",
                         event->event_x, event->event_y));
        /* If the user hits the close button, we exit(0) */
        if (event->event_x >= btn_close.x && event->event_x < btn_close.x + btn_close.width) {
            // running = false;
            window_kill();
            return;
        }
        button_t const *button = get_button_at(buttons, event->event_x, event->event_y);
        if (!button) {
            return;
        }

        /* With no_startup_id, start_application() only forks and executes the
         * action, and touches i3's state only in the child, so this thread may
         * call it. The launcher exists before any nagbar with buttons does. */
        global.applicationLauncher->start_application(button->action, true);

        /* TODO: unset flag, re-render */
    }

    /**
     * Tries to position the rectangle on the output with input focus.
     * If unsuccessful, try to position on primary output.
     */
    void set_window_position_focus(xcb::xproto::rectangle_t *result) {
        bool success = false;

        /* To avoid the input window disappearing while determining its position */
        xcb::xproto::grab_server(xconn);

        if (auto const input_focus = xcb::xproto::get_input_focus(xconn).get(); !input_focus || input_focus->focus == xcb::none) {
            LOG("Failed to receive the current input focus or no window has the input focus right now.\n");
        } else if (auto const geometry = xcb::xproto::get_geometry(xconn, input_focus->focus).get(); !geometry) {
            LOG("Failed to received window geometry.\n");
        } else if (auto const coordinates = xcb::xproto::translate_coordinates(xconn, input_focus->focus, root_screen->root, geometry->x, geometry->y).get();
                   !coordinates) {
            LOG("Failed to translate coordinates.\n");
        } else {
            LOG(fmt::sprintf("Found current focus at x = %i / y = %i.\n", coordinates->dst_x, coordinates->dst_y));
            result->x = coordinates->dst_x;
            result->y = coordinates->dst_y;
            success = true;
        }

        xcb::xproto::ungrab_server(xconn);
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
        int text_width = font->predict_text_width(button->label);
        button->width = text_width + 2 * BTN_PADDING + 2 * BTN_BORDER;
        button->x = position - button->width;

        /* draw border */
        bar->draw_util_rectangle(color_border,
                                 position - button->width,
                                 MSG_PADDING - BTN_PADDING - BTN_BORDER,
                                 button->width,
                                 font->height + 2 * BTN_PADDING + 2 * BTN_BORDER);
        /* draw background */
        bar->draw_util_rectangle(color_button_background,
                                 position - button->width + BTN_BORDER,
                                 MSG_PADDING - BTN_PADDING,
                                 text_width + 2 * BTN_PADDING,
                                 font->height + 2 * BTN_PADDING);
        /* draw label */
        bar->draw_util_text(*font, button->label, color_text, color_button_background,
                            position - button->width + BTN_BORDER + BTN_PADDING,
                            MSG_PADDING,
                            200);
        return button->width;
    }

    /**
     * Tries to position the rectangle on the primary output.
     */
    void set_window_position_primary(xcb::xproto::rectangle_t *result) {
        auto resources_cookie = xcb::randr::get_screen_resources_current(xconn, root_screen->root);
        auto primary_cookie = xcb::randr::get_output_primary(xconn, root_screen->root);

        auto const primary = primary_cookie.get();
        if (!primary) {
            LOG("Could not determine the primary output.\n");
            return;
        }

        auto const res = resources_cookie.get();
        if (!res) {
            LOG("Could not query screen resources.\n");
            return;
        }

        auto const output = xcb::randr::get_output_info(xconn, primary->output, res->config_timestamp).get();
        if (!output || output->crtc == xcb::none) {
            LOG("Could not query primary screen.\n");
            return;
        }

        auto const crtc = xcb::randr::get_crtc_info(xconn, output->crtc, res->config_timestamp).get();
        if (!crtc) {
            LOG("Could not get CRTC.\n");
            return;
        }

        LOG(fmt::sprintf("Found primary output on position x = %i / y = %i / w = %i / h = %i.\n",
                         crtc->x, crtc->y, crtc->width, crtc->height));
        if (crtc->width == 0 || crtc->height == 0) {
            LOG("Primary output is not active, ignoring it.\n");
            return;
        }

        result->x = crtc->x;
        result->y = crtc->y;
    }

    /*
     * Handles expose events (redraws of the window) and rendering in general. Will
     * be called from the code with event == NULL or from X with event != NULL.
     *
     */
    int handle_expose(xcb::xproto::expose_event_t *event) {
        /* draw background */
        bar->draw_util_clear_surface(color_background);
        /* draw message */
        bar->draw_util_text(*font, prompt, color_text, color_background,
                            MSG_PADDING, MSG_PADDING,
                            bar->width - 2 * MSG_PADDING);

        int position = bar->width - (MSG_PADDING - BTN_BORDER - BTN_PADDING);

        /* render close button */
        position -= button_draw(&btn_close, position);
        position -= CLOSE_BTN_GAP;

        /* render custom buttons */
        for (int i = 0; i < buttons.size(); i++) {
            position -= BTN_GAP;
            position -= button_draw(&buttons[i], position);
        }

        /* border line at the bottom */
        bar->draw_util_rectangle(color_border_bottom, 0, bar->height - BAR_BORDER, bar->width, BAR_BORDER);

        xconn->flush();
        return 1;
    }

  public:
    Nagbar() = delete;

    Nagbar(bar_type_t bar_type, std::string prompt, std::string pattern, std::vector<button_t> &buttons)
        : prompt(prompt), buttons(buttons) {
        btn_close.label = "X";

        int screens;
        auto connection = xcb::connect(nullptr, &screens);
        if (!connection) {
            throw std::runtime_error("Cannot open display");
        }
        handle = xcb::xcbcompat::adopt(std::move(*connection));
        conn = handle.get();
        xconn = xcb::xcbcompat::connection(conn);

        /* Init startup notification. */
        // SnDisplay *sndisplay = sn_xcb_display_new(conn, nullptr, nullptr);
        // SnLauncheeContext *sncontext = sn_launchee_context_new_from_environment(sndisplay, screens);
        // sn_display_unref(sndisplay);

        root_screen = get_screen(conn, screens);
        drawing.emplace(conn, root_screen);

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

        /* No init_dpi(): i3 set the DPI, which logical_px() reads, before it
         * started any nagbar. */
        font = load_font(conn, root_screen, pattern);

        /* Place requests for the atoms we need as soon as possible */
        ATOM_cookie = xcb::xproto::intern_atom(xconn, false, "ATOM");
        CARDINAL_cookie = xcb::xproto::intern_atom(xconn, false, "CARDINAL");

        MSG_PADDING = logical_px(root_screen, 8);
        BTN_PADDING = logical_px(root_screen, 3);
        BTN_BORDER = logical_px(root_screen, 3);
        BTN_GAP = logical_px(root_screen, 20);
        CLOSE_BTN_GAP = logical_px(root_screen, 15);
        BAR_BORDER = logical_px(root_screen, 2);
    }

    void create_window(bool position_on_primary) {
        /* Default values if we cannot determine the preferred window position. */
        xcb::xproto::rectangle_t win_pos{50, 50, 500, static_cast<uint16_t>(font->height + 2 * MSG_PADDING + BAR_BORDER)};
        if (position_on_primary) {
            set_window_position_primary(&win_pos);
        } else {
            set_window_position_focus(&win_pos);
        }

        xcb_cursor_context_t *cursor_ctx;
        if (xcb_cursor_context_new(conn, root_screen, &cursor_ctx) < 0) {
            throw std::runtime_error("Cannot allocate xcursor context");
        }
        xcb_cursor_t cursor = xcb_cursor_load_cursor(cursor_ctx, "left_ptr");
        xcb_cursor_context_free(cursor_ctx);

        /* Open an input window */
        win = xconn->generate_id();

        xcb::xproto::create_window(
            xconn,
            xcb::copy_from_parent,
            win,                                                 /* the window id */
            root_screen->root,                                   /* parent == root */
            win_pos.x, win_pos.y, win_pos.width, win_pos.height, /* dimensions */
            0,                                                   /* x11 border = 0, we draw our own */
            xcb::xproto::window_class_t::input_output,
            xcb::copy_from_parent,                               /* copy visual from parent */
            xcb::xproto::CreateWindowValueList{}
                .with_background_pixel(0)
                .with_event_mask(xcb::xproto::event_mask_t::exposure |
                                 xcb::xproto::event_mask_t::structure_notify |
                                 xcb::xproto::event_mask_t::button_press |
                                 xcb::xproto::event_mask_t::button_release)
                .with_cursor(cursor));
        // if (sncontext) {
        //     sn_launchee_context_setup_window(sncontext, win);
        // }

        /* Map the window (make it visible) */
        xcb::xproto::map_window(xconn, win);

        /* Setup NetWM atoms */
        auto const atom_reply = ATOM_cookie.get();
        if (!atom_reply) {
            throw std::runtime_error("Could not get atom ATOM");
        }
        A_ATOM = atom_reply->atom;

        auto const cardinal_reply = CARDINAL_cookie.get();
        if (!cardinal_reply) {
            throw std::runtime_error("Could not get atom CARDINAL");
        }
        A_CARDINAL = cardinal_reply->atom;

        /* Set dock mode */
        xcb_atom_t const dock = i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE_DOCK];
        x_change_property(*xconn, xcb::xproto::prop_mode_t::replace, win, i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE],
                          A_ATOM, std::span{&dock, 1});

        /* Reserve some space at the top of the screen: _NET_WM_STRUT_PARTIAL's left, right, top, bottom,
         * left_start_y, left_end_y, right_start_y, right_end_y, top_start_x, top_end_x, bottom_start_x and
         * bottom_end_x. */
        std::array<uint32_t, 12> strut_partial{};
        strut_partial[2] = static_cast<uint32_t>(font->height + logical_px(root_screen, 6)); /* top */
        strut_partial[8] = 0;                                                                 /* top_start_x */
        strut_partial[9] = 800;                                                               /* top_end_x */

        x_change_property(*xconn, xcb::xproto::prop_mode_t::replace, win, i3::atoms[i3::Atom::_NET_WM_STRUT_PARTIAL],
                          A_CARDINAL, std::span{strut_partial});

        /* Initialize the drawable bar */
        bar = std::make_unique<surface_t>(*drawing, win, get_visualtype(root_screen), win_pos.width, win_pos.height);

        /* Startup complete. */
        // if (sncontext) {
        //     sn_launchee_context_complete(sncontext);
        //     sn_launchee_context_unref(sncontext);
        // }
    }

    /*
     * Handles the nagbar's events until its window is destroyed, its
     * connection fails, or wake_fd becomes readable (i3 stops the nagbar).
     *
     */
    void start_event_loop(int wake_fd) {
        /* Grab the keyboard to get all input */
        xconn->flush();

        std::array fds{pollfd{.fd = xconn->get_file_descriptor(), .events = POLLIN, .revents = 0},
                       pollfd{.fd = wake_fd, .events = POLLIN, .revents = 0}};
        while (running) {
            auto const event = xconn->poll_for_event();
            if (!event) {
                if (xconn->connection_has_error()) {
                    break;
                }
                xconn->flush();
                if (poll(fds.data(), fds.size(), -1) == -1 && errno != EINTR) {
                    ELOG(fmt::sprintf("poll() failed: %s\n", std::strerror(errno)));
                    break;
                }
                if (fds[1].revents != 0) {
                    break;
                }
                continue;
            }

            if (event->response_type == 0) {
                ELOG(fmt::sprintf("X11 Error received! sequence %x\n", event->sequence));
                continue;
            }

            /* Strip off the highest bit (set if the event is generated) */
            int type = (event->response_type & 0x7F);

            switch (type) {
                case xcb::xproto::EXPOSE:
                    if (auto *expose = reinterpret_cast<xcb::xproto::expose_event_t *>(event.get()); expose->count == 0) {
                        handle_expose(expose);
                    }

                    break;

                case xcb::xproto::BUTTON_PRESS:
                    nagbar_handle_button_press(reinterpret_cast<xcb::xproto::button_press_event_t *>(event.get()));
                    break;

                case xcb::xproto::BUTTON_RELEASE:
                    handle_button_release(reinterpret_cast<xcb::xproto::button_release_event_t *>(event.get()));
                    break;

                case xcb::xproto::CONFIGURE_NOTIFY: {
                    auto *configure_notify = reinterpret_cast<xcb::xproto::configure_notify_event_t *>(event.get());
                    if (configure_notify->width > 0 && configure_notify->height > 0) {
                        bar->draw_util_surface_set_size(configure_notify->width, configure_notify->height);
                    }
                    break;
                }
                case xcb::xproto::DESTROY_NOTIFY:
                    running = false;
                    LOG(fmt::sprintf("DESTROY\n"));
                    break;
                case xcb::xproto::UNMAP_NOTIFY:
                    LOG(fmt::sprintf("UNMAP\n"));
                    break;
                default:
                    LOG(fmt::sprintf("Unhandled event of type %d\n", type));
                    break;
            }
        }
    }
};

/*
 * The nagbar's thread. NagbarThread's destructor stops it, which makes
 * wake_fd readable: the event loop ends, and the nagbar closes its window and
 * its connection.
 *
 */
static void draw_nagbar(std::stop_token stop,
                        int wake_fd,
                        std::atomic<bool> *done,
                        std::string prompt,
                        std::vector<button_t> buttons,
                        bar_type_t bar_type,
                        bool position_on_primary,
                        std::string pattern) {
    std::stop_callback wake{stop, [wake_fd] {
                                std::uint64_t const one = 1;
                                [[maybe_unused]] auto const written = write(wake_fd, &one, sizeof(one));
                            }};

    /* An exception that left this thread would terminate i3, and exit() would
     * end i3 under its main thread. */
    try {
        Nagbar nagbar{bar_type, prompt, pattern, buttons};

        nagbar.create_window(position_on_primary);

        nagbar.start_event_loop(wake_fd);
    } catch (std::exception const &e) {
        ELOG(fmt::sprintf("Could not show the nagbar: %s\n", e.what()));
    }

    done->store(true, std::memory_order_release);
}

NagbarThread::wake_fd_t::wake_fd_t()
    : fd(eventfd(0, EFD_CLOEXEC)) {
    if (fd == -1) {
        throw std::system_error(errno, std::system_category(), "eventfd()");
    }
}

NagbarThread::wake_fd_t::~wake_fd_t() {
    close(fd);
}

NagbarThread::NagbarThread(nagbar_kind_t kind, std::vector<button_t> buttons, std::string prompt,
                           std::string pattern, bar_type_t bar_type, bool position_on_primary)
    : kind(kind),
      thread(draw_nagbar, wake.fd, &done, std::move(prompt), std::move(buttons), bar_type, position_on_primary,
             std::move(pattern)) {
}

/*
 * Shows a nagbar with the given parameters, unless kind is CONFIG_ERROR or
 * COMMAND_ERROR and a nagbar of that kind is shown already. It runs until its
 * user closes it, or until kill_nagbars() closes it.
 *
 */
void start_nagbar(nagbar_kind_t kind,
                  std::vector<button_t> buttons,
                  std::string prompt,
                  std::string pattern,
                  bar_type_t bar_type,
                  bool position_on_primary) {
    /* Joins the threads of the nagbars that their users closed. */
    std::erase_if(global.nagbars, [](NagbarThread const &nagbar) { return !nagbar.running(); });

    if (kind != nagbar_kind_t::OTHER &&
        std::ranges::any_of(global.nagbars, [kind](NagbarThread const &nagbar) { return nagbar.kind == kind; })) {
        DLOG("A nagbar of this kind is shown already, not starting another one.\n");
        return;
    }

    try {
        global.nagbars.emplace_back(kind, std::move(buttons), std::move(prompt), std::move(pattern), bar_type, position_on_primary);
    } catch (std::system_error const &e) {
        ELOG(fmt::sprintf("Could not start the nagbar: %s\n", e.what()));
    }
}

/*
 * Closes the nagbars of the given kind, or all of them, and waits until their
 * threads have ended.
 *
 */
void kill_nagbars(std::optional<nagbar_kind_t> kind) {
    std::erase_if(global.nagbars, [kind](NagbarThread const &nagbar) { return !kind || nagbar.kind == *kind; });
}
