cmake_minimum_required(VERSION 3.19)

target_sources(clox
        PRIVATE object.cpp
        PRIVATE string_object.cpp
        PRIVATE function_object.cpp
        PRIVATE closure_object.cpp
        PRIVATE list_object.cpp
        PRIVATE class_object.cpp
        PRIVATE instance_object.cpp
        PRIVATE bounded_method_object.cpp
        PRIVATE upvalue_object.cpp
        PRIVATE map_object.cpp
        PRIVATE native_function_object.cpp)

target_sources(clox_test
        PRIVATE object.cpp
        PRIVATE string_object.cpp
        PRIVATE function_object.cpp
        PRIVATE closure_object.cpp
        PRIVATE list_object.cpp
        PRIVATE class_object.cpp
        PRIVATE instance_object.cpp
        PRIVATE bounded_method_object.cpp
        PRIVATE upvalue_object.cpp
        PRIVATE map_object.cpp
        PRIVATE native_function_object.cpp)

target_include_directories(clox PRIVATE include)

target_include_directories(clox_test PRIVATE include)




