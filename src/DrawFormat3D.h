//DrawFormat3D.h

#ifndef _DRAW_FORMAT_3D_H
#define _DRAW_FORMAT_3D_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Format3D.h>

class DrawFormat3D{
private :
	
	Format3D *object = 0;
	
	GLuint sp = -1;//Shader Program
	GLuint vBuffer;
	
	bool valid_pos, valid_norm, valid_coord, valid_color;
	GLint aPos, aNorm, aCoord, aColor;
	
	int stride;
	int aPosOffset, aNormOffset, aCoordOffset, aColorOffset;
	
	bool complete;
	
	void link();
	
public :
	
	DrawFormat3D(GLuint sp, Format3D *o);
	DrawFormat3D();
	~DrawFormat3D();
	
	void attachObject(Format3D *o);
	void attachProgram(GLuint s);
	
	void draw();
};

#endif //_DRAW_FORMAT_3D_H
