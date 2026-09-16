cmake_minimum_required(VERSION 3.23)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(learnopengl)

add_executable(logl)

set(SHADERS_DIR ${CMAKE_SOURCE_DIR}/src/shaders)

add_custom_command(
  DEPENDS
    ${SHADERS_DIR}/model.frag
    ${SHADERS_DIR}/model.vert
  OUTPUT ${SHADERS_DIR}/shaders.h
  COMMAND python embed_shaders.py
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_subdirectory(dependencies/glfw)
add_subdirectory(dependencies/glm)
find_package(OpenGL)

target_include_directories(logl PRIVATE dependencies/glad/include)
target_include_directories(logl PRIVATE dependencies/stb-image)
target_include_directories(logl PRIVATE dependencies/imgui)
target_include_directories(logl PRIVATE dependencies/imgui/backends)

target_sources(logl
  PRIVATE
    src/main.cpp
    src/util.cpp
    src/shader.cpp
    src/shaders/shaders.h
    src/texture.cpp
    src/camera.cpp

    dependencies/glad/src/gl.c
    dependencies/stb-image/stb_image.cpp

    dependencies/imgui/imgui.cpp
    dependencies/imgui/imgui_demo.cpp
    dependencies/imgui/imgui_draw.cpp
    dependencies/imgui/imgui_tables.cpp
    dependencies/imgui/imgui_widgets.cpp
    dependencies/imgui/misc/cpp/imgui_stdlib.cpp
    dependencies/imgui/backends/imgui_impl_opengl3.cpp
    dependencies/imgui/backends/imgui_impl_glfw.cpp
)

target_link_libraries(logl
  PRIVATE
    glfw
    OpenGL::GL
    glm::glm)
