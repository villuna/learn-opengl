constexpr const char *MODEL_FRAG = R"---(#version 330 core
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
)---";

constexpr const char *MODEL_VERT = R"---(#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 outTexCoord;

void main() {
    gl_Position = projection * view * model * vec4(pos, 1);
    outTexCoord = texCoord;
}
)---";
