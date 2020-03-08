#version 310 es

precision mediump float;

layout(location = 1) uniform vec4 uColor;

out vec4 fragColor;

void main() {
    fragColor = uColor;
}
