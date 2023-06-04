/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * startup.c: Startup notification code. Ensures a startup notification context
 *            is setup when launching applications. We store the current
 *            workspace to open windows in that startup notification context on
 *            the appropriate workspace.
 *
 */
#include <cassert>

#include <cstdlib>
#include <cstring>

#include <xcb/xcb.h>

#include "i3string.h"
#include "log.h"
#include "draw.h"
#include "font.h"
#include "wrapper.h"
#include "dpi.h"

#include "util.h"
#include "i3_ipc/include/i3-ipc.h"
#include "tree.h"
#include "xcb.h"
#include "i3.h"
#include "con.h"
#include "startup.h"

#include <paths.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define SN_API_NOT_YET_FROZEN 1
#include <libsn/sn-launcher.h>
#include "global.h"
#include <config.h>

static std::vector<std::unique_ptr<Startup_Sequence>> startup_sequences{};

/*
 * After 60 seconds, a timeout will be triggered for each startup sequence.
 *
 * The timeout will just trigger completion of the sequence, so the normal
 * completion process takes place (startup_monitor_event will free it).
 *
 */
static void startup_timeout(EV_P_ ev_timer *w, int revents) {
    const char *id = sn_launcher_context_get_startup_id((SnLauncherContext*)w->data);
    DLOG(fmt::sprintf("Timeout for startup sequence %s\n",  id));

    auto seq_ptr = std::ranges::find_if(startup_sequences, [&id](auto &currrent) {
        return strcmp(currrent->id, id) == 0;
    });

    /* Unref the context (for the timeout itself, see start_application) */
    sn_launcher_context_unref((SnLauncherContext*)w->data);

    if (seq_ptr == startup_sequences.end()) {
        DLOG("Sequence already deleted, nevermind.\n");
        delete w;
        return;
    }

    /* Complete the startup sequence, will trigger its deletion. */
    sn_launcher_context_complete((SnLauncherContext*)w->data);
    delete w;
}

/*
 * Some applications (such as Firefox) mark a startup sequence as completed
 * *before* they even map a window. Therefore, we cannot entirely delete the
 * startup sequence once it’s marked as complete. Instead, we’ll mark it for
 * deletion in 30 seconds and use that chance to delete old sequences.
 *
 * This function returns the number of active (!) startup notifications, that
 * is, those which are not marked for deletion yet. This is used for changing
 * the root window cursor.
 *
 */
static int _prune_startup_sequences() {
    time_t current_time = time(nullptr);
    int active_sequences = 0;

    /* Traverse the list and delete everything which was marked for deletion 30
     * seconds ago or earlier. */
    for (auto it = startup_sequences.begin(); it != startup_sequences.end(); ++it) {
        auto &current = *it;
        if (current != nullptr) {
            if (current->delete_at == 0) {
                active_sequences++;
                continue;
            }

            if (current_time <= current->delete_at) {
                continue;
            }
        } else {
            DLOG("Found a nullptr in the startup sequence list.\n");
        }

        it = startup_sequences.erase(it);
    }

    return active_sequences;
}

/*
 * Deletes a startup sequence, ignoring whether its timeout has elapsed.
 * Useful when e.g. a window is moved between workspaces and its children
 * shouldn't spawn on the original workspace.
 *
 */
Startup_Sequence::~Startup_Sequence() {
    auto sequence = this;
    DLOG(fmt::sprintf("Deleting startup sequence %s, delete_at = %lld, current_time = %lld\n",
         sequence->id, (long long)sequence->delete_at, (long long)time(nullptr)));

    /* Unref the context, will be free()d */
    sn_launcher_context_unref(sequence->context);

    free(sequence->id);
    free(sequence->workspace);
}

/*
 * Starts the given application by passing it through a shell. We use double
 * fork to avoid zombie processes. As the started application’s parent exits
 * (immediately), the application is reparented to init (process-id 1), which
 * correctly handles children, so we don’t have to do it :-).
 *
 * The shell used to start applications is the system's bourne shell (i.e.,
 * /bin/sh).
 *
 * The no_startup_id flag determines whether a startup notification context
 * (and ID) should be created, which is the default and encouraged behavior.
 *
 */
