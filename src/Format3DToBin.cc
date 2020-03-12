//Format3DToBin.cc

#include "BinToFormat3D.h"

#include <Format3D.h>
#include <Bin.h>

#include <iostream>
#include <string>
#include <vector>
#include <utility>

Bin *Format3DToBin(Format3D *f3d){
	Bin *result = new Bin;
	int offset = 3;
	
	size_t size = sizeof(int) * (3 +
		f3d->attribCount + 
		(f3d->vertCount*f3d->attribCount) + 
		f3d->elementCount
	);
	
	result->size = size;
	result->data = new char[size];
	
	int *datai = (int*)result->data;
	float *dataf = (float*)result->data;
	Format3DEnum *datae = (Format3DEnum*)result->data;
	
	datai[0] = f3d->attribCount;
	datai[1] = f3d->vertCount;
	datai[2] = f3d->elementCount;
	
	for(int i=0; i<f3d->attribCount; ++i){
		datae[offset] = f3d->attribs[i];
		offset++;
	}
	
	for(int i=0; i<(f3d->vertCount*f3d->attribCount); ++i){
		dataf[offset] = f3d->verts[i];
		offset++;
	}
	
	for(int i=0; i<f3d->elementCount; ++i){
		datai[offset] = f3d->elements[i];
		offset++;
	}
		
	return result;
}