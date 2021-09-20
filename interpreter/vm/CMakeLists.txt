cmake_minimum_required(VERSION 3.19)

target_sources(clox
        PRIVATE vm.cpp
        PRIVATE object.cpp
        PRIVATE string_object.cpp
        PRIVATE function_object.cpp
        PRIVATE heap.cpp
        PRIVATE value.cpp
        PRIVATE chunk.cpp)

target_sources(clox_test
        PRIVATE vm.cpp
        PRIVATE object.cpp
        PRIVATE string_object.cpp
        PRIVATE function_object.cpp
        PRIVATE heap.cpp
        PRIVATE value.cpp
        PRIVATE chunk.cpp)