void start_application(const std::string_view &command, bool no_startup_id) {
    SnLauncherContext *context = nullptr;

    if (!no_startup_id) {
        /* Create a startup notification context to monitor the progress of this
         * startup. */
        context = sn_launcher_context_new(sndisplay, global.x->conn->default_screen());
        sn_launcher_context_set_name(context, "i3");
        sn_launcher_context_set_description(context, "exec command in i3");
        /* Chop off everything starting from the first space (if there are any
         * spaces in the command), since we don’t want the parameters. */
        auto space = std::ranges::find(command, ' ');
        const std::basic_string_view<char> &view = std::string_view(command.begin(), space);
        sn_launcher_context_initiate(context, "i3", view.data(), global.last_timestamp);

        /* Trigger a timeout after 60 seconds */
        auto *timeout = new ev_timer();
        ev_timer_init(timeout, startup_timeout, 60.0, 0.);
        timeout->data = context;
        ev_timer_start(main_loop, timeout);

        LOG(fmt::sprintf("startup id = %s\n",  sn_launcher_context_get_startup_id(context)));

        /* Save the ID and current workspace in our internal list of startup
         * sequences */
        Con *ws = focused->con_get_workspace();
        auto sequence = std::make_unique<Startup_Sequence>();
        sequence->id = sstrdup(sn_launcher_context_get_startup_id(context));
        sequence->workspace = sstrdup(ws->name.c_str());
        sequence->context = context;
        startup_sequences.push_back(std::move(sequence));

        /* Increase the refcount once (it starts with 1, so it will be 2 now) for
         * the timeout. Even if the sequence gets completed, the timeout still
         * needs the context (but will unref it then) */
        sn_launcher_context_ref(context);
    }

    LOG(fmt::sprintf("executing: %s\n",  command));
    if (fork() == 0) {
        /* Child process */
        setsid();
        setrlimit(RLIMIT_CORE, &global.original_rlimit_core);
        signal(SIGPIPE, SIG_DFL);
        if (fork() == 0) {
            /* Setup the environment variable(s) */
            if (!no_startup_id)
                sn_launcher_context_setup_child_process(context);
            setenv("I3SOCK", current_socketpath.c_str(), 1);

            execl(_PATH_BSHELL, _PATH_BSHELL, "-c", command.data(), NULL);
            /* not reached */
        }
        _exit(EXIT_SUCCESS);
    }
    wait(nullptr);

    if (!no_startup_id) {
        /* Change the pointer of the root window to indicate progress */
        global.x->xcursor_set_root_cursor(XCURSOR_CURSOR_WATCH);
    }
}

/*
 * Called by libstartup-notification when something happens
 *
 */
void startup_monitor_event(SnMonitorEvent *event, void *userdata) {
    SnStartupSequence *snsequence;

    snsequence = sn_monitor_event_get_startup_sequence(event);

    /* Get the corresponding internal startup sequence */
    const char *id = sn_startup_sequence_get_id(snsequence);
    auto seq_ptr = std::ranges::find_if(startup_sequences, [&id](auto &current) {
        return strcmp(current->id, id) == 0;
    });

    if (seq_ptr == startup_sequences.end()) {
        DLOG(fmt::sprintf("Got event for startup sequence that we did not initiate (ID = %s). Ignoring.\n",  id));
        return;
    }

    auto &sequence = *seq_ptr;

    switch (sn_monitor_event_get_type(event)) {
        case SN_MONITOR_EVENT_INITIATED:
        case SN_MONITOR_EVENT_CHANGED:
        case SN_MONITOR_EVENT_CANCELED:
            break;
        case SN_MONITOR_EVENT_COMPLETED:
            DLOG(fmt::sprintf("startup sequence %s completed\n",  sn_startup_sequence_get_id(snsequence)));

            /* Mark the given sequence for deletion in 30 seconds. */
            time_t current_time = time(nullptr);
            sequence->delete_at = current_time + 30;
            DLOG(fmt::sprintf("Will delete startup sequence %s at timestamp %lld\n",
                 sequence->id, (long long)sequence->delete_at));

            if (_prune_startup_sequences() == 0) {
                DLOG("No more startup sequences running, changing root window cursor to default pointer.\n");
                /* Change the pointer of the root window to indicate progress */
                global.x->xcursor_set_root_cursor(XCURSOR_CURSOR_POINTER);
            }
            break;
    }
}

/*
 * Renames workspaces that are mentioned in the startup sequences.
 *
 */
