#include "camera.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include <algorithm>

Camera::Camera() :
    position(0, 0, 3), up(0, 1, 0), target(0, 0, -1), matrix(1.0), fov(glm::radians(40.0)),
    aspectRatio(1), pitch(0), yaw(glm::radians(-90.0))
{
    updateMatrix();
}

void Camera::updateMatrix() {
    glm::mat4x4 view = glm::lookAt(position, position + target, up);
    glm::mat4x4 projection = glm::perspective(
        fov,
        aspectRatio,
        0.1f, // near
        100.0f // far
    );

    matrix = projection * view;
}

void Camera::resize(float width, float height) {
    aspectRatio = width / height;
    updateMatrix();
}

void Camera::update(GLFWwindow *window, float moveSpeed) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        setPosition(position + target * moveSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        setPosition(position - target * moveSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        setPosition(position - glm::normalize(glm::cross(target, up)) * moveSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        setPosition(position + glm::normalize(glm::cross(target, up)) * moveSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        setPosition(position + up * moveSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        setPosition(position - up * moveSpeed);
    }
}

void Camera::rotate(float dx, float dy) {
    float sensitivity = 0.05;
    yaw += sensitivity * dx;
    pitch += sensitivity * dy;

    pitch = std::clamp(pitch, -89.0f, 89.0f);
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    target = glm::normalize(direction);

    updateMatrix();
}
