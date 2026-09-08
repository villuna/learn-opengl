#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 col;
layout (location = 2) in vec2 texCoord;

out vec3 outColour;
out vec2 outTexCoord;

uniform float time;

void main() {
    gl_Position = vec4(pos, 1);
    gl_Position.xy = gl_Position.xy * mat2x2(cos(time), sin(time), -sin(time), cos(time));
    outColour = col;
    outTexCoord = texCoord;
}
