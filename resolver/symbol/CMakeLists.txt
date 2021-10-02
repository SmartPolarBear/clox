cmake_minimum_required(VERSION 3.19)

target_sources(clox
        PRIVATE symbol.cpp
        PRIVATE scope_collection.cpp
        PRIVATE binding.cpp)

target_sources(clox_test
        PRIVATE symbol.cpp
        PRIVATE scope_collection.cpp
        PRIVATE binding.cpp)



