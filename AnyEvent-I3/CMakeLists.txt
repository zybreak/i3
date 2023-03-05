
find_package(Perl REQUIRED)

add_custom_command(
        OUTPUT AnyEvent-I3
        COMMAND cp -r ${CMAKE_CURRENT_SOURCE_DIR}/* .
)

add_custom_command(
        OUTPUT Makefile
        COMMAND ${PERL_EXECUTABLE} Makefile.PL
        DEPENDS AnyEvent-I3
)

add_custom_command(
        OUTPUT AnyEvent-I3.stamp
        COMMAND make && touch ../AnyEvent-I3.stamp
        DEPENDS Makefile
)

add_custom_target(anyevent-i3 ALL
        DEPENDS AnyEvent-I3.stamp)
