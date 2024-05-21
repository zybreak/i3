# COMMANDS OLD

add_custom_target(Commands ALL
        COMMAND ${PERL_EXECUTABLE}
        ${PROJECT_SOURCE_DIR}/generate-command-parser.pl
        --input=${PROJECT_SOURCE_DIR}/parser-specs/commands.spec
        --prefix=commands
        COMMENT "Generating commands"
        BYPRODUCTS
        ${CMAKE_CURRENT_BINARY_DIR}/GENERATED_commands_enums.ixx
        ${CMAKE_CURRENT_BINARY_DIR}/GENERATED_commands_tokens.ixx
        ${CMAKE_CURRENT_BINARY_DIR}/GENERATED_commands_call.ixx)

add_library(i3_commands_old STATIC)
target_sources(i3_commands_old PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/commands.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/commands_parser.cpp
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES
        BASE_DIRS ${CMAKE_SOURCE_DIR}/i3/src/commands/old ${CMAKE_BINARY_DIR}/i3/src/commands/old
        FILES
        ${CMAKE_CURRENT_BINARY_DIR}/GENERATED_commands_enums.ixx
        ${CMAKE_CURRENT_BINARY_DIR}/GENERATED_commands_tokens.ixx
        ${CMAKE_CURRENT_BINARY_DIR}/GENERATED_commands_call.ixx
        cmdp_token.ixx
        commands_parser.ixx
        commands.ixx
        i3_commands_old.ixx
        parser_stack.ixx
        )
target_link_libraries(i3_commands_old PUBLIC nlohmann_json::nlohmann_json utils fmt::fmt libi3ipc i3_commands_base)
target_include_directories(i3_commands_old PUBLIC "${PROJECT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
add_dependencies(i3_commands_old Commands)
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/GENERATED_commands_enums.ixx PROPERTIES GENERATED TRUE)
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/GENERATED_commands_tokens.ixx PROPERTIES GENERATED TRUE)
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/GENERATED_commands_call.ixx PROPERTIES GENERATED TRUE)
