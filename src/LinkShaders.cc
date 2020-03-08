//LinkShaders.cc

#include "LinkShaders.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

GLuint LinkShaders(GLuint vert, GLuint frag){
	GLuint result = glCreateProgram();
	
	glAttachShader(result, vert);
	glAttachShader(result, frag);
	
	glLinkProgram(result);
	
	//Begin error checking
	GLint isLinked = 0;
	
	glGetProgramiv(result, GL_LINK_STATUS, (int*)&isLinked);
	if(isLinked == GL_FALSE){
		GLint logSize = 0;
		glGetProgramiv(result, GL_INFO_LOG_LENGTH, &logSize);
		
		// The maxLength includes the NULL character
		GLchar *errorLog = new GLchar[logSize+1];
		glGetShaderInfoLog(result, logSize, &logSize, &errorLog[0]);
		
		errorLog[logSize] = 0;//stop reading! we're done!
		
		std::cout << "Program link error : " << std::endl;
		
		std::cout << errorLog << std::endl;
		
		//clean up the error log...
		delete errorLog;
		
		//TODO : finish this function
		glDeleteProgram(result);

		//pause for the user to see the error.
		std::cout << "paused..." << std::endl;
		std::string temp; std::cin >> temp;

		return -1;
	}
	
	glDetachShader(result, vert);
	glDetachShader(result, frag);
	
	return result;
}