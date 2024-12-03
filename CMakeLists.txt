cmake_minimum_required(VERSION 3.29.20240416 FATAL_ERROR)

set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "0e5b6991-d74f-4b3d-a41c-cf096e0b2508")

project(i3 VERSION 4.24 LANGUAGES CXX)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_MODULE_STD ON)
set(CMAKE_COLOR_DIAGNOSTICS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fprofile-instr-generate -fcoverage-mapping")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")

# Sanitizers makes the unit test fail
if (I3_ASAN)
set(USE_SANITIZER Address)
include(sanitizers)
add_compile_definitions(I3_ASAN_ENABLED=1)
##add_compile_options(-fsanitize-ignorelist=${PROJECT_SOURCE_DIR}/asan-ignorelist.txt)
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize-ignorelist=${PROJECT_SOURCE_DIR}/asan-ignorelist.txt")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-deprecated-declarations -Wno-unused-const-variable")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_CXX_COMPILER_LAUNCHER ccache)
endif()

set(CMAKE_LINKER_TYPE LLD)

configure_file(config.h.in config.h)

include(CTest)

find_package(Perl REQUIRED)

add_compile_definitions(EV_MULTIPLICITY=1 EV_COMPAT3=0)
add_subdirectory(lib/i3_ipc)
add_subdirectory(lib/xpp)
add_subdirectory(i3)
add_subdirectory(testcases)
#add_subdirectory(i3-msg)
#add_subdirectory(utils)
#add_subdirectory(docs)
#add_subdirectory(man)