//FetchTLE.cc

#include "FetchTLE.h"

#include <iostream>
#include <string>
#include <cstdlib>

#include <curl/curl.h>

struct FileOut{
	std::string filename;
	FILE *file;
};

static size_t write_file(void *buffer, size_t size, size_t nmemb, void *fo_voidp){
	FileOut *fo = (struct FileOut*)fo_voidp;
	
	if(fo->file==0){
		fo->file = fopen(fo->filename.c_str(), "wb");
		
		if(!fo->file){
			std::cout << "Error opening file" << std::endl;
			return -1; //This will let CURL know to stop.
		} else {
			std::cout << "Creating file : " << fo->filename << std::endl;
		}
	}
	
	return fwrite(buffer, size, nmemb, fo->file);
}

void FetchTLE(std::string url){
	CURL *curl;
	CURLcode res;
	
	struct FileOut fo;
	fo.filename = url.substr( url.find_last_of("/\\")+1 );
	fo.file = 0;
	
	curl = curl_easy_init();
	if(curl){
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_file);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &fo);
		
		res = curl_easy_perform(curl);
		
		if(res != CURLE_OK){
			std::cout << "CURL Error! : " << curl_easy_strerror(res) << std::endl;
		}
		
		if(fo.file){
			fclose(fo.file);
		}
		
		curl_easy_cleanup(curl);
	} else {
		std::cout << "Error initializeing CURL!" << std::endl;
	}
}
