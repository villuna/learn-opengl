#version 330 core

layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 tex;

uniform mat4x4 projection;

out vec2 outTex;

void main() {
    gl_Position = projection * vec4(pos, 0, 1);
    outTex = tex;
}
