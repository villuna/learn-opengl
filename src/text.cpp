#include "text.h"
#include "freetype/freetype.h"
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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

TextRenderer::TextRenderer() {
    if (FT_Init_FreeType(&ft)) {
        throw std::runtime_error("Couldn't initialise FreeType");
    }
}

TextRenderer::~TextRenderer() {
    FT_Done_FreeType(ft);
}

void TextRenderer::load_font(const char *filename, int pixelSize) {
    FT_Face face;

    if (FT_New_Face(ft, filename, 0, &face))
        throw std::runtime_error("Couldn't load font");

    if (FT_Set_Pixel_Sizes(face, 0, pixelSize))
        throw std::runtime_error("Couldn't set font size");

    for (int i = 0; i < 128; i++) {
        char c = (char)i;
        characters.insert({c, Character(face, c)});
    }

    FT_Done_Face(face);
}
