cmake_minimum_required(VERSION 3.19)

target_sources(clox PRIVATE scanner.cpp)

target_include_directories(clox PRIVATE include)