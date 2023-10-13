# COMMANDS OLD

add_library(i3_commands_old STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/commands.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/parser_stack.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/commands_parser.cpp
        )
target_link_libraries(i3_commands_old PUBLIC nlohmann_json::nlohmann_json utils fmt::fmt)
target_include_directories(i3_commands_old PUBLIC "${PROJECT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
add_dependencies(i3_commands_old Commands)
