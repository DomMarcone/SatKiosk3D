//PlyToFormat3D.cc

#include "PlyToFormat3D.h"

#include <Format3D.h>
#include <LoadText.h>

#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <thread>
#include <ctime>

void _reduceWhitespace(std::string &str){
	int i=1;
	
	while(i < str.length() ){
		if(str[i-1] == ' ' && str[i] == ' '){
			str.erase(i);
		} else {
			++i;
		}
	}
}

//With error checks for comments...
std::string _findLine(std::string &inText, std::string &str, std::size_t lineStart = 0){
	std::size_t lineEnd = inText.find('\n', lineStart)+1;
	
	if(inText.empty())return "";
	if(str.empty())return "";
	
	while(lineStart != std::string::npos){
		std::string line = inText.substr(lineStart, lineEnd - lineStart);
		
		//std::cout << lineStart << " : " << line << std::endl;
		
		if(line.find("comment")==std::string::npos){
			//line isn't a comment...
			
			if(line.find(str)!=std::string::npos){
				return line;
			}
		}
		
		lineStart = lineEnd;
		lineEnd = inText.find('\n', lineStart)+1;
	}
	
	return "";//Can't find it...
}

std::string _findLine(std::string &inText, const char *str, std::size_t lineStart = 0){
	std::string s(str);
	_findLine(inText, s, lineStart);
}

std::string _getLine(std::string &inText, std::size_t start){
	std::size_t end;
	
	//skip initial newline characters
	while(inText[start]=='\n' && start < inText.length() )start++;
	
	end = inText.find('\n', start);
	
	return inText.substr(start, end - start);
}

bool _isValidFormat(std::string &plyText){
	std::string formatLine = _findLine( plyText, "format");
	
	if(formatLine.empty()){
		std::cout << "Couldn't find format line in PLY." << std::endl;
		return false;
	}
	
	if(formatLine.find("ascii") != std::string::npos){
		return true;
	}
	
	return false;
}

std::string _getValue(std::string &plyText, std::string token, std::size_t start = 0){
	std::string str = _findLine(plyText, token, start);
	
	if(str.empty())return "";
	
	str.erase(0, str.find(token) + token.length());
	
	return str;
}

std::string _getStringPart(std::string line, int part){
	std::size_t start=0, end;
	
	while(line[start]=='\n' && start < line.length() )start++;
	while(line[start]==' ' && start < line.length() )start++;
	
	end = line.find(' ', start);
	
	for(int i=0; i<part; ++i){
		start = end;
		while(line[start]==' ' && start < line.length() )start++;
		end = line.find(' ', start);
	}
	
	return line.substr(start, end);
}

Format3DEnum _parseFormat3DEnum(std::string &str){
	if(str.empty())return FormatERROR;
	while(str.length() > 0 && *str.begin()==' ')str.erase(str.begin());
	
	while(str.length() > 0 && 
		(*(str.end()-1)==' ' || *(str.end()-1)=='\n')
		)str.erase(str.end()-1);
		
	//position
	if(str[0] == 'x')return FormatX;
	if(str[0] == 'y')return FormatY;
	if(str[0] == 'z')return FormatZ;
	
	//normals
	if(str[0] == 'n' && str.length()>1){
		if(str[1] == 'x')return FormatNX;
		if(str[1] == 'y')return FormatNY;
		if(str[1] == 'z')return FormatNZ;
	}
	
	//texture coords
	if(str[0] == 's')return FormatS;
	if(str[0] == 't')return FormatT;
	
	//colors
	if(str.compare("red")==0)return FormatR;
	if(str.compare("green")==0)return FormatG;
	if(str.compare("blue")==0)return FormatB;
	
	return FormatERROR;
}

void _fillAttribVector(std::string &plyText, std::vector<Format3DEnum> &av){
	std::string token = "property";
	std::size_t position = plyText.find(token);
	std::string str = _getValue(plyText, token, position);
	
	while(!str.empty()){
		std::string type = _getStringPart(str, 0);
		std::string value = _getStringPart(str, 1);
		
		Format3DEnum parsed = _parseFormat3DEnum(value);
		
		if(parsed!=FormatERROR){
			av.push_back(parsed);
		}
		
		position = plyText.find(token, position+1);
		if(position == std::string::npos)return;
		str = _getValue(plyText, token, position);
	}
}

typedef struct _vectorAttribArray{
	int size;
	std::vector<float> propertyX;
	std::vector<float> propertyY;
	std::vector<float> propertyZ;
	std::vector<float> propertyNX;
	std::vector<float> propertyNY;
	std::vector<float> propertyNZ;
	std::vector<float> propertyS;
	std::vector<float> propertyT;
	std::vector<float> propertyR;
	std::vector<float> propertyG;
	std::vector<float> propertyB;
} _vectorAttribArray;


inline void _fillSingleVectorAttribArray(_vectorAttribArray &vaa, Format3DEnum f3e, float value){
	std::vector<float> *outVectorf = 0;
	std::vector<int> *outVectori = 0;
	
	switch(f3e){
	case FormatX :
		outVectorf = &vaa.propertyX;
		break;
	case FormatY :
		outVectorf = &vaa.propertyY;
		break;
	case FormatZ :
		outVectorf = &vaa.propertyZ;
		break;
		
	case FormatNX :
		outVectorf = &vaa.propertyNX;
		break;
	case FormatNY :
		outVectorf = &vaa.propertyNY;
		break;
	case FormatNZ :
		outVectorf = &vaa.propertyNZ;
		break;
		
	case FormatS :
		outVectorf = &vaa.propertyS;
		break;
	case FormatT :
		outVectorf = &vaa.propertyT;
		break;
		
	case FormatR :
		outVectorf = &vaa.propertyR;
		break;
	case FormatG :
		outVectorf = &vaa.propertyG;
		break;
	case FormatB :
		outVectorf = &vaa.propertyB;
		break;
	}
	
	if(outVectorf!=0){
		outVectorf->push_back(value);
	}
	
	if(outVectori!=0){
		outVectori->push_back((int)value);
	}
}

std::size_t _fillVectorAttribArrays(
	std::string &plyText, 
	std::vector<Format3DEnum> &attribs, 
	_vectorAttribArray &vaa, 
	int count, 
	std::size_t start){
	
	std::vector<std::thread> threads;
	
	vaa.size = count;
		
	while(count>0){
		std::string line = _getLine(plyText, start);
		std::size_t lineStart=0, lineEnd;
		//std::cout << line << std::endl;
		
		//skip initial spaces
		while(line[lineStart]==' ')lineStart++;
		
		lineEnd = line.find(' ', lineStart+1);
		
		for(int i=0; i<attribs.size(); ++i){
			float value = atof( line.substr(lineStart, lineEnd-lineStart).c_str() );
			
			_fillSingleVectorAttribArray(vaa, attribs[i], value);
			
			lineStart = lineEnd;
			lineEnd = line.find(' ', lineStart+1);
		}
		
		count--;
		start += line.length()+1;
	}
	
	return start;
}

void _appendTriangulated(std::vector<int> &elements, std::vector<int> &face){
	if(face.size()<3)return;//enough points?
	
	//The rest of this algorithm is a simple fan triangluation...
	
	int pivot, left, right;
	
	pivot = face[0];
	
	for(int i=1; i<face.size()-1; ++i){
		left = face[i];
		right = face[i+1];
		
		elements.push_back(pivot);
		elements.push_back(left);
		elements.push_back(right);
	}
}

void _appendElementString(std::vector<int> &elements, std::string &line){
	std::size_t end, start=0;
	std::vector<int> points;
	int count;
	
	//skip whitespace
	while(line[start]==' ')start++;
	end = line.find(' ', start+1);
	
	count = atoi( line.substr(start,end).c_str() );
		
	start = end;
	end = line.find(' ', start+1);
	
	while(count>0){
		std::string sub = line.substr(start,end);
		
		points.push_back( atoi(sub.c_str()) );
		
		count--;
		start = end;
		end = line.find(' ', start+1);
	}
	
	_appendTriangulated(elements, points);
}


std::size_t _fillElementArray(
	std::string &plyText, 
	std::vector<int> &elements, 
	int count, 
	std::size_t start){
		
	while(count>0){
		std::size_t lineStart=0, lineEnd;
		std::string line = _getLine(plyText, start);
		
		//skip initial spaces
		while(line[lineStart]==' ')lineStart++;
		
		lineEnd = line.find(' ', lineStart+1);
		
		_appendElementString(elements, line);
		
		count--;
		start += line.length()+1;
	}
	
	return start;
}

bool _vectorContainsFormat3DEnum(std::vector<Format3DEnum> &formatVector, Format3DEnum format){
	
	for(int i=0; i<formatVector.size(); ++i){
		if(format == formatVector[i])return true;
	}

	return false;
}

inline void _fillDataFromVector(float *dest, int stride, int offset, std::vector<float> &vec){
	for(int i=0; i<vec.size(); ++i){
		dest[ stride*i + offset ] = vec[i];
	}
}

inline void _fillDataFromVectori(float *dest, int stride, int offset, std::vector<int> &vec){
	for(int i=0; i<vec.size(); ++i){
		/*
		int tempi = (int)vec[i];
		float *tempf = (float*)&tempi;
		
		dest[ stride*i + offset ] = *tempf;
		*/
		//OpenGL ES 2.0 doesn't support int attributes
		dest[ stride*i + offset ] = (float)vec[i];
	}
}

