cmake_minimum_required(VERSION 3.19)

target_sources(clox
        PRIVATE parser.cpp
        PRIVATE ast_printer.cpp)

target_include_directories(clox PRIVATE include)

target_sources(clox_test
        PRIVATE parser.cpp
        PRIVATE ast_printer.cpp)

target_include_directories(clox_test PRIVATE include)

