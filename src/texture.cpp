#include "texture.h"
#include "stb_image.h"
#include <stdexcept>

Texture::Texture(const char *filename, int format, int filterMode) : Texture() {
    int channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(filename, &width, &height, &channels, 0);

    if (!data) {
        throw std::runtime_error("Couldn't load image");
    }

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMode);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
}

Texture::Texture(const char *filename, int format) : Texture(filename, format, GL_NEAREST) {}

Texture::~Texture() {
    if (id != GL_NONE)
        glDeleteTextures(1, &id);
}

void Texture::use() {
    glBindTexture(GL_TEXTURE_2D, id);
}
