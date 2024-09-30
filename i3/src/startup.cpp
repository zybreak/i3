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
module;
#include <xcb/xcb.h>

#include <ev.h>

#include <config.h>

#include <sys/time.h>

#include <xcb/shape.h>
#include <xcb/xcb_keysyms.h>

#define explicit dont_use_cxx_explicit
#include <xcb/xkb.h>
#undef explicit

#include <paths.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <unistd.h>

#define SN_API_NOT_YET_FROZEN 1
#include <libsn/sn-launcher.h>
#include <libsn/sn-monitor.h>
#undef SN_API_NOT_YET_FROZEN
#include <fmt/printf.h>
module i3;

import std;
import utils;
import log;

static std::vector<std::unique_ptr<Startup_Sequence>> startup_sequences{};

static Startup_Sequence* startup_sequence_get(i3Window *cwindow, xcb_get_property_reply_t* startup_id_reply, bool ignore_mapped_leader);

void remove_startup_sequence(std::string id) {
    std::erase_if(startup_sequences, [&id](auto &currrent) {
      return currrent->id == id;
    });
}

/*
 * After 60 seconds, a timeout will be triggered for each startup sequence.
 *
 * The timeout will just trigger completion of the sequence, so the normal
 * completion process takes place (startup_monitor_event will free it).
 *
 */
