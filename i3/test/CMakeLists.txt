find_package(GTest CONFIG)
add_executable(FooTest
        foo_test.cpp
        mock.cpp
        ../src/config/old/config_parser.cpp
        ../src/parser_stack.cpp
        ../src/global.cpp
        ../src/log.cpp
        ../src/safewrappers.cpp
)
add_dependencies(FooTest Config)
target_link_libraries(FooTest PUBLIC ${libraries} GTest::gtest_main)
target_include_directories(FooTest PUBLIC ${include_dirs} "../src/config/old")
target_compile_options(FooTest PUBLIC ${compile_options} -DTEST_PARSER)

add_executable(RegexTest
        regex_test.cpp
        ../src/regex.cpp
        ../src/safewrappers.cpp
        ../src/log.cpp
)
target_link_libraries(RegexTest PUBLIC ${libraries} GTest::gtest_main)
target_include_directories(RegexTest PUBLIC ${include_dirs})

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
    commands_parser.cpp
    mock.cpp
    ../src/commands/commands_parser.cpp
    ../src/parser_stack.cpp
    ../src/global.cpp
    ../src/log.cpp
    ../src/safewrappers.cpp
)
add_dependencies(test.commands_parser Commands)
target_link_libraries(test.commands_parser PUBLIC ${libraries})
target_include_directories(test.commands_parser PUBLIC ${include_dirs} "../src/config/old")
target_compile_options(test.commands_parser PUBLIC ${compile_options} -DTEST_PARSER)

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

include(GoogleTest)
include(Pytest.cmake)
gtest_discover_tests(FooTest)
gtest_discover_tests(RegexTest)

pytest_discover_tests(
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COLLECTION_ARGS "-m not slow"
        EXECUTION_ARGS "--cmd=$<TARGET_FILE_DIR:test.config_parser>"
        #TEST_PREFIX "PYTEST_"
        #TEST_SUFFIX "_${CMAKE_SYSTEM_PROCESSOR}"
        XML_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/report
)

#add_test(NAME config_parser_new
#        COMMAND ${Python_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/test/test_config-parser_new.py
#        WORKING_DIRECTORY $<TARGET_FILE_DIR:test.config_parser_new>
#)

#add_test(NAME config_parser
#        COMMAND ${Python_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/test/test_config-parser.py
#        WORKING_DIRECTORY $<TARGET_FILE_DIR:test.config_parser>
#)
