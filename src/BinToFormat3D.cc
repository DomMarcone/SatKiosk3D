//BinToFormat3D.cc

#include "BinToFormat3D.h"

#include <Format3D.h>
#include <LoadBin.h>

#include <iostream>
#include <string>
#include <vector>
#include <utility>

Format3D *BinToFormat3D(Bin *bin){
	Format3D *result = new Format3D;
	int offset = 3;
	
	int *datai = (int*)bin->data;
	float *dataf = (float*)bin->data;
	Format3DEnum *datae = (Format3DEnum*)bin->data;
	
	result->attribCount = datai[0];
	result->vertCount = datai[1];
	result->elementCount = datai[2];
	
	if(bin->size != sizeof(int) * ( 3 + 
		result->attribCount +
		(result->vertCount*result->attribCount) +
		result->elementCount )
	){
		std::cout << "Error! Size of bin isn't lining up with the Format3D data" << std::endl;
		delete result;
		return 0;
	}
	
	result->attribs = new Format3DEnum[result->attribCount];
	result->verts = new float[result->vertCount*result->attribCount];
	result->elements = new int[result->elementCount];
	
	for(int i=0; i<result->attribCount; ++i){
		result->attribs[i] = datae[offset];
		offset++;
	}
	
	for(int i=0; i<(result->vertCount*result->attribCount); ++i){
		result->verts[i] = dataf[offset];
		offset++;
	}
	
	for(int i=0; i<result->elementCount; ++i){
		result->elements[i] = datai[offset];
		offset++;
	}
		
	return result;
}