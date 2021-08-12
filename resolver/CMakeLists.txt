cmake_minimum_required(VERSION 3.19)

target_include_directories(clox PRIVATE include)

target_sources(clox
        PRIVATE resolver.cpp
        PRIVATE lox_type.cpp
        PRIVATE callable_type.cpp
        PRIVATE object_type.cpp
        PRIVATE class_type.cpp
        PRIVATE instance_type.cpp
        PRIVATE symbol.cpp)


target_include_directories(clox_test PRIVATE include)

target_sources(clox_test
        PRIVATE resolver.cpp
        PRIVATE lox_type.cpp
        PRIVATE callable_type.cpp
        PRIVATE object_type.cpp
        PRIVATE class_type.cpp
        PRIVATE instance_type.cpp
        PRIVATE symbol.cpp)


