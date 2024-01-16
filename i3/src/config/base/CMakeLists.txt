add_library(i3_config_base INTERFACE)
target_sources(i3_config_base INTERFACE
        base_config_applier.h
        base_resource_database.h
        base_parser.h
)
target_include_directories(i3_config_base INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
