//RenderEarth.cc

#include "RenderEarth.h"

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
#include <string>

RenderEarth::RenderEarth(Camera *cam){
	init();
	setCamera(cam);
}

RenderEarth::RenderEarth(){
	init();
}

void RenderEarth::init(){
	Image earth_image_diff, earth_image_night;
	std::string plyText;

	et.setRotationOffset(-3.1415928);
	
	plyText = LoadText("res/models/earth.ply");
	
	earth_model = PlyToFormat3D(plyText);
	
	plyText.clear();
	
	std::cout << "earth model loaded." << std::endl;
	
	LoadImagePNG(earth_image_diff, "res/images/map_diff.png");
	LoadImagePNG(earth_image_night, "res/images/map_night.png");
	std::cout << "earth images loaded." << std::endl;
	
	//Earth Shader
	sVert = CompileShader(LoadText("res/shaders/earth_vert.glsl"), GL_VERTEX_SHADER);
	sFrag = CompileShader(LoadText("res/shaders/earth_frag.glsl"), GL_FRAGMENT_SHADER);
	
	program = LinkShaders(sVert, sFrag);
	std::cout << "earth shaders compiled and linked." << std::endl;
	
	glUseProgram(program);
	uDiff = glGetUniformLocation(program, "uDiff");
	uNight = glGetUniformLocation(program, "uNight");
	uSunDirection = glGetUniformLocation(program, "uSunDirection");
	uViewProjection = glGetUniformLocation(program, "uViewProjection");
	uModel = glGetUniformLocation(program, "uModel");
		
	earth_texture_diff = ImageToTexture(earth_image_diff);
	earth_texture_night = ImageToTexture(earth_image_night);
	
	earth_drawer = new DrawFormat3D(program, earth_model);
}

RenderEarth::~RenderEarth(){
	glDeleteProgram(program);
	delete earth_drawer;
	DeleteFormat3D(earth_model);
}

void RenderEarth::setCamera(Camera *cam){
	camera = cam;
}

void RenderEarth::setTime(time_t t){
	et.setCurrentTime(t);
	sunDirection = et.getSunDirection();
}

vec3 *RenderEarth::getSunDirection(){ return sunDirection; }

void RenderEarth::draw(){
	glUseProgram(program);
	
	glUniformMatrix4fv(
		uModel, 
		1, 
		GL_FALSE, 
		(const GLfloat*) et.getTransform()
	);
	
	glUniformMatrix4fv(
		uViewProjection, 
		1, 
		GL_FALSE, 
		(const GLfloat*) camera->getTransform()
	);
	
	glUniform3fv(uSunDirection, 1, *sunDirection);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, earth_texture_diff);
	glUniform1i(uDiff, 0);
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, earth_texture_night);
	glUniform1i(uNight, 1);
	
	earth_drawer->draw();
}
