add_library(libi3ipc STATIC)
target_sources(libi3ipc PUBLIC
        src/ipc_connect.cpp src/ipc_recv_message.cpp src/ipc_send_message.cpp
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES
        BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
        FILES src/i3-ipc.cppm)
#target_compile_options(libi3ipc PUBLIC "-pthread" "-fsized-deallocation")
target_compile_options(libi3ipc PUBLIC "-Xclang" "-fno-validate-pch")