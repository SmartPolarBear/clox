cmake_minimum_required(VERSION 3.19)


target_sources(clox
        PRIVATE classic.cpp
        PRIVATE vm.cpp)


target_sources(clox_test
        PRIVATE classic.cpp
        PRIVATE vm.cpp)
