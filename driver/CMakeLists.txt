cmake_minimum_required(VERSION 3.19)

add_subdirectory(adapter)

target_sources(clox
        PRIVATE driver.cpp
        PRIVATE run.cpp)


target_sources(clox_test
        PRIVATE driver.cpp
        PRIVATE run.cpp)

target_include_directories(clox PRIVATE include)

target_include_directories(clox_test PRIVATE include)