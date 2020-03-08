//RenderEarth.h
#ifndef _RENDER_EARTH_H
#define _RENDER_EARTH_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Format3D.h>
#include <LoadText.h>
#include <PlyToFormat3D.h>
#include <Image.h>
#include <LoadImagePNG.h>
#include <Camera.h>
#include <EarthTransform.h>
#include <CompileShader.h>
#include <LinkShaders.h>
#include <DrawFormat3D.h>
#include <ImageToTexture.h>

#include <linmath.h>

#include <ctime>

class RenderEarth{
private :
	GLuint sFrag, sVert, program, earth_texture_diff, earth_texture_night;
	GLint uViewProjection, uModel, uSunDirection;
	GLint uDiff, uNight;
	
	Format3D *earth_model;
	DrawFormat3D *earth_drawer;
	EarthTransform et;
	
	Camera *camera;
	
	void init();
	
public :
	
	RenderEarth(Camera *cam);
	RenderEarth();
	~RenderEarth();
	
	void setCamera(Camera *cam);
	void setTime(time_t t);
	
	void draw();
};

#endif //_RENDER_EARTH_H
