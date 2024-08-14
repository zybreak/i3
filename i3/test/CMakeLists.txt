find_package(GTest CONFIG)

include(GoogleTest)

set(CODE_COVERAGE ON)
include(code-coverage)

add_code_coverage_all_targets()

# Unit tests
add_executable(RegexTest)
target_sources(RegexTest PUBLIC regex_test.cpp)
target_link_libraries(RegexTest PUBLIC i3_lib GTest::gtest_main)
gtest_discover_tests(RegexTest PROPERTIES ENVIRONMENT "LLVM_PROFILE_FILE=${CMAKE_BINARY_DIR}/coverage/regex_test.profraw")
target_code_coverage(RegexTest ALL)

add_executable(MatchTest)
target_sources(MatchTest PUBLIC match_test.cpp)
target_link_libraries(MatchTest PUBLIC i3_lib GTest::gtest_main)
gtest_discover_tests(MatchTest PROPERTIES ENVIRONMENT "LLVM_PROFILE_FILE=${CMAKE_BINARY_DIR}/coverage/match_test.profraw")
target_code_coverage(MatchTest ALL)

add_executable(ConTest)
target_sources(ConTest PUBLIC con_test.cpp)
target_link_libraries(ConTest PUBLIC i3_lib GTest::gtest_main GTest::gmock)
gtest_discover_tests(ConTest PROPERTIES ENVIRONMENT "LLVM_PROFILE_FILE=${CMAKE_BINARY_DIR}/coverage/con_test.profraw")
target_code_coverage(ConTest ALL)

add_executable(ConfigNewTest)
target_sources(ConfigNewTest PUBLIC config_new_test.cpp)
target_link_libraries(ConfigNewTest PUBLIC i3_lib GTest::gtest_main GTest::gmock)
gtest_discover_tests(ConfigNewTest PROPERTIES ENVIRONMENT "LLVM_PROFILE_FILE=${CMAKE_BINARY_DIR}/coverage/config_new_test.profraw")
target_code_coverage(ConfigNewTest ALL)

add_executable(ConfigOldTest)
target_sources(ConfigOldTest PUBLIC config_old_test.cpp)
target_link_libraries(ConfigOldTest PUBLIC i3_lib GTest::gtest_main GTest::gmock)
gtest_discover_tests(ConfigOldTest PROPERTIES ENVIRONMENT "LLVM_PROFILE_FILE=${CMAKE_BINARY_DIR}/coverage/config_old_test.profraw")
target_code_coverage(ConfigOldTest ALL)

add_executable(FormatPlaceholdersTest)
target_sources(FormatPlaceholdersTest PUBLIC format_placeholders_test.cpp)
target_link_libraries(FormatPlaceholdersTest PUBLIC i3_lib GTest::gtest_main GTest::gmock)
gtest_discover_tests(FormatPlaceholdersTest PROPERTIES ENVIRONMENT "LLVM_PROFILE_FILE=${CMAKE_BINARY_DIR}/coverage/format_placeholders_test.profraw")
target_code_coverage(FormatPlaceholdersTest ALL)

add_executable(RectTest)
target_sources(RectTest PUBLIC rect_test.cpp)
target_link_libraries(RectTest PUBLIC i3_lib GTest::gtest_main GTest::gmock)
gtest_discover_tests(RectTest PROPERTIES ENVIRONMENT "LLVM_PROFILE_FILE=${CMAKE_BINARY_DIR}/coverage/rect_test.profraw")
target_code_coverage(RectTest ALL)

add_executable(CommandsNewTest)
target_sources(CommandsNewTest PUBLIC commands_new_test.cpp)
target_link_libraries(CommandsNewTest PUBLIC i3_lib GTest::gtest_main GTest::gmock)
gtest_discover_tests(CommandsNewTest PROPERTIES ENVIRONMENT "LLVM_PROFILE_FILE=${CMAKE_BINARY_DIR}/coverage/commands_new_test.profraw")
target_code_coverage(CommandsNewTest ALL)

add_executable(CommandsOldTest)
target_sources(CommandsOldTest PUBLIC commands_old_test.cpp)
target_link_libraries(CommandsOldTest PUBLIC i3_lib GTest::gtest_main GTest::gmock)
gtest_discover_tests(CommandsOldTest PROPERTIES ENVIRONMENT "LLVM_PROFILE_FILE=${CMAKE_BINARY_DIR}/coverage/commands_old_test.profraw")
target_code_coverage(CommandsOldTest ALL)

add_executable(UtilsTest)
target_sources(UtilsTest PUBLIC utils_test.cpp)
target_link_libraries(UtilsTest PUBLIC i3_lib GTest::gtest_main GTest::gmock)
gtest_discover_tests(UtilsTest PROPERTIES ENVIRONMENT "LLVM_PROFILE_FILE=${CMAKE_BINARY_DIR}/coverage/utils_test.profraw")
target_code_coverage(UtilsTest ALL)

add_executable(UtilTest)
target_sources(UtilTest PUBLIC util_test.cpp)
target_link_libraries(UtilTest PUBLIC i3_lib GTest::gtest_main GTest::gmock)
gtest_discover_tests(UtilTest PROPERTIES ENVIRONMENT "LLVM_PROFILE_FILE=${CMAKE_BINARY_DIR}/coverage/util_test.profraw")
target_code_coverage(UtilTest ALL)

add_executable(WorkspaceManagerTest)
target_sources(WorkspaceManagerTest PUBLIC workspace_manager_test.cpp)
target_link_libraries(WorkspaceManagerTest PUBLIC i3_lib GTest::gtest_main GTest::gmock)
gtest_discover_tests(WorkspaceManagerTest PROPERTIES ENVIRONMENT "LLVM_PROFILE_FILE=${CMAKE_BINARY_DIR}/coverage/workspace_manager_test.profraw")
target_code_coverage(WorkspaceManagerTest ALL)

add_executable(ConfigurationTest)
target_sources(ConfigurationTest PUBLIC configuration_test.cpp)
target_link_libraries(ConfigurationTest PUBLIC i3_lib GTest::gtest_main GTest::gmock)
gtest_discover_tests(ConfigurationTest PROPERTIES ENVIRONMENT "LLVM_PROFILE_FILE=${CMAKE_BINARY_DIR}/coverage/configuration_test.profraw")
target_code_coverage(ConfigurationTest ALL)

add_executable(JsonTest)
target_sources(JsonTest PUBLIC json_test.cpp)
target_link_libraries(JsonTest PUBLIC i3_lib GTest::gtest_main GTest::gmock)
gtest_discover_tests(JsonTest PROPERTIES ENVIRONMENT "LLVM_PROFILE_FILE=${CMAKE_BINARY_DIR}/coverage/json_test.profraw")
target_code_coverage(JsonTest ALL)

#add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} DEPENDS RegexTest MatchTest ConTest ConfigNewTest ConfigOldTest)

#add_custom_target(coverage COMMAND
#        llvm-profdata merge -sparse ${CMAKE_BINARY_DIR}/coverage/*.profraw -o ${CMAKE_BINARY_DIR}/coverage/coverage.profdata &&
#        llvm-cov show --show-branches=count --instr-profile ${CMAKE_BINARY_DIR}/coverage/coverage.profdata -object $<TARGET_FILE:RegexTest> -object $<TARGET_FILE:MatchTest> -object $<TARGET_FILE:ConTest> -object $<TARGET_FILE:ConfigNewTest> -object $<TARGET_FILE:ConfigOldTest> > ${CMAKE_BINARY_DIR}/coverage/coverage.txt
#        DEPENDS check)

# End unit tests
