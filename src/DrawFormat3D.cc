//DrawFormat3D.cc

#include "DrawFormat3D.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Format3D.h>

#include <iostream>

DrawFormat3D::DrawFormat3D(GLuint shaderProgram, Format3D *obj){
	attachObject(obj);
	attachProgram(shaderProgram);
	
	link();
}

DrawFormat3D::DrawFormat3D(){
	complete = false;
}

void DrawFormat3D::attachObject(Format3D *o){
	object = o;
	link();
}

void DrawFormat3D::attachProgram(GLuint s){
	if(!glIsProgram(s)){
		std::cout << "DrawFormat3D::attachProgram() : Error! Invalid program!" << std::endl;
		return;
	}
	
	sp = s;
	link();
}


void DrawFormat3D::link(){
	int offset = 0;
	
	//Can we link objects?
	if(sp==-1 || object==0){
		complete = false;
		return;
	}
	
	valid_pos = false;
	valid_coord = false;
	valid_norm = false;
	valid_color = false;
	
	//Beware... there is flawed logic here
	//We're relying on Format3D to be completely valid
	for(int i=0; i<object->attribCount; ++i){
		switch(object->attribs[i]){
		case FormatX :
			valid_pos = true;
			aPosOffset = offset;
			offset += 3 * sizeof(float);
			break;
		
		case FormatNX :
			valid_norm = true;
			aNormOffset = offset;
			offset += 3 * sizeof(float);
			break;

		case FormatS :
			valid_coord = true;
			aCoordOffset = offset;
			offset += 2 * sizeof(float);
			break;
		
		case FormatR :
			valid_color = true;
			aColorOffset = offset;
			offset += 3 * sizeof(float);
			break;
		}
	}
	
	stride = offset;
	
	glGenBuffers(1, &vBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	glBufferData(GL_ARRAY_BUFFER, object->vertCount * object->attribCount * sizeof(float), object->verts, GL_STATIC_DRAW);
	
	//add attrib if attrib exists in the vertex data
	if(valid_pos) {
		aPos = glGetAttribLocation(sp, "aPos");
		if(aPos!=-1){
			glEnableVertexAttribArray(aPos);
			glVertexAttribPointer(aPos, 3, GL_FLOAT, GL_FALSE,
				stride, (void*) aPosOffset);
		} else 
			valid_pos = false;
	}
	
	if(valid_norm) {
		aNorm = glGetAttribLocation(sp, "aNorm");
		if(aNorm!=-1){
			glEnableVertexAttribArray(aNorm);
			glVertexAttribPointer(aNorm, 3, GL_FLOAT, GL_FALSE,
				stride, (void*) aNormOffset);
		} else 
			valid_norm = false;
	}	
	
	if(valid_coord) {
		aCoord = glGetAttribLocation(sp, "aCoord");
		if(aCoord!=-1){
			glEnableVertexAttribArray(aCoord);
			glVertexAttribPointer(aCoord, 2, GL_FLOAT, GL_FALSE,
				stride, (void*) aCoordOffset);
		} else 
			valid_coord = false;
	}
	
	if(valid_color) {
		aColor = glGetAttribLocation(sp, "aColor");
		if(aColor!=-1){
			glEnableVertexAttribArray(aColor);
			glVertexAttribPointer(aColor, 3, GL_FLOAT, GL_FALSE,
				stride, (void*) aColorOffset);
		} else 
			valid_color = false;
	}
	
	complete = true;
}

void DrawFormat3D::draw(){
	if(complete){		
		glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
		
		if(valid_pos){
			glEnableVertexAttribArray(aPos);
			glVertexAttribPointer(aPos, 3, GL_FLOAT, GL_FALSE,
				stride, (void*) aPosOffset);
		}
		
		if(valid_norm){
			glEnableVertexAttribArray(aNorm);
			glVertexAttribPointer(aNorm, 3, GL_FLOAT, GL_FALSE,
				stride, (void*) aNormOffset);
		}
		
		if(valid_coord){
			glEnableVertexAttribArray(aCoord);
			glVertexAttribPointer(aCoord, 2, GL_FLOAT, GL_FALSE,
				stride, (void*) aCoordOffset);
		}
		
		if(valid_color){
			glEnableVertexAttribArray(aColor);
			glVertexAttribPointer(aColor, 3, GL_FLOAT, GL_FALSE,
				stride, (void*) aColorOffset);
		}
		
		glDrawElements(GL_TRIANGLES, 
			object->elementCount,
			GL_UNSIGNED_INT, 
			(const GLvoid*)object->elements);
		
	} else {
		static bool first = true;
		
		if(first){
			std::cout << "DrawFormat3D::draw() : Error! Draw called on incomplete link!" << std::endl;
			first = false;//Don't do this ten thousand times...
		}
	}
}

DrawFormat3D::~DrawFormat3D(){
	glDeleteBuffers(1, &vBuffer);
}
