/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 */

#include <cstdint>
#include <cstdlib>
#include <sys/types.h>

#include <xcb/xcb.h>
#include <xcb/xcb_keysyms.h>

#include "i3string.h"
#include "log.h"
#include "draw.h"
#include "font.h"
#include "wrapper.h"
#include "dpi.h"

#include "util.h"
#include "xcb.h"
#include "i3.h"
#include "configuration.h"
#include "randr.h"
#include "sighandler.h"
#include "global.h"

#include <sys/wait.h>
#include <unistd.h>
#include <filesystem>

class dialog_t {
   public:
    xcb_window_t id{};
    xcb_colormap_t colormap{};
    Rect dims{};
    surface_t surface{};
    ~dialog_t() {
        xcb_free_colormap(**global.x, colormap);
        draw_util_surface_free(**global.x, &surface);
        xcb_destroy_window(**global.x, id);
    }
};

static std::vector<std::unique_ptr<dialog_t>> dialogs{};
static int raised_signal;
static int backtrace_done = 0;

static int sighandler_backtrace();
static void sighandler_setup();
static void sighandler_create_dialogs();
static void sighandler_destroy_dialogs();
static void sighandler_handle_expose();
static void sighandler_draw_dialog(dialog_t *dialog);
static void sighandler_handle_key_press(xcb_key_press_event_t *event);

static i3String *message_intro;
static i3String *message_intro2;
static i3String *message_option_backtrace;
static i3String *message_option_restart;
static i3String *message_option_forget;
static int dialog_width;
static int dialog_height;

static int border_width = 2;
static int margin = 4;

/*
 * Attach gdb to pid_parent and dump a backtrace to i3-backtrace.$pid in the
 * tmpdir
 */
static int sighandler_backtrace() {
    char *tmpdir = getenv("TMPDIR");
    if (tmpdir == nullptr)
        tmpdir = (char*)"/tmp";

    pid_t pid_parent = getpid();

    char *filename = nullptr;
    int suffix = 0;
    /* Find a unique filename for the backtrace (since the PID of i3 stays the
     * same), so that we don’t overwrite earlier backtraces. */
    do {
        FREE(filename);
        sasprintf(&filename, "%s/i3-backtrace.%d.%d.txt", tmpdir, pid_parent, suffix);
        suffix++;
    } while (std::filesystem::exists(filename));

    pid_t pid_gdb = fork();
    if (pid_gdb < 0) {
        DLOG("Failed to fork for GDB\n");
        return -1;
    } else if (pid_gdb == 0) {
        /* child */
        int stdin_pipe[2],
            stdout_pipe[2];

        if (pipe(stdin_pipe) == -1) {
            ELOG("Failed to init stdin_pipe\n");
            return -1;
        }
        if (pipe(stdout_pipe) == -1) {
            ELOG("Failed to init stdout_pipe\n");
            return -1;
        }

        /* close standard streams in case i3 is started from a terminal; gdb
         * needs to run without controlling terminal for it to work properly in
         * this situation */
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);

        /* We provide pipe file descriptors for stdin/stdout because gdb < 7.5
         * crashes otherwise, see
         * https://sourceware.org/bugzilla/show_bug.cgi?id=14114 */
        dup2(stdin_pipe[0], STDIN_FILENO);
        dup2(stdout_pipe[1], STDOUT_FILENO);

        char *pid_s, *gdb_log_cmd;
        sasprintf(&pid_s, "%d", pid_parent);
        sasprintf(&gdb_log_cmd, "set logging file %s", filename);

        char *args[] = {
                (char*)"gdb",
            global.start_argv[0],
                (char*)"-p",
            pid_s,
                (char*)"-batch",
                (char*)"-nx",
                (char*)"-ex", gdb_log_cmd,
                (char*)"-ex", (char*)"set logging on",
                (char*)"-ex", (char*)"bt full",
                (char*)"-ex", (char*)"quit",
            nullptr};
        execvp(args[0], args);
        DLOG("Failed to exec GDB\n");
        exit(EXIT_FAILURE);
    }
    int status = 0;

    waitpid(pid_gdb, &status, 0);

    /* see if the backtrace was successful or not */
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        DLOG("GDB did not run properly\n");
        return -1;
    } else if (!std::filesystem::exists(filename)) {
        DLOG("GDB executed successfully, but no backtrace was generated\n");
        return -1;
    }
    return 1;
}

