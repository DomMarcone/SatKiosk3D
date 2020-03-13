//RenderMoon.h
#ifndef _RENDER_MOON_H
#define _RENDER_MOON_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Format3D.h>
#include <LoadFormat3D.h>
#include <Image.h>
#include <LoadImagePNG.h>
#include <Camera.h>
#include <CompileShader.h>
#include <LinkShaders.h>
#include <DrawFormat3D.h>
#include <ImageToTexture.h>
#include <TLEType.h>

#include <linmath.h>

#include <chrono>
#include <string>

#define MOON_ECCENTRICITY  0.0549f
#define MOON_MEAN_MOTION   0.036600943572f
#define MOON_MEAN_ANOMALY -0.162f //Estimated

class RenderMoon{
private :
	GLuint sFrag, sVert, program, moon_texture_diff;
	GLint uViewProjection, uModel, uSunDirection;
	GLint uDiff;
	
	Format3D *moon_model;
	DrawFormat3D *moon_drawer;
	
	vec3 *sunDirection;
	
	Camera *camera;
	
	tle_t tle;
	mat4x4 model, ring;
	
	void init();
	
public :
	
	RenderMoon(Camera *cam);
	RenderMoon();
	~RenderMoon();
	
	void setCamera(Camera *cam);
	void setTime(std::chrono::system_clock::time_point ct);
	void setSunDirection(vec3 *direction);
	
	void draw();
};

#endif //_RENDER_MOON_H
