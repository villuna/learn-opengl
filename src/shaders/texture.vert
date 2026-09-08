#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 col;
layout (location = 2) in vec2 texCoord;

out vec3 outColour;
out vec2 outTexCoord;

uniform mat4x4 trans;

void main() {
    gl_Position = trans * vec4(pos, 1);
    outColour = col;
    outTexCoord = texCoord;
}
