find_package(GTest CONFIG)

include(GoogleTest)

set(CODE_COVERAGE ON)
include(code-coverage)

add_code_coverage_all_targets()

# Unit tests
add_executable(UnitTest)
target_sources(UnitTest PUBLIC
        commands_new_test.cpp
        commands_old_test.cpp
        config_new_test.cpp
        config_old_test.cpp
        configuration_test.cpp
        con_test.cpp
        dump_node_test.cpp
        format_placeholders_test.cpp
        load_layout_test.cpp
        match_test.cpp
        rect_test.cpp
        regex_test.cpp
        utils_test.cpp
        util_test.cpp
        workspace_manager_test.cpp
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES
        FILES
        mocks.cppm
)
target_link_libraries(UnitTest PUBLIC i3_lib GTest::gtest_main GTest::gmock)
gtest_discover_tests(UnitTest)
target_code_coverage(UnitTest ALL)

#add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} DEPENDS RegexTest MatchTest ConTest ConfigNewTest ConfigOldTest)

#add_custom_target(coverage COMMAND
#        llvm-profdata merge -sparse ${CMAKE_BINARY_DIR}/coverage/*.profraw -o ${CMAKE_BINARY_DIR}/coverage/coverage.profdata &&
#        llvm-cov show --show-branches=count --instr-profile ${CMAKE_BINARY_DIR}/coverage/coverage.profdata -object $<TARGET_FILE:RegexTest> -object $<TARGET_FILE:MatchTest> -object $<TARGET_FILE:ConTest> -object $<TARGET_FILE:ConfigNewTest> -object $<TARGET_FILE:ConfigOldTest> > ${CMAKE_BINARY_DIR}/coverage/coverage.txt
#        DEPENDS check)

# End unit tests
