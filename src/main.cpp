#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cassert>
#include <cmath>
#include <unistd.h>
#include <iostream>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <deque>
#include <stdexcept>
#include <stb_image.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

#include "camera.h"
#include "glm/ext/matrix_transform.hpp"
#include "shader.h"
#include "texture.h"
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
    bool show_fps, vsync_off, multisample;

    Args() : show_fps(false), vsync_off(false), multisample(false) {}
    Args(int argc, char **argv) : Args() {
        char opt;

        while ((opt = getopt(argc, argv, "vfm")) != -1) {
            switch (opt) {
            case 'v':
                vsync_off = true;
                break;
            case 'f':
                show_fps = true;
                break;
            case 'm':
                multisample = true;
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

    bool cameraEnabled;
    float mouseX, mouseY;

    // Vertex buffer object - handle to the vertex buffer on the gpu
    GLuint vbo;
    // Vertex array object - stores data telling the gpu how to interpret the vertex buffer
    GLuint vao;
    GLuint ebo;
    Shader shaderProgram;
    std::array<Texture, 2> textures;

    Camera camera;
    glm::mat4x4 projection;

    std::deque<double> frame_samples;

    void process_input(float dt) {
        float moveSpeed = 2 * dt;

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            cameraEnabled = false;
            ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
        } else if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            cameraEnabled = true;
            ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
        }

        if (cameraEnabled) {
            camera.update(window, moveSpeed);
        }
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
            shaderProgram.setMat4x4("view", camera.getMatrix());
            shaderProgram.setMat4x4("projection", projection);
            shaderProgram.setFloat("time", time);
            textures[m % 2].use();

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Initialises GLFW and OpenGL and sets up the window ready to be rendered to.
    void init_window(int width, int height) {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        if (args.multisample) {
            glfwWindowHint(GLFW_SAMPLES, 8);
        }

        window = glfwCreateWindow(width, height, "Hello OpenGL",
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
        resize(width, height);

        glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int width, int height) {
            App *app = static_cast<App*>(glfwGetWindowUserPointer(window));
            app->resize(width, height);
        });

        glfwSetCursorPosCallback(window, [](GLFWwindow *window, double x, double y) {
            App *app = static_cast<App*>(glfwGetWindowUserPointer(window));
            app->onMouseMove(x, y);
        });

        if (args.multisample) {
            glEnable(GL_MULTISAMPLE);
        }
        glEnable(GL_DEPTH_TEST);

        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init();

        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->AddFontDefaultVector();
        io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
        ImGui::GetStyle().FontSizeBase = 16;
    }

    void resize(int width, int height) {
        windowWidth = width;
        windowHeight = height;
        glViewport(0, 0, windowWidth, windowHeight);
        camera.resize(windowWidth, windowHeight);
    }

    void onMouseMove(float x, float y) {
        if (ImGui::GetIO().WantCaptureMouse)
            return;

        float dx = x - mouseX;
        float dy = mouseY - y;
        mouseX = x;
        mouseY = y;

        if (cameraEnabled) {
            camera.rotate(dx, dy);
        }
    }

public:
    App(Args args) :
        args(args), window(nullptr), bg_colour(21, 0, 54), shaderProgram(), projection(1.0),
        mouseX((float)WINDOW_WIDTH / 2), mouseY((float)WINDOW_HEIGHT / 2), cameraEnabled(false)
    {
        init_window(WINDOW_WIDTH, WINDOW_HEIGHT);
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

        textures[0] = Texture("assets/house md.jpg", GL_RGB);
        textures[1] = Texture("assets/don chan.png", GL_RGBA);
    }

    ~App() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
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
            glfwPollEvents();
            process_input(dt);

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::Begin("Hello OpenGL + IMGui");

            if (!cameraEnabled) {
                ImGui::Text("Press C to enter camera mode");
            } else {
                ImGui::Text("Press Esc to exit camera mode");
            }
            float fov = glm::degrees(camera.getFov());
            if (ImGui::SliderFloat("FOV", &fov, 1.0, 90.0)) {
                camera.setFov(glm::radians(fov));
            }

            ImGui::End();

            render();

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
