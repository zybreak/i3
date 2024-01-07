cmake_minimum_required(VERSION 3.28.0)

project(libev VERSION 4.33 LANGUAGES CXX)

include(CheckIncludeFileCXX)
include(CheckFunctionExists)
include(CheckCXXSymbolExists)
include(CheckCXXSourceCompiles)

check_include_file_cxx(sys/inotify.h HAVE_SYS_INOTIFY_H)
check_include_file_cxx(sys/epoll.h HAVE_SYS_EPOLL_H)
check_include_file_cxx(sys/event.h HAVE_SYS_EVENT_H)
check_include_file_cxx(port.h HAVE_PORT_H)
check_include_file_cxx(poll.h HAVE_POLL_H)
check_include_file_cxx(sys/timerfd.h HAVE_SYS_TIMERFD_H)

check_include_file_cxx(sys/select.h HAVE_SYS_SELECT_H)
check_include_file_cxx(sys/eventfd.h HAVE_SYS_EVENTFD_H)
check_include_file_cxx(sys/signalfd.h HAVE_SYS_SIGNALFD_H)
check_include_file_cxx(linux/aio_abi.h HAVE_LINUX_AIO_ABI_H)
check_include_file_cxx(linux/fs.h HAVE_LINUX_FS_H)

check_function_exists(inotify_init HAVE_INOTIFY_INIT)
check_function_exists(epoll_ctl HAVE_EPOLL_CTL)
check_function_exists(kqueue HAVE_KQUEUE)
check_function_exists(port_create HAVE_PORT_CREATE)
check_function_exists(poll HAVE_POLL)
check_function_exists(select HAVE_SELECT)
check_function_exists(eventfd HAVE_EVENTFD)
check_function_exists(signalfd HAVE_SIGNALFD)

check_function_exists(floor HAVE_FLOOR)
check_function_exists(nanosleep HAVE_NANOSLEEP)
check_function_exists(clock_gettime HAVE_CLOCK_GETTIME)

check_cxx_source_compiles("
#include <unistd.h>
#include <sys/syscall.h>
#include <time.h>
int main() {
    struct timespec ts;
    int status = syscall (SYS_clock_gettime, CLOCK_REALTIME, &ts);
    return 0;
}
" HAVE_CLOCK_SYSCALL)

check_cxx_source_compiles("
#include <linux/fs.h>

int main() {
    __kernel_rwf_t x;
    return 0;
}
" HAVE_KERNEL_RWF_T)


configure_file(config.h.in ev_config.h)

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

add_compile_definitions(EV_MULTIPLICITY=1 EV_COMPAT3=0)