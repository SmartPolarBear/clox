cmake_minimum_required(VERSION 3.19)

add_subdirectory(classic)

target_sources(clox
        PRIVATE native_functions.cpp)

target_sources(clox_test
        PRIVATE native_functions.cpp)

