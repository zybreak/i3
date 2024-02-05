add_library(i3_config_old STATIC)
target_sources(i3_config_old
        PUBLIC
        config_directives.cpp
        config_parser.cpp
        parser_stack.cpp
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES
        FILES
        config_result.ixx
        parser_ctx.ixx
        config_directives.ixx
        config_parser.ixx
        parser_stack.ixx
        i3_config_old.ixx
)
target_link_libraries(i3_config_old PUBLIC ${libraries} utils i3_config_base i3_log)
target_include_directories(i3_config_old PUBLIC ${include_dirs} "${CMAKE_CURRENT_SOURCE_DIR}")
add_dependencies(i3_config_old Config)
