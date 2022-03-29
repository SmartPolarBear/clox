cmake_minimum_required(VERSION 3.19)

target_sources(clox
        PRIVATE clock.cpp)

target_sources(clox_test
        PRIVATE clock.cpp)
