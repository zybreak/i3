add_library(libi3ipc STATIC)
target_sources(libi3ipc PUBLIC
        src/ipc_connect.cpp src/ipc_recv_message.cpp src/ipc_send_message.cpp src/i3-ipc.cpp
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES
        BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
        FILES src/i3-ipc.cppm)
#target_compile_options(libi3ipc PUBLIC "-pthread" "-fsized-deallocation")
target_compile_options(libi3ipc PUBLIC "-Xclang" "-fno-validate-pch")

find_package(libev CONFIG REQUIRED)
target_link_libraries(libi3ipc PUBLIC libev::libev)