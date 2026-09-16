#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Camera {
    float fov, aspectRatio;
    float pitch, yaw;
    glm::vec3 position, up, target;
    glm::mat4x4 matrix;

    void updateMatrix();

public:
    Camera();

    glm::mat4x4 getMatrix() { return matrix; }
    void resize(float width, float height);
    void update(GLFWwindow *window, float moveSpeed);
    void rotate(float dx, float dy);

    glm::vec3 getPosition() { return position; }
    void setPosition(glm::vec3 position) {
        this->position = position;
        updateMatrix();
    }

    glm::vec3 getTarget() { return target; }
    void setTarget(glm::vec3 target) {
        this->target = target;
        updateMatrix();
    }

    glm::vec3 getUp() { return up; }

    float getFov() { return fov; }
    void setFov(float fov) {
        this->fov = fov;
        updateMatrix();
    }
};
