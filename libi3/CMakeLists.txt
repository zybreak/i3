find_package(PkgConfig REQUIRED)

pkg_check_modules(pangocairo_dep REQUIRED pangocairo)
list(APPEND libraries ${pangocairo_dep_LINK_LIBRARIES})
list(APPEND include_dirs ${pangocairo_dep_INCLUDE_DIRS})

add_library(libi3 STATIC
  src/dpi.cpp
  src/draw_util.cpp
  src/font.cpp
  src/format_placeholders.cpp
  src/get_colorpixel.cpp
  src/get_config_path.cpp
  src/get_mod_mask.cpp
  src/get_process_filename.cpp
  src/get_visualtype.cpp
  src/log.cpp
  src/resolve_tilde.cpp
  src/root_atom_contents.cpp
  src/safewrappers.cpp
  src/string.cpp
  src/ucs2_conversion.cpp
  src/mkdirp.cpp)

target_link_libraries(libi3 PUBLIC ${libraries})
target_include_directories(libi3 PUBLIC
        "${PROJECT_BINARY_DIR}"
        include
        ${include_dirs}
        )
