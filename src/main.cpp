#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <unistd.h>
#include <iostream>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <deque>
#include <stdexcept>
#include <stb_image.h>

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "shader.h"
#include "util.h"
#include "shaders/shaders.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define FPS_SAMPLES 10

float vertices[] = {
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
    -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f
};

const glm::vec3 cubePositions[] = {
    glm::vec3( 0.0f, 0.0f, 0.0f),
    glm::vec3( 2.0f, 5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3( 2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f, 3.0f, -7.5f),
    glm::vec3( 1.3f, -2.0f, -2.5f),
    glm::vec3( 1.5f, 2.0f, -2.5f),
    glm::vec3( 1.5f, 0.2f, -1.5f),
    glm::vec3(-1.3f, 1.0f, -1.5f)
};

const unsigned int indices[] = {
    0, 1, 3,
    1, 2, 3
};

struct Args {
    bool show_fps, vsync_off;

    Args() : show_fps(false) {}
    Args(int argc, char **argv) : Args() {
        char opt;

        while ((opt = getopt(argc, argv, "vf")) != -1) {
            switch (opt) {
            case 'v':
                vsync_off = true;
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
    float windowWidth;
    float windowHeight;
    colour bg_colour;
    Args args;

    // Vertex buffer object - handle to the vertex buffer on the gpu
    GLuint vbo;
    // Vertex array object - stores data telling the gpu how to interpret the vertex buffer
    GLuint vao;
    GLuint ebo;
    GLuint textures[2];
    Shader shaderProgram;

    float mix;
    glm::mat4x4 view;
    glm::mat4x4 projection;

    std::deque<double> frame_samples;

    void process_input(float dt) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            mix += 0.5 * dt;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            mix -= 0.5 * dt;
        }

        mix = std::clamp(mix, 0.0f, 1.0f);
    }

    void render() {
        set_clear_colour(bg_colour);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        double time = glfwGetTime();
        double offset = std::sin(time) * 0.5;

        glBindVertexArray(vao);

        for (int m = 0; m < 10; m++) {
            glm::mat4x4 model(1.0f);
            model = glm::translate(model, cubePositions[m]);
            model = glm::rotate(model, (float)time * glm::radians(20.0f * m),
                glm::vec3(0.5f, 1.0f, 0.0f));

            shaderProgram.use();
            shaderProgram.setFloat("horizOffset", offset);
            shaderProgram.setMat4x4("model", model);
            shaderProgram.setMat4x4("view", view);
            shaderProgram.setMat4x4("projection", projection);
            shaderProgram.setFloat("time", time);
            glBindTexture(GL_TEXTURE_2D, textures[m % 2]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(window);
    }

    // Initialises GLFW and OpenGL and sets up the window ready to be rendered to.
    void init_window() {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        //glfwWindowHint(GLFW_SAMPLES, 8);

        window = glfwCreateWindow(windowWidth, windowHeight, "Hello OpenGL",
            nullptr, nullptr);

        if (window == nullptr) {
            throw std::runtime_error("Couldn't initialise glfw window");
        }

        // Attach gl context to window
        glfwMakeContextCurrent(window);

        if (args.vsync_off) {
            glfwSwapInterval(0);
        } else {
            glfwSwapInterval(1);
        }

        // Use glad to load OpenGL extension functions
        if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
            throw std::runtime_error("Couldn't initialise GLAD");
        }

        // Set the window user pointer to this so we can access the god struct from glfw callbacks
        glfwSetWindowUserPointer(window, (void *)this);

        // Set the size of the viewport and set it to resize automatically
        glViewport(0, 0, windowWidth, windowHeight);
        glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int width, int height) {
            App *app = static_cast<App *>(glfwGetWindowUserPointer(window));
            app->resize(width, height);
        });

        //glEnable(GL_MULTISAMPLE);
        glEnable(GL_DEPTH_TEST);
    }

    void resize(int width, int height) {
        windowWidth = width;
        windowHeight = height;
        //projection = glm::perspective(
        //    glm::radians(45.0f), // fov
        //    (float)windowWidth / (float)windowHeight, // aspect ratio
        //    0.1f, // near
        //    100.0f // far
        //);
    }

public:
    App(Args args) :
        args(args), window(nullptr), windowWidth(WINDOW_WIDTH), windowHeight(WINDOW_HEIGHT),
        bg_colour(21, 0, 54), shaderProgram(), mix(0.3), view(1.0), projection(1.0)
    {
        init_window();
        shaderProgram = Shader(MODEL_VERT, MODEL_FRAG);

        // Initialise vbo and vao for the triangle
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*5, (void*) 0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)*5, (void*) sizeof(float[3]));
        glEnableVertexAttribArray(1);

        // Load textures, first one is a jpg with no transparency, second is a png with transparency
        glGenTextures(2, textures);

        int width, height, channels;
        // Need to flip the textures to align with the opengl rendering axes
        stbi_set_flip_vertically_on_load(true);
        unsigned char *data = stbi_load("assets/house md.jpg", &width, &height, &channels, 0);

        if (!data) {
            throw std::runtime_error("Couldn't load jpg");
        }

        glBindTexture(GL_TEXTURE_2D, textures[0]);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);

        data = stbi_load("assets/don chan.png", &width, &height, &channels, 0);

        if (!data) {
            throw std::runtime_error("Couldn't load jpg");
        }

        glBindTexture(GL_TEXTURE_2D, textures[1]);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);

        // Tell the gpu which texture corresponds to which varname in the shader
        shaderProgram.use();
        shaderProgram.setInt("texture1", 0);
        shaderProgram.setInt("texture2", 1);

        // note that we’re translating the scene in the reverse direction
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        projection = glm::perspective(
            glm::radians(45.0f), // fov
            (float)windowWidth / (float)windowHeight, // aspect ratio
            0.1f, // near
            100.0f // far
        );
    }

    ~App() {
        glfwTerminate();
    }

    // Just to be safe
    App(const App&) = delete;
    App& operator=(const App&) = delete;
    App(App&&) = delete;
    App& operator=(App&&) = delete;

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
        double dt = 0;
        std::deque<double> samples;

        // Render loop
        while (!glfwWindowShouldClose(window)) {
            process_input(dt);

            render();

            glfwPollEvents();

            double newTime = glfwGetTime();
            dt = newTime - time;
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
