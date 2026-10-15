add_library(utils STATIC)
target_sources(utils PRIVATE
        resolve_tilde.cpp
        parse_string.cpp
        safewrappers.cpp
        boolstr.cpp
        get_process_filename.cpp
        get_colorpixel.cpp
        mkdirp.cpp
        nonblock.cpp
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES
        FILES utils.cppm criteria_state.cppm
        format_placeholders.cppm)
#target_compile_options(utils PUBLIC "-pthread" "-fsized-deallocation")
target_compile_options(utils PUBLIC "-Xclang" "-fno-validate-pch")
target_link_libraries(utils PUBLIC i3_log)
target_compile_features(utils PUBLIC cxx_std_26)
