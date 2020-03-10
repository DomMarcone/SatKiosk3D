#version 310 es

uniform mat4 uViewProjection;
uniform mat4 uModel;

/*layout(location = 0)*/ in vec3 aPos;
/*layout(location = 2)*/ in vec3 aNorm;
/*layout(location = 1)*/ in vec2 aCoord;

out vec2 vCoord;
out vec3 vNormal;

void main() {
	vCoord = aCoord;
	vec4 newNorm = uModel*vec4(aNorm, 1.0);
	vNormal = newNorm.xyz;
	gl_Position = uViewProjection*uModel*vec4(aPos, 1.0);
}
