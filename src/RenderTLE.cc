//RenderTLE.cc

#include "RenderTLE.h"

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
#include <ctime>
#include <thread>

void RenderTLE::init(){
	float verts[TLE_RING_VERTS*3];
	
	for(int i=0; i<TLE_RING_VERTS; i++){
		float a = (float)i/TLE_RING_VERTS;
		
		a *= M_PI*2.0;
		
		verts[i*3 + 0] = (float)cos(a);
		verts[i*3 + 1] = 0.f;
		verts[i*3 + 2] = (float)sin(a);
	}
	
	glGenBuffers(1, &uModelBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, uModelBuffer);
	glBufferData(GL_UNIFORM_BUFFER, MAX_TLE_ELEMENTS * sizeof(mat4x4), NULL, GL_STATIC_DRAW);
	
	glGenBuffers(1, &vRingBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vRingBuffer);
	glBufferData(GL_ARRAY_BUFFER, TLE_RING_VERTS * 3 * sizeof(float), verts, GL_STATIC_DRAW);
	
	std::cout << "tle ring buffered." << std::endl;
	
	ring.sVert = CompileShader(LoadText("res/shaders/tle_vert.glsl"), GL_VERTEX_SHADER);
	ring.sFrag = CompileShader(LoadText("res/shaders/tle_frag.glsl"), GL_FRAGMENT_SHADER);
	
	ring.program = LinkShaders(ring.sVert, ring.sFrag);
	glUseProgram(ring.program);
	
	sat.sVert = CompileShader(LoadText("res/shaders/tle_sat_vert.glsl"), GL_VERTEX_SHADER);	
	sat.sFrag = ring.sFrag;
	sat.program = LinkShaders(sat.sVert, sat.sFrag);
	
	glUseProgram(sat.program);
	
	std::cout << "tle compiled and linked shaders." << std::endl;
	
	aPos = 0;
	
	glEnableVertexAttribArray(aPos);
	glVertexAttribPointer(aPos, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
	
	ring.uModel = glGetUniformBlockIndex(ring.program, "uModel");
	glUniformBlockBinding(ring.program, ring.uModel, 2);
	
	sat.uModel = glGetUniformBlockIndex(sat.program, "uModel");
	glUniformBlockBinding(sat.program, sat.uModel, 2);
	
	sat.uAngle = glGetUniformLocation(sat.program, "uAngle");
		
	uViewProjection = 0;
	uColor = 1;
	
}

RenderTLE::RenderTLE(){
	init();
}

RenderTLE::RenderTLE(Camera *cam){
	init();
	setCamera(cam);
}

RenderTLE::~RenderTLE(){
	//Nothing to do yet
}

void RenderTLE::setCamera(Camera *cam){
	camera = cam;
}

void _fillThread(mat4x4 *tle_m, std::vector<tle_t> &tle_v, int stride, int offset ){
	for(int i=offset; i<tle_v.size(); i += stride){
		TLEToMat4x4(tle_m[i], &tle_v[i]);
	}
}

bool RenderTLE::loadFile(std::string filename){
	if(!LoadTLE(tle_v, filename)){
		return false;
	}
	
	if(tle_v.size() > MAX_TLE_ELEMENTS)
		tle_v.resize(MAX_TLE_ELEMENTS);
	
	std::size_t thread_count = std::thread::hardware_concurrency();
	
	/* // Single threaded example
	for(int i=0; i<tle_v.size(); ++i){
		TLEToMat4x4(tle_model[i], &tle_v[i]);
	}
	*/
	
	std::vector<std::thread> fillThread(thread_count-1);
	
	for(int i=0; i<thread_count-1; ++i){
		fillThread[i] = std::thread(_fillThread, tle_model, std::ref(tle_v), thread_count, i);
	}
	
	_fillThread(tle_model, tle_v, thread_count, thread_count-1);
	
	for(int i=0; i<thread_count-1; ++i){
		fillThread[i].join();
	}
	
	glBindBuffer(GL_UNIFORM_BUFFER, uModelBuffer);
	glBufferData(GL_UNIFORM_BUFFER, tle_v.size() * sizeof(mat4x4), tle_model, GL_STATIC_DRAW);
	
	return true;
}

void _computePositionsFillThread(float *tle_angle, std::vector<tle_t> &tle_v, time_t t, float seconds, int stride, int offset){
	for(int i=offset; i<tle_v.size(); i+=stride){
		tle_angle[i] = TLEToAngle(&tle_v[i], t, seconds);
	}
}

void RenderTLE::computePositions(time_t t, float seconds){
	std::size_t thread_count = std::thread::hardware_concurrency();
	/*//Single threaded
	for(int i=0; i<tle_v.size(); ++i){
		tle_angle[i] = TLEToAngle(&tle_v[i], t);
	}
	*/
	
	std::vector<std::thread> threads(thread_count-1);
	
	for(int i=0; i<thread_count-1; ++i)
		threads[i] = std::thread(
			_computePositionsFillThread,
			tle_angle,
			std::ref(tle_v),
			t, seconds, 
			thread_count, i
		);
		
	_computePositionsFillThread(tle_angle, tle_v, t, seconds, thread_count, thread_count-1);
	
	for(int i=0; i<thread_count-1; ++i)
		threads[i].join();
}

void RenderTLE::draw(){
	mat4x4 orbit;
	
	tle_t tle;

	glUseProgram(ring.program);
	glUniformMatrix4fv(
		uViewProjection, 
		1, 
		GL_FALSE, 
		(const GLfloat*) camera->getTransform()
	);
	
	for(int i=0; i<tle_v.size(); i+=MAX_TLE_INSTANCES){
		int count = tle_v.size()-i;
		
		if(count > MAX_TLE_INSTANCES)
			count = MAX_TLE_INSTANCES;

		glUseProgram(ring.program);

		glBindBuffer(GL_UNIFORM_BUFFER, uModelBuffer);
		
		glUniformBlockBinding(ring.program, 2, uModelBuffer);
		glBindBufferRange(GL_UNIFORM_BUFFER, 2, uModelBuffer, i * sizeof(mat4x4), count * sizeof(mat4x4) ); 
		
		glBindBuffer(GL_ARRAY_BUFFER, vRingBuffer);
		
		glEnableVertexAttribArray(aPos);
		glVertexAttribPointer(aPos, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
		
		glUniform4f(uColor, 0.8f, 1.0f, 0.9f, 0.125f);	
		
		glDrawArraysInstanced(GL_LINE_LOOP, 0, TLE_RING_VERTS, count);
		
		//Render individual satellites
		glUseProgram(sat.program);
		
		glDisableVertexAttribArray(aPos);
		
		glUniformMatrix4fv(
			uViewProjection, 
			1, 
			GL_FALSE, 
			(const GLfloat*) camera->getTransform()
		);
		
		glBindBuffer(GL_UNIFORM_BUFFER, uModelBuffer);
		
		glUniformBlockBinding(sat.program, 2, uModelBuffer);
		glBindBufferRange(GL_UNIFORM_BUFFER, 2, uModelBuffer, i * sizeof(mat4x4), count * sizeof(mat4x4) ); 
		
		glUniform1fv(sat.uAngle, count, &tle_angle[i]);

		glUniform4f(uColor, 1.f, 1.f, 1.f, 0.5f);	
		
		glDrawArrays(GL_POINTS, 0, count);
	}
}