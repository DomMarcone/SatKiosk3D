//RenderSun.cc

#include "RenderSun.h"

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

RenderSun::RenderSun(Camera *cam){
	init();
	setCamera(cam);
}

RenderSun::RenderSun(){
	init();
}

unsigned int RenderSun::elements[6] = {
	0, 2, 1,
	1, 2, 3
};

void RenderSun::init(){
	float verts[] = {
		-1.f, 1.f, 0.f,
		1.f, 1.f, 0.f,
		-1.f, -1.f, 0.f,
		1.f, -1.f, 0.f,
	};
	
	
	Image sun_image;
	
	LoadImagePNG(sun_image, "res/images/sun_glare.png");
	std::cout << "sun image loaded." << std::endl;
	
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	//Sun Shader
	sVert = CompileShader(LoadText("res/shaders/sun_vert.glsl"), GL_VERTEX_SHADER);
	sFrag = CompileShader(LoadText("res/shaders/sun_frag.glsl"), GL_FRAGMENT_SHADER);
	
	program = LinkShaders(sVert, sFrag);
	std::cout << "sun shaders compiled and linked." << std::endl;
	
	glUseProgram(program);
	uTex = glGetUniformLocation(program, "uTex");
	uSunDirection = glGetUniformLocation(program, "uSunDirection");
	uViewProjection = glGetUniformLocation(program, "uViewProjection");
	uAspect = glGetUniformLocation(program, "uAspect");
	
	aPos = glGetAttribLocation(program, "aPos");
	glEnableVertexAttribArray(aPos);
	glVertexAttribPointer(aPos, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
				
	sun_texture = ImageToTexture(sun_image);
	
}

RenderSun::~RenderSun(){
	glDeleteBuffers(1, &buffer);
	glDeleteProgram(program);
}

void RenderSun::setCamera(Camera *cam){
	camera = cam;
}

void RenderSun::setDirection(vec3 *direction){
	sunDirection = direction;
	/**sunDirection[0] = 0.f;
	*sunDirection[1] = 0.f;
	*sunDirection[2] = 0.f;*/
}

void RenderSun::draw(){
	glUseProgram(program);
	
	glUniformMatrix4fv(
		uViewProjection, 
		1, 
		GL_FALSE, 
		(const GLfloat*) camera->getTransform()
	);
	
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	
	glEnableVertexAttribArray(aPos);
	glVertexAttribPointer(aPos, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
	
	glUniform1f(uAspect, camera->getAspect());
	
	glUniform3fv(uSunDirection, 1, *sunDirection);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sun_texture);
	glUniform1i(uTex, 0);
	
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, elements);
	
	glClear(GL_DEPTH_BUFFER_BIT);
}
