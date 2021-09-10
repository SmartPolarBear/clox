cmake_minimum_required(VERSION 3.19)

add_subdirectory(native)
add_subdirectory(vm)
add_subdirectory(codegen)
add_subdirectory(classic)

target_sources(clox
        PRIVATE interpreter.cpp)

target_include_directories(clox PRIVATE include)

target_sources(clox_test
        PRIVATE interpreter.cpp)

target_include_directories(clox_test PRIVATE include)


