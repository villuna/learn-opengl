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

#include "glm/ext/matrix_transform.hpp"
#include "shader.h"
#include "util.h"
#include "shaders/shaders.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define FPS_SAMPLES 10

struct vertex {
    float pos[3];
    float col[3];
    float tex_coord[2];
};

vertex vertices[] = {
    {.pos = {0.5f, 0.5f, 0.0f}, .col = {1.0f, 0.0f, 0.0f}, .tex_coord = {2.0f, 2.0f}}, // top right
    {.pos = {0.5f, -0.5f, 0.0f}, .col = {0.0f, 1.0f, 0.0f}, .tex_coord = {2.0f, 0.0f}}, // bottom right
    {.pos = {-0.5f, -0.5f, 0.0f}, .col = {0.0f, 0.0f, 1.0f}, .tex_coord = {0.0f, 0.0f}}, // bottom left
    {.pos = {-0.5f, 0.5f, 0.0f}, .col = {1.0f, 1.0f, 0.0f}, .tex_coord = {0.0f, 2.0f}} // top left
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
    glm::mat4x4 trans;

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
        glClear(GL_COLOR_BUFFER_BIT);
        double time = glfwGetTime();
        double offset = std::sin(time) * 0.5;

        trans = glm::mat4x4(1.0);
        trans = glm::translate(trans, glm::vec3(0.7, 0.2, 0.0));
        trans = glm::rotate(trans, (float)time, glm::vec3(0, 0, 1));

        shaderProgram.use();
        shaderProgram.setFloat("horizOffset", offset);
        shaderProgram.setFloat("mixAmount", mix);
        shaderProgram.setMat4x4("trans", trans);
        for (int i = 0; i < 2; i++) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, textures[i]);
        }
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
    }

    // Initialises GLFW and OpenGL and sets up the window ready to be rendered to.
    void init_window() {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_SAMPLES, 8);

        window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Hello OpenGL",
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

        // Set the size of the viewport and set it to resize automatically
        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int width, int height) {
            glViewport(0, 0, width, height);
        });

        glEnable(GL_MULTISAMPLE);
    }

public:
    App(Args args) :
        args(args), window(nullptr), bg_colour(21, 0, 54),
        shaderProgram(), mix(0.3), trans(1.0)
    {
        init_window();
        shaderProgram = Shader(TEXTURE_VERT, TEXTURE_FRAG);

        // Initialise vbo and vao for the triangle
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*) 0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*) sizeof(float[3]));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*) sizeof(float[6]));
        glEnableVertexAttribArray(2);

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
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);

        // Tell the gpu which texture corresponds to which varname in the shader
        shaderProgram.use();
        shaderProgram.setInt("texture1", 0);
        shaderProgram.setInt("texture2", 1);

        trans = glm::scale(trans, glm::vec3(1.5));
        trans = glm::rotate(trans, glm::radians(45.0f), glm::vec3(0, 0, 1));
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
