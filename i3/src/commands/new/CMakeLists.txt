find_package(antlr4-runtime 4.11.1 REQUIRED)

# COMMANDS NEW

add_library(i3_commands_new STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/commands_parser.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/parser-specs/commandsGrammarBaseListener.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/parser-specs/commandsLexer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/parser-specs/commandsGrammar.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/parser-specs/commandsGrammarListener.cpp
        )
target_link_libraries(i3_commands_new PUBLIC nlohmann_json::nlohmann_json antlr4_static utils fmt::fmt)
target_include_directories(i3_commands_new PUBLIC ${antlr4-runtime_INCLUDE_DIR}
        ${include_dirs}
        )
