add_custom_target(Config ALL
        COMMAND ${PERL_EXECUTABLE}
        ${PROJECT_SOURCE_DIR}/generate-command-parser.pl
        --input=${PROJECT_SOURCE_DIR}/parser-specs/config.spec
        --prefix=config
        COMMENT "Generating config"
        BYPRODUCTS
        ${CMAKE_CURRENT_BINARY_DIR}/GENERATED_config_enums.cppm
        ${CMAKE_CURRENT_BINARY_DIR}/GENERATED_config_tokens.cppm
        ${CMAKE_CURRENT_BINARY_DIR}/GENERATED_config_call.cppm)

add_library(i3_config_old STATIC)
target_sources(i3_config_old
        PUBLIC
        config_directives.cpp
        config_parser.cpp
        parser_stack.cpp
        parser_ctx.cpp
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES
        BASE_DIRS ${CMAKE_SOURCE_DIR}/i3/src/config/old ${CMAKE_BINARY_DIR}/i3/src/config/old
        FILES
        ${CMAKE_CURRENT_BINARY_DIR}/GENERATED_config_enums.cppm
        ${CMAKE_CURRENT_BINARY_DIR}/GENERATED_config_tokens.cppm
        ${CMAKE_CURRENT_BINARY_DIR}/GENERATED_config_call.cppm
        cmdp_token.cppm
        parser_ctx.cppm
        config_directives.cppm
        config_parser.cppm
        parser_stack.cppm
        i3_config_old.cppm)
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/GENERATED_config_enums.cppm PROPERTIES GENERATED TRUE)
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/GENERATED_config_tokens.cppm PROPERTIES GENERATED TRUE)
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/GENERATED_config_call.cppm PROPERTIES GENERATED TRUE)
target_link_libraries(i3_config_old PUBLIC ${libraries} utils i3_config_base i3_log)
target_include_directories(i3_config_old PUBLIC ${include_dirs} "${CMAKE_CURRENT_SOURCE_DIR}")
add_dependencies(i3_config_old Config)
