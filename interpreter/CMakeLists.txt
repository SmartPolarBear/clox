cmake_minimum_required(VERSION 3.19)

add_subdirectory(native)
add_subdirectory(vm)
add_subdirectory(codegen)
add_subdirectory(classic)

target_sources(clox
        PRIVATE interpreter.cpp
        PRIVATE evaluating_result.cpp
        PRIVATE environment.cpp
        PRIVATE lox_function.cpp
        PRIVATE lox_class.cpp
        PRIVATE lox_instance.cpp)

target_include_directories(clox PRIVATE include)

target_sources(clox_test
        PRIVATE interpreter.cpp
        PRIVATE evaluating_result.cpp
        PRIVATE environment.cpp
        PRIVATE lox_function.cpp
        PRIVATE lox_class.cpp
        PRIVATE lox_instance.cpp)

target_include_directories(clox_test PRIVATE include)


