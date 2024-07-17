add_library(i3_config_base STATIC)
target_sources(i3_config_base INTERFACE
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES
        FILES
        config_result_ir.cppm
        base_config_applier.cppm
        base_parser.cppm
        base_resource_database.cppm
        i3_config_base.cppm
)
target_include_directories(i3_config_base INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
#target_compile_options(i3_config_base PUBLIC "-pthread" "-fsized-deallocation")
target_compile_options(i3_config_base PUBLIC "-Xclang" "-fno-validate-pch")
