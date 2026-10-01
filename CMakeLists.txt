cmake_minimum_required(VERSION 3.20)

project(
    R2D2Compiler
    VERSION 0.2.1
    DESCRIPTION "Front-end da linguagem BD-1"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(FLEX 2.6 REQUIRED)
find_package(BISON 3.8 REQUIRED)

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/generated")
flex_target(
    BD1Scanner
    "${CMAKE_CURRENT_SOURCE_DIR}/src/lexer.l"
    "${CMAKE_CURRENT_BINARY_DIR}/generated/lexer.cpp"
)

bison_target(
    BD1Parser
    "${CMAKE_CURRENT_SOURCE_DIR}/src/parser.y"
    "${CMAKE_CURRENT_BINARY_DIR}/generated/parser_generated.cpp"
    DEFINES_FILE "${CMAKE_CURRENT_BINARY_DIR}/generated/parser_generated.hpp"
    COMPILE_FLAGS "-Wall -Wcounterexamples"
)

add_library(
    r2d2_frontend
    src/ast.cpp
    src/diagnostic.cpp
    src/lexer.cpp
    src/parser.cpp
    src/parser_driver.cpp
    src/scanner.cpp
    src/token.cpp
    ${FLEX_BD1Scanner_OUTPUTS}
    ${BISON_BD1Parser_OUTPUTS}
)

target_include_directories(
    r2d2_frontend
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_BINARY_DIR}/generated"
)

target_compile_options(
    r2d2_frontend
    PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wpedantic>
)

add_executable(r2d2c src/main.cpp)
target_link_libraries(r2d2c PRIVATE r2d2_frontend)
target_compile_definitions(r2d2c PRIVATE R2D2_VERSION="${PROJECT_VERSION}")

include(CTest)
if(BUILD_TESTING)
    add_executable(lexer_tests tests/lexer_tests.cpp)
    target_link_libraries(lexer_tests PRIVATE r2d2_frontend)

    add_executable(parser_tests tests/parser_tests.cpp)
    target_link_libraries(parser_tests PRIVATE r2d2_frontend)

    add_test(NAME lexer_unit COMMAND lexer_tests)
    add_test(NAME parser_unit COMMAND parser_tests)
    add_test(
        NAME cli_valid_example
        COMMAND r2d2c tokens "${CMAKE_CURRENT_SOURCE_DIR}/examples/navigation.bd1"
    )
    set_tests_properties(
        cli_valid_example
        PROPERTIES PASS_REGULAR_EXPRESSION "analise lexica: sucesso"
    )

    add_test(
        NAME cli_invalid_example
        COMMAND r2d2c tokens "${CMAKE_CURRENT_SOURCE_DIR}/examples/lexical-errors.bd1"
    )
    set_tests_properties(cli_invalid_example PROPERTIES WILL_FAIL TRUE)

    add_test(NAME cli_version COMMAND r2d2c --version)
    set_tests_properties(
        cli_version
        PROPERTIES PASS_REGULAR_EXPRESSION "r2d2c 0.2.1"
    )

    foreach(golden_case IN ITEMS valid_ast invalid_syntax demo)
        add_test(
            NAME "cli_golden_${golden_case}"
            COMMAND
                "${CMAKE_COMMAND}"
                "-DPROGRAM=$<TARGET_FILE:r2d2c>"
                "-DCASE=${golden_case}"
                "-DSOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}"
                -P "${CMAKE_CURRENT_SOURCE_DIR}/tests/run_cli_golden.cmake"
        )
    endforeach()
endif()

install(TARGETS r2d2c RUNTIME DESTINATION bin)
