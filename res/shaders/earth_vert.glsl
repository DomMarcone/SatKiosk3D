#version 310 es

uniform mat4 uViewProjection;
uniform mat4 uModel;
uniform vec3 uSunDirection;

/*layout(location = 0)*/ in vec3 aPos;
/*layout(location = 2)*/ in vec3 aNorm;
/*layout(location = 1)*/ in vec2 aCoord;

out vec2 vCoord;
out float vDayAmount;

void main() {
	vCoord = aCoord;
	vec4 newNorm = uModel*vec4(aNorm, 1.0);
	vDayAmount = max(dot(newNorm.xyz, uSunDirection), 0.0);
	vDayAmount = min(vDayAmount*32.0, 1.0);
	gl_Position = uViewProjection*uModel*vec4(aPos, 1.0);
}
