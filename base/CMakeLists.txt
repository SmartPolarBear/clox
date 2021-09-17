cmake_minimum_required(VERSION 3.19)

target_sources(clox
        PRIVATE configuration.cpp)

target_include_directories(clox PRIVATE include)

target_sources(clox_test
        PRIVATE configuration.cpp)

target_include_directories(clox_test PRIVATE include)