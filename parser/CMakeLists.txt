cmake_minimum_required(VERSION 3.19)

target_include_directories(clox PRIVATE include)

target_sources(clox PRIVATE parser.cpp)