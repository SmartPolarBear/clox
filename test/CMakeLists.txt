cmake_minimum_required(VERSION 3.19)

include(GoogleTest)

add_subdirectory(gtest)

add_executable(clox_test
        test_scaffold_console.cpp
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
        PUBLIC GSL)

gtest_add_tests(TARGET clox_test)

add_test(NAME clox_test COMMAND $<TARGET_FILE:clox_test>)