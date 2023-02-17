cmake_minimum_required(VERSION 3.23)

project(i3 VERSION 4.19.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include (${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

configure_file(config.h.in config.h)

find_package(Perl REQUIRED)

add_custom_target(Commands ALL
        COMMAND ${PERL_EXECUTABLE}
            ${PROJECT_SOURCE_DIR}/generate-command-parser.pl
            --input=${PROJECT_SOURCE_DIR}/parser-specs/commands.spec
            --prefix=command
        COMMENT "Generating commands"
        BYPRODUCTS
            'GENERATED_command_enums.h'
            'GENERATED_command_tokens.h'
            'GENERATED_command_call.h')

add_custom_target(Config ALL
        COMMAND ${PERL_EXECUTABLE}
            ${PROJECT_SOURCE_DIR}/generate-command-parser.pl
            --input=${PROJECT_SOURCE_DIR}/parser-specs/config.spec
            --prefix=config
        COMMENT "Generating config"
        BYPRODUCTS
            'GENERATED_config_enums.h'
            'GENERATED_config_tokens.h'
            'GENERATED_config_call.h')

add_subdirectory(libi3)
add_subdirectory(lib/i3_ipc)
add_subdirectory(i3)
#add_subdirectory(i3-msg)
#add_subdirectory(utils)
#add_subdirectory(docs)
#add_subdirectory(man)
#add_subdirectory(AnyEvent-I3)
#add_subdirectory(testcases)