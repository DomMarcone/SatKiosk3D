//Bin.cc

#include "Bin.h"

#include <cstdlib>

void DeleteBin(Bin *b){
	if(b){
		b->size = 0;
		delete b->data;
	}
}
