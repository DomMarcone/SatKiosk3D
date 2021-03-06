//RenderEarth.h
#ifndef _RENDER_EARTH_H
#define _RENDER_EARTH_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Format3D.h>
#include <LoadFormat3D.h>
#include <Image.h>
#include <LoadImagePNG.h>
#include <Camera.h>
#include <EarthTransform.h>
#include <CompileShader.h>
#include <LinkShaders.h>
#include <DrawFormat3D.h>
#include <ImageToTexture.h>

#include <linmath.h>

#include <chrono>
#include <string>

class RenderEarth{
private :
	GLuint sFrag, sVert, program, earth_texture_diff, earth_texture_night;
	GLint uViewProjection, uModel, uSunDirection;
	GLint uDiff, uNight;
	
	Format3D *earth_model;
	DrawFormat3D *earth_drawer;
	EarthTransform et;
	
	vec3 *sunDirection;
	
	Camera *camera;
	
	void init();
	
public :
	
	RenderEarth(Camera *cam);
	RenderEarth();
	~RenderEarth();
	
	void setCamera(Camera *cam);
	void setTime(std::chrono::system_clock::time_point ct);
	
	vec3 *getSunDirection();
	
	void draw();
};

#endif //_RENDER_EARTH_H
