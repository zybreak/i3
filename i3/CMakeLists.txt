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
        src/assignments.cpp
        src/atoms.cpp
        src/autostarts.cpp
        src/bindings.cpp
        src/click.cpp
        src/commands/commands_parser.cpp
        src/commands/commands_applier.cpp
        src/con.cpp
        src/configuration.cpp
        src/config/config_applier.cpp
        src/drag.cpp
        src/ewmh.cpp
        src/floating.cpp
        src/font.cpp
        src/format_placeholders.cpp
        src/property_handlers.cpp
        src/ipc.cpp
        src/is_background_set.cpp
        src/load_layout.cpp
        src/nagbar.cpp
        src/manage.cpp
        src/match.cpp
        src/move.cpp
        src/output.cpp
        src/program_arguments.cpp
        src/randr.cpp
        src/rect.cpp
        src/regex.cpp
        src/render.cpp
        src/resize.cpp
        src/restore_layout.cpp
        src/sighandler.cpp
        src/startup.cpp
        src/string.cpp
        src/dpi.cpp
        src/draw_util.cpp
        src/log.cpp
        src/root_atom_contents.cpp
        src/sync.cpp
        #src/tree.cpp
        src/util.cpp
        src/window.cpp
        src/workspace.cpp
        src/xcb.cpp
        src/x.cpp
        src/keysyms.cpp
        src/event_handler.cpp
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
        FILES
        src/i3-lib.ixx
        src/include/assignments.ixx
        src/include/autostarts.ixx
        src/include/bindings.ixx
        src/include/click.ixx
        src/include/configuration.ixx
        src/include/con.ixx
        src/include/util.ixx
        src/include/x.ixx
        src/include/workspace.ixx
        src/include/keysyms.ixx
        src/include/event_handler.ixx
        src/include/shape.ixx
        src/include/xkb.ixx
        src/include/handlers.ixx
        src/include/criteria_state.ixx
        src/include/resize.ixx
        src/include/window.ixx
        src/include/match.ixx
        #src/include/tree.ixx
        src/include/startup.ixx
        src/include/draw.ixx
        src/include/sync.ixx
        src/include/output.ixx
        src/include/rect.ixx
        src/include/render.ixx
        src/include/randr.ixx
        src/include/font.ixx
        src/include/format_placeholders.ixx
        src/include/xcb.ixx
        src/include/drag.ixx
        src/include/dpi.ixx
        src/include/get_visualtype.ixx
        src/include/ewmh.ixx
        src/include/floating.ixx
        src/include/move.ixx
        src/include/sighandler.ixx
        src/include/ipc.ixx
        src/include/load_layout.ixx
        src/include/restore_layout.ixx
        src/include/program_arguments.ixx
        src/include/root_atom_contents.ixx
        src/include/nagbar.ixx
        src/include/manage.ixx
        src/include/i3string.ixx
        src/include/log.ixx
        src/include/i3_regex.ixx
        src/include/internal.ixx
        )
target_link_libraries(i3_lib PUBLIC ${libraries} i3_commands_old utils std)
target_include_directories(i3_lib PUBLIC ${include_dirs} src/commands src/config/old)
target_compile_options(i3_lib PUBLIC ${compile_options})
add_dependencies(i3_lib libi3ipc)

add_executable(i3
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
target_link_libraries(i3 PUBLIC i3_lib libi3ipc i3_config_new i3_config_old)
target_include_directories(i3 PUBLIC ${include_dirs} "${CMAKE_CURRENT_SOURCE_DIR}/src/config/old")

add_subdirectory(test)