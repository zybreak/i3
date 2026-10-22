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

# i3 on Xvfb, on the layer: grabs, AllowEvents, the save set and AllocColor (depth 8). Its test client is compiled
# against the system's libxcb and XTEST, so the test needs Xvfb, i3-msg, a C compiler and pkg-config's xcb-xtest.
find_program(XVFB_PROGRAM Xvfb)
find_program(I3_MSG_PROGRAM i3-msg)
find_program(CC_PROGRAM cc)
pkg_check_modules(xcb_xtest_dep QUIET xcb-xtest)
if(XVFB_PROGRAM AND I3_MSG_PROGRAM AND CC_PROGRAM AND xcb_xtest_dep_FOUND)
    foreach(depth IN ITEMS 24 8)
        add_test(NAME i3-xcbcompat-smoke-depth${depth}
                 COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/xcbcompat/smoke.sh $<TARGET_FILE:i3> 9${depth}
                         ${CMAKE_CURRENT_BINARY_DIR}/xcbcompat-smoke-depth${depth} ${depth})
        set_tests_properties(i3-xcbcompat-smoke-depth${depth} PROPERTIES
                             LABELS it RESOURCE_LOCK xvfb ENVIRONMENT_MODIFICATION "LD_LIBRARY_PATH=unset:")
    endforeach()
else()
    message(STATUS "Xvfb, i3-msg, cc or xcb-xtest missing: the i3-xcbcompat-smoke tests are left out")
endif()

# i3 -C on a configuration with workspace assignments and for_window, assign and no_focus directives, which it parses
# without i3's running state. It needs Xvfb: the configuration loads fonts and colours on an X connection.
if(XVFB_PROGRAM)
    add_test(NAME i3-check-config
             COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/check_config.sh $<TARGET_FILE:i3> 931
                     ${CMAKE_CURRENT_BINARY_DIR}/check-config)
    set_tests_properties(i3-check-config PROPERTIES
                         LABELS it RESOURCE_LOCK xvfb ENVIRONMENT_MODIFICATION "LD_LIBRARY_PATH=unset:")
else()
    message(STATUS "Xvfb missing: the i3-check-config test is left out")
endif()

#add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} DEPENDS RegexTest MatchTest ConTest ConfigNewTest ConfigOldTest)

#add_custom_target(coverage COMMAND
#        llvm-profdata merge -sparse ${CMAKE_BINARY_DIR}/coverage/*.profraw -o ${CMAKE_BINARY_DIR}/coverage/coverage.profdata &&
#        llvm-cov show --show-branches=count --instr-profile ${CMAKE_BINARY_DIR}/coverage/coverage.profdata -object $<TARGET_FILE:RegexTest> -object $<TARGET_FILE:MatchTest> -object $<TARGET_FILE:ConTest> -object $<TARGET_FILE:ConfigNewTest> -object $<TARGET_FILE:ConfigOldTest> > ${CMAKE_BINARY_DIR}/coverage/coverage.txt
#        DEPENDS check)

# End unit tests
