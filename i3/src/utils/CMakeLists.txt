add_library(utils STATIC)
target_sources(utils PUBLIC
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
        FILES utils.ixx
        format_placeholders.ixx)
#target_compile_options(utils PUBLIC "-pthread" "-fsized-deallocation")
target_compile_options(utils PUBLIC "-Xclang" "-fno-validate-pch")
