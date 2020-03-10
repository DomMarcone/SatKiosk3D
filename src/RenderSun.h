//RenderSun.h
#ifndef _RENDER_SUN_H
#define _RENDER_SUN_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <LoadText.h>
#include <Image.h>
#include <LoadImagePNG.h>
#include <Camera.h>
#include <CompileShader.h>
#include <LinkShaders.h>
#include <ImageToTexture.h>

#include <linmath.h>

#include <ctime>
#include <string>

class RenderSun{
private :
	GLuint sFrag, sVert, program, buffer, sun_texture;
	GLint uSunDirection, uViewProjection, uAspect, aPos;
	GLint uTex;
	
	vec3 *sunDirection;
	
	static unsigned int elements[6];
	
	Camera *camera;
	
	void init();
	
public :
	
	RenderSun(Camera *cam);
	RenderSun();
	~RenderSun();
	
	void setCamera(Camera *cam);
	void setDirection(vec3 *direction);
	
	void draw();
};

#endif //_RENDER_SUN_H
