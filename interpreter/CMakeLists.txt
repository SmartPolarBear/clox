cmake_minimum_required(VERSION 3.19)

add_subdirectory(native_functions)

target_sources(clox
        PRIVATE interpreter.cpp
        PRIVATE environment.cpp)

target_include_directories(clox PRIVATE include)

