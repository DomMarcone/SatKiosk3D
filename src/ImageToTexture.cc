//ImageToTexture.cc

#include "ImageToTexture.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Image.h>

#include <iostream>

GLenum _convertFormat(int bpp){
	switch(bpp){
	case 1 :
		return GL_RED;
		break;
	case 2 :
		return GL_RG;
		break;
	case 3 :
		return GL_RGB;
		break;
	case 4 :
		return GL_RGBA;
		break;
	default :
		std::cout << "Error : couldn't determine the image format!" << std::endl;
		break;
	}
	return 0;
}


GLuint ImageToTexture(Image &img){
	GLuint result;
	
	glGenTextures(1, &result);
	
	glBindTexture(GL_TEXTURE_2D, result);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glTexImage2D(GL_TEXTURE_2D, 0, 
		_convertFormat(img.getBytesPerPixel()), 
		img.getWidth(), 
		img.getHeight(), 
		0, _convertFormat(img.getBytesPerPixel()), 
		GL_UNSIGNED_BYTE, img.getData());
		
	glGenerateMipmap(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D, 0);
	
	return result;
}