# Begin command and config test

#add_executable(test.config_parser_new
#    config_parser_new.cpp
#    mock_new.cpp
#    ../src/global.cpp
#    ../src/log.cpp
#)
#target_link_libraries(test.config_parser_new PUBLIC ${libraries} i3_config_new)
#target_include_directories(test.config_parser_new PUBLIC ${include_dirs} "../src/config/new")
#target_compile_options(test.config_parser_new PUBLIC ${compile_options} -DTEST_PARSER)

add_executable(test.commands_parser
    commands_parser_old.cpp
    mock.cpp
    ../src/global.cpp
    ../src/log.cpp
    ../src/safewrappers.cpp
)
target_link_libraries(test.commands_parser PUBLIC ${libraries} i3_commands_old)
target_include_directories(test.commands_parser PUBLIC ${include_dirs} "../src/config/old")
target_compile_options(test.commands_parser PUBLIC ${compile_options} -DTEST_PARSER)

#add_executable(test.commands_parser_new
#        commands_parser_new.cpp
#        mock_new.cpp
#        commands_mock.cpp
#        ../src/log.cpp
#        )
#target_link_libraries(test.commands_parser_new PUBLIC ${libraries} i3_commands_new)
#target_include_directories(test.commands_parser_new PUBLIC ${include_dirs} "../src/commands/old" "../src/config/new")

add_executable(test.config_parser
    config_parser.cpp
    mock.cpp
    ../src/global.cpp
    ../src/log.cpp
    ../src/safewrappers.cpp
)
target_link_libraries(test.config_parser PUBLIC ${libraries} i3_config_old)
target_include_directories(test.config_parser PUBLIC ${include_dirs} "../src/config/old")
target_compile_options(test.config_parser PUBLIC ${compile_options} -DTEST_PARSER)

# End command and config test

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

add_subdirectory(unittests)
