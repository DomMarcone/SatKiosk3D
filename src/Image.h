//Image.h
#ifndef _IMAGE_H
#define _IMAGE_H

class Image{
private :
	int width, height;
	
	int bytesPerPixel;
	
	char *data;

public :
	
	Image();
	~Image();
	
	char *getData();
	void setData(char *data);
	
	int getWidth();
	int getHeight();
	
	void setSize(int width, int height);
	
	void setBytesPerPixel(int bpp);
	int getBytesPerPixel();
	
	void clear();
};

#endif //_IMAGE_H
