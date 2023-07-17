#include <err.h>
#include <cerrno>
#include <climits>
#include <fstream>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>

#include "i3string.h"
#include "log.h"
#include "draw.h"
#include "font.h"
#include "wrapper.h"
#include "dpi.h"

#include "get_process_filename.h"
#include "get_visualtype.h"
#include "util.h"
#include "i3_ipc/include/i3-ipc.h"
#include "tree.h"
#include "manage.h"
#include "i3.h"
#include "configuration.h"
#include "bindings.h"
#include "config_parser.h"
#include "nagbar.h"

#include <cctype>
#include <fcntl.h>
#include <cinttypes>
#include <libgen.h>
#include <clocale>
#include <sys/wait.h>
#include <unistd.h>
#include <fmt/core.h>
#include <getopt.h>
#include <paths.h>

#include <xcb/randr.h>
#include <xcb/xcb.h>
#include <xcb/xcb_aux.h>
#include <xcb/xcb_cursor.h>

#if defined(__OpenBSD__)
#include <sys/cdefs.h>
#endif


/*
 * Handler which will be called when we get a SIGCHLD for the nagbar, meaning
 * it exited (or could not be started, depending on the exit code).
 *
 */
static void nagbar_exited(EV_P_ ev_child *watcher, int revents) {
    ev_child_stop(EV_A_ watcher);

    int exitcode = WEXITSTATUS(watcher->rstatus);
    if (!WIFEXITED(watcher->rstatus)) {
        ELOG(fmt::sprintf("i3-nagbar (%d) did not exit normally. This is not an error if the config was reloaded while a nagbar was active.\n",  watcher->pid));
    } else if (exitcode != 0) {
        ELOG(fmt::sprintf("i3-nagbar (%d) process exited with status %d\n",  watcher->pid, exitcode));
    } else {
        DLOG(fmt::sprintf("i3-nagbar (%d) process exited with status %d\n",  watcher->pid, exitcode));
    }

    auto *nagbar_pid = (pid_t*)watcher->data;
    if (nagbar_pid != nullptr && *nagbar_pid == watcher->pid) {
        /* Only reset if the watched nagbar is the active nagbar */
        *nagbar_pid = -1;
    }

    delete watcher;
}

#define NAGBAR_ATOMS_XMACRO \
xmacro(ATOM) \
xmacro(CARDINAL)

#define xmacro(atom) xcb_atom_t A_##atom;
NAGBAR_ATOMS_XMACRO
#undef xmacro

#define MSG_PADDING logical_px(root_screen, 8)
#define BTN_PADDING logical_px(root_screen, 3)
#define BTN_BORDER logical_px(root_screen, 3)
#define BTN_GAP logical_px(root_screen, 20)
#define CLOSE_BTN_GAP logical_px(root_screen, 15)
#define BAR_BORDER logical_px(root_screen, 2)

static xcb_connection_t *conn;
static surface_t bar;
static button_t btn_close;
static xcb_screen_t *root_screen;
static i3Font *font;

/* Result of get_colorpixel() for the various colors. */
static color_t color_background;        /* background of the bar */
static color_t color_button_background; /* background for buttons */
static color_t color_border;            /* color of the button border */
static color_t color_border_bottom;     /* color of the bottom border */
static color_t color_text;              /* color of the text */

static const button_t *get_button_at(const std::vector<button_t> &buttons, int16_t x, int16_t y) {
    for (const auto &button : buttons) {
        if (x >= (button.x) && x <= (button.x + button.width)) {
            return &button;
        }
    }

    return nullptr;
}

static void handle_button_press(xcb_button_press_event_t *event) {
    printf("button pressed on x = %d, y = %d\n",
            event->event_x, event->event_y);
    /* TODO: set a flag for the button, re-render */
}

/*
 * Called when the user releases the mouse button. Checks whether the
 * coordinates are over a button and executes the appropriate action.
 *
 */