void startup_sequence_rename_workspace(const char *old_name, const char *new_name) {
    for (auto &current : startup_sequences) {
        if (strcmp(current->workspace, old_name) != 0)
            continue;
         DLOG(fmt::sprintf("Renaming workspace \"%s\" to \"%s\" in startup sequence %s.\n",
             old_name, new_name, current->id));
        free(current->workspace);
        current->workspace = sstrdup(new_name);
    }
}

/*
 * Gets the stored startup sequence for the _NET_STARTUP_ID of a given window.
 *
 */
std::vector<std::unique_ptr<Startup_Sequence>>::iterator startup_sequence_get(i3Window *cwindow,
                                              xcb_get_property_reply_t* startup_id_reply, bool ignore_mapped_leader) {
    /* The _NET_STARTUP_ID is only needed during this function, so we get it
     * here and don’t save it in the 'cwindow'. */
    if (startup_id_reply == nullptr || xcb_get_property_value_length(startup_id_reply) == 0) {
        DLOG(fmt::sprintf("No _NET_STARTUP_ID set on window 0x%08x\n",  cwindow->id));
        if (cwindow->leader == XCB_NONE)
            return startup_sequences.end();

        /* This is a special case that causes the leader's startup sequence
         * to only be returned if it has never been mapped, useful primarily
         * when trying to delete a sequence.
         *
         * It's generally inappropriate to delete a leader's sequence when
         * moving a child window, but if the leader has no container, it's
         * likely permanently unmapped and the child is the "real" window. */
        if (ignore_mapped_leader && con_by_window_id(cwindow->leader) != nullptr) {
            DLOG(fmt::sprintf("Ignoring leader window 0x%08x\n",  cwindow->leader));
            return startup_sequences.end();
        }

        DLOG(fmt::sprintf("Checking leader window 0x%08x\n",  cwindow->leader));

        auto property = global.x->conn->get_property_unchecked(false, cwindow->leader,
                                  A__NET_STARTUP_ID, XCB_GET_PROPERTY_TYPE_ANY, 0, 512);
        startup_id_reply = property.get().get();

        if (startup_id_reply == nullptr ||
            xcb_get_property_value_length(startup_id_reply) == 0) {
            DLOG("No _NET_STARTUP_ID set on the leader either\n");
            return startup_sequences.end();
        }
    }

    auto startup_id = fmt::sprintf("%.*s", xcb_get_property_value_length(startup_id_reply),
              (char *)xcb_get_property_value(startup_id_reply));

    auto seq_ptr = std::ranges::find_if(startup_sequences, [&startup_id](auto &current) {
        return strcmp(current->id, startup_id.c_str()) == 0;
    });

    if (seq_ptr == startup_sequences.end()) {
        DLOG(fmt::sprintf("WARNING: This sequence (ID %s) was not found\n",  startup_id));
        return startup_sequences.end();
    }

    return seq_ptr;
}

/*
 * Checks if the given window belongs to a startup notification by checking if
 * the _NET_STARTUP_ID property is set on the window (or on its leader, if it’s
 * unset).
 *
 * If so, returns the workspace on which the startup was initiated.
 * Returns NULL otherwise.
 *
 */
char *startup_workspace_for_window(i3Window *cwindow, xcb_get_property_reply_t *startup_id_reply) {
    auto seq_ptr = startup_sequence_get(cwindow, startup_id_reply, false);
    if (seq_ptr == startup_sequences.end())
        return nullptr;

    /* If the startup sequence's time span has elapsed, delete it. */
    time_t current_time = time(nullptr);
    if ((*seq_ptr)->delete_at > 0 && current_time > (*seq_ptr)->delete_at) {
        DLOG(fmt::sprintf("Deleting expired startup sequence %s\n",  (*seq_ptr)->id));
        startup_sequences.erase(seq_ptr);

        return nullptr;
    }

    return (*seq_ptr)->workspace;
}

/*
 * Deletes the startup sequence for a window if it exists.
 *
 */
void startup_sequence_delete_by_window(i3Window *win) {

    auto startup_id_reply = global.x->conn->get_property(false, win->id, A__NET_STARTUP_ID, XCB_GET_PROPERTY_TYPE_ANY, 0, 512);

    auto sequence = startup_sequence_get(win, (startup_id_reply.get() != nullptr) ? startup_id_reply.get().get() : nullptr, true);
    if (sequence != startup_sequences.end()) {
        startup_sequences.erase(sequence);
    }
}
