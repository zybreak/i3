cmake_minimum_required(VERSION 3.28.0)

project(libev VERSION 4.33 LANGUAGES CXX)

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

#AC_CHECK_FUNCS(clock_gettime, [], [
#        dnl on linux, try syscall wrapper first
#        if test $(uname) = Linux; then
#        AC_MSG_CHECKING(for clock_gettime syscall)
#        AC_LINK_IFELSE([AC_LANG_PROGRAM(
#        [#include <unistd.h>
#        #include <sys/syscall.h>
#        #include <time.h>],
#        [struct timespec ts; int status = syscall (SYS_clock_gettime, CLOCK_REALTIME, &ts)])],
#        [ac_have_clock_syscall=1
#        AC_DEFINE(HAVE_CLOCK_SYSCALL, 1, Define to 1 to use the syscall interface for clock_gettime)
#        AC_MSG_RESULT(yes)],
#        [AC_MSG_RESULT(no)])
#        fi
#        if test -z "$LIBEV_M4_AVOID_LIBRT" && test -z "$ac_have_clock_syscall"; then
#        AC_CHECK_LIB(rt, clock_gettime)
#        unset ac_cv_func_clock_gettime
#        AC_CHECK_FUNCS(clock_gettime)
#        fi
#        ])
#
#AC_CHECK_FUNCS(nanosleep, [], [
#        if test -z "$LIBEV_M4_AVOID_LIBRT"; then
#        AC_CHECK_LIB(rt, nanosleep)
#        unset ac_cv_func_nanosleep
#        AC_CHECK_FUNCS(nanosleep)
#        fi
#        ])
#
#AC_CHECK_TYPE(__kernel_rwf_t, [
#        AC_DEFINE(HAVE_KERNEL_RWF_T, 1, Define to 1 if linux/fs.h defined kernel_rwf_t)
#        ], [], [#include <linux/fs.h>])
#
#        if test -z "$LIBEV_M4_AVOID_LIBM"; then
#        LIBM=m
#        fi
#        AC_SEARCH_LIBS(floor, $LIBM, [AC_DEFINE(HAVE_FLOOR, 1, Define to 1 if the floor function is available)])
#

configure_file(config.h.in config.h)

add_library(libev STATIC)
target_sources(libev PUBLIC
    ev.cpp
    INTERFACE
    ev.h
    ev++.h
)

target_include_directories(libev PUBLIC
        "${CMAKE_CURRENT_BINARY_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}")

