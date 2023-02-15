add_executable(i3
  src/assignments.cpp
  src/atoms.cpp
  src/autostarts.cpp
  src/bindings.cpp
  src/boolstr.cpp
  src/click.cpp
  src/commands.cpp
  src/commands_parser.cpp
  src/con.cpp
  src/config/old/config_directives.cpp
  src/config/old/config_parser.cpp
  src/config/new/config_parser.cpp
  src/config/new/config_applier.cpp
  src/config/new/parser-specs/configBaseListener.cpp
  src/config/new/parser-specs/configLexer.cpp
  src/config/new/parser-specs/configParser.cpp
  src/config/new/parser-specs/configListener.cpp
  src/configuration.cpp
  src/drag.cpp
  src/ewmh.cpp
  src/floating.cpp
  src/global.cpp
  src/handlers.cpp
  src/ipc.cpp
  src/is_background_set.cpp
  src/key_press.cpp
  src/load_layout.cpp
  src/nagbar.cpp
  src/main.cpp
  src/manage.cpp
  src/match.cpp
  src/move.cpp
  src/nonblock.cpp
  src/output.cpp
  src/parser_stack.cpp
  src/program_arguments.cpp
  src/randr.cpp
  src/rect.cpp
  src/regex.cpp
  src/render.cpp
  src/resize.cpp
  src/restore_layout.cpp
  src/scratchpad.cpp
  src/sighandler.cpp
  src/startup.cpp
  src/sync.cpp
  src/tree.cpp
  src/util.cpp
  src/version.cpp
  src/window.cpp
  src/workspace.cpp
  src/xcb.cpp
  src/x.cpp
  src/xcursor.cpp)

find_package(PkgConfig REQUIRED)
find_package(Iconv)
include(CheckLibraryExists)

check_library_exists(m pow "" LIBM)
find_library(LIBEV ev REQUIRED)
if(LIBM)
    list(APPEND libraries m)
endif()

find_library(LIBEV ev REQUIRED)
list(APPEND libraries ${LIBEV})

pkg_check_modules(libsn_dep REQUIRED libstartup-notification-1.0)
list(APPEND libraries ${libsn_dep_LINK_LIBRARIES})
list(APPEND include_dirs ${libsn_dep_INCLUDE_DIRS})
list(APPEND compile_options ${libsn_dep_CFLAGS_OTHER})

find_package(X11 REQUIRED COMPONENTS xcb xcb_xkb xcb_randr xcb_util xcb_keysyms xcb_icccm xkbcommon xkbcommon_X11)
list(APPEND libraries X11 X11::xcb X11::xcb_xkb X11::xcb_randr X11::xcb_util X11::xcb_keysyms X11::xcb_icccm X11::xkbcommon X11::xkbcommon_X11)

pkg_check_modules(xcb_shape_dep REQUIRED xcb-shape)
list(APPEND libraries ${xcb_shape_dep_LINK_LIBRARIES})
list(APPEND include_dirs ${xcb_shape_dep_INCLUDE_DIRS})
list(APPEND compile_options ${xcb_shape_dep_CFLAGS_OTHER})

pkg_check_modules(xcb_util_cursor_dep REQUIRED xcb-cursor)
list(APPEND libraries ${xcb_util_cursor_dep_LINK_LIBRARIES})
list(APPEND include_dirs ${xcb_util_cursor_dep_INCLUDE_DIRS})
list(APPEND compile_options ${xcb_util_cursor_dep_CFLAGS_OTHER})

pkg_check_modules(xcb_util_xrm_dep REQUIRED xcb-xrm)
list(APPEND libraries ${xcb_util_xrm_dep_LINK_LIBRARIES})
list(APPEND include_dirs ${xcb_util_xrm_dep_INCLUDE_DIRS})
list(APPEND compile_options ${xcb_util_xrm_dep_CFLAGS_OTHER})

pkg_check_modules(libpcre_dep REQUIRED libpcre>=8.10)
list(APPEND libraries ${libpcre_dep_LINK_LIBRARIES})
list(APPEND include_dirs ${libpcre_dep_INCLUDE_DIRS})
list(APPEND compile_options ${libpcre_dep_CFLAGS_OTHER})

pkg_check_modules(cairo_dep REQUIRED cairo>=1.14.4)
list(APPEND libraries ${cairo_dep_LINK_LIBRARIES})
list(APPEND include_dirs ${cairo_dep_INCLUDE_DIRS})
list(APPEND compile_options ${cairo_dep_CFLAGS_OTHER})

pkg_check_modules(pangocairo_dep REQUIRED pangocairo)
list(APPEND libraries ${pangocairo_dep_LINK_LIBRARIES})
list(APPEND include_dirs ${pangocairo_dep_INCLUDE_DIRS})
list(APPEND compile_options ${pangocairo_dep_CFLAGS_OTHER})

pkg_check_modules(glib_dep REQUIRED glib-2.0>=2.52.0)
list(APPEND libraries ${glib_dep_LINK_LIBRARIES})
list(APPEND include_dirs ${glib_dep_INCLUDE_DIRS})
list(APPEND compile_options ${glib_dep_CFLAGS_OTHER})

pkg_check_modules(gobject_dep REQUIRED gobject-2.0)
list(APPEND libraries ${gobject_dep_LINK_LIBRARIES})
list(APPEND include_dirs ${gobject_dep_INCLUDE_DIRS})
list(APPEND compile_options ${gobject_dep_CFLAGS_OTHER})

find_package(fmt REQUIRED)
list(APPEND libraries fmt::fmt)

find_package(nlohmann_json 3.11.2 REQUIRED)
list(APPEND libraries nlohmann_json::nlohmann_json)

find_package(antlr4-runtime REQUIRED)
list(APPEND libraries antlr4_shared)
list(APPEND include_dirs ${ANTLR4_INCLUDE_DIR})

list(APPEND libraries libi3 libi3ipc)

add_dependencies(i3 libi3 libi3ipc)
target_link_libraries(i3 PUBLIC ${libraries})
target_include_directories(i3 PUBLIC
        "${PROJECT_BINARY_DIR}"
        "${PROJECT_SOURCE_DIR}"
        "${PROJECT_SOURCE_DIR}/lib"
        "src"
        "src/include"
        "src/config/old"
        ${include_dirs}
        )
target_compile_options(i3 PUBLIC ${compile_options})
