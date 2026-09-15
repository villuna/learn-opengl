#pragma once

#include <glad/gl.h>
#include <utility>

struct Texture {
    GLuint id;
    int width, height;

    Texture() : id(GL_NONE), width(0), height(0) {}
    Texture(const char *filename, int format, int filterMode);
    Texture(const char *filename, int format);
    ~Texture();

    // Rule of 5
    Texture(const Texture& other) = delete;
    Texture& operator=(const Texture& other) = delete;
    Texture(Texture&& other) {
        std::swap(other.id, this->id);
        std::swap(other.height, this->height);
        std::swap(other.width, this->width);
    }
    Texture& operator=(Texture&& other) {
        std::swap(other.id, this->id);
        std::swap(other.height, this->height);
        std::swap(other.width, this->width);
        return *this;
    }

    void use();
};
