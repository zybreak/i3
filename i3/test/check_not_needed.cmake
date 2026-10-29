# cmake -DPROGRAMS=<program>[;<program>…] -DLIBRARIES=<library>[;<library>…] -DREADELF=<readelf> -P check_not_needed.cmake
#
# Checks that no program needs any of the libraries itself (readelf's NEEDED). Unlike check_no_libxcb_wm.cmake, it doesn't
# check what the programs load: the libraries they need may load these libraries themselves.
foreach(program IN LISTS PROGRAMS)
    execute_process(COMMAND ${READELF} -d ${program} OUTPUT_VARIABLE dynamic ERROR_VARIABLE err RESULT_VARIABLE rc)
    if(NOT rc EQUAL 0)
        message(FATAL_ERROR "readelf -d ${program} failed:\n${dynamic}${err}")
    endif()
    string(REGEX MATCHALL "\\(NEEDED\\)[^\n]*" needed "${dynamic}")
    list(LENGTH needed count)
    if(count EQUAL 0)
        message(FATAL_ERROR "readelf -d ${program} lists no NEEDED library:\n${dynamic}")
    endif()
    foreach(library IN LISTS LIBRARIES)
        if(needed MATCHES "\\[${library}\\.so")
            message(FATAL_ERROR "${program} needs ${library}:\n${needed}")
        endif()
    endforeach()
    message(STATUS "${program}: ${count} NEEDED, none of ${LIBRARIES}")
endforeach()
