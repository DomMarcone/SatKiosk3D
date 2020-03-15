//LoadFormat3D.cc

#include "LoadFormat3D.h"

#include <Format3D.h>
#include <LoadText.h>
#include <LoadBin.h>
#include <SaveBin.h>
#include <BinToFormat3D.h>
#include <Format3DToBin.h>
#include <PlyToFormat3D.h>

#include <iostream>
#include <string>

Format3D *LoadFormat3D(std::string filename){
	Format3D *result = 0;
	Bin *bin;
	
	std::string extension = filename.substr( filename.find_last_of('.') ,filename.length());
	
	if(extension.empty()){
		std::cout << "Error! Could not determine filetype of " << filename << std::endl;
		return 0;
	}
	
	//Go ahead, if it's already a bin
	if(extension.compare(".bin")==0){		
		bin = LoadBin(filename);
		result = BinToFormat3D(bin);
		
		DeleteBin(bin);
		
		return result;
	}
	
	if(extension.compare(".plyb")==0 ||
		extension.compare(".ply")==0 ){
				
		std::string bakedFilename = filename;
		bakedFilename += ".bin";
		//Look for a baked file!
		
		bin = LoadBin(bakedFilename, false);
		
		if(bin==0){
			std::string plyText = LoadText(filename);
			std::cout << "Creating baked file " << bakedFilename << std::endl;
			
			result = PlyToFormat3D( plyText	);
			
			bin = Format3DToBin(result);
			
			SaveBin(bin, bakedFilename);
			
			DeleteBin(bin);
			
			extension = ".bin";
		} else {
			//We have a pre-baked file already...			
			result = BinToFormat3D(bin);
			
			DeleteBin(bin);
		}
	}
	
	return result;
}
