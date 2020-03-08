//CompileShader.cc

#include "CompileShader.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

GLuint CompileShader(std::string source, GLenum type){
	GLuint result;
	const char *source_p = source.c_str();
	int source_s = source.size();
	
	result = glCreateShader(type);
	
	glShaderSource(result, 1, &source_p, &source_s);
	
	glCompileShader(result);
	
	//Begin the error handling...
	GLint success = 0;
	glGetShaderiv(result, GL_COMPILE_STATUS, &success);
	
	if(success == GL_FALSE){
		GLint logSize = 0;
		glGetShaderiv(result, GL_INFO_LOG_LENGTH, &logSize);
		
		// The maxLength includes the NULL character
		GLchar *errorLog = new GLchar[logSize+1];
		glGetShaderInfoLog(result, logSize, &logSize, &errorLog[0]);
		
		std::cout << "Shader compile error : " << std::endl;
		
		std::cout << errorLog << std::endl;
		
		//clean up the error log...
		delete errorLog;
		
		glDeleteShader(result);

		//pause for the user to see the error.
		std::cout << "paused..." << std::endl;
		std::string temp; std::cin >> temp;

		return -1;
	}
	
	return result;
}
