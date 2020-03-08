//GPSConvert.cc

#include "GPSConvert.h"

#define _USE_MATH_DEFINES

#include <cmath>

//helper functions
float to_radians(float in){
	return (M_PI*in)/180.0;
}

float to_degrees(float in){
	return (180.0*in)/M_PI;
}


void to_gps(gps_t *target, xyz_t source){
	float plane_length = sqrt(source[0]*source[0] + source[2]*source[2]);
	float magnitude = sqrt(plane_length*plane_length + source[1]*source[1]);
	
	target->latitude = to_degrees(atan2( (double)source[1], (double)plane_length ));
	target->longitude = to_degrees( atan2((double)source[2], (double)source[0]));
	target->altitude = magnitude - SEA_LEVEL;
}


void to_xyz(xyz_t target, gps_t *source){
	float to_core = SEA_LEVEL + source->altitude;
	float radius_at_latitude = to_core * cos(to_radians(source->latitude));
	
	
	target[0] = radius_at_latitude * cos(to_radians(source->longitude));
	target[1] = to_core * sin(to_radians(source->latitude));
	target[2] = radius_at_latitude * sin(to_radians(source->longitude));
}
