#version 330 core
out vec4 fragColour;

in vec3 outColour;
in vec2 outTexCoord;

uniform sampler2D textureSampler;

void main() {
    fragColour = texture(textureSampler, outTexCoord) * vec4(outColour, 1.0);
}
