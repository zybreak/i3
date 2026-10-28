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

find_package(X11 REQUIRED COMPONENTS xcb xcb_cursor xcb_shape xcb_xrm xcb_xkb xcb_randr xcb_util xcb_keysyms xkbcommon xkbcommon_X11)
list(APPEND libraries X11 X11::xcb X11::xcb_cursor X11::xcb_shape X11::xcb_xrm X11::xcb_xkb X11::xcb_randr X11::xcb_util X11::xcb_keysyms X11::xkbcommon X11::xkbcommon_X11)

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
list(APPEND libraries fmt::fmt-header-only)

find_package(spdlog CONFIG REQUIRED)

find_package(nlohmann_json CONFIG REQUIRED)
list(APPEND libraries nlohmann_json::nlohmann_json)

find_package(di CONFIG REQUIRED)
list(APPEND libraries di::di)

# xcbpp opens i3's X connections, and its libxcb compatibility layer (xcbpp::xcbcompat, a libxcb.so.1 on xcbpp's
# runtime) replaces libxcb in i3's process: i3's own libxcb calls, cairo, xkbcommon-x11, libstartup-notification and
# libxcb's extension libraries reach the X server through xcbpp. i3 must never link the system's libxcb.so: the linker
# keeps only the first library named libxcb.so.1 it is given, and the dynamic loader loads only one. So every target
# that names it links the layer instead: FindX11's X11::xcb, and Conan's xorg targets (through cairo and pango's
# libxft), whose pkg-config data names libxcb as the system library "xcb" for most of them. Linking the layer alone
# also makes the linker check that every libxcb function i3 and its libraries call is one the layer provides.
find_package(xcbpp CONFIG REQUIRED)
if(NOT TARGET xcbpp::xcbcompat)
    message(FATAL_ERROR "xcbpp was built without its libxcb compatibility layer (Conan option xcbpp/*:xcbcompat)")
endif()
list(APPEND libraries xcbpp::xcbpp xcbpp::xcbcompat)

get_target_property(xcbcompat_configs xcbpp::xcbcompat IMPORTED_CONFIGURATIONS)
list(GET xcbcompat_configs 0 xcbcompat_config)
get_target_property(xcbcompat_location xcbpp::xcbcompat IMPORTED_LOCATION_${xcbcompat_config})
set_target_properties(X11::xcb PROPERTIES IMPORTED_LOCATION "${xcbcompat_location}")

# Replaces the system library "xcb" in a target's link interface with the layer. The name stands alone, or opens or
# closes a $<CONFIG:…> generator expression; "xorg::xcb" isn't it.
function(i3_link_xcbcompat_instead_of_libxcb target)
    get_target_property(libraries ${target} INTERFACE_LINK_LIBRARIES)
    string(REGEX REPLACE "(^|;|[^:]:)xcb(;|>|$)" "\\1\\2" replaced "${libraries}")
    if(NOT replaced STREQUAL libraries)
        set_target_properties(${target} PROPERTIES INTERFACE_LINK_LIBRARIES "${replaced};xcbpp::xcbcompat")
    endif()
endfunction()

i3_link_xcbcompat_instead_of_libxcb(xorg_DEPS_TARGET)
foreach(component IN LISTS xorg_COMPONENT_NAMES)
    string(REPLACE "::" "_" component_name ${component})
    i3_link_xcbcompat_instead_of_libxcb(xorg_${component_name}_DEPS_TARGET)
endforeach()

# i3 uses xcbpp's xcb.icccm and xcb.ewmh, not libxcb-wm's xcb-icccm and xcb-ewmh, and links neither. Conan's xorg
# targets name both as system libraries (xorg::xorg all of xorg's, which cairo and pango's libxft link): they are
# dropped from them, as "xcb" is above. The test i3-links-no-libxcb-wm (test/CMakeLists.txt) checks the binaries.
function(i3_drop_system_library target library)
    get_target_property(libraries ${target} INTERFACE_LINK_LIBRARIES)
    string(REGEX REPLACE "(^|;|[^:]:)${library}(;|>|$)" "\\1\\2" dropped "${libraries}")
    if(NOT dropped STREQUAL libraries)
        set_target_properties(${target} PROPERTIES INTERFACE_LINK_LIBRARIES "${dropped}")
    endif()
endfunction()

foreach(library xcb-icccm xcb-ewmh)
    i3_drop_system_library(xorg_DEPS_TARGET ${library})
    foreach(component IN LISTS xorg_COMPONENT_NAMES)
        string(REPLACE "::" "_" component_name ${component})
        i3_drop_system_library(xorg_${component_name}_DEPS_TARGET ${library})
    endforeach()
endforeach()

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
target_sources(i3_lib PRIVATE
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
        src/bindings.cppm
        src/program_arguments.cppm
        src/rect.cppm
        src/i3_regex.cppm)

target_link_libraries(i3_lib PUBLIC ${libraries} i3_config_base i3_config_new i3_config_old i3_commands_base i3_commands_old i3_commands_new utils i3_log i3_json)
target_include_directories(i3_lib PUBLIC ${include_dirs} src/commands src/config/old)
target_compile_options(i3_lib PUBLIC ${compile_options})
add_dependencies(i3_lib libi3ipc)
target_compile_features(i3_lib PUBLIC cxx_std_26)

add_library(i3_json STATIC)
target_sources(i3_json PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES
        FILES
        src/json.cppm)
target_link_libraries(i3_json PUBLIC nlohmann_json::nlohmann_json)
target_compile_features(i3_json PUBLIC cxx_std_26)

add_executable(i3)
target_sources(i3 PRIVATE src/main.cpp)
#target_link_libraries(i3 PUBLIC i3_lib libi3ipc i3_config_new i3_config_old)
target_link_libraries(i3 PUBLIC i3_lib libi3ipc)
target_include_directories(i3 PUBLIC ${include_dirs} "${CMAKE_CURRENT_SOURCE_DIR}/src/config/old")
target_compile_options(i3 PUBLIC "-Xclang" "-fno-validate-pch")

add_subdirectory(test)

add_compile_definitions(EV_MULTIPLICITY=1 EV_COMPAT3=0)

