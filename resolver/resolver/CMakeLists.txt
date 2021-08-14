cmake_minimum_required(VERSION 3.19)

target_sources(clox
        PRIVATE resolver.cpp
        PRIVATE checks.cpp
        PRIVATE expressions.cpp
        PRIVATE statements.cpp
        PRIVATE type_expressions.cpp)

target_sources(clox_test
        PRIVATE resolver.cpp
        PRIVATE checks.cpp
        PRIVATE expressions.cpp
        PRIVATE statements.cpp
        PRIVATE type_expressions.cpp)

