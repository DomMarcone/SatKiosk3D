//LinkShaders.h 
#ifndef _LINK_SHADERS_H
#define _LINK_SHADERS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

//Returns a shader program

GLuint LinkShaders(GLuint vert, GLuint frag);

#endif //_LINK_SHADERS_H
