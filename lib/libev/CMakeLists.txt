cmake_minimum_required(VERSION 3.28.0)

project(libev VERSION 4.33 LANGUAGES C)

include(CheckIncludeFile)
include(CheckFunctionExists)

check_include_file(sys/inotify.h HAVE_SYS_INOTIFY_H)
check_include_file(sys/epoll.h HAVE_SYS_EPOLL_H)
check_include_file(sys/event.h HAVE_SYS_EVENT_H)
check_include_file(port.h HAVE_PORT_H)
check_include_file(poll.h HAVE_POLL_H)
check_include_file(sys/timerfd.h HAVE_SYS_TIMERFD_H)

check_include_file(sys/select.h HAVE_SYS_SELECT_H)
check_include_file(sys/eventfd.h HAVE_SYS_EVENTFD_H)
check_include_file(sys/signalfd.h HAVE_SYS_SIGNALFD_H)
check_include_file(linux/aio_abi.h HAVE_LINUX_AIO_ABI_H)
check_include_file(linux/fs.h HAVE_LINUX_FS_H)

check_function_exists(inotify_init HAVE_INOTIFY_INIT)
check_function_exists(epoll_ctl HAVE_EPOLL_CTL)
check_function_exists(kqueue HAVE_KQUEUE)
check_function_exists(port_create HAVE_PORT_CREATE)
check_function_exists(poll HAVE_POLL)
check_function_exists(select HAVE_SELECT)
check_function_exists(eventfd HAVE_EVENTFD)
check_function_exists(signalfd HAVE_SIGNALFD)

configure_file(config.h.in config.h)

add_library(libev STATIC)
target_sources(libev PUBLIC
    ev.c
    event.c
    #ev_epoll.c
    #ev_iouring.c
    #ev_kqueue.c
    #ev_linuxaio.c
    #ev_poll.c
    #ev_port.c
    #ev_select.c
    #ev_win32.c
    INTERFACE
    ev.h
    ev++.h
    event.h
)

target_include_directories(libev PUBLIC
        "${CMAKE_CURRENT_BINARY_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}")

