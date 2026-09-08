cmake_minimum_required(VERSION 3.23)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(learnopengl)

add_executable(logl)

set(SHADERS_DIR ${CMAKE_SOURCE_DIR}/src/shaders)

add_custom_command(
  DEPENDS
    ${SHADERS_DIR}/texture.frag
    ${SHADERS_DIR}/texture.vert
  OUTPUT ${SHADERS_DIR}/shaders.h
  COMMAND runghc embed_shaders.hs
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_subdirectory(dependencies/glfw)
add_subdirectory(dependencies/glm)
find_package(OpenGL)

target_include_directories(logl PRIVATE dependencies/glad/include)
target_include_directories(logl PRIVATE dependencies/stb-image)

target_sources(logl
  PRIVATE
    src/main.cpp
    src/util.cpp
    src/shader.cpp
    src/shaders/shaders.h
    dependencies/glad/src/gl.c
    dependencies/stb-image/stb_image.cpp
)

target_link_libraries(logl
  PRIVATE
    glfw
    OpenGL::GL
    glm::glm)
