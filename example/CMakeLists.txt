project(example LANGUAGES CXX)

file(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}/resources/" GLVIZ_RESOURCES_DIR)
configure_file(config.hpp.in "${CMAKE_CURRENT_BINARY_DIR}/config.hpp")

add_executable(example
    example.cpp
)

target_include_directories(example
    PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(example
    glviz
)
