cmake_minimum_required(VERSION 3.19)

target_sources(clox
        PRIVATE interpreter.cpp
        PRIVATE evaluating_result.cpp
        PRIVATE environment.cpp
        PRIVATE lox_function.cpp
        PRIVATE lox_class.cpp
        PRIVATE lox_instance.cpp)

target_sources(clox_test
        PRIVATE interpreter.cpp
        PRIVATE evaluating_result.cpp
        PRIVATE environment.cpp
        PRIVATE lox_function.cpp
        PRIVATE lox_class.cpp
        PRIVATE lox_instance.cpp)