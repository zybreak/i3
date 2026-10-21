# cmake -DPROGRAM=<program> -DLAYER=<the layer's library> -P check_xcbcompat.cmake
#
# Checks that the program loads xcbpp's libxcb compatibility layer as its libxcb.so.1, found through its RUNPATH, and
# not the system's libxcb. Run it without LD_LIBRARY_PATH.
execute_process(COMMAND ldd ${PROGRAM} OUTPUT_VARIABLE out ERROR_VARIABLE err RESULT_VARIABLE rc)
if(NOT rc EQUAL 0)
    message(FATAL_ERROR "ldd ${PROGRAM} failed:\n${out}${err}")
endif()

string(REGEX MATCH "libxcb\\.so\\.1 => ([^ \n]+)" found "${out}")
if(NOT found)
    message(FATAL_ERROR "${PROGRAM} doesn't load libxcb.so.1:\n${out}")
endif()

file(REAL_PATH "${CMAKE_MATCH_1}" loaded)
file(REAL_PATH "${LAYER}" layer)
if(NOT loaded STREQUAL layer)
    message(FATAL_ERROR "${PROGRAM} loads ${loaded} as libxcb.so.1, not the compatibility layer (${layer})")
endif()
message(STATUS "libxcb.so.1 => ${loaded}")