static void handle_button_release(const std::vector<button_t> &buttons, xcb_button_release_event_t *event) {
    printf("button released on x = %d, y = %d\n",
            event->event_x, event->event_y);
    /* If the user hits the close button, we exit(0) */
    if (event->event_x >= btn_close.x && event->event_x < btn_close.x + btn_close.width)
        exit(0);
    const button_t *button = get_button_at(buttons, event->event_x, event->event_y);
    if (!button)
        return;

    /* We need to create a custom script containing our actual command
     * since not every terminal emulator which is contained in
     * i3-sensible-terminal supports -e with multiple arguments (and not
     * all of them support -e with one quoted argument either).
     *
     * NB: The paths need to be unique, that is, don’t assume users close
     * their nagbars at any point in time (and they still need to work).
     * */
    char *script_path = get_process_filename("nagbar-cmd");

    int fd = open(script_path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        warn("Could not create temporary script to store the nagbar command");
        return;
    }
    FILE *script = fdopen(fd, "w");
    if (script == nullptr) {
        warn("Could not fdopen() temporary script to store the nagbar command");
        return;
    }
    fprintf(script, "#!%s\nrm %s\n%s", _PATH_BSHELL, script_path, button->action);
    /* Also closes fd */
    fclose(script);

    char *link_path;
    //char *exe_path = get_exe_path(argv0);
    sasprintf(&link_path, "%s.nagbar_cmd", script_path);
    //if (symlink(exe_path, link_path) == -1) {
    //    err(EXIT_FAILURE, "Failed to symlink %s to %s", link_path, exe_path);
    //}

    char *terminal_cmd;
    if (button->terminal) {
        sasprintf(&terminal_cmd, "i3-sensible-terminal -e %s", link_path);
    } else {
        terminal_cmd = sstrdup(link_path);
    }
    printf("terminal_cmd = %s\n", terminal_cmd);

    //start_application(terminal_cmd);

    free(link_path);
    free(terminal_cmd);
    free(script_path);
    //free(exe_path);

    /* TODO: unset flag, re-render */
}

/*
 * Draws a button and returns its width
 *
 */
static int button_draw(button_t *button, int position) {
    int text_width = predict_text_width(conn, root_screen, button->label);
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
    draw_util_text(conn, button->label, &bar, color_text, color_button_background,
            position - button->width + BTN_BORDER + BTN_PADDING,
            MSG_PADDING,
            200);
    return button->width;
}

/*
 * Handles expose events (redraws of the window) and rendering in general. Will
 * be called from the code with event == NULL or from X with event != NULL.
 *
 */
