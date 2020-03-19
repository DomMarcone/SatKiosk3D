//RenderTLE.h 
#ifndef _RENDER_TLE_H
#define _RENDER_TLE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Camera.h>
#include <CompileShader.h>
#include <LinkShaders.h>
#include <TLEType.h>
#include <TLEToMat4x4.h>
#include <TLEToAngle.h>
#include <LoadText.h>
#include <LoadTLE.h>

#include <linmath.h>

#include <iostream>
#include <cmath>
#include <vector>
#include <thread>
#include <utility>
#include <chrono>
#include <thread>

#define TLE_RING_VERTS 64

#define MAX_TLE_INSTANCES 256
#define MAX_TLE_ELEMENTS 4096

#ifndef M_PI
#define M_PI 3.14159265359
#endif //M_PI

class RenderTLE{
private :
	GLuint vRingBuffer;
	GLuint uModelBuffer;
	
	struct {
		GLuint sFrag, sVert, program;
		GLint uModel;
	} ring;
	
	struct {
		GLuint sFrag, sVert, program;
		GLint uModel, uAngle;
	} sat;
	
	GLint aPos;
	GLint uViewProjection, uColor;
	
	Camera *camera;
	
	std::vector<tle_t> tle_v;
	
	mat4x4 tle_model[MAX_TLE_ELEMENTS];
	float tle_angle[MAX_TLE_ELEMENTS];
	
	void init();
	
public :
	
	RenderTLE();
	RenderTLE(Camera *cam);
	
	~RenderTLE();
	
	void computeRings(std::chrono::system_clock::time_point current_time, bool all=false);
	void computePositions(std::chrono::system_clock::time_point current_time);
	
	void setCamera(Camera *cam);
	bool loadFile(std::string filename);
	
	void draw(bool draw_rings=true, bool draw_sats=true);
	
};

#endif //_RENDER_TLE_H
