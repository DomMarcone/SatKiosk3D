//SaveBin.cc

#include "SaveBin.h"

#include <Bin.h>

#include <iostream>
#include <fstream>
#include <string>

void SaveBin(Bin *bin, std::string filename, bool showErrors){
	std::ofstream outFile(filename, std::ios::out | std::ios::binary);
	
	if(!outFile.is_open()){
		if(showErrors)std::cout << "Error opening " << filename << std::endl;
		outFile.close();
		return;
	}
	
	outFile.write(bin->data, bin->size);
	
	outFile.close();
}