static void sighandler_setup() {
    border_width = logical_px(global.x->root_screen, border_width);
    margin = logical_px(global.x->root_screen, margin);

    int num_lines = 5;
    message_intro = new i3String{"i3 has just crashed. Please report a bug for this."};
    message_intro2 = new i3String{"To debug this problem, you can either attach gdb or choose from the following options:"};
    message_option_backtrace = new i3String{"- 'b' to save a backtrace (requires gdb)"};
    message_option_restart = new i3String{"- 'r' to restart i3 in-place"};
    message_option_forget = new i3String{"- 'f' to forget the previous layout and restart i3"};

    int width_longest_message = predict_text_width(**global.x, global.x->root_screen, message_intro2);

    dialog_width = width_longest_message + 2 * border_width + 2 * margin;
    dialog_height = num_lines * config.font->height + 2 * border_width + 2 * margin;
}

static void sighandler_create_dialogs() {
    for (Output *output : global.randr->outputs) {
        if (!output->active) {
            continue;
        }

        auto dialog = std::make_unique<dialog_t>();

        xcb_visualid_t visual = get_visualid_by_depth(global.x->root_depth);
        dialog->colormap = xcb_generate_id(**global.x);
        xcb_create_colormap(**global.x, XCB_COLORMAP_ALLOC_NONE, dialog->colormap, global.x->root, visual);

        uint32_t mask = 0;
        uint32_t values[4];
        int i = 0;

        /* Needs to be set in the case of a 32-bit root depth. */
        mask |= XCB_CW_BACK_PIXEL;
        values[i++] = global.x->root_screen->black_pixel;

        /* Needs to be set in the case of a 32-bit root depth. */
        mask |= XCB_CW_BORDER_PIXEL;
        values[i++] = global.x->root_screen->black_pixel;

        mask |= XCB_CW_OVERRIDE_REDIRECT;
        values[i++] = 1;

        /* Needs to be set in the case of a 32-bit root depth. */
        mask |= XCB_CW_COLORMAP;
        values[i++] = dialog->colormap;

        dialog->dims.x = output->rect.x + (output->rect.width / 2);
        dialog->dims.y = output->rect.y + (output->rect.height / 2);
        dialog->dims.width = dialog_width;
        dialog->dims.height = dialog_height;

        /* Make sure the dialog is centered. */
        dialog->dims.x -= dialog->dims.width / 2;
        dialog->dims.y -= dialog->dims.height / 2;

        dialog->id = create_window((xcb_connection_t*)**global.x, dialog->dims, global.x->root_depth, visual,
                                   XCB_WINDOW_CLASS_INPUT_OUTPUT, XCURSOR_CURSOR_POINTER,
                                   true, mask, values);

        draw_util_surface_init(**global.x, &(dialog->surface), dialog->id, get_visualtype_by_id(visual),
                               dialog->dims.width, dialog->dims.height);

        xcb_grab_keyboard(**global.x, false, dialog->id, XCB_CURRENT_TIME, XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC);

        /* Confine the pointer to the crash dialog. */
        xcb_grab_pointer(**global.x, false, dialog->id, XCB_NONE, XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC, dialog->id,
                         XCB_NONE, XCB_CURRENT_TIME);

        dialogs.push_back(std::move(dialog));
    }

    sighandler_handle_expose();
    xcb_flush(**global.x);
}

static void sighandler_destroy_dialogs() {
    dialogs.clear();

    xcb_flush(**global.x);
}

static void sighandler_handle_expose() {
    for (auto &current : dialogs) {
        sighandler_draw_dialog(current.get());
    }

    xcb_flush(**global.x);
}

