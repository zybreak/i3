add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/lib/i3test.pm
        COMMAND sed -e "\"s,@abs_top_builddir@,${CMAKE_BINARY_DIR},g;s,@abs_top_srcdir@,${CMAKE_SOURCE_DIR},g\"" lib/i3test.pm.in > ${CMAKE_CURRENT_BINARY_DIR}/lib/i3test.pm
        DEPENDS lib/i3test.pm.in
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/complete-run.pl
        COMMAND sed -e "\"s,@abs_top_builddir@,${CMAKE_BINARY_DIR},g;s,@abs_top_srcdir@,${CMAKE_SOURCE_DIR},g\"" complete-run.pl.in > ${CMAKE_CURRENT_BINARY_DIR}/complete-run.pl
        COMMAND chmod +x ${CMAKE_CURRENT_BINARY_DIR}/complete-run.pl
        DEPENDS complete-run.pl.in
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(complete-run
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/complete-run.pl ${CMAKE_CURRENT_BINARY_DIR}/lib/i3test.pm)

#add_test(NAME it COMMAND complete-run.pl --parallel 1)