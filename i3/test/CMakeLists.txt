find_package(GTest CONFIG)

include(GoogleTest)

# Unit tests

add_executable(RegexTest
        unittests/regex_test.cpp
        ../src/regex.cpp
        ../src/safewrappers.cpp
        ../src/log.cpp
)
target_link_libraries(RegexTest PUBLIC ${libraries} GTest::gtest_main)
target_include_directories(RegexTest PUBLIC ${include_dirs})
gtest_discover_tests(RegexTest)

add_executable(MatchTest
        unittests/match_test.cpp
        ${i3_src}
        )
target_link_libraries(MatchTest PUBLIC ${libraries} GTest::gtest_main)
target_include_directories(MatchTest PUBLIC ${include_dirs} "../src/config/old")
gtest_discover_tests(MatchTest)

add_executable(ConTest
        unittests/con_test.cpp
        ${i3_src}
        )
target_link_libraries(ConTest PUBLIC ${libraries} GTest::gtest_main GTest::gmock)
target_include_directories(ConTest PUBLIC ${include_dirs} "../src/config/old")
gtest_discover_tests(ConTest)

# End unit tests

# Begin command and config test

add_executable(test.config_parser_new
    config_parser_new.cpp
    mock_new.cpp
    ../src/config/new/config_parser.cpp
    ../src/config/new/parser-specs/configGrammarBaseListener.cpp
    ../src/config/new/parser-specs/configLexer.cpp
    ../src/config/new/parser-specs/configGrammar.cpp
    ../src/config/new/parser-specs/configGrammarListener.cpp
    ../src/global.cpp
    ../src/log.cpp
)
target_link_libraries(test.config_parser_new PUBLIC ${libraries})
target_include_directories(test.config_parser_new PUBLIC ${include_dirs} "../src/config/new")
target_compile_options(test.config_parser_new PUBLIC ${compile_options} -DTEST_PARSER)

add_executable(test.commands_parser
    commands_parser_old.cpp
    mock.cpp
    ../src/commands/old/commands_parser.cpp
    ../src/parser_stack.cpp
    ../src/global.cpp
    ../src/log.cpp
    ../src/safewrappers.cpp
)
add_dependencies(test.commands_parser Commands)
target_link_libraries(test.commands_parser PUBLIC ${libraries})
target_include_directories(test.commands_parser PUBLIC ${include_dirs} "../src/config/old")
target_compile_options(test.commands_parser PUBLIC ${compile_options} -DTEST_PARSER)

add_executable(test.commands_parser_new
        commands_parser_new.cpp
        mock_new.cpp
        commands_mock.cpp
        ../src/log.cpp
        ../src/commands/new/commands_parser.cpp
        ../src/commands/new/parser-specs/commandsGrammarBaseListener.cpp
        ../src/commands/new/parser-specs/commandsLexer.cpp
        ../src/commands/new/parser-specs/commandsGrammar.cpp
        ../src/commands/new/parser-specs/commandsGrammarListener.cpp
        )
target_link_libraries(test.commands_parser_new PUBLIC ${libraries})
target_include_directories(test.commands_parser_new PUBLIC ${include_dirs} "../src/config/new")

add_executable(test.config_parser
    config_parser.cpp
    mock.cpp
    ../src/config/old/config_parser.cpp
    ../src/parser_stack.cpp
    ../src/global.cpp
    ../src/log.cpp
    ../src/safewrappers.cpp
)
add_dependencies(test.config_parser Config)
target_link_libraries(test.config_parser PUBLIC ${libraries})
target_include_directories(test.config_parser PUBLIC ${include_dirs} "../src/config/old")
target_compile_options(test.config_parser PUBLIC ${compile_options} -DTEST_PARSER)

find_package(Python REQUIRED)

include(Pytest.cmake)

pytest_discover_tests(
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COLLECTION_ARGS "-m not slow"
        EXECUTION_ARGS "--cmd=$<TARGET_FILE_DIR:test.config_parser>"
        #TEST_PREFIX "PYTEST_"
        #TEST_SUFFIX "_${CMAKE_SYSTEM_PROCESSOR}"
        XML_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/report
)

# End command and config test
