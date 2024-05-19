add_library(i3_config_base STATIC)
target_sources(i3_config_base INTERFACE
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES
        FILES
        base_config_applier.ixx
        base_parser.ixx
        base_resource_database.ixx
        i3_config_base.ixx
)
target_include_directories(i3_config_base INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
#target_compile_options(i3_config_base PUBLIC "-pthread" "-fsized-deallocation")
target_compile_options(i3_config_base PUBLIC "-Xclang" "-fno-validate-pch")
