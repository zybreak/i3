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

find_package(libev CONFIG REQUIRED)
list(APPEND libraries libev::libev)

find_package(X11 REQUIRED COMPONENTS xcb xcb_cursor xcb_shape xcb_xrm xcb_xkb xcb_randr xcb_util xcb_keysyms xcb_icccm xkbcommon xkbcommon_X11)
list(APPEND libraries X11 X11::xcb X11::xcb_cursor X11::xcb_shape X11::xcb_xrm X11::xcb_xkb X11::xcb_randr X11::xcb_util X11::xcb_keysyms X11::xcb_icccm X11::xkbcommon X11::xkbcommon_X11)

find_package(PCRE2 CONFIG REQUIRED)
list(APPEND libraries pcre2::pcre2)

find_package(cairo CONFIG REQUIRED)
list(APPEND libraries cairo::cairo)

find_package(pango CONFIG REQUIRED COMPONENTS pangocairo)
list(APPEND libraries pango::pango)
list(APPEND libraries pango::pangocairo)

find_package(glib CONFIG REQUIRED COMPONENTS gobject-2.0)
list(APPEND libraries glib::glib)
list(APPEND libraries glib::gobject-2.0)

find_package(fmt CONFIG REQUIRED)
list(APPEND libraries fmt::fmt)

find_package(spdlog CONFIG REQUIRED)

find_package(nlohmann_json CONFIG REQUIRED)
list(APPEND libraries nlohmann_json::nlohmann_json)

find_package(di CONFIG REQUIRED)
list(APPEND libraries di::di)

list(APPEND libraries libi3ipc)

list(APPEND include_dirs
        "${PROJECT_BINARY_DIR}"
        "${PROJECT_SOURCE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/commands"
        )

add_subdirectory(src/commands/base)
add_subdirectory(src/commands/new)
add_subdirectory(src/commands/old)
add_subdirectory(src/config/base)
add_subdirectory(src/config/new)
add_subdirectory(src/config/old)
add_subdirectory(src/utils)
add_subdirectory(src/log)

add_library(i3_lib STATIC)
target_sources(i3_lib PUBLIC
        src/autostarts.cpp
        src/assignments.cpp
        src/atoms.cpp
        src/bindings.cpp
        src/commands/commands_parser.cpp
        src/commands/commands_applier.cpp
        src/con/con.cpp
        src/con/con_functions.cpp
        src/con/outputcon.cpp
        src/con/rootcon.cpp
        src/con/concon.cpp
        src/con/floatingcon.cpp
        src/con/dockcon.cpp
        src/con/workspacecon.cpp
        src/config/configuration.cpp
        src/config/config_applier.cpp
        src/drag.cpp
        src/dump_node.cpp
        src/ewmh.cpp
        src/floating.cpp
        src/font.cpp
        src/gaps.cpp
        src/handlers/click.cpp
        src/handlers/client_message.cpp
        src/handlers/handlers.cpp
        src/handlers/notify.cpp
        src/handlers/handle_property.cpp
        src/ipc.cpp
        src/load_layout.cpp
        src/nagbar/nagbar.cpp
        src/manage.cpp
        src/match.cpp
        src/move.cpp
        src/output.cpp
        src/randr.cpp
        src/render.cpp
        src/resize.cpp
        src/restore_layout.cpp
        src/sighandler.cpp
        src/startup.cpp
        src/dpi.cpp
        src/draw.cpp
        src/handlers/tiling_drag.cpp
        src/handlers/resize.cpp
        src/handlers/floating.cpp
        src/tree.cpp
        src/util.cpp
        src/window.cpp
        src/workspace.cpp
        src/event_handler.cpp
        src/xkb.cpp
        src/xcb.cpp
        src/x.cpp
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES
        FILES
        src/i3-lib.cppm
        src/assignments.cppm
        src/autostarts.cppm
        src/atoms.cppm
        src/bindings.cppm
        src/config/config_applier.cppm
        src/config/configuration.cppm
        src/config/resource_database.cppm
        src/commands/commands.cppm
        src/commands/commands_applier.cppm
        src/handlers/handle_property.cppm
        src/con/con.cppm
        src/dump_node.cppm
        src/gaps.cppm
        src/util.cppm
        src/x.cppm
        src/workspace.cppm
        src/event_handler.cppm
        src/xkb.cppm
        src/handlers/handlers.cppm
        src/criteria_state.cppm
        src/resize.cppm
        src/window.cppm
        src/match.cppm
        src/tree.cppm
        src/startup.cppm
        src/draw.cppm
        src/sync.cppm
        src/output.cppm
        src/rect.cppm
        src/render.cppm
        src/randr.cppm
        src/font.cppm
        src/xcb.cppm
        src/drag.cppm
        src/dpi.cppm
        src/get_visualtype.cppm
        src/ewmh.cppm
        src/floating.cppm
        src/move.cppm
        src/sighandler.cppm
        src/ipc.cppm
        src/is_background_set.cppm
        src/load_layout.cppm
        src/restore_layout.cppm
        src/program_arguments.cppm
        src/root_atom_contents.cppm
        src/nagbar/nagbar.cppm
        src/manage.cppm
        src/i3_regex.cppm
        src/internal.cppm
        )
target_link_libraries(i3_lib PUBLIC ${libraries} i3_config_base i3_config_new i3_config_old i3_commands_base i3_commands_old i3_commands_new utils xpp i3_log)
target_include_directories(i3_lib PUBLIC ${include_dirs} src/commands src/config/old)
target_compile_options(i3_lib PUBLIC ${compile_options})
add_dependencies(i3_lib libi3ipc)

add_executable(i3
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
#target_link_libraries(i3 PUBLIC i3_lib libi3ipc i3_config_new i3_config_old)
target_link_libraries(i3 PUBLIC i3_lib libi3ipc)
target_include_directories(i3 PUBLIC ${include_dirs} "${CMAKE_CURRENT_SOURCE_DIR}/src/config/old")
target_compile_options(i3 PUBLIC "-Xclang" "-fno-validate-pch")

add_subdirectory(test)

add_compile_definitions(EV_MULTIPLICITY=1 EV_COMPAT3=0)

