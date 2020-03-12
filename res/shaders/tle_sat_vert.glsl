#version 310 es

layout(location = 0) uniform mat4 uViewProjection;
layout(std140, binding = 2) uniform uModel {
	mat4 model[256];
};

//layout(location = 0) in vec3 aPos;
uniform float uAngle[256];

void main() {
	vec4 pos = vec4(
		cos(uAngle[gl_VertexID]),
		0.0,
		sin(uAngle[gl_VertexID]),
		1.0
	);

	gl_Position = uViewProjection * model[gl_VertexID] * pos;
	gl_PointSize = (4500.0) / exp2(log(gl_Position.z));//No intense reasoning behind this
	gl_PointSize = max(gl_PointSize, 2.0);
}
