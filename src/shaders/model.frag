#version 330 core
out vec4 fragColour;

in vec2 outTexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

uniform float mixAmount;
uniform float time;

void main() {
    fragColour = mix(texture(texture1, outTexCoord),
                     texture(texture2, outTexCoord), mixAmount);
    fragColour = mix(fragColour, vec4(1, 0, 0, 0), sin(time));
}
