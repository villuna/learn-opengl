#version 330 core
out vec4 fragColour;

in vec3 outColour;
in vec2 outTexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

uniform float mixAmount;

void main() {
    fragColour = mix(texture(texture1, outTexCoord),
                     texture(texture2, outTexCoord), mixAmount);
}
