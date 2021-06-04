cmake_minimum_required(VERSION 3.19.2)
project(clox)

set(CMAKE_CXX_STANDARD 20)

# FIXME: this should be a option
set(PYTHON "C:\\Users\\cleve\\.conda\\pkgs\\python-3.9.5-h6244533_3\\python.exe")

add_subdirectory(test)
add_subdirectory(libs)

add_executable(clox main.cpp scanner/scanner.cpp scanner/include/scanner.h)

add_custom_target(parser_classes_inc
        COMMAND ${PYTHON} ${CMAKE_CURRENT_SOURCE_DIR}/parser/generator/parser_gen.py -c ${CMAKE_CURRENT_SOURCE_DIR}/parser/config/classes.json -H ${CMAKE_CURRENT_SOURCE_DIR}/parser/config/classes.head -t ${CMAKE_CURRENT_SOURCE_DIR}/parser/config/classes.tail -o ${CMAKE_CURRENT_SOURCE_DIR}/parser/include/parser_classes.inc
        BYPRODUCTS {CMAKE_CURRENT_SOURCE_DIR}/parser/include/parser_classes.inc
        COMMENT "Generating parser expression classes"
        )

add_dependencies(clox parser_classes_inc)

add_subdirectory(base)
add_subdirectory(driver)
add_subdirectory(logger)
add_subdirectory(scanner)
add_subdirectory(parser)

target_link_libraries(clox
        PUBLIC GSL
        PUBLIC argparse)