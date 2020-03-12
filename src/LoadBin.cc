//LoadBin.cc

#include "LoadBin.h"

#include <Bin.h>

#include <iostream>
#include <fstream>
#include <string>

Bin *LoadBin(std::string filename, bool showErrors){
	Bin *result = new Bin;
	std::streampos start, end;
	
	std::ifstream inFile(filename, std::ios::in | std::ios::binary);
	
	if(!inFile.is_open()){
		if(showErrors)std::cout << "Error opening " << filename << std::endl;
		inFile.close();
		delete result;
		return 0;
	}
	inFile.seekg(0, std::ios::end);
	end = inFile.tellg();
	inFile.seekg(0, std::ios::beg);
	start = inFile.tellg();
	
	result->size = end-start;
	
	result->data = new char[result->size];
	
	inFile.read(result->data, result->size);
	
	inFile.close();
	
	return result;
}

