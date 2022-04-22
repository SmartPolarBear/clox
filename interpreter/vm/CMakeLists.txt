cmake_minimum_required(VERSION 3.19)

target_sources(clox
        PRIVATE vm.cpp
        PRIVATE heap.cpp
        PRIVATE value.cpp
        PRIVATE chunk.cpp
        PRIVATE garbage_collector.cpp)

target_sources(clox_test
        PRIVATE vm.cpp
        PRIVATE heap.cpp
        PRIVATE value.cpp
        PRIVATE chunk.cpp
        PRIVATE garbage_collector.cpp)

