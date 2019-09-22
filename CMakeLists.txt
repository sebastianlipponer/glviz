cmake_minimum_required(VERSION 3.8.2 FATAL_ERROR)

project(glviz
    LANGUAGES CXX
    VERSION 1.0
)

option(BUILD_EXAMPLE "Build the example application." ON)

set(CMAKE_DEBUG_POSTFIX "d")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/.extern/install")

# Visual studio solution directories.
set_property(GLOBAL PROPERTY USE_FOLDERS on)

# Enable simultaneous compilation of source files for MSVC.
add_compile_options(
    $<$<CXX_COMPILER_ID:MSVC>:/MP>
)

# Put all executables and libraries into a common directory.
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")
set(LIBRARY_OUTPUT_PATH    "${PROJECT_BINARY_DIR}/bin")

add_subdirectory(glviz)

if (BUILD_EXAMPLE)
    add_subdirectory(example)
endif()
