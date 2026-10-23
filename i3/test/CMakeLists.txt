find_package(GTest CONFIG)

include(GoogleTest)

set(CODE_COVERAGE ON)
include(code-coverage)

add_code_coverage_all_targets()

# Unit tests
add_executable(UnitTest)
target_sources(UnitTest PRIVATE
        commands_new_test.cpp
        commands_old_test.cpp
        config_new_test.cpp
        config_old_test.cpp
        configuration_test.cpp
        con_test.cpp
        dump_node_test.cpp
        format_placeholders_test.cpp
        ipc_test.cpp
        load_layout_test.cpp
        match_test.cpp
        rect_test.cpp
        regex_test.cpp
        tree_test.cpp
        utils_test.cpp
        util_test.cpp
        workspace_manager_test.cpp
        manage_test.cpp
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES
        FILES
        mocks.cppm
)
target_link_libraries(UnitTest PUBLIC i3_lib GTest::gtest_main GTest::gmock)
gtest_discover_tests(UnitTest)
target_code_coverage(UnitTest ALL)
target_compile_features(UnitTest PUBLIC cxx_std_26)

# i3 loads xcbpp's libxcb compatibility layer as its libxcb.so.1, never the system's (see i3/CMakeLists.txt).
add_test(NAME i3-loads-xcbcompat
         COMMAND ${CMAKE_COMMAND} -DPROGRAM=$<TARGET_FILE:i3> -DLAYER=${xcbcompat_location}
                 -P ${CMAKE_CURRENT_SOURCE_DIR}/check_xcbcompat.cmake)
set_tests_properties(i3-loads-xcbcompat PROPERTIES ENVIRONMENT_MODIFICATION "LD_LIBRARY_PATH=unset:")

find_program(XVFB_PROGRAM Xvfb)
find_program(CC_PROGRAM cc)

# Tests on Xvfb (label "it"): i3 -C (CheckConfigTest), i3 on the layer driven by a test client (XcbcompatSmokeTest:
# grabs, AllowEvents, nagbars, the save set, and AllocColor at depth 8) and i3 with several RandR 1.5 monitors
# (MultiMonitorTest). Each test starts an Xvfb of its own, on a display that Xvfb picks (-displayfd), so they run in
# parallel without a resource lock; see xvfb_fixture.cppm.
#
# They run without LD_LIBRARY_PATH, as i3 runs installed: i3 finds xcbpp's libxcb compatibility layer through its
# RUNPATH, which LD_LIBRARY_PATH would override (with the system libxcb's directory, a fallback, i3 would load the
# system's libxcb and fail XcbcompatSmokeTest's checks of the layer), and the smoke test's client must load the system's
# libxcb, not a libxcb.so.1 found on LD_LIBRARY_PATH.
if(XVFB_PROGRAM)
    add_executable(ITTest)
    target_sources(ITTest PRIVATE
            check_config_test.cpp
            multi_monitor_test.cpp
            PUBLIC
            FILE_SET cxx_modules TYPE CXX_MODULES
            FILES
            child_process.cppm
            i3_runner.cppm
            randr_monitors.cppm
            xvfb_fixture.cppm
            xvfb_server.cppm
    )
    target_link_libraries(ITTest PUBLIC xcbpp::xcbpp libi3ipc i3_json GTest::gtest_main GTest::gmock)
    target_compile_definitions(ITTest PRIVATE I3_TEST_XVFB="${XVFB_PROGRAM}" I3_TEST_I3="$<TARGET_FILE:i3>")
    target_compile_features(ITTest PUBLIC cxx_std_26)
    add_dependencies(ITTest i3)

    # The smoke test's client (xcbcompat/client.c) runs on the system's libxcb and XTEST, so it is compiled with cc
    # and pkg-config's flags, outside of i3's link, which links the layer in libxcb's place.
    pkg_check_modules(xcbcompat_client_dep QUIET xcb xcb-xtest)
    if(CC_PROGRAM AND xcbcompat_client_dep_FOUND)
        set(xcbcompat_client ${CMAKE_CURRENT_BINARY_DIR}/xcbcompat-test-client)
        add_custom_command(OUTPUT ${xcbcompat_client}
                COMMAND ${CC_PROGRAM} -O1 -o ${xcbcompat_client} ${CMAKE_CURRENT_SOURCE_DIR}/xcbcompat/client.c
                        ${xcbcompat_client_dep_CFLAGS} ${xcbcompat_client_dep_LDFLAGS}
                DEPENDS xcbcompat/client.c
                VERBATIM)
        add_custom_target(xcbcompat-client DEPENDS ${xcbcompat_client})
        add_dependencies(ITTest xcbcompat-client)
        target_sources(ITTest PRIVATE xcbcompat_smoke_test.cpp)
        target_compile_definitions(ITTest PRIVATE I3_TEST_XCBCOMPAT_CLIENT="${xcbcompat_client}")
    else()
        message(STATUS "cc or pkg-config's xcb-xtest missing: XcbcompatSmokeTest is left out of ITTest")
    endif()

    gtest_discover_tests(ITTest PROPERTIES LABELS it TIMEOUT 120 ENVIRONMENT_MODIFICATION "LD_LIBRARY_PATH=unset:")
else()
    message(STATUS "Xvfb missing: the tests on Xvfb (ITTest) are left out")
endif()

#add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} DEPENDS RegexTest MatchTest ConTest ConfigNewTest ConfigOldTest)

#add_custom_target(coverage COMMAND
#        llvm-profdata merge -sparse ${CMAKE_BINARY_DIR}/coverage/*.profraw -o ${CMAKE_BINARY_DIR}/coverage/coverage.profdata &&
#        llvm-cov show --show-branches=count --instr-profile ${CMAKE_BINARY_DIR}/coverage/coverage.profdata -object $<TARGET_FILE:RegexTest> -object $<TARGET_FILE:MatchTest> -object $<TARGET_FILE:ConTest> -object $<TARGET_FILE:ConfigNewTest> -object $<TARGET_FILE:ConfigOldTest> > ${CMAKE_BINARY_DIR}/coverage/coverage.txt
#        DEPENDS check)

# End unit tests