static void sighandler_draw_dialog(dialog_t *dialog) {
    const color_t black = draw_util_hex_to_color(**global.x, global.x->root_screen, "#000000");
    const color_t white = draw_util_hex_to_color(**global.x, global.x->root_screen, "#FFFFFF");
    const color_t red = draw_util_hex_to_color(**global.x, global.x->root_screen, "#FF0000");

    /* Start with a clean slate and draw a red border. */
    draw_util_clear_surface(&(dialog->surface), red);
    draw_util_rectangle(&(dialog->surface), black, border_width, border_width,
                        dialog->dims.width - 2 * border_width, dialog->dims.height - 2 * border_width);

    int y = border_width + margin;
    const int x = border_width + margin;
    const int max_width = dialog->dims.width - 2 * x;

    draw_util_text(**global.x, message_intro, &(dialog->surface), white, black, x, y, max_width);
    y += config.font->height;

    draw_util_text(**global.x, message_intro2, &(dialog->surface), white, black, x, y, max_width);
    y += config.font->height;

    char *bt_color = (char*)"#FFFFFF";
    if (backtrace_done < 0) {
        bt_color = (char*)"#AA0000";
    } else if (backtrace_done > 0) {
        bt_color = (char*)"#00AA00";
    }
    draw_util_text(**global.x, message_option_backtrace, &(dialog->surface), draw_util_hex_to_color(**global.x, global.x->root_screen, bt_color), black, x, y, max_width);
    y += config.font->height;

    draw_util_text(**global.x, message_option_restart, &(dialog->surface), white, black, x, y, max_width);
    y += config.font->height;

    draw_util_text(**global.x, message_option_forget, &(dialog->surface), white, black, x, y, max_width);
    y += config.font->height;
}

static void sighandler_handle_key_press(xcb_key_press_event_t *event) {
    uint16_t state = event->state;

    /* Apparently, after activating numlock once, the numlock modifier
     * stays turned on (use xev(1) to verify). So, to resolve useful
     * keysyms, we remove the numlock flag from the event state */
    state &= ~global.x->xcb_numlock_mask;

    xcb_keysym_t sym = global.keysyms->key_press_lookup_keysym(event, state);

    if (sym == 'b') {
        DLOG("User issued core-dump command.\n");

        /* fork and exec/attach GDB to the parent to get a backtrace in the
         * tmpdir */
        backtrace_done = sighandler_backtrace();
        sighandler_handle_expose();
    } else if (sym == 'r') {
        sighandler_destroy_dialogs();
        i3_restart(false);
    } else if (sym == 'f') {
        sighandler_destroy_dialogs();
        i3_restart(true);
    }
}

static void handle_signal(int sig, siginfo_t *info, void *data) {
    DLOG(fmt::sprintf("i3 crashed. SIG: %d\n",  sig));

    struct sigaction action{};
    action.sa_handler = SIG_DFL;
    action.sa_flags = 0;
    sigemptyset(&action.sa_mask);
    sigaction(sig, &action, nullptr);
    raised_signal = sig;

    sighandler_setup();
    sighandler_create_dialogs();

    xcb_generic_event_t *event;
    /* Yay, more own eventhandlers… */
    while ((event = xcb_wait_for_event(**global.x))) {
        /* Strip off the highest bit (set if the event is generated) */
        int type = (event->response_type & 0x7F);
        switch (type) {
            case XCB_KEY_PRESS:
                sighandler_handle_key_press((xcb_key_press_event_t *)event);
                break;
            case XCB_EXPOSE:
                if (((xcb_expose_event_t *)event)->count == 0) {
                    sighandler_handle_expose();
                }

                break;
        }

        free(event);
    }
}

/*
 * Configured a signal handler to gracefully handle crashes and allow the user
 * to generate a backtrace and rescue their session.
 *
 */
void setup_signal_handler() {
    struct sigaction action{};

    action.sa_sigaction = handle_signal;
    action.sa_flags = SA_NODEFER | SA_RESETHAND | SA_SIGINFO;
    sigemptyset(&action.sa_mask);

    /* Catch all signals with default action "Core", see signal(7) */
    if (sigaction(SIGQUIT, &action, nullptr) == -1 ||
        sigaction(SIGILL, &action, nullptr) == -1 ||
        sigaction(SIGABRT, &action, nullptr) == -1 ||
        sigaction(SIGFPE, &action, nullptr) == -1 ||
        sigaction(SIGSEGV, &action, nullptr) == -1)
        ELOG("Could not setup signal handler.\n");
}
