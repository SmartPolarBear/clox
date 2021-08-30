cmake_minimum_required(VERSION 3.19)

add_subdirectory(resolver)
add_subdirectory(symbol)

target_include_directories(clox PRIVATE include)

target_sources(clox
        PRIVATE lox_type.cpp
        PRIVATE callable_type.cpp
        PRIVATE object_type.cpp
        PRIVATE class_type.cpp
        PRIVATE instance_type.cpp
        PRIVATE union_type.cpp
        PRIVATE array_type.cpp
        PRIVATE any_type.cpp
        PRIVATE void_type.cpp
        PRIVATE initializer_list_type.cpp
        PRIVATE binding.cpp)


target_include_directories(clox_test PRIVATE include)

target_sources(clox_test
        PRIVATE lox_type.cpp
        PRIVATE callable_type.cpp
        PRIVATE object_type.cpp
        PRIVATE class_type.cpp
        PRIVATE instance_type.cpp
        PRIVATE union_type.cpp
        PRIVATE array_type.cpp
        PRIVATE any_type.cpp
        PRIVATE void_type.cpp
        PRIVATE initializer_list_type.cpp
        PRIVATE binding.cpp)


