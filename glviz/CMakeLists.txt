# embed_resource.
project(embed_resource
    LANGUAGES CXX
)

add_executable(embed_resource
    embed_resource/embed_resource.cpp
)
add_executable(GLviz::embed_resource ALIAS embed_resource)

set_target_properties(embed_resource PROPERTIES
    DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}"
)

# Disable stupid MSVC warnings.
target_compile_definitions(embed_resource
    PRIVATE $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
)

project(GLviz
    VERSION 1.0
    LANGUAGES CXX
)

find_package(SDL2 REQUIRED CONFIG)
find_package(Eigen3 REQUIRED CONFIG)
find_package(glew REQUIRED CONFIG)
find_package(ImGui REQUIRED CONFIG)

# GLviz shader.
set(SHADER_GLSL
    src/shader/mesh3_fs.glsl
    src/shader/mesh3_gs.glsl
    src/shader/mesh3_vs.glsl
    src/shader/sphere_fs.glsl
    src/shader/sphere_vs.glsl
)

include(GLvizShaderWrapCpp)
glviz_shader_wrap_cpp(SHADER_CPP ${SHADER_GLSL})

add_library(shader STATIC)
add_library(GLviz::shader ALIAS shader)

target_sources(shader PRIVATE
    ${SHADER_GLSL}
    ${SHADER_CPP}
)

source_group("Shader Files" FILES ${SHADER_GLSL})

# GLviz library.
add_library(glviz STATIC)
add_library(GLviz::glviz ALIAS glviz)

target_sources(glviz PRIVATE
    src/glviz.hpp
    src/glviz.cpp
    src/camera.hpp
    src/camera.cpp
    src/shader.cpp
    src/trackball.hpp
    src/trackball.cpp
    src/program.hpp
    src/program.cpp
    src/buffer.hpp
    src/buffer.cpp
    src/utility.cpp
    src/utility.hpp
    src/shader.hpp
)

target_include_directories(glviz
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Disable stupid MSVC warnings.
target_compile_definitions(glviz
    PRIVATE $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
            $<$<CXX_COMPILER_ID:MSVC>:_SCL_SECURE_NO_WARNINGS>
)

target_link_libraries(glviz PUBLIC
    shader
    Eigen3::Eigen
    ImGui::imgui
    ImGui::imgui_sdl
    SDL2::SDL2main
    SDL2::SDL2-static
    GLEW::glew_s
)

set(CMAKE_INSTALL_LIBDIR     "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_INCLUDEDIR "${CMAKE_INSTALL_PREFIX}/include")
set(CMAKE_INSTALL_BINDIR     "${CMAKE_INSTALL_PREFIX}/bin")
set(CMAKE_INSTALL_MODULEDIR  "${CMAKE_INSTALL_PREFIX}/cmake/Modules")

set(config_install_dir "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")
set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")

set(version_config "${generated_dir}/${PROJECT_NAME}ConfigVersion.cmake")
set(project_config "${generated_dir}/${PROJECT_NAME}Config.cmake")
set(TARGETS_EXPORT_NAME "${PROJECT_NAME}Targets")
set(namespace "${PROJECT_NAME}::")

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${version_config}" COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_SOURCE_DIR}/cmake/Config.cmake.in"
    ${project_config}
    INSTALL_DESTINATION "${config_install_dir}"
)

install(TARGETS glviz shader embed_resource
    EXPORT "${TARGETS_EXPORT_NAME}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

install(
    FILES src/glviz.hpp
          src/camera.hpp
          src/program.hpp
          src/buffer.hpp
          src/utility.hpp
          src/shader.hpp
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/GLviz"
)

install(
    FILES "${project_config}" "${version_config}"
    DESTINATION "${config_install_dir}"
)

install(
    FILES "${CMAKE_SOURCE_DIR}/cmake/Modules/GLvizShaderWrapCpp.cmake"
    DESTINATION "${CMAKE_INSTALL_MODULEDIR}"
)

install(
    EXPORT GLvizTargets
    NAMESPACE "${namespace}"
    DESTINATION "${config_install_dir}"
)
