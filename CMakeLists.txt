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
add_subdirectory(codegen)


target_link_libraries(clox
        PUBLIC GSL
        PUBLIC argparse)

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


if (TemporaryFixStdFormat)

    message(STATUS "Define __cpp_lib_format to workaround IDE's disability to discover std::format.")

    # Suppress Warning C4005 for macro redefinition.
    # TODO: it's dangerous. Remove it right after CLion fix the bug.

    target_compile_options(clox
            PRIVATE /wd4005)

    target_compile_definitions(clox
            PRIVATE -D__cpp_lib_format)

    target_compile_options(clox_test
            PRIVATE /wd4005)

    target_compile_definitions(clox_test
            PRIVATE -D__cpp_lib_format)
endif ()

add_custom_target(parser_classes_inc
        COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/parser/generator/parser_gen.py -c ${CMAKE_CURRENT_SOURCE_DIR}/parser/config/classes.json -H ${CMAKE_CURRENT_SOURCE_DIR}/parser/config/classes.head -t ${CMAKE_CURRENT_SOURCE_DIR}/parser/config/classes.tail -p ${CMAKE_CURRENT_SOURCE_DIR}/parser/include/parser/gen/parser_classes.inc -s ${CMAKE_CURRENT_SOURCE_DIR}/parser/include/parser/gen/parser_base.inc
        BYPRODUCTS {CMAKE_CURRENT_SOURCE_DIR}/parser/include/parser_classes.inc
        COMMENT "Generating parser expression classes"
        )
