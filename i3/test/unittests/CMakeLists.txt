find_package(GTest CONFIG)

include(GoogleTest)

# Unit tests
add_executable(RegexTest)
target_sources(RegexTest PUBLIC regex_test.cpp)
target_link_libraries(RegexTest PUBLIC i3_lib GTest::gtest_main)
gtest_discover_tests(RegexTest PROPERTIES ENVIRONMENT "LLVM_PROFILE_FILE=${CMAKE_BINARY_DIR}/coverage/regex_test.profraw")

add_executable(MatchTest)
target_sources(MatchTest PUBLIC match_test.cpp)
target_link_libraries(MatchTest PUBLIC i3_lib GTest::gtest_main)
gtest_discover_tests(MatchTest PROPERTIES ENVIRONMENT "LLVM_PROFILE_FILE=${CMAKE_BINARY_DIR}/coverage/match_test.profraw")

add_executable(ConTest)
target_sources(ConTest PUBLIC con_test.cpp)
target_link_libraries(ConTest PUBLIC i3_lib GTest::gtest_main GTest::gmock)
gtest_discover_tests(ConTest PROPERTIES ENVIRONMENT "LLVM_PROFILE_FILE=${CMAKE_BINARY_DIR}/coverage/con_test.profraw")

add_executable(ConfigNewTest)
target_sources(ConfigNewTest PUBLIC config_new_test.cpp)
target_link_libraries(ConfigNewTest PUBLIC i3_lib GTest::gtest_main GTest::gmock)
gtest_discover_tests(ConfigNewTest PROPERTIES ENVIRONMENT "LLVM_PROFILE_FILE=${CMAKE_BINARY_DIR}/coverage/config_new_test.profraw")

add_executable(ConfigOldTest)
target_sources(ConfigOldTest PUBLIC config_old_test.cpp)
target_link_libraries(ConfigOldTest PUBLIC i3_lib GTest::gtest_main GTest::gmock)
gtest_discover_tests(ConfigOldTest PROPERTIES ENVIRONMENT "LLVM_PROFILE_FILE=${CMAKE_BINARY_DIR}/coverage/config_old_test.profraw")

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} DEPENDS RegexTest MatchTest ConTest ConfigNewTest ConfigOldTest)

add_custom_target(coverage COMMAND
        llvm-profdata merge -sparse ${CMAKE_BINARY_DIR}/coverage/*.profraw -o ${CMAKE_BINARY_DIR}/coverage/coverage.profdata &&
        llvm-cov show --show-branches=count --instr-profile ${CMAKE_BINARY_DIR}/coverage/coverage.profdata -object $<TARGET_FILE:RegexTest> -object $<TARGET_FILE:MatchTest> -object $<TARGET_FILE:ConTest> -object $<TARGET_FILE:ConfigNewTest> -object $<TARGET_FILE:ConfigOldTest> > ${CMAKE_BINARY_DIR}/coverage/coverage.txt
        DEPENDS check)

# End unit tests
