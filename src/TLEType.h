//TLEType.h
#ifndef _TLE_TYPE_H
#define _TLE_TYPE_H

#include <ctime>

typedef struct tle_t{
	char *name;
	
	float inclination;
	float right_ascension;
	float eccentricity;
	float argument_of_periapsis;
	float mean_motion;
	float mean_anomaly;
	time_t epoch;
	
	//Computed
	float radius;
} tle_t;

#endif //_TLE_TYPE_H
