#version 330 core
layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec3 vertexCol;

uniform float horizOffset;

out vec3 outCol;
out vec3 position;

void main() {
    vec4 outPos = vec4(vertexPos, 1.0);
    outPos.z = outPos.x + outPos.y;
    outPos.x += horizOffset;
    outCol = vertexCol;
    position = outPos.xyz;
    gl_Position = outPos;
}
