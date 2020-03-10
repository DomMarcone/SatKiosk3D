#version 310 es

precision mediump float;

uniform sampler2D uDiff;
uniform sampler2D uNight;

uniform vec3 uSunDirection;

in vec2 vCoord;
in vec3 vNormal;

out vec4 fragColor;

void main() {
	float dayAmount = max(dot(vNormal, uSunDirection), 0.0);
        dayAmount = min(dayAmount*8.0, 1.0);
	
	vec4 nightLights = vec4(1.0, 0.7, 0.2, 1.0);
	fragColor = max(dayAmount, 0.25)*texture(uDiff, vCoord);
	fragColor += ((1.0-dayAmount)*texture(uNight, vCoord).r) * nightLights;
	fragColor.a = 1.0;
}
