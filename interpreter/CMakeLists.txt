cmake_minimum_required(VERSION 3.19)

target_sources(clox
        PRIVATE interpreter.cpp
        PRIVATE environment.cpp)

target_include_directories(clox PRIVATE include)

