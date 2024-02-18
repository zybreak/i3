# COMMANDS OLD

add_library(i3_commands_old STATIC)
target_sources(i3_commands_old PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/commands.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/commands_parser.cpp
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES
        FILES
        commands_parser.ixx
        commands.ixx
        i3_commands_old.ixx
        parser_stack.ixx
        )
target_link_libraries(i3_commands_old PUBLIC nlohmann_json::nlohmann_json utils fmt::fmt libi3ipc i3_commands_base)
target_include_directories(i3_commands_old PUBLIC "${PROJECT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
add_dependencies(i3_commands_old Commands)
