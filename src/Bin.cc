//Bin.cc

#include "Bin.h"

void DeleteBin(Bin *b){
	if(b){
		b->size = 0;
		delete b->data;
	}
}
