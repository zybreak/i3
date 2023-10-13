find_package(PkgConfig REQUIRED)
include(CheckLibraryExists)

check_library_exists(m pow "" LIBM)
if(LIBM)
    list(APPEND libraries m)
endif()

pkg_check_modules(libsn_dep REQUIRED libstartup-notification-1.0)
list(APPEND libraries ${libsn_dep_LINK_LIBRARIES})
list(APPEND include_dirs ${libsn_dep_INCLUDE_DIRS})
list(APPEND compile_options ${libsn_dep_CFLAGS_OTHER})

find_package(libev)
list(APPEND libraries libev::libev)

find_package(X11 REQUIRED COMPONENTS xcb xcb_cursor xcb_shape xcb_xrm xcb_xkb xcb_randr xcb_util xcb_keysyms xcb_icccm xkbcommon xkbcommon_X11)
list(APPEND libraries X11 X11::xcb X11::xcb_cursor X11::xcb_shape X11::xcb_xrm X11::xcb_xkb X11::xcb_randr X11::xcb_util X11::xcb_keysyms X11::xcb_icccm X11::xkbcommon X11::xkbcommon_X11)

find_package(PCRE CONFIG)
list(APPEND libraries pcre2::pcre2)

pkg_check_modules(cairo_dep REQUIRED cairo>=1.14.4)
list(APPEND libraries ${cairo_dep_LINK_LIBRARIES})
list(APPEND include_dirs ${cairo_dep_INCLUDE_DIRS})
list(APPEND compile_options ${cairo_dep_CFLAGS_OTHER})

pkg_check_modules(pangocairo_dep REQUIRED pangocairo)
list(APPEND libraries ${pangocairo_dep_LINK_LIBRARIES})
list(APPEND include_dirs ${pangocairo_dep_INCLUDE_DIRS})
list(APPEND compile_options ${pangocairo_dep_CFLAGS_OTHER})

find_package(glib CONFIG COMPONENTS gobject-2.0)
list(APPEND libraries glib::glib)
list(APPEND libraries glib::gobject-2.0)

find_package(fmt CONFIG REQUIRED)
list(APPEND libraries fmt::fmt)

find_package(nlohmann_json 3.11.2 REQUIRED)
list(APPEND libraries nlohmann_json::nlohmann_json)

list(APPEND libraries libi3ipc)

list(APPEND include_dirs
        "${PROJECT_SOURCE_DIR}"
        "${PROJECT_SOURCE_DIR}/lib"
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/commands"
        )

list(APPEND i3_src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/assignments.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/atoms.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/autostarts.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bindings.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/click.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/commands/commands_parser.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/commands/commands_applier.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/con.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/configuration.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/config/config_applier.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/drag.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ewmh.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/floating.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/font.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/format_placeholders.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/global.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/property_handlers.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ipc.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/is_background_set.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/load_layout.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/nagbar.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/manage.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/match.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/move.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/nonblock.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/output.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/program_arguments.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/randr.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/rect.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/regex.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/render.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/resize.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/restore_layout.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/sighandler.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/startup.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/string.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/dpi.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/draw_util.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/get_process_filename.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/get_visualtype.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/log.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/root_atom_contents.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/mkdirp.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/sync.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/tree.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/util.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/window.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/workspace.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/xcb.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/x.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/keysyms.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/event_handler.cpp
        )


#add_subdirectory(src/commands/new)
add_subdirectory(src/commands/old)
add_subdirectory(src/config/new)
add_subdirectory(src/config/old)
add_subdirectory(src/utils)

add_library(i3_lib STATIC)
target_sources(i3_lib PUBLIC
        ${i3_src}
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES
        FILES src/i3-lib.ixx
        )
target_link_libraries(i3_lib PUBLIC ${libraries} i3_commands_old utils)
target_include_directories(i3_lib PUBLIC ${include_dirs} src/commands src/config/old)
target_compile_options(i3_lib PUBLIC ${compile_options})
add_dependencies(i3_lib libi3ipc)

add_executable(i3
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
target_link_libraries(i3 PUBLIC i3_lib libi3ipc i3_config_new i3_config_old)
target_include_directories(i3 PUBLIC ${include_dirs} "${CMAKE_CURRENT_SOURCE_DIR}/src/config/old")

add_subdirectory(test)