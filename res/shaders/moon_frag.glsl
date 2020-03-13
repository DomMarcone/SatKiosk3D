#version 310 es

precision mediump float;

uniform sampler2D uDiff;

uniform vec3 uSunDirection;

in vec2 vCoord;
in vec3 vNormal;

out vec4 fragColor;

void main() {
	vec3 norm = normalize(vNormal);
	
	float light = 2.0*max(dot(norm, uSunDirection), 0.05);
	
	fragColor = light * texture(uDiff, vCoord).r*vec4(1.0, 1.0, 1.0, 1.0);
	fragColor.a = 1.0;
}
