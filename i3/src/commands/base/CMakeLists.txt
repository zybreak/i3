add_library(i3_commands_base STATIC)
target_sources(i3_commands_base INTERFACE
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES
        FILES
        i3_commands_base.cppm
)
target_link_libraries(i3_commands_base utils i3_json)
target_include_directories(i3_commands_base INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
#target_compile_options(i3_commands_base PUBLIC "-pthread" "-fsized-deallocation")
target_compile_options(i3_commands_base PUBLIC "-Xclang" "-fno-validate-pch")
target_compile_features(i3_commands_base PUBLIC cxx_std_26)
