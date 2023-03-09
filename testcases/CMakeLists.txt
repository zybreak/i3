include(CTest)

find_package(Perl REQUIRED)

add_custom_command(
        OUTPUT complete-run.pl
        COMMAND sed ARGS -e 's,@abs_top_builddir@,${CMAKE_CURRENT_BINARY_DIR},g' -e 's,@abs_top_srcdir@,${CMAKE_CURRENT_SOURCE_DIR},g' ${CMAKE_CURRENT_LIST_DIR}/complete-run.pl.in > complete-run.pl
)

add_custom_command(
        OUTPUT i3test.pm
        COMMAND sed ARGS -e 's,@abs_top_builddir@,${CMAKE_CURRENT_BINARY_DIR},g' -e 's,@abs_top_srcdir@,${CMAKE_CURRENT_SOURCE_DIR},g' ${CMAKE_CURRENT_LIST_DIR}/lib/i3test.pm.in > i3test.pm
)

add_custom_target(complete_run ALL
        COMMAND ${PERL_EXECUTABLE}
        complete-run.pl
        DEPENDS complete-run.pl i3test.pm)

#enable_testing()

#add_test(NAME CompleteRun
#        COMMAND complete_run
#)