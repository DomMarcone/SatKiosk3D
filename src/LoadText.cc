//LoadText.cc

#include "LoadText.h"

#include <iostream>
#include <string>
#include <fstream>

std::string LoadText(std::string filename){
	std::string result, line;
	
	std::ifstream inFile(filename);
	
	if(!inFile.is_open()){
		std::cout << "Error opening " << filename << std::endl;
		return "";
	}
	
	while( getline(inFile, line) ){
		result += line;
		result += "\n";
	} 
	
	inFile.close();
	
	return result;
}