//TLEType.h
#ifndef _TLE_TYPE_H
#define _TLE_TYPE_H

#include <chrono>

typedef struct tle_t{
	char *name;
	
	float inclination;
	float right_ascension;
	float eccentricity;
	float argument_of_periapsis;
	float mean_motion;
	float mean_motion2;
	float mean_motion3;
	float mean_anomaly;
	float bstar;
	std::chrono::system_clock::time_point epoch;
	
	//Computed
	//float radius;//TODO : Remove
} tle_t;

#endif //_TLE_TYPE_H
