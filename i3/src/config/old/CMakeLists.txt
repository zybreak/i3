add_custom_target(Config ALL
        COMMAND ${PERL_EXECUTABLE}
        ${PROJECT_SOURCE_DIR}/generate-command-parser.pl
        --input=${PROJECT_SOURCE_DIR}/parser-specs/config.spec
        --prefix=config
        COMMENT "Generating config"
        BYPRODUCTS
        ${CMAKE_CURRENT_BINARY_DIR}/GENERATED_config_enums.ixx
        ${CMAKE_CURRENT_BINARY_DIR}/GENERATED_config_tokens.ixx
        ${CMAKE_CURRENT_BINARY_DIR}/GENERATED_config_call.ixx)

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
        ${CMAKE_CURRENT_BINARY_DIR}/GENERATED_config_enums.ixx
        ${CMAKE_CURRENT_BINARY_DIR}/GENERATED_config_tokens.ixx
        ${CMAKE_CURRENT_BINARY_DIR}/GENERATED_config_call.ixx
        cmdp_token.ixx
        parser_ctx.ixx
        config_directives.ixx
        config_parser.ixx
        parser_stack.ixx
        i3_config_old.ixx)
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/GENERATED_config_enums.ixx PROPERTIES GENERATED TRUE)
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/GENERATED_config_tokens.ixx PROPERTIES GENERATED TRUE)
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/GENERATED_config_call.ixx PROPERTIES GENERATED TRUE)
target_link_libraries(i3_config_old PUBLIC ${libraries} utils i3_config_base i3_log)
target_include_directories(i3_config_old PUBLIC ${include_dirs} "${CMAKE_CURRENT_SOURCE_DIR}")
add_dependencies(i3_config_old Config)
