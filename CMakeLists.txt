cmake_minimum_required(VERSION 3.23)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(learnopengl)

add_executable(logl)

add_subdirectory(dependencies/glfw)
find_package(OpenGL)

target_include_directories(logl PRIVATE dependencies/glad/include)

target_sources(logl
  PRIVATE
    src/main.cpp
    dependencies/glad/src/gl.c
)

target_link_libraries(logl
  PRIVATE
    glfw
    OpenGL::GL)
