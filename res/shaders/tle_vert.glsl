#version 310 es

layout(location = 0) uniform mat4 uViewProjection;
layout(std140, binding = 2) uniform uModel {
	mat4 model[256];
};

layout(location = 0) in vec3 aPos;

void main() {
	gl_Position = uViewProjection * model[gl_InstanceID] * vec4(aPos, 1.0);
}
