#include <cassert>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <deque>
#include <sstream>
#include <stdexcept>
#include "util.h"
#include "shaders/shaders.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define FPS_SAMPLES 10

const float vertices[] = {
     0.5,  0.5, 0.0, // top right
     0.5, -0.5, 0.0, // bottom right
    -0.5, -0.5, 0.0, // bottom left
    -0.5,  0.5, 0.0  // top left
};

const unsigned int indices[] = {
    0, 1, 3,
    1, 2, 3
};

struct Args {
    bool show_fps, vsync;

    Args() : show_fps(false) {}
    Args(int argc, char **argv) : Args() {
        char opt;

        while ((opt = getopt(argc, argv, "vf")) != -1) {
            switch (opt) {
            case 'v':
                vsync = true;
                break;
            case 'f':
                show_fps = true;
                break;
            default:
                throw std::runtime_error("couldn't parse command line args");
            }
        }
    }
};

class App {
    GLFWwindow *window;
    colour bg_colour;
    Args args;

    // Vertex buffer object - handle to the vertex buffer on the gpu
    unsigned int vbo;
    // Element buffer object - handle to the index buffer on the gpu
    unsigned int ebo;
    // Vertex array object - stores data telling the gpu how to interpret the vertex buffer
    unsigned int vao;

    unsigned int shaderProgram;

    std::deque<double> frame_samples;

    void process_input() {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }
    }

    void render() {
        set_clear_colour(bg_colour);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
    }

    // Checks to see if a shader compiled correctly and throws a runtime error if not
    void checkShader(unsigned int shaderId) {
        int success;
        char infoLog[512];
        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);

        if (!success) {
            glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
            std::stringstream errMsg;
            errMsg << "Error compiling shader: ";
            errMsg << infoLog;
            throw std::runtime_error(errMsg.str());
        }
    }

    void checkShaderProgram(unsigned int programId) {
        int success;
        char infoLog[512];
        glGetProgramiv(programId, GL_LINK_STATUS, &success);

        if (!success) {
            glGetProgramInfoLog(programId, 512, NULL, infoLog);
            std::stringstream errMsg;
            errMsg << "Error linking shader program: ";
            errMsg << infoLog;
            throw std::runtime_error(errMsg.str());
        }
    }

    // Initialises GLFW and OpenGL and sets up the window ready to be rendered to.
    void init_window() {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Hello OpenGL",
            nullptr, nullptr);

        if (window == nullptr) {
            throw std::runtime_error("Couldn't initialise glfw window");
        }

        // Attach gl context to window
        glfwMakeContextCurrent(window);

        if (args.vsync) {
            glfwSwapInterval(0);
        } else {
            glfwSwapInterval(1);
        }

        // Use glad to load OpenGL extension functions
        if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
            throw std::runtime_error("Couldn't initialise GLAD");
        }

        // Set the size of the viewport and set it to resize automatically
        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int width, int height) {
            glViewport(0, 0, width, height);
        });
    }

public:
    App(Args args) :
        args(args), window(nullptr), bg_colour(21, 0, 54)
    {
        init_window();

        // Initialise vbo and vao for the triangle
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // Create the vertex and index buffer
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
        glEnableVertexAttribArray(0);

        // Compile shaders
        unsigned int vertexShader, fragmentShader;

        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &TRIANGLE_VERT, NULL);
        glCompileShader(vertexShader);
        checkShader(vertexShader);

        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &TRIANGLE_FRAG, NULL);
        glCompileShader(fragmentShader);
        checkShader(fragmentShader);

        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    ~App() {
        glfwTerminate();
    }

    void update_fps_counter(double dt) {
        double sample = 1 / dt;
        frame_samples.push_back(sample);
        if (frame_samples.size() > FPS_SAMPLES) {
            frame_samples.pop_front();
        }

        double fps = 0;
        for (auto s = frame_samples.begin(); s != frame_samples.end(); s++) {
            fps += *s;
        }
        fps = fps / (double)frame_samples.size();

        if (args.show_fps) {
            std::cout << "fps: " << fps << std::endl;
        }
    }

    void run() {
        double time = glfwGetTime();
        std::deque<double> samples;

        // Render loop
        while (!glfwWindowShouldClose(window)) {
            process_input();

            render();

            glfwPollEvents();

            double newTime = glfwGetTime();
            double dt = newTime - time;
            time = newTime;
            update_fps_counter(dt);
        }
    }
};

int main(int argc, char **argv) {
    try {
        Args args(argc, argv);

        App app(args);
        app.run();

        return 0;
    } catch (std::runtime_error e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return -1;
    }
}
