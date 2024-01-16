add_library(i3_config_old STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/parser_stack.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/config_directives.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/config_parser.cpp
        )
target_link_libraries(i3_config_old PUBLIC ${libraries} utils i3_config_base)
target_include_directories(i3_config_old PUBLIC ${include_dirs} "${CMAKE_CURRENT_SOURCE_DIR}")
add_dependencies(i3_config_old Config)
