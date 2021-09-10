cmake_minimum_required(VERSION 3.19)

include(GoogleTest)

add_subdirectory(gtest)

add_executable(clox_test
        test_scaffold_console.cpp
        test_interpreter_adapter.cpp
        comment.cpp
        return.cpp
        scoop.cpp
        class.cpp
        function.cpp
        expression.cpp
        conditional.cpp
        loop.cpp
        )

target_include_directories(clox_test
        PRIVATE include
        PRIVATE test_src
        PRIVATE ${gtest_SOURCE_DIR}
        PRIVATE ${gtest_SOURCE_DIR}/include)

target_link_libraries(clox_test
        PUBLIC gtest
        PUBLIC gtest_main
        PUBLIC GSL
        PUBLIC argparse
        PUBLIC magic_enum)

gtest_discover_tests(clox_test)
