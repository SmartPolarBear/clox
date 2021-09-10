cmake_minimum_required(VERSION 3.19)

target_sources(clox
        PRIVATE driver.cpp
        PRIVATE classic.cpp
        PRIVATE run.cpp
        PRIVATE interpreter_adapter.cpp)

target_include_directories(clox PRIVATE include)

target_sources(clox_test
        PRIVATE driver.cpp
        PRIVATE classic.cpp
        PRIVATE run.cpp
        PRIVATE interpreter_adapter.cpp)

target_include_directories(clox_test PRIVATE include)