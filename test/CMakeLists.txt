cmake_minimum_required(VERSION 3.19)

add_subdirectory(gtest)

add_executable(clox_test test_main.cpp)

add_test(NAME clox_test COMMAND clox_test)

target_link_libraries(clox_test PUBLIC gtest)