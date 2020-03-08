//Format3D.h 
#ifndef _FORMAT_3D_H
#define _FORMAT_3D_H

enum Format3DEnum {
	//Position
	FormatX, 
	FormatY,
	FormatZ,
	
	//Normals
	FormatNX, 
	FormatNY,
	FormatNZ,
	
	//Texture
	FormatS, 
	FormatT,
	
	//Colors
	FormatR, 
	FormatG, 
	FormatB, 
	
	//For errors...
	FormatERROR
};


typedef struct Format3D {
	int attribCount;
	Format3DEnum *attribs;
	
	int vertCount;
	float *verts;
	
	int elementCount;
	int *elements;
	
} Format3D;


void DeleteFormat3D(Format3D *f);

#endif //_FORMAT_3D_H
