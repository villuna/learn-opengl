#include "camera.h"
#include "glm/ext/matrix_transform.hpp"

Camera::Camera() : position(0, 0, 3), up(0, 1, 0), target(0, 0, -1), view(1.0) {
    updateMatrix();
}

void Camera::updateMatrix() {
    view = glm::lookAt(position, position + target, up);
}
