//RenderTLE.cc

#include "RenderTLE.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Camera.h>
#include <CompileShader.h>
#include <LinkShaders.h>
#include <TLEType.h>
#include <SGPToMat4x4.h>
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
	glBufferData(GL_UNIFORM_BUFFER, MAX_TLE_ELEMENTS * sizeof(mat4x4), NULL, GL_DYNAMIC_DRAW);
	
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

bool RenderTLE::loadFile(std::string filename){
	if(!LoadTLE(sgp_v, filename)){
		return false;
	}
	
	if(sgp_v.size() > MAX_TLE_ELEMENTS)
		sgp_v.resize(MAX_TLE_ELEMENTS);
	
	return true;
}

void _fillThread(mat4x4 *tle_m, float *tle_angle, std::vector<sgp4_t> &sgp_v, std::chrono::system_clock::time_point current_time, int stride, int offset ){
	for(int i=offset; i<sgp_v.size(); i += stride){
		sgp4_solve_at_time(&sgp_v[i], current_time);
		SGPToMat4x4(tle_m[i], &sgp_v[i]);
		tle_angle[i] = -sgp_v[i].uk + 3.14159265f/2.f;
	}
}

void RenderTLE::compute(std::chrono::system_clock::time_point current_time){
	std::size_t thread_count = std::thread::hardware_concurrency();
	/* // Single threaded example
	for(int i=0; i<sgp_v.size(); ++i){
		sgp4_solve_at_time(&sgp_v[i], current_time);
		SGPToMat4x4(tle_model[i], &sgp_v[i]);
	}
	*/
	std::vector<std::thread> fillThread(thread_count-1);
		
	for(int i=0; i<thread_count-1; ++i){
		fillThread[i] = std::thread(_fillThread, tle_model, tle_angle, std::ref(sgp_v), current_time, thread_count, i);
	}
	
	_fillThread(tle_model, tle_angle, sgp_v, current_time, thread_count, thread_count-1);
	
	for(int i=0; i<thread_count-1; ++i){
		fillThread[i].join();
	}
	
	glBindBuffer(GL_UNIFORM_BUFFER, uModelBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sgp_v.size() * sizeof(mat4x4), tle_model, GL_DYNAMIC_DRAW);
}

void RenderTLE::draw(bool draw_rings, bool draw_sats){
	mat4x4 orbit;
	
	tle_t tle;
	
	float ring_visiblity = 1.f/sgp_v.size();
	if(ring_visiblity < 0.06125f)
		ring_visiblity = 0.06125f;
	
	if(ring_visiblity > 0.5f)
		ring_visiblity = 0.5f;
	
	if(!draw_rings && !draw_sats)
		return;

	glUseProgram(ring.program);
	glUniformMatrix4fv(
		uViewProjection, 
		1, 
		GL_FALSE, 
		(const GLfloat*) camera->getTransform()
	);
	
	for(int i=0; i<sgp_v.size(); i+=MAX_TLE_INSTANCES){
		int count = sgp_v.size()-i;
		
		if(count > MAX_TLE_INSTANCES)
			count = MAX_TLE_INSTANCES;
		
		if(draw_rings){
			glUseProgram(ring.program);

			glBindBuffer(GL_UNIFORM_BUFFER, uModelBuffer);
			
			glUniformBlockBinding(ring.program, 2, uModelBuffer);
			glBindBufferRange(GL_UNIFORM_BUFFER, 2, uModelBuffer, i * sizeof(mat4x4), count * sizeof(mat4x4) ); 
			
			glBindBuffer(GL_ARRAY_BUFFER, vRingBuffer);
			
			glEnableVertexAttribArray(aPos);
			glVertexAttribPointer(aPos, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
			
			glUniform4f(uColor, 0.8f, 1.0f, 0.9f, ring_visiblity);	
			
			glDrawArraysInstanced(GL_LINE_LOOP, 0, TLE_RING_VERTS, count);
			
			glDisableVertexAttribArray(aPos);
		}
		
		if(draw_sats){
			glUseProgram(sat.program);
			
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
}