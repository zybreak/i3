add_library(libi3ipc STATIC)
target_sources(libi3ipc PUBLIC
        src/ipc_connect.cpp src/ipc_recv_message.cpp src/ipc_send_message.cpp src/i3-ipc.cpp
        INTERFACE
        i3_ipc/i3-ipc.h
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES
        BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
        FILES src/i3-ipc.ixx)
target_compile_options(libi3ipc PUBLIC "-pthread" "-fsized-deallocation")

target_include_directories(libi3ipc PUBLIC
        "${PROJECT_BINARY_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}")

find_package(libev)
target_link_libraries(libi3ipc PUBLIC libev::libev)