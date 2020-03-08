//CompileShader.h 
#ifndef _COMPILE_SHADER_H
#define _COMPILE_SHADER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

GLuint CompileShader(std::string source, GLenum type);

#endif //_COMPILE_SHADER_H
