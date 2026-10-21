#pragma once

#include "glm/ext/vector_int2.hpp"
#include <glad/gl.h>
#include <ft2build.h>
#include <unordered_map>
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

class TextRenderer {
    FT_Library ft;
    std::unordered_map<char, Character> characters;

public:
    TextRenderer();
    ~TextRenderer();
    TextRenderer(const TextRenderer&) = delete;
    TextRenderer& operator=(const TextRenderer&) = delete;
    // Don't feel like doing any move operator stuff since I'm never going to move this object

    void load_font(const char *filename, int pixelSize);
};
