//RenderMoon.cc

#include "RenderMoon.h"

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
#include <intrin_linmath.h>

#include <chrono>
#include <string>

RenderMoon::RenderMoon(Camera *cam){
	init();
	setCamera(cam);
}

RenderMoon::RenderMoon(){
	init();
}

void RenderMoon::init(){
	Image moon_image_norm;
	std::string plyText;
	
	moon_model = LoadFormat3D("res/models/moon.ply");
	
	std::cout << "moon model loaded." << std::endl;
	
	LoadImagePNG(moon_image_norm, "res/images/moon_diff.png");
	std::cout << "moon images loaded." << std::endl;
	
	//Moon Shader
	sVert = CompileShader(LoadText("res/shaders/moon_vert.glsl"), GL_VERTEX_SHADER);
	sFrag = CompileShader(LoadText("res/shaders/moon_frag.glsl"), GL_FRAGMENT_SHADER);
	
	program = LinkShaders(sVert, sFrag);
	std::cout << "moon shaders compiled and linked." << std::endl;
	
	glUseProgram(program);
	uDiff = glGetUniformLocation(program, "uDiff");
	uSunDirection = glGetUniformLocation(program, "uSunDirection");
	uViewProjection = glGetUniformLocation(program, "uViewProjection");
	uModel = glGetUniformLocation(program, "uModel");
	
	moon_texture_diff = ImageToTexture(moon_image_norm);
	
	moon_drawer = new DrawFormat3D(program, moon_model);
	
	{
		mat4x4 radius, translate;

		float foci, a, b;
		float eccentricity = (1.f - MOON_ECCENTRICITY);
		
		mat4x4_identity(radius);
		
		b = 1.f/sqrt(
				MOON_ECCENTRICITY*MOON_ECCENTRICITY + 1.f
			);
			
		a = 1.f/b;
		
		radius[0][0] = 384399.f * b;
		radius[2][2] = 384399.f * a;
		
		foci = sqrt(
			radius[2][2]*radius[2][2] -
			radius[0][0]*radius[0][0]
		);
		
		mat4x4_translate(translate, 0.f, 0.f, foci);
		intrin_mat4x4_mul(ring, radius, translate);
	}	
}

RenderMoon::~RenderMoon(){
	glDeleteProgram(program);
	delete moon_drawer;
	DeleteFormat3D(moon_model);
}

void RenderMoon::setCamera(Camera *cam){
	camera = cam;
}

void RenderMoon::setTime(std::chrono::system_clock::time_point ct){
	vec4 position, angular_position_vec4;
	float angle;
	
	double period = 86400.0/MOON_MEAN_MOTION;
	
	std::chrono::duration<double> elapsed_seconds = ct.time_since_epoch();
	
	double angular_position_double = fmod(
		elapsed_seconds.count(), 
		period
	)/period;
	
	angular_position_double *= 3.14159265*2.0;
	angular_position_double += MOON_MEAN_ANOMALY;
	angular_position_double -= 3.14159256;
	
	angular_position_double += MOON_ECCENTRICITY * -cos(angular_position_double);
	
	angle = (float)angular_position_double;
	
	angular_position_vec4[0] = cosf(-angle);
	angular_position_vec4[1] = 0.f;
	angular_position_vec4[2] = sinf(-angle);
	angular_position_vec4[3] = 0.f;
	
	mat4x4_mul_vec4(position, ring, angular_position_vec4);
	
	mat4x4_translate(model, position[0], position[1], position[2]);
}

void RenderMoon::setSunDirection(vec3 *direction){ 
	sunDirection = direction;
}

void RenderMoon::draw(){
	glUseProgram(program);
	
	glUniformMatrix4fv(
		uModel, 
		1, 
		GL_FALSE, 
		(const GLfloat*) model
	);
	
	glUniformMatrix4fv(
		uViewProjection, 
		1, 
		GL_FALSE, 
		(const GLfloat*) camera->getTransform()
	);
	
	glUniform3fv(uSunDirection, 1, *sunDirection);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, moon_texture_diff);
	glUniform1i(uDiff, 0);
	
	moon_drawer->draw();
}
