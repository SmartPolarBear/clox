cmake_minimum_required(VERSION 3.19)

target_sources(clox
        PRIVATE clock.cpp
        PRIVATE len.cpp)

target_sources(clox_test
        PRIVATE clock.cpp
        PRIVATE len.cpp)

