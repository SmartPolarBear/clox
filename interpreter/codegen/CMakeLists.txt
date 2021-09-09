cmake_minimum_required(VERSION 3.19)

target_sources(clox
        PRIVATE codegen.cpp)

target_sources(clox_test
        PRIVATE codegen.cpp)


