cmake_minimum_required(VERSION 3.19.2)
project(clox)

set(CMAKE_CXX_STANDARD 20)

if (${PYTHON} STREQUAL "")
    message(ERROR "A python interpreter with higher version than 3.9.0 is required")
else ()
    message(STATUS "Use python executable ${PYTHON}")
endif ()

add_subdirectory(test)
add_subdirectory(libs)

add_executable(clox main.cpp)

add_dependencies(clox parser_classes_inc)

add_subdirectory(base)
add_subdirectory(driver)
add_subdirectory(logger)
add_subdirectory(scanner)
add_subdirectory(parser)
add_subdirectory(helper)
add_subdirectory(interpreter)
add_subdirectory(resolver)
add_subdirectory(codegen)

target_link_libraries(clox
        PUBLIC GSL
        PUBLIC argparse)

if (${ENABLE_ASAN})
    message(STATUS "Use address sanitizer.")

    # TODO
else ()
    message(STATUS "Not use address sanitizer.")
endif ()


add_custom_target(parser_classes_inc
        COMMAND ${PYTHON} ${CMAKE_CURRENT_SOURCE_DIR}/parser/generator/parser_gen.py -c ${CMAKE_CURRENT_SOURCE_DIR}/parser/config/classes.json -H ${CMAKE_CURRENT_SOURCE_DIR}/parser/config/classes.head -t ${CMAKE_CURRENT_SOURCE_DIR}/parser/config/classes.tail -p ${CMAKE_CURRENT_SOURCE_DIR}/parser/include/parser/gen/parser_classes.inc -s ${CMAKE_CURRENT_SOURCE_DIR}/parser/include/parser/gen/parser_base.inc
        BYPRODUCTS {CMAKE_CURRENT_SOURCE_DIR}/parser/include/parser_classes.inc
        COMMENT "Generating parser expression classes"
        )