#version 310 es

precision mediump float;

uniform sampler2D uTex;

in vec2 vCoord;

out vec4 fragColor;

void main() {
	fragColor = texture(uTex, vCoord);
}
