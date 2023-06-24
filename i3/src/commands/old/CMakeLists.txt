# COMMANDS OLD

add_library(i3_commands_old STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../../boolstr.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../commands.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/parser_stack.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/commands_parser.cpp
        )
target_link_libraries(i3_commands_old PUBLIC ${libraries})
target_include_directories(i3_commands_old PUBLIC ${include_dirs})
add_dependencies(i3_commands_old Commands)
