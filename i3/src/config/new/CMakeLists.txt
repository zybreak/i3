find_package(antlr4-runtime 4.11.1 REQUIRED)

add_library(i3_config_new STATIC)

target_sources(i3_config_new PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/config_parser.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/parser-specs/configGrammarBaseListener.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/parser-specs/configLexer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/parser-specs/configGrammar.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/parser-specs/configGrammarListener.cpp
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES
        FILES
        config_parser.ixx
        )
target_link_libraries(i3_config_new PUBLIC antlr4_static ${libraries} i3_config_base i3_log)
target_include_directories(i3_config_new PUBLIC ${include_dirs} ${antlr4-runtime_INCLUDE_DIR}
        #"${CMAKE_CURRENT_SOURCE_DIR}/src/config/new"
        )
