constexpr const char *MODEL_FRAG = R"---(#version 330 core

vec3 hsv2rgb(vec3 c) {
  vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
  vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
  return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

vec3 hsv2rgb(float h, float s, float v) {
    return hsv2rgb(vec3(h, s, v));
}

out vec4 fragColour;

in vec2 outTexCoord;

uniform sampler2D cubeTexture;

uniform float time;

void main() {
    float colourSpeed = 0.2;

    fragColour = texture(cubeTexture, outTexCoord);
    fragColour = fragColour * vec4(hsv2rgb(mod(time * colourSpeed, 1), 0.5, 1), 1);
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