static int handle_expose(xcb_connection_t *conn, i3String *prompt, std::vector<button_t> &buttons, xcb_expose_event_t *event) {
    /* draw background */
    draw_util_clear_surface(&bar, color_background);
    /* draw message */
    draw_util_text(conn, prompt, &bar, color_text, color_background,
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

/**
 * Tries to position the rectangle on the primary output.
 */
static void set_window_position_primary(xcb_rectangle_t *result) {
    xcb_randr_get_screen_resources_current_cookie_t rcookie = xcb_randr_get_screen_resources_current(conn, global.x->root);
    xcb_randr_get_output_primary_cookie_t pcookie = xcb_randr_get_output_primary(conn, global.x->root);

    xcb_randr_get_output_primary_reply_t *primary = nullptr;
    xcb_randr_get_screen_resources_current_reply_t *res = nullptr;

    if ((primary = xcb_randr_get_output_primary_reply(conn, pcookie, nullptr)) == nullptr) {
        //LOG("Could not determine the primary output.\n");
        free(res);
        free(primary);
        return;
    }

    if ((res = xcb_randr_get_screen_resources_current_reply(conn, rcookie, nullptr)) == nullptr) {
        //LOG("Could not query screen resources.\n");
        free(res);
        free(primary);
        return;
    }

    xcb_randr_get_output_info_reply_t *output =
            xcb_randr_get_output_info_reply(conn,
                    xcb_randr_get_output_info(conn, primary->output, res->config_timestamp),
                    nullptr);
    if (output == nullptr || output->crtc == XCB_NONE) {
        //LOG("Could not query primary screen.\n");
        free(res);
        free(primary);
        return;
    }

    xcb_randr_get_crtc_info_reply_t *crtc =
            xcb_randr_get_crtc_info_reply(conn,
                    xcb_randr_get_crtc_info(conn, output->crtc, res->config_timestamp),
                    nullptr);
    if (crtc == nullptr) {
        //LOG("Could not get CRTC.\n");
        free(res);
        free(primary);
        return;
    }

    //LOG("Found primary output on position x = %i / y = %i / w = %i / h = %i.\n",
    //    crtc->x, crtc->y, crtc->width, crtc->height);
    if (crtc->width == 0 || crtc->height == 0) {
        //LOG("Primary output is not active, ignoring it.\n");
        free(res);
        free(primary);
        return;
    }

    result->x = crtc->x;
    result->y = crtc->y;
    free(res);
    free(primary);
}

/**
 * Tries to position the rectangle on the output with input focus.
 * If unsuccessful, try to position on primary output.
 */
static void set_window_position_focus(xcb_rectangle_t *result) {
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

    //LOG("Found current focus at x = %i / y = %i.\n", coordinates->dst_x, coordinates->dst_y);
    result->x = coordinates->dst_x;
    result->y = coordinates->dst_y;
    success = true;

    free_resources:
    xcb_ungrab_server(conn);
    free(input_focus);
    free(coordinates);
    free(geometry);
    if (!success) {
        //LOG("Could not position on focused output, trying to position on primary output.\n");
        set_window_position_primary(result);
    }
}

static void draw_nagbar(i3String *prompt,
                        std::vector<button_t> &buttons,
                        const bar_type_t bar_type,
                        const bool position_on_primary,
                        const char *pattern) {

    /* The following lines are a terribly horrible kludge. Because terminal
     * emulators have different ways of interpreting the -e command line
     * argument (some need -e "less /etc/fstab", others need -e less
     * /etc/fstab), we need to write commands to a script and then just run
     * that script. However, since on some machines, $XDG_RUNTIME_DIR and
     * $TMPDIR are mounted with noexec, we cannot directly execute the script
     * either.
     *
     * Initially, we tried to pass the command via the environment variable
     * _I3_NAGBAR_CMD. But turns out that some terminal emulators such as
     * xfce4-terminal run all windows from a single master process and only
     * pass on the command (not the environment) to that master process.
     *
     * Therefore, we symlink i3-nagbar (which MUST reside on an executable
     * filesystem) with a special name and run that symlink. When i3-nagbar
     * recognizes it’s started as a binary ending in .nagbar_cmd, it strips off
     * the .nagbar_cmd suffix and runs /bin/sh on argv[0]. That way, we can run
     * a shell script on a noexec filesystem.
     *
     * From a security point of view, i3-nagbar is just an alias to /bin/sh in
     * certain circumstances. This should not open any new security issues, I
     * hope. */
    char *cmd = nullptr;
    /*
    const size_t argv0_len = strlen(argv[0]);
    if (argv0_len > strlen(".nagbar_cmd") &&
        strcmp(argv[0] + argv0_len - strlen(".nagbar_cmd"), ".nagbar_cmd") == 0) {
        unlink(argv[0]);
        cmd = sstrdup(argv[0]);
        *(cmd + argv0_len - strlen(".nagbar_cmd")) = '\0';
        execl(_PATH_BSHELL, _PATH_BSHELL, cmd, NULL);
        err(EXIT_FAILURE, "execl(%s, %s, %s)", _PATH_BSHELL, _PATH_BSHELL, cmd);
    }
    */

    btn_close.label = new i3String{"X"};

    int screens;
    if ((conn = xcb_connect(nullptr, &screens)) == nullptr ||
        xcb_connection_has_error(conn))
        errx(EXIT_FAILURE, "Cannot open display");

/* Place requests for the atoms we need as soon as possible */
#define xmacro(atom) \
    xcb_intern_atom_cookie_t atom##_cookie = xcb_intern_atom(conn, 0, strlen(#atom), #atom);
    NAGBAR_ATOMS_XMACRO
#undef xmacro

    /* Init startup notification. */
    //SnDisplay *sndisplay = sn_xcb_display_new(conn, nullptr, nullptr);
    //SnLauncheeContext *sncontext = sn_launchee_context_new_from_environment(sndisplay, screens);
    //sn_display_unref(sndisplay);

    root_screen = xcb_aux_get_screen(conn, screens);
    global.x->root = root_screen->root;

    if (bar_type == TYPE_ERROR) {
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
    font = load_font(conn, root_screen, pattern, true);
    set_font(font);

#if defined(__OpenBSD__)
    if (pledge("stdio rpath wpath cpath getpw proc exec", NULL) == -1)
        err(EXIT_FAILURE, "pledge");
#endif


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
    auto win = xcb_generate_id(conn);

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
#define xmacro(name)                                                                       \
    do {                                                                                   \
        xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(conn, name##_cookie, NULL); \
        if (!reply)                                                                        \
            errx(EXIT_FAILURE, "Could not get atom " #name);                               \
                                                                                           \
        A_##name = reply->atom;                                                            \
        free(reply);                                                                       \
    } while (0);
    NAGBAR_ATOMS_XMACRO
#undef xmacro

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

    /* Grab the keyboard to get all input */
    xcb_flush(conn);

    xcb_generic_event_t *event;
    while ((event = xcb_wait_for_event(conn)) != nullptr) {
        if (event->response_type == 0) {
            fprintf(stderr, "X11 Error received! sequence %x\n", event->sequence);
            continue;
        }

        /* Strip off the highest bit (set if the event is generated) */
        int type = (event->response_type & 0x7F);

        switch (type) {
            case XCB_EXPOSE:
                if (((xcb_expose_event_t *)event)->count == 0) {
                    handle_expose(conn, prompt, buttons, (xcb_expose_event_t *)event);
                }

                break;

            case XCB_BUTTON_PRESS:
                handle_button_press((xcb_button_press_event_t *)event);
                break;

            case XCB_BUTTON_RELEASE:
                handle_button_release(buttons, (xcb_button_release_event_t *)event);
                break;

            case XCB_CONFIGURE_NOTIFY: {
                auto *configure_notify = (xcb_configure_notify_event_t *)event;
                if (configure_notify->width > 0 && configure_notify->height > 0) {
                    draw_util_surface_set_size(&bar, configure_notify->width, configure_notify->height);
                }
                break;
            }
        }

        free(event);
    }

    draw_util_surface_free(conn, &bar);
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
                  const char *prompt,
                  const char *pattern,
                  bar_type_t bar_type,
                  bool position_on_primary) {
    if (nagbar_pid != nullptr && *nagbar_pid != -1) {
        DLOG(fmt::sprintf("i3-nagbar already running (PID %d), not starting again.\n",  *nagbar_pid));
        return;
    }

    auto pid = fork();
    if (pid == -1) {
        warn("Could not fork()");
        return;
    }

    if (pid == 0) {
        /* child */
        global.run_atexit = false;
        draw_nagbar(new i3String{prompt}, buttons, bar_type, position_on_primary, pattern);
    } else {
        /* parent */
        DLOG(fmt::sprintf("Starting i3-nagbar with PID %d\n",  pid));

        if (nagbar_pid != nullptr) {
            *nagbar_pid = pid;
        }

        /* install a child watcher */
        auto *child = new ev_child();
        ev_child_init(child, &nagbar_exited, pid, 0);
        child->data = nagbar_pid;
        ev_child_start(main_loop, child);
    }
}

/*
 * Kills the i3-nagbar process, if nagbar_pid != -1.
 *
 * If wait_for_it is set (restarting i3), this function will waitpid(),
 * otherwise, ev is assumed to handle it (reloading).
 *
 */
void kill_nagbar(pid_t nagbar_pid, bool wait_for_it) {
    if (nagbar_pid == -1)
        return;

    if (kill(nagbar_pid, SIGTERM) == -1)
        warn("kill(configerror_nagbar) failed");

    if (!wait_for_it)
        return;

    /* When restarting, we don’t enter the ev main loop anymore and after the
     * exec(), our old pid is no longer watched. So, ev won’t handle SIGCHLD
     * for us and we would end up with a <defunct> process. Therefore we
     * waitpid() here. */
    waitpid(nagbar_pid, nullptr, 0);
}
