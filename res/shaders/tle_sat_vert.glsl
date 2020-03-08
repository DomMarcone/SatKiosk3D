#version 310 es

layout(location = 0) uniform mat4 uViewProjection;
layout(std140, binding = 2) uniform uModel {
	mat4 model[128];
};

//layout(location = 0) in vec3 aPos;
uniform float uAngle[128];

void main() {
	vec4 pos = vec4(
		cos(uAngle[gl_VertexID]),
		0.0,
		sin(uAngle[gl_VertexID]),
		1.0
	);

	gl_Position = uViewProjection * model[gl_VertexID] * pos;
	gl_PointSize = 2.35 * 3.0;//No intense reasoning behind this
}
