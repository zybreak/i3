add_library(i3_commands_base STATIC)
target_sources(i3_commands_base INTERFACE
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES
        FILES
        base_commands_applier.ixx
        i3_commands_base.ixx
        command_result_ir.ixx
        command_result.ixx
)
target_include_directories(i3_commands_base INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_options(i3_commands_base PUBLIC "-pthread" "-fsized-deallocation")