static void startup_timeout(EV_P_ ev_timer *w, int revents) {
    Startup_Sequence *sequence = (Startup_Sequence*)w->data;
    DLOG(fmt::format("Timeout for startup sequence {}", sequence->id));

    /* Complete the startup sequence, will trigger its deletion. */
    sn_launcher_context_complete(sequence->context);
    sequence->stop_timer();
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
    auto current_time = std::chrono::system_clock::now();
    int active_sequences = 0;

    /* Traverse the list and delete everything which was marked for deletion 30
     * seconds ago or earlier. */
    for (auto it = startup_sequences.begin(); it != startup_sequences.end(); ++it) {
        auto &current = *it;
        if (current != nullptr) {
            if (!current->delete_at) {
                active_sequences++;
                continue;
            }

            if (current_time <= current->delete_at.value()) {
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
    DLOG(fmt::format("Deleting startup sequence {}", this->id));

    this->stop_timer();
    
    /* Unref the context, will be free()d */
    sn_launcher_context_unref(this->context);
}

void Startup_Sequence::stop_timer() {
    if (this->timeout == nullptr) {
        return;
    }

    ev_timer_stop(global.main_loop, this->timeout);
    delete this->timeout;
    this->timeout = nullptr;
}

Startup_Sequence::Startup_Sequence(int screen, WorkspaceCon *ws, std::string name, std::string description, std::string launchee, xcb_timestamp_t last_timestamp) {
    /* Create a startup notification context to monitor the progress of this
         * startup. */
    SnLauncherContext *context = sn_launcher_context_new(sndisplay, screen);
    sn_launcher_context_set_name(context, name.c_str());
    sn_launcher_context_set_description(context, description.c_str());
    sn_launcher_context_initiate(context, "i3", launchee.c_str(), last_timestamp);

    /* Trigger a timeout after 60 seconds */
    this->timeout = new ev_timer();
    this->timeout->data = this;
    ev_timer_init(this->timeout, startup_timeout, 60.0, 0.);
    ev_timer_start(global.main_loop, this->timeout);

    LOG(fmt::sprintf("startup id = %s\n",  sn_launcher_context_get_startup_id(context)));

    this->id = sn_launcher_context_get_startup_id(context);
    this->workspace = ws->name;
    this->context = context;
}

/*
 * Starts the given application by passing it through a shell. Zombie processes
 * will be collected by ev in the default loop, we don't have to manually
 * deal with it.
 *
 * The shell used to start applications is the system's bourne shell (i.e.,
 * /bin/sh).
 *
 * The no_startup_id flag determines whether a startup notification context
 * (and ID) should be created, which is the default and encouraged behavior.
 *
 */
void start_application(const std::string_view command, bool no_startup_id) {
    SnLauncherContext *context = nullptr;

    if (!no_startup_id) {

        /* Chop off everything starting from the first space (if there are any
         * spaces in the command), since we don’t want the parameters. */
        auto space = std::ranges::find(command, ' ');
        const auto &view = std::string_view(command.begin(), space);
        std::string launchee = view.data();

        /* Save the ID and current workspace in our internal list of startup
         * sequences */
        WorkspaceCon *ws = global.focused->con_get_workspace();
        
        context = startup_sequences.emplace_back(std::make_unique<Startup_Sequence>(global.x->conn->default_screen(), ws, "i3", "exec command in i3", launchee, global.last_timestamp))->context;
    }

    LOG(fmt::sprintf("executing: %s\n",  command));
    if (fork() == 0) {
        /* Child process.
         * It will be reaped by ev, even though there is no corresponding ev_child */
        setsid();
        setrlimit(RLIMIT_CORE, &global.original_rlimit_core);
        signal(SIGPIPE, SIG_DFL);
        /* Setup the environment variable(s) */
        if (!no_startup_id) {
            sn_launcher_context_setup_child_process(context);
        }
        setenv("I3SOCK", global.current_socketpath.c_str(), 1);

        execl(_PATH_BSHELL, _PATH_BSHELL, "-c", command.data(), nullptr);
        /* not reached */
    }

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
    using namespace std::literals;

    SnStartupSequence *snsequence;

    snsequence = sn_monitor_event_get_startup_sequence(event);

    /* Get the corresponding internal startup sequence */
    const char *id = sn_startup_sequence_get_id(snsequence);
    auto seq_ptr = std::ranges::find_if(startup_sequences, [&id](auto &current) {
        return strcmp(current->id.c_str(), id) == 0;
    });

    if (seq_ptr == startup_sequences.end()) {
        DLOG(fmt::format("Got event for startup sequence that we did not initiate (ID = {}). Ignoring.", id));
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
            auto current_time = std::chrono::system_clock::now();
            sequence->delete_at = current_time + 30s;
            DLOG(fmt::format("Will delete startup sequence {}",
                 sequence->id));

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
        if (strcmp(current->workspace.c_str(), old_name) != 0) {
            continue;
        }
        DLOG(fmt::format("Renaming workspace \"{}\" to \"{}\" in startup sequence {}.\n",
                old_name, new_name, current->id));
        current->workspace = new_name;
    }
}

/*
 * Gets the stored startup sequence for the _NET_STARTUP_ID of a given window.
 *
 */
static Startup_Sequence* startup_sequence_get(i3Window *cwindow,
                                              xcb_get_property_reply_t* startup_id_reply, bool ignore_mapped_leader) {
    /* The _NET_STARTUP_ID is only needed during this function, so we get it
     * here and don’t save it in the 'cwindow'. */
    if (startup_id_reply == nullptr || xcb_get_property_value_length(startup_id_reply) == 0) {
        DLOG(fmt::sprintf("No _NET_STARTUP_ID set on window 0x%08x\n",  cwindow->id));
        if (cwindow->leader == XCB_NONE) {
            return nullptr;
        }

        /* This is a special case that causes the leader's startup sequence
         * to only be returned if it has never been mapped, useful primarily
         * when trying to delete a sequence.
         *
         * It's generally inappropriate to delete a leader's sequence when
         * moving a child window, but if the leader has no container, it's
         * likely permanently unmapped and the child is the "real" window. */
        if (ignore_mapped_leader && con_by_window_id(cwindow->leader) != nullptr) {
            DLOG(fmt::sprintf("Ignoring leader window 0x%08x\n",  cwindow->leader));
            return nullptr;
        }

        DLOG(fmt::sprintf("Checking leader window 0x%08x\n",  cwindow->leader));

        auto property = global.x->conn->get_property_unchecked(false, cwindow->leader,
                                  i3::atoms[i3::Atom::_NET_STARTUP_ID], XCB_GET_PROPERTY_TYPE_ANY, 0, 512);
        startup_id_reply = property.get().get();

        if (startup_id_reply == nullptr ||
            xcb_get_property_value_length(startup_id_reply) == 0) {
            DLOG("No _NET_STARTUP_ID set on the leader either\n");
            return nullptr;
        }
    }

    auto startup_id = fmt::sprintf("%.*s", xcb_get_property_value_length(startup_id_reply),
              (char *)xcb_get_property_value(startup_id_reply));

    auto seq_ptr = std::ranges::find_if(startup_sequences, [&startup_id](auto &current) {
        return current->id == startup_id;
    });

    if (seq_ptr == startup_sequences.end()) {
        DLOG(fmt::sprintf("WARNING: This sequence (ID %s) was not found\n",  startup_id));
        return nullptr;
    }

    return seq_ptr->get();
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
std::optional<std::string> startup_workspace_for_window(i3Window *cwindow, xcb_get_property_reply_t *startup_id_reply) {
    auto *seq = startup_sequence_get(cwindow, startup_id_reply, false);
    if (seq == nullptr) {
        return std::nullopt;
    }

    /* If the startup sequence's time span has elapsed, delete it. */
    auto current_time = std::chrono::system_clock::now();
    if (seq->delete_at && current_time > *seq->delete_at) {
        DLOG(fmt::format("Deleting expired startup sequence {}",  seq->id));
        remove_startup_sequence(seq->id);

        return std::nullopt;
    }

    return seq->workspace;
}

/*
 * Deletes the startup sequence for a window if it exists.
 *
 */
void startup_sequence_delete_by_window(i3Window *win) {

    auto startup_id_reply = global.x->conn->get_property(false, win->id, i3::atoms[i3::Atom::_NET_STARTUP_ID], XCB_GET_PROPERTY_TYPE_ANY, 0, 512);

    auto *sequence = startup_sequence_get(win, (startup_id_reply.get() != nullptr) ? startup_id_reply.get().get() : nullptr, true);
    if (sequence != nullptr) {
        remove_startup_sequence(sequence->id);
    }
}
