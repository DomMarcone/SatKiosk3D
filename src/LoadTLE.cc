//LoadTLE.cc

#include "LoadTLE.h"

#include <TLEType.h>
#include <SGP.h>

#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <fstream>
#include <ctime>
#include <cmath>
#include <chrono>
#include <string>

#define PHYSICS_GM 3.9857605760e5

bool LoadTLE(std::vector<sgp4_t> &sgpVector, std::string filename){
	
	tle_t temp;
	sgp4_t temps;
	bool valid;
	
	std::string line0, line1, line2;
	
	std::ifstream inFile(filename);
	
	if(!inFile.is_open()){
		std::cout << "Error opening " << filename << std::endl;
		return false;
	}
	
	while( 
		getline(inFile, line0) &&
		getline(inFile, line1) &&
		getline(inFile, line2)
	){
		int year, current_year;
		float period, day;
		struct tm time_tm, *current_tm;
		time_t current_time, epoch;
		std::string time_string;
		
		temp.name = (char*)line0.c_str();
		
		temp.inclination = atof( line2.substr(8, 8).c_str() );
		temp.right_ascension = atof( line2.substr(17, 8).c_str() );
		temp.eccentricity = atof( line2.substr(26, 7).c_str() )/10000000.f;
		temp.argument_of_periapsis = atof( line2.substr(34, 8).c_str() );
		temp.mean_anomaly = atof( line2.substr(43, 8).c_str() );
		temp.mean_motion = atof( line2.substr(52, 11).c_str() );
		temp.mean_motion2 = atof( line1.substr(33, 9).c_str() );
		temp.mean_motion3 = atof( line1.substr(44, 7).c_str() )/100000.0;
		temp.bstar = atof( line1.substr(53, 6).c_str() )/100000.0;
		
		//Handle exponents
		temp.mean_motion3 *= pow( 10.0, atof( line1.substr(50, 2).c_str() ) );
		temp.bstar *= pow( 10.0, atof( line1.substr(59, 2).c_str() ) );
		
		year = atoi( line1.substr(18,2).c_str() );
		day = atof( line1.substr(20,12).c_str() );
		
		//Convert degrees to radians
		temp.inclination = temp.inclination * 3.14159265f/180.f;
		temp.right_ascension = temp.right_ascension * 3.14159265f/180.f;
		temp.argument_of_periapsis = temp.argument_of_periapsis * 3.14159265f/180.f;
		temp.mean_anomaly = temp.mean_anomaly * 3.14159265f/180.f;
		
		//Convert to minutes
		temp.mean_motion = (3.14159265*2.0)/((24.0*60.0)/temp.mean_motion);
		
		time(&current_time);
		current_tm = gmtime(&current_time);
		
		current_year = current_tm->tm_year;
		
		//I think it's safe to say, TLE's will be gone before 2099
		if(current_year > 100)
			current_year -= 100;
		
		if(year <= current_year){
			year += 100;//Sync with 1900
		}
		
		time_tm.tm_year = year;
		time_tm.tm_yday = day;
		day -= time_tm.tm_yday;//remove day offset
		day *= 24.f;
		time_tm.tm_hour = day;
		day -= time_tm.tm_hour;//remove hour offset
		day *= 60.f;
		time_tm.tm_min = day;
		day -= time_tm.tm_min;//remove minute offset
		day *= 60.f;
		time_tm.tm_sec = day;
		
		//Must be 0, or mktime() will return -1
		time_tm.tm_mday = 0;
		time_tm.tm_mon = 0;
		time_tm.tm_wday = 0;
		time_tm.tm_isdst = 0;
		
		epoch = mktime(&time_tm);
		
		temp.epoch = std::chrono::system_clock::from_time_t(epoch);
		
		sgp4_solve_init(&temps, &temp);
		
		sgpVector.push_back(temps);
	} 
	
	std::cout << "Loaded " << sgpVector.size() << " elements." << std::endl;
	
	inFile.close();
	return true;
}
