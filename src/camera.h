#pragma once

#include <glm/glm.hpp>

class Camera {
    glm::vec3 position;
    glm::vec3 up;
    glm::vec3 target;

    glm::mat4x4 view;

    void updateMatrix();

public:
    Camera();

    glm::mat4x4 getViewMatrix() { return view; }

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
};
