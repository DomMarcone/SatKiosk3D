//LoadImagePNG.cc

#include "LoadImagePNG.h"
#include "Image.h"

#include <png.h>

#include <iostream>
#include <string>
#include <cstdlib>
#include <stdio.h>

void LoadImagePNG(Image &img, std::string filename){
	int width, height;
	png_byte color_type;
	png_byte bit_depth;
	int bytesPerPixel = 0;

	png_structp png_ptr;
	png_infop info_ptr;
	int number_of_passes;
	png_bytep * row_pointers;
	
	char header[8];
	
	FILE *fp = fopen(filename.c_str(), "rb");
	
	if(!fp){
		std::cout << "Could not load " << filename << std::endl;
		return;
	}
	
	fread(header, 1, 8, fp);
	if(png_sig_cmp((png_const_bytep)header, 0, 8)){
		std::cout << "File " << filename << " is not recognized as a PNG file." << std::endl;
		fclose(fp);
		return;
	}
	
	//Initialize stuff
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	
	if (!png_ptr){
		std::cout << "[read_png_file] png_create_read_struct failed" << std::endl;
		fclose(fp);
		return;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr){
		std::cout << "[read_png_file] png_create_info_struct failed" << std::endl;
		fclose(fp);
		return;
	}

	if (setjmp(png_jmpbuf(png_ptr))){
		std::cout << "[read_png_file] Error during init_io" << std::endl;
		fclose(fp);
		return;
	}
	

	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, 8);

	png_read_info(png_ptr, info_ptr);

	width = png_get_image_width(png_ptr, info_ptr);
	height = png_get_image_height(png_ptr, info_ptr);
	color_type = png_get_color_type(png_ptr, info_ptr);
	bit_depth = png_get_bit_depth(png_ptr, info_ptr);

	number_of_passes = png_set_interlace_handling(png_ptr);
	png_read_update_info(png_ptr, info_ptr);
	
	//Setup Image
	img.setSize(width,height);
	
	//Determine the number of bytes per pixel...
	switch(color_type){
	case PNG_COLOR_TYPE_GRAY :
		bytesPerPixel = 1;
		break;
	case PNG_COLOR_TYPE_GRAY_ALPHA :
		bytesPerPixel = 2;
		break;
	case PNG_COLOR_TYPE_RGB :
		bytesPerPixel = 3;
		break;
	case PNG_COLOR_TYPE_RGB_ALPHA :
		bytesPerPixel = 4;
		break;
	}
	
	if(!bytesPerPixel){
		std::cout << "Couldn't determine bytes per pixel of " << filename << std::endl;
		fclose(fp);
		return;
	}
		
	img.setBytesPerPixel(bytesPerPixel);
	img.setData( new char[bytesPerPixel * height * width] );
	
	//Read file
	if (setjmp(png_jmpbuf(png_ptr))){
		std::cout << "[read_png_file] Error during read_image" << std::endl;
		fclose(fp);
		return;
	}

	row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
	for (int y=0; y<height; y++)
			//Load upside down
			row_pointers[height-y-1] = (png_byte*) img.getData() + (y * width * bytesPerPixel);

	png_read_image(png_ptr, row_pointers);
	
	//Cleanup temporary stuff
	free(row_pointers);
	
	//std::cout << "Loaded " << filename << std::endl;
	
	fclose(fp);
}

