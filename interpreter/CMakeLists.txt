cmake_minimum_required(VERSION 3.19)

add_subdirectory(native_functions)

target_sources(clox
        PRIVATE interpreter.cpp
        PRIVATE environment.cpp
        PRIVATE lox_function.cpp
        PRIVATE lox_class.cpp
        PRIVATE lox_instance.cpp)

target_include_directories(clox PRIVATE include)

