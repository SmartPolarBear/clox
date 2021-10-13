cmake_minimum_required(VERSION 3.19.2)

project(clox)

include(CTest)

set(CMAKE_CXX_STANDARD 23)

find_package(Python COMPONENTS Interpreter)

if (!Python_FOUND)
    message(ERROR "A python interpreter with higher version than 3.9.0 is required")
else ()
    message(STATUS "Use python executable ${Python_EXECUTABLE}")
endif ()


add_executable(clox main.cpp)
target_compile_options(clox
        PUBLIC /std:c++latest)

add_dependencies(clox parser_classes_inc)

add_subdirectory(libs)

# Enable unit testing
enable_testing()
add_subdirectory(test)

add_subdirectory(base)
add_subdirectory(driver)
add_subdirectory(logger)
add_subdirectory(scanner)
add_subdirectory(parser)
add_subdirectory(helper)
add_subdirectory(interpreter)
add_subdirectory(resolver)
add_subdirectory(tools)

target_link_libraries(clox
        PUBLIC GSL
        PUBLIC argparse
        PUBLIC magic_enum)

if (${ENABLE_ASAN})
    message(STATUS "Use address sanitizer.")
    target_link_libraries(clox
            PRIVATE -fsanitize=address)

    target_link_options(clox
            PRIVATE -fsanitize=address)

    target_compile_options(clox
            PRIVATE -fsanitize=address)

    set(addressSanitizerEnabled true)
else ()
    message(STATUS "Not use address sanitizer.")
endif ()

if (ENABLE_DEBUG_STRESS_GC)
    target_compile_definitions(clox
            PRIVATE -DDEBUG_STRESS_GC=1)
endif ()

if (ENABLE_DEBUG_LOGGING_GC)
    target_compile_definitions(clox
            PRIVATE -DDEBUG_LOGGING_GC=1)
endif ()


add_custom_target(parser_classes_inc
        COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/parser/generator/parser_gen.py -c ${CMAKE_CURRENT_SOURCE_DIR}/parser/config/classes.json -H ${CMAKE_CURRENT_SOURCE_DIR}/parser/config/classes.head -t ${CMAKE_CURRENT_SOURCE_DIR}/parser/config/classes.tail -p ${CMAKE_CURRENT_SOURCE_DIR}/parser/include/parser/gen/parser_classes.inc -s ${CMAKE_CURRENT_SOURCE_DIR}/parser/include/parser/gen/parser_base.inc
        BYPRODUCTS {CMAKE_CURRENT_SOURCE_DIR}/parser/include/parser_classes.inc
        COMMENT "Generating parser expression classes"
        )