Format3D *_generateFormat3D(_vectorAttribArray &vaa, 
	std::vector<Format3DEnum> &attribs, 
	std::vector<int> &elements){
	
	Format3D *result = new Format3D;
	int offset=0, stride = attribs.size();
	
	result->vertCount = vaa.size;
	result->attribCount = attribs.size();
	result->elementCount = elements.size();
	
	result->verts = new float[ result->vertCount * result->attribCount ];
	result->attribs = new Format3DEnum[ result->attribCount ];
	result->elements = new int[ result->elementCount ];
	
	//Fill vector and attributes
	if(_vectorContainsFormat3DEnum(attribs, FormatX)){
		//std::cout << "Contains property x" << std::endl;
		result->attribs[offset] = FormatX;
		
		_fillDataFromVector(result->verts, stride, offset, vaa.propertyX);
		offset++;
	}
	
	if(_vectorContainsFormat3DEnum(attribs, FormatY)){
		//std::cout << "Contains property y" << std::endl;
		result->attribs[offset] = FormatY;
		
		_fillDataFromVector(result->verts, stride, offset, vaa.propertyY);
		offset++;
	}
	
	if(_vectorContainsFormat3DEnum(attribs, FormatZ)){
		//std::cout << "Contains property z" << std::endl;
		result->attribs[offset] = FormatZ;
		
		_fillDataFromVector(result->verts, stride, offset, vaa.propertyZ);
		offset++;
	}
	
	//Normals...
	if(_vectorContainsFormat3DEnum(attribs, FormatNX)){
		//std::cout << "Contains property nx" << std::endl;
		result->attribs[offset] = FormatNX;
		
		_fillDataFromVector(result->verts, stride, offset, vaa.propertyNX);
		offset++;
	}
	
	if(_vectorContainsFormat3DEnum(attribs, FormatNY)){
		//std::cout << "Contains property ny" << std::endl;
		result->attribs[offset] = FormatNY;
		
		_fillDataFromVector(result->verts, stride, offset, vaa.propertyNY);
		offset++;
	}
	
	if(_vectorContainsFormat3DEnum(attribs, FormatNZ)){
		//std::cout << "Contains property nz" << std::endl;
		result->attribs[offset] = FormatNZ;
		
		_fillDataFromVector(result->verts, stride, offset, vaa.propertyNZ);
		offset++;
	}
	
	//Texture coordinates
	if(_vectorContainsFormat3DEnum(attribs, FormatS)){
		//std::cout << "Contains property s" << std::endl;
		result->attribs[offset] = FormatS;
		
		_fillDataFromVector(result->verts, stride, offset, vaa.propertyS);
		offset++;
	}
	
	if(_vectorContainsFormat3DEnum(attribs, FormatT)){
		//std::cout << "Contains property t" << std::endl;
		result->attribs[offset] = FormatT;
		
		_fillDataFromVector(result->verts, stride, offset, vaa.propertyT);
		offset++;
	}
	
	//Colors...
	if(_vectorContainsFormat3DEnum(attribs, FormatR)){
		//std::cout << "Contains property red" << std::endl;
		result->attribs[offset] = FormatR;
		
		_fillDataFromVector(result->verts, stride, offset, vaa.propertyR);
		offset++;
	}
	
	if(_vectorContainsFormat3DEnum(attribs, FormatG)){
		//std::cout << "Contains property green" << std::endl;
		result->attribs[offset] = FormatG;
		
		_fillDataFromVector(result->verts, stride, offset, vaa.propertyG);
		offset++;
	}
	
	if(_vectorContainsFormat3DEnum(attribs, FormatB)){
		//std::cout << "Contains property blue" << std::endl;
		result->attribs[offset] = FormatB;
		
		_fillDataFromVector(result->verts, stride, offset, vaa.propertyB);
		offset++;
	}
	//Done filling attribs and vectors...
	
	//fill elements...
	for(int i=0; i<elements.size(); ++i){
		result->elements[i] = elements[i];
	}
	
	return result;
}

Format3D *PlyToFormat3D(std::string &plyText){
	
	int verts, faces;
	std::size_t position = 0;
	Format3D *result;
	
	//clock_t end, start = clock();
	
	_vectorAttribArray vaa;
	
	std::vector<Format3DEnum> attribs;
	
	std::vector<int> elements;
	
	_reduceWhitespace(plyText);
	//end = clock();	std::cout << "_reduceWhitespace() : " << (end-start) << "ms" << std::endl; start = end;
	
	if(!_isValidFormat(plyText)){
		std::cout << "Invalid PLY format!" << std::endl;
		return 0;
	}

	//end = clock();	std::cout << "_isValidFormat() : " << (end-start) << "ms" << std::endl; start = end;
	
	verts = atoi( _getValue( plyText, "element vertex").c_str() );
	faces = atoi( _getValue( plyText, "element face").c_str() );
	
	if(verts==0 || faces==0){
		std::cout << "Error parsing PLY!" << std::endl;
		return 0;
	}
		
	_fillAttribVector( plyText, attribs);
	//end = clock();	std::cout << "_fillAttribVector() : " << (end-start) << "ms" << std::endl; start = end;
	
	position = plyText.find("end_header") + 10;
	
	position = _fillVectorAttribArrays(plyText, attribs, vaa, verts, position);
	//end = clock();	std::cout << "_fillVectorAttribArrays() : " << (end-start) << "ms" << std::endl; start = end;
	
	_fillElementArray(plyText, elements, faces, position);
	//end = clock();	std::cout << "_fillElementArray() : " << (end-start) << "ms" << std::endl; start = end;
	
	result = _generateFormat3D(vaa, attribs, elements);
	//end = clock();	std::cout << "_generateFormat3D() : " << (end-start) << "ms" << std::endl; start = end;
	return result;
}
