#version 330 core
out vec4 fragColour;

in vec3 outCol;
in vec3 position;

void main() {
    fragColour = vec4(position, 1);
}
