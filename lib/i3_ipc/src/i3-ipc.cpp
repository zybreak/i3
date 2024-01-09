module;
#include <ev.h>
#include <string_view>
#include <string>
#include <vector>
module i3ipc;

/*
 * ipc_new_client_on_fd() only sets up the event handler
 * for activity on the new connection and inserts the file descriptor into
 * the list of clients.
 *
 * This variant is useful for the inherited IPC connection when restarting.
 *
 */

;
ipc_client::ipc_client(ev_loop *loop, int fd, void (*read_callback_t)(ev_loop *loop, ev_io *w, int revents), void (*write_callback_t)(ev_loop *loop, ev_io *w, int revents)) : loop(loop) {
    this->fd = fd;

    read_callback = new ev_io{};
    read_callback->data = this;
    ev_io_init(read_callback, read_callback_t, fd, EV_READ);
    ev_io_start(loop, read_callback);

    write_callback = new ev_io{};
    write_callback->data = this;
    ev_io_init(write_callback, write_callback_t, fd, EV_WRITE);
}

ipc_client::~ipc_client() {
    ev_io_stop(loop, read_callback);
    delete read_callback;
    ev_io_stop(loop, write_callback);
    delete write_callback;
    if (timeout) {
        ev_timer_stop(loop, timeout);
        delete timeout;
    }

    free(buffer);
}
