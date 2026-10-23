#pragma once

#include "shader.h"
#include <glm/glm.hpp>
#include <glad/gl.h>
#include <ft2build.h>
#include <string>
#include <unordered_map>
#include <utility>
#include FT_FREETYPE_H

struct Character {
    GLuint texId;
    glm::ivec2 size;
    glm::ivec2 bearing;
    int advance;

    Character(FT_Face face, char c);
    ~Character();

    Character(const Character&) = delete;
    Character& operator=(const Character&) = delete;

    Character(Character&& other);
    Character& operator=(Character&& other);
};

class Text {
    GLuint vbo, vao;
    std::vector<float> data;
    size_t fontId;
    std::string text;

public:
    Text() : vbo(GL_NONE), vao(GL_NONE) {}

    ~Text() {
        glDeleteTextures(1, &vbo);
        glDeleteVertexArrays(1, &vao);
    }

    Text(const Text& other) = delete;
    Text& operator=(const Text& other) = delete;

    Text(Text&& other) :
        vao(std::exchange(other.vao, GL_NONE)),
        vbo(std::exchange(other.vbo, GL_NONE)),
        data(std::move(other.data)),
        fontId(other.fontId),
        text(std::move(other.text))
    {}

    Text& operator=(Text&& other) {
        vao = std::exchange(other.vao, GL_NONE);
        vbo = std::exchange(other.vbo, GL_NONE);
        data = std::move(other.data);
        fontId = other.fontId;
        text = std::move(other.text);
        return *this;
    }

    void update();

    friend class TextRenderer;
};

class TextRenderer {
    FT_Library ft;
    std::vector<std::unordered_map<char, Character>> fonts;
    Shader shader;

public:
    TextRenderer() {}
    ~TextRenderer();
    TextRenderer(const TextRenderer&) = delete;
    TextRenderer& operator=(const TextRenderer&) = delete;
    // Don't feel like doing any move operator stuff since I'm never going to move this object

    void init();
    size_t load_font(const char *filename, int pixelSize);
    Text create_text(const std::string& text, size_t fontId, glm::vec2 pos);
    void render(const Text& text, glm::vec4 col, glm::vec2 windowSize);
};
