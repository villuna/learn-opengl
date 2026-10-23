#include "text.h"
#include "freetype/freetype.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "shaders/shaders.h"
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <utility>

Character::Character(FT_Face font, char c) {
    if (FT_Load_Char(font, c, FT_LOAD_RENDER)) {
        throw std::runtime_error("Couldn't load character");
    }

    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RED,
        font->glyph->bitmap.width,
        font->glyph->bitmap.rows,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        font->glyph->bitmap.buffer
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    size = glm::ivec2(font->glyph->bitmap.width, font->glyph->bitmap.rows);
    bearing = glm::ivec2(font->glyph->bitmap_left, font->glyph->bitmap_top);
    advance = font->glyph->advance.x;
}

Character::~Character() {
    if (texId != GL_NONE) {
        glDeleteTextures(1, &texId);
    }
}

Character::Character(Character&& other) :
    texId(std::exchange(other.texId, GL_NONE)), size(other.size), bearing(other.bearing),
    advance(other.advance)
{}

Character& Character::operator=(Character&& other) {
    texId = std::exchange(other.texId, GL_NONE);
    size = other.size;
    bearing = other.bearing;
    advance = other.advance;
    return *this;
}

void TextRenderer::init() {
    if (FT_Init_FreeType(&ft)) {
        throw std::runtime_error("Couldn't initialise FreeType");
    }

    shader = Shader(TEXT_VERT, TEXT_FRAG);
}

TextRenderer::~TextRenderer() {
    FT_Done_FreeType(ft);
}

size_t TextRenderer::load_font(const char *filename, int pixelSize) {
    FT_Face face;

    if (FT_New_Face(ft, filename, 0, &face))
        throw std::runtime_error("Couldn't load font");

    if (FT_Set_Pixel_Sizes(face, 0, pixelSize))
        throw std::runtime_error("Couldn't set font size");

    fonts.emplace_back();

    for (int i = 0; i < 128; i++) {
        try {
            fonts.back().try_emplace((char)i, face, (char)i);
        } catch (std::exception e) {
            std::cerr << "Warning: couldn't load character " << (char)i << std::endl;
        }
    }

    FT_Done_Face(face);

    return fonts.size() - 1;
}

Text TextRenderer::create_text(const std::string& text, size_t fontId, glm::vec2 pos) {
    Text t;
    t.text = text;
    t.fontId = fontId;

    glGenBuffers(1, &t.vbo);
    glGenVertexArrays(1, &t.vao);

    std::vector<float> data;
    data.reserve(text.size() * 6 * 4);

    float x = pos.x;
    float y = pos.y;

    for (int i = 0; i < text.size(); i++) {
        const Character& ch = fonts[fontId].at(text[i]);

        float xpos = x + ch.bearing.x;
        float ypos = y - ch.size.y + ch.bearing.y;

        std::array<float, 24> vertices {
            xpos, ypos + ch.size.y, 0, 0,
            xpos, ypos, 0, 1,
            xpos + ch.size.x, ypos, 1, 1,

            xpos, ypos + ch.size.y, 0, 0,
            xpos + ch.size.x, ypos, 1, 1,
            xpos + ch.size.x, ypos + ch.size.y, 1, 0
        };

        for (auto v = vertices.begin(); v != vertices.end(); v++) {
            data.push_back(*v);
        }

        x += (float)ch.advance / 64;
    }

    glBindVertexArray(t.vao);
    glBindBuffer(GL_ARRAY_BUFFER, t.vbo);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    t.data = std::move(data);

    return t;
}

void TextRenderer::render(const Text& text, glm::vec4 col, glm::vec2 windowSize) {
    shader.use();
    shader.setVec4("textColour", col);
    glBindVertexArray(text.vao);
    glm::mat4 projection = glm::ortho(0.f, windowSize.x, 0.f, windowSize.y);
    shader.setMat4x4("projection", projection);
    std::unordered_map<char, Character>& map = fonts.at(text.fontId);


    // Unfortunately I have to do a draw call for every character
    // since the glyphs are not on the same texture.
    // TODO font atlas?
    for (int i = 0; i < text.text.size(); i++) {
        glBindTexture(GL_TEXTURE_2D, map.at(text.text[i]).texId);
        glDrawArrays(GL_TRIANGLES, i * 6, 6);
    }
}

void Text::update() {
    float jitter = 1;

    auto randf = []() {
        return 2*(float)rand()/(float)RAND_MAX - 1;
    };

    for (int i = 0; i < data.size() / 24; i++) {
        // Since we have two vertices that are shared between two triangles we want to keep those
        // vertices together.
        // This would have been easier if I had used a vertex and index buffer. Maybe I should do
        // that. TODO.
        float randx[4];
        float randy[4];
        for (int i = 0; i < 4; i++) {
            randx[i] = randf() * jitter;
            randy[i] = randf() * jitter;
        }

        data[i * 24] += randx[0];
        data[i * 24 + 1] += randy[0];

        data[i * 24 + 4] += randx[1];
        data[i * 24 + 5] += randy[1];

        data[i * 24 + 8] += randx[2];
        data[i * 24 + 9] += randy[2];

        data[i * 24 + 12] += randx[0];
        data[i * 24 + 13] += randy[0];

        data[i * 24 + 16] += randx[2];
        data[i * 24 + 17] += randy[2];

        data[i * 24 + 20] += randx[3];
        data[i * 24 + 21] += randy[3];
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, data.size() * sizeof(float), data.data());
}
