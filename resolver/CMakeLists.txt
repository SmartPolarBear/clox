cmake_minimum_required(VERSION 3.19)

add_subdirectory(resolver)
add_subdirectory(symbol)
add_subdirectory(type)

target_include_directories(clox PRIVATE include)

target_include_directories(clox_test PRIVATE include)




