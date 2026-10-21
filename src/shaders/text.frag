#version 330 core

in vec2 outTex;

out vec4 colour;
uniform sampler2D glyphTexture;
uniform vec4 textColour;

void main() {
    vec4 glyphColour = vec4(1, 1, 1, texture(glyphTexture, outTex));
    colour = glyphColour * textColour;
}
