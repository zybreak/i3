add_library(i3_log STATIC)
target_sources(i3_log PUBLIC
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES
        FILES
        log.cppm
)
target_link_libraries(i3_log PUBLIC spdlog::spdlog)
#target_compile_options(i3_log PUBLIC "-pthread" "-fsized-deallocation")
#target_compile_options(i3_log PUBLIC "-Xclang" "-fno-validate-pch")
