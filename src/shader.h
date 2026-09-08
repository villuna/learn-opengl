#pragma once

#include <glad/gl.h>
#include <string>
#include <glm/glm.hpp>
#include <utility>

class Shader {
public:
    GLuint id;

    Shader() noexcept : id(GL_NONE) {}
    Shader(const char *vertex, const char *fragment);
    ~Shader();

    // Rule of 5 stuff. The learn opengl tutorial just has the shader silently leak memory...
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    Shader(Shader&& other) {
        std::swap(this->id, other.id);
    }
    Shader& operator=(Shader&& other) {
        std::swap(this->id, other.id);
        return *this;
    }

    void use();

    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setMat4x4(const std::string& name, glm::mat4x4 value) const;
};
