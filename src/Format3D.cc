//Format3D.cc

#include "Format3D.h"

void DeleteFormat3D(Format3D *f){
	if(f==0)return;
	
	f->elementCount = 0;
	delete f->elements;
	
	f->vertCount = 0;
	delete f->verts;
	
	f->attribCount = 0;
	delete f->attribs;
	
	delete f;
}
