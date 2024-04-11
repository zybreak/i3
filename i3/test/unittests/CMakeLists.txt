find_package(GTest CONFIG)

include(GoogleTest)

# Unit tests
add_executable(RegexTest)
target_sources(RegexTest PUBLIC regex_test.cpp)
target_link_libraries(RegexTest PUBLIC i3_lib GTest::gtest_main)
gtest_discover_tests(RegexTest)

add_executable(MatchTest)
target_sources(MatchTest PUBLIC match_test.cpp)
target_link_libraries(MatchTest PUBLIC ${libraries} i3_lib i3_config_old i3_config_new i3_commands_new i3_commands_old GTest::gtest_main)
target_include_directories(MatchTest PUBLIC ${include_dirs} "../src/config/old")
gtest_discover_tests(MatchTest)

add_executable(ConTest)
target_sources(ConTest PUBLIC con_test.cpp)
target_link_libraries(ConTest PUBLIC ${libraries} i3_lib i3_config_old i3_config_new i3_commands_new i3_commands_old GTest::gtest_main GTest::gmock)
target_include_directories(ConTest PUBLIC ${include_dirs} "../src/config/old")
gtest_discover_tests(ConTest)

# End unit tests
