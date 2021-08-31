/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * log.c: Logging functions.
 *
 */
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include <xcb/xcb.h>

#include "libi3.h"
#include "data.h"
#include "log.h"

#include <ev.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <sys/time.h>
#include <unistd.h>

static bool debug_logging = false;
static bool verbose = false;
static FILE *errorfile;
char *errorfilename;

typedef struct log_client {
    int fd;

    TAILQ_ENTRY(log_client)
    clients;
} log_client;

TAILQ_HEAD(log_client_head, log_client)
log_clients = TAILQ_HEAD_INITIALIZER(log_clients);

/*
 * Initializes logging by creating an error logfile in /tmp (or
 * XDG_RUNTIME_DIR, see get_process_filename()).
 *
 * Will be called twice if --shmlog-size is specified.
 *
 */
void init_logging(void) {
    if (!errorfilename) {
        if (!(errorfilename = get_process_filename("errorlog")))
            fprintf(stderr, "Could not initialize errorlog\n");
        else {
            errorfile = fopen(errorfilename, "w");
            if (!errorfile) {
                fprintf(stderr, "Could not initialize errorlog on %s: %s\n",
                        errorfilename, strerror(errno));
            } else {
                if (fcntl(fileno(errorfile), F_SETFD, FD_CLOEXEC)) {
                    fprintf(stderr, "Could not set close-on-exec flag\n");
                }
            }
        }
    }
    atexit(purge_zerobyte_logfile);
}

/*
 * Set verbosity of i3. If verbose is set to true, informative messages will
 * be printed to stdout. If verbose is set to false, only errors will be
 * printed.
 *
 */
void set_verbosity(bool _verbose) {
    verbose = _verbose;
}

/*
 * Get debug logging.
 *
 */
bool get_debug_logging(void) {
    return debug_logging;
}

/*
 * Set debug logging.
 *
 */
void set_debug_logging(const bool _debug_logging) {
    debug_logging = _debug_logging;
}

/*
 * Logs the given message to stdout (if print is true) while prefixing the
 * current time to it. Additionally, the message will be saved in the i3 SHM
 * log if enabled.
 * This is to be called by *LOG() which includes filename/linenumber/function.
 *
 */
static void vlog(const bool print, const char *fmt, va_list args) {

    if (!(print)) {
        return;
    }

    /* Precisely one page to not consume too much memory but to hold enough
     * data to be useful. */
    static char message[4096];
    static struct tm result;
    static time_t t;
    static struct tm *tmp;

    /* Get current time */
    t = time(NULL);
    /* Convert time to local time (determined by the locale) */
    tmp = localtime_r(&t, &result);
    /* Generate time prefix */
    strftime(message, sizeof(message), "%x %X - ", tmp);

    /*
     * logbuffer  print
     * ----------------
     *  true      true   format message, save, print
     *  true      false  format message, save
     *  false     true   print message only
     *  false     false  INVALID, never called
     */
    printf("%s", message);
    vprintf(fmt, args);
}

/*
 * Logs the given message to stdout while prefixing the current time to it,
 * but only if verbose mode is activated.
 *
 */
void verboselog(char *fmt, ...) {
    va_list args;

    if (!verbose)
        return;

    va_start(args, fmt);
    vlog(verbose, fmt, args);
    va_end(args);
}

/*
 * Logs the given message to stdout while prefixing the current time to it.
 *
 */
void errorlog(char *fmt, ...) {
    va_list args;

    va_start(args, fmt);
    vlog(true, fmt, args);
    va_end(args);

    /* also log to the error logfile, if opened */
    va_start(args, fmt);
    vfprintf(errorfile, fmt, args);
    fflush(errorfile);
    va_end(args);
}

/*
 * Logs the given message to stdout while prefixing the current time to it,
 * but only if debug logging was activated.
 * This is to be called by DLOG() which includes filename/linenumber
 *
 */
void debuglog(char *fmt, ...) {
    va_list args;

    if (!(debug_logging))
        return;

    va_start(args, fmt);
    vlog(debug_logging, fmt, args);
    va_end(args);
}

/*
 * Deletes the unused log files. Useful if i3 exits immediately, eg.
 * because --get-socketpath was called. We don't care for syscall
 * failures. This function is invoked automatically when exiting.
 */
void purge_zerobyte_logfile(void) {
    struct stat st;
    char *slash;

    if (!errorfilename)
        return;

    /* don't delete the log file if it contains something */
    if ((stat(errorfilename, &st)) == -1 || st.st_size > 0)
        return;

    if (unlink(errorfilename) == -1)
        return;

    if ((slash = strrchr(errorfilename, '/')) != NULL) {
        *slash = '\0';
        /* possibly fails with ENOTEMPTY if there are files (or
         * sockets) left. */
        rmdir(errorfilename);
    }
}

char *current_log_stream_socket_path = NULL;

/*
 * Handler for activity on the listening socket, meaning that a new client
 * has just connected and we should accept() them. Sets up the event handler
 * for activity on the new connection and inserts the file descriptor into
 * the list of log clients.
 *
 */
void log_new_client(EV_P_ struct ev_io *w, int revents) {
    struct sockaddr_un peer;
    socklen_t len = sizeof(struct sockaddr_un);
    int fd;
    if ((fd = accept(w->fd, (struct sockaddr *)&peer, &len)) < 0) {
        if (errno != EINTR) {
            perror("accept()");
        }
        return;
    }

    /* Close this file descriptor on exec() */
    (void)fcntl(fd, F_SETFD, FD_CLOEXEC);

    set_nonblock(fd);

    log_client *client = scalloc(1, sizeof(log_client));
    client->fd = fd;
    TAILQ_INSERT_TAIL(&log_clients, client, clients);

    DLOG("log: new client connected on fd %d\n", fd);
}
