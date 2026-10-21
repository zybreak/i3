add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/lib/i3test.pm
        COMMAND sed -e "\"s,@abs_top_builddir@,${CMAKE_BINARY_DIR},g;s,@abs_top_srcdir@,${CMAKE_SOURCE_DIR},g\"" lib/i3test.pm.in > ${CMAKE_CURRENT_BINARY_DIR}/lib/i3test.pm
        DEPENDS lib/i3test.pm.in
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/complete-run.pl
        COMMAND sed -e "\"s,@abs_top_builddir@,${CMAKE_BINARY_DIR},g;s,@abs_top_srcdir@,${CMAKE_SOURCE_DIR},g\"" complete-run.pl.in > ${CMAKE_CURRENT_BINARY_DIR}/complete-run.pl
        COMMAND chmod +x ${CMAKE_CURRENT_BINARY_DIR}/complete-run.pl
        DEPENDS complete-run.pl.in
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# ALL: the ctest test below runs the generated script.
add_custom_target(complete-run ALL
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/complete-run.pl ${CMAKE_CURRENT_BINARY_DIR}/lib/i3test.pm)

# The Perl suite, label "it". It needs Xvfb, Xephyr and the CPAN modules that install-perl-deps.sh installs into
# I3_TEST_PERL_PREFIX. It passes when every test in baseline/passing.txt passes.
set(I3_TEST_PERL_PREFIX "${CMAKE_SOURCE_DIR}/build/perl5" CACHE PATH
        "Prefix of the Perl suite's CPAN modules (testcases/install-perl-deps.sh)")
add_test(NAME complete-run COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/run-it.sh WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(complete-run PROPERTIES LABELS it TIMEOUT 7200
        ENVIRONMENT "PERL5LIB=${I3_TEST_PERL_PREFIX}/lib/perl5")