add_library(libi3ipc STATIC
  src/ipc_connect.cpp
  src/ipc_recv_message.cpp
  src/ipc_send_message.cpp)

target_include_directories(libi3ipc PUBLIC
        "${PROJECT_BINARY_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/include")