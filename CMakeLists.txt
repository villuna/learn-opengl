cmake_minimum_required(VERSION 3.23)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(learnopengl)

add_executable(logl)

set(SHADERS_DIR ${CMAKE_SOURCE_DIR}/src/shaders)

add_custom_command(
  DEPENDS
    ${SHADERS_DIR}/triangle.frag
    ${SHADERS_DIR}/triangle.vert
    ${SHADERS_DIR}/triangle2.frag
  OUTPUT ${SHADERS_DIR}/shaders.h
  COMMAND runghc embed_shaders.hs
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_subdirectory(dependencies/glfw)
find_package(OpenGL)

target_include_directories(logl PRIVATE dependencies/glad/include)

target_sources(logl
  PRIVATE
    src/main.cpp
    src/util.cpp
    src/shaders/shaders.h
    dependencies/glad/src/gl.c
)

target_link_libraries(logl
  PRIVATE
    glfw
    OpenGL::GL)
