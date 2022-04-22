cmake_minimum_required(VERSION 3.19)

add_subdirectory(function)

target_sources(clox
        PRIVATE native_manager.cpp
        PRIVATE native_function.cpp
        PRIVATE native_method.cpp)

target_sources(clox_test
        PRIVATE native_manager.cpp
        PRIVATE native_function.cpp
        PRIVATE native_method.cpp)

target_include_directories(clox PRIVATE include)

target_include_directories(clox_test PRIVATE include)
