#include <iostream>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <stdexcept>
#include "util.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

const colour CLEAR_COLOUR(21, 0, 54);

// Initialise GLFW and return the window
GLFWwindow *initialise() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Hello OpenGL",
        nullptr, nullptr);

    if (window == nullptr) {
        glfwTerminate();
        throw std::runtime_error("Couldn't initialise glfw window");
    }

    // Attach gl context to window
    glfwMakeContextCurrent(window);

    // Use glad to load OpenGL extension functions
    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
        throw std::runtime_error("Couldn't initialise GLAD");
    }

    // Set the size of the viewport and set it to resize automatically
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int width, int height) {
        glViewport(0, 0, width, height);
    });

    return window;
}

void process_input(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

int main() {
    try {
        GLFWwindow *window = initialise();

        // Render loop
        while (!glfwWindowShouldClose(window)) {
            process_input(window);

            // Rendering goes here
            clear_colour(CLEAR_COLOUR);
            glClear(GL_COLOR_BUFFER_BIT);

            glfwPollEvents();
            glfwSwapBuffers(window);
        }

        // goodbye
        glfwTerminate();
    } catch (std::exception e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}
