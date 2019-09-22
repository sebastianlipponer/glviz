
# Use static GLEW.
add_definitions(-DGLEW_STATIC)

file(TO_NATIVE_PATH "${PROJECT_SOURCE_DIR}/resources/" GLVIZ_RESOURCES_DIR)
configure_file(config.hpp.in "${CMAKE_CURRENT_BINARY_DIR}/config.hpp")

add_executable(example
    example.cpp
)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${glviz_INCLUDE_DIRS}
)

add_dependencies(example
    glviz
)

target_link_libraries(example
    ${glviz_LIBRARIES}
)
