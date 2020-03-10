#version 310 es

//precision mediump float;

uniform mat4 uViewProjection;
uniform vec3 uSunDirection;
uniform float uAspect;

/*layout(location = 0)*/ in vec3 aPos;

out vec2 vCoord;

void main() {
	vec4 pos = uViewProjection * vec4(1.5e11 * uSunDirection, 1.0);
	
	//Because we're only rendering 6 verts, branching shouldn't cause problems.
	if(pos.w < 0.0){
		gl_Position = vec4(0.0, 0.0, 0.0, 1.0);
	} else {
		pos = pos/pos.w;
		
		gl_Position = pos + vec4(aPos, 1.0);
		gl_Position.y *= uAspect;
		gl_Position.z = -1.0;
	}
	
	vCoord = (aPos.xy/2.0) + vec2(0.5);
}
