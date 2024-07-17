add_library(i3_commands_base STATIC)
target_sources(i3_commands_base INTERFACE
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES
        FILES
        base_commands_applier.cppm
        i3_commands_base.cppm
        command_result_ir.cppm
        command_result.cppm
)
target_include_directories(i3_commands_base INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
#target_compile_options(i3_commands_base PUBLIC "-pthread" "-fsized-deallocation")
target_compile_options(i3_commands_base PUBLIC "-Xclang" "-fno-validate-pch")
