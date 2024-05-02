add_library(fn INTERFACE)
target_sources(fn INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/fn.hpp)
target_include_directories(fn INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})