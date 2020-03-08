#version 310 es

precision mediump float;

uniform sampler2D uDiff;
uniform sampler2D uNight;

in vec2 vCoord;
in float vDayAmount;

out vec4 fragColor;

void main() {
	vec4 nightLights = vec4(1.0, 0.7, 0.2, 1.0);
    fragColor = max(vDayAmount, 0.25)*texture(uDiff, vCoord);
	fragColor += ((1.0-vDayAmount)*texture(uNight, vCoord).r) * nightLights;
	fragColor.a = 1.0;
}
