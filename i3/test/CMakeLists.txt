find_package(GTest CONFIG)

include(GoogleTest)

set(CODE_COVERAGE ON)
include(code-coverage)

add_code_coverage_all_targets()

# Unit tests
add_executable(RegexTest)
target_sources(RegexTest PUBLIC regex_test.cpp)
target_link_libraries(RegexTest PUBLIC i3_lib GTest::gtest_main)
gtest_discover_tests(RegexTest)
target_code_coverage(RegexTest ALL)

add_executable(MatchTest)
target_sources(MatchTest PUBLIC match_test.cpp)
target_link_libraries(MatchTest PUBLIC i3_lib GTest::gtest_main GTest::gmock)
gtest_discover_tests(MatchTest)
target_code_coverage(MatchTest ALL)

add_executable(ConTest)
target_sources(ConTest PUBLIC con_test.cpp)
target_link_libraries(ConTest PUBLIC i3_lib GTest::gtest_main GTest::gmock)
gtest_discover_tests(ConTest)
target_code_coverage(ConTest ALL)

add_executable(ConfigNewTest)
target_sources(ConfigNewTest PUBLIC config_new_test.cpp)
target_link_libraries(ConfigNewTest PUBLIC i3_lib GTest::gtest_main GTest::gmock)
gtest_discover_tests(ConfigNewTest)
target_code_coverage(ConfigNewTest ALL)

add_executable(ConfigOldTest)
target_sources(ConfigOldTest PUBLIC config_old_test.cpp)
target_link_libraries(ConfigOldTest PUBLIC i3_lib GTest::gtest_main GTest::gmock)
gtest_discover_tests(ConfigOldTest)
target_code_coverage(ConfigOldTest ALL)

add_executable(FormatPlaceholdersTest)
target_sources(FormatPlaceholdersTest PUBLIC format_placeholders_test.cpp)
target_link_libraries(FormatPlaceholdersTest PUBLIC i3_lib GTest::gtest_main GTest::gmock)
gtest_discover_tests(FormatPlaceholdersTest)
target_code_coverage(FormatPlaceholdersTest ALL)

add_executable(RectTest)
target_sources(RectTest PUBLIC rect_test.cpp)
target_link_libraries(RectTest PUBLIC i3_lib GTest::gtest_main GTest::gmock)
gtest_discover_tests(RectTest)
target_code_coverage(RectTest ALL)

add_executable(CommandsNewTest)
target_sources(CommandsNewTest PUBLIC commands_new_test.cpp)
target_link_libraries(CommandsNewTest PUBLIC i3_lib GTest::gtest_main GTest::gmock)
gtest_discover_tests(CommandsNewTest)
target_code_coverage(CommandsNewTest ALL)

add_executable(CommandsOldTest)
target_sources(CommandsOldTest PUBLIC commands_old_test.cpp)
target_link_libraries(CommandsOldTest PUBLIC i3_lib GTest::gtest_main GTest::gmock)
gtest_discover_tests(CommandsOldTest)
target_code_coverage(CommandsOldTest ALL)

add_executable(UtilsTest)
target_sources(UtilsTest PUBLIC utils_test.cpp)
target_link_libraries(UtilsTest PUBLIC i3_lib GTest::gtest_main GTest::gmock)
gtest_discover_tests(UtilsTest)
target_code_coverage(UtilsTest ALL)

add_executable(UtilTest)
target_sources(UtilTest PUBLIC util_test.cpp)
target_link_libraries(UtilTest PUBLIC i3_lib GTest::gtest_main GTest::gmock)
gtest_discover_tests(UtilTest)
target_code_coverage(UtilTest ALL)

add_executable(WorkspaceManagerTest)
target_sources(WorkspaceManagerTest PUBLIC workspace_manager_test.cpp)
target_link_libraries(WorkspaceManagerTest PUBLIC i3_lib GTest::gtest_main GTest::gmock)
gtest_discover_tests(WorkspaceManagerTest)
target_code_coverage(WorkspaceManagerTest ALL)

add_executable(ConfigurationTest)
target_sources(ConfigurationTest PUBLIC configuration_test.cpp)
target_link_libraries(ConfigurationTest PUBLIC i3_lib GTest::gtest_main GTest::gmock)
gtest_discover_tests(ConfigurationTest)
target_code_coverage(ConfigurationTest ALL)

add_executable(DumpNodeTest)
target_sources(DumpNodeTest PUBLIC dump_node_test.cpp)
target_link_libraries(DumpNodeTest PUBLIC i3_lib GTest::gtest_main GTest::gmock)
gtest_discover_tests(DumpNodeTest)
target_code_coverage(DumpNodeTest ALL)

add_executable(LoadLayoutTest)
target_sources(LoadLayoutTest PUBLIC load_layout_test.cpp)
target_link_libraries(LoadLayoutTest PUBLIC i3_lib GTest::gtest_main GTest::gmock)
gtest_discover_tests(LoadLayoutTest)
target_code_coverage(LoadLayoutTest ALL)

#add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} DEPENDS RegexTest MatchTest ConTest ConfigNewTest ConfigOldTest)

#add_custom_target(coverage COMMAND
#        llvm-profdata merge -sparse ${CMAKE_BINARY_DIR}/coverage/*.profraw -o ${CMAKE_BINARY_DIR}/coverage/coverage.profdata &&
#        llvm-cov show --show-branches=count --instr-profile ${CMAKE_BINARY_DIR}/coverage/coverage.profdata -object $<TARGET_FILE:RegexTest> -object $<TARGET_FILE:MatchTest> -object $<TARGET_FILE:ConTest> -object $<TARGET_FILE:ConfigNewTest> -object $<TARGET_FILE:ConfigOldTest> > ${CMAKE_BINARY_DIR}/coverage/coverage.txt
#        DEPENDS check)

# End unit tests
