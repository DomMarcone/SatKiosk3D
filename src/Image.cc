//Image.cc
#include "Image.h"

Image::Image(){
	height = 0;
	width = 0;
	bytesPerPixel = 0;
	data = 0;
}

Image::~Image(){
	if(data)delete data;
}

char *Image::getData(){
	return data;
}

void Image::setData(char *d){
	data = d;
}

int Image::getWidth(){return width;}
int Image::getHeight(){return height;}

void Image::setSize(int w, int h){
	width = w;
	height = h;
}


void Image::setBytesPerPixel(int bpp){
	bytesPerPixel = bpp;
}

int Image::getBytesPerPixel(){
	return bytesPerPixel;
}

void Image::clear(){
	width = 0;
	height = 0;
	if(data)delete data;
}
