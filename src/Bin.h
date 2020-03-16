//Bin.h
#ifndef _BINARY_FORMAT
#define _BINARY_FORMAT

#include <cstdlib>

typedef struct Bin {
	size_t size;
	char *data;
} Bin;

void DeleteBin(Bin *b);

#endif //_BINARY_FORMAT
