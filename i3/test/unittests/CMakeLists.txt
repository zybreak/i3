find_package(GTest CONFIG)

include(GoogleTest)

# Unit tests
add_executable(TreeTest tree_test.cpp)
target_link_libraries(TreeTest PUBLIC ${libraries} GTest::gtest_main i3_lib)
target_include_directories(TreeTest PUBLIC ${include_dirs})
gtest_discover_tests(TreeTest)

add_executable(RegexTest regex_test.cpp)
target_link_libraries(RegexTest PUBLIC ${libraries} GTest::gtest_main i3_lib)
target_include_directories(RegexTest PUBLIC ${include_dirs})
gtest_discover_tests(RegexTest)

add_executable(MatchTest
        match_test.cpp
        ../../src/autostarts.cpp
        )
target_link_libraries(MatchTest PUBLIC ${libraries} i3_lib i3_config_old i3_config_new i3_commands_new i3_commands_old GTest::gtest_main)
target_include_directories(MatchTest PUBLIC ${include_dirs} "../src/config/old")
gtest_discover_tests(MatchTest)

add_executable(ConTest
        con_test.cpp
        ../../src/autostarts.cpp
        )
target_link_libraries(ConTest PUBLIC ${libraries} i3_lib i3_config_old i3_config_new i3_commands_new i3_commands_old GTest::gtest_main GTest::gmock)
target_include_directories(ConTest PUBLIC ${include_dirs} "../src/config/old")
gtest_discover_tests(ConTest)

# End unit tests
