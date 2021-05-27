cmake_minimum_required(VERSION 3.19.2)
project(clox)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(test)
add_subdirectory(libs)

add_executable(clox main.cpp scanner/scanner.cpp scanner/include/scanner.h)

add_subdirectory(base)
add_subdirectory(driver)
add_subdirectory(logger)
add_subdirectory(scanner)
add_subdirectory(parser)

target_link_libraries(clox
        PUBLIC GSL
        PUBLIC argparse)