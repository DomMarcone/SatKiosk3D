//TLEToAngle.cc

//Calculate the angular position of a TLE

#include "TLEToAngle.h"

#include <TLEType.h>

#include <linmath.h>

#include <iostream>
#include <cmath>
#include <ctime>

//Requires further testing and debugging.
float TLEToAngle(tle_t *t, time_t current_time, float seconds){
	float angle;
	double period = 86400.0/t->mean_motion;
	double angular_position = fmod(
		difftime(
			current_time, 
			t->epoch
		) + seconds, 
		period
	)/period;
	
	angular_position *= 3.14159265*2.0;
	angular_position += t->mean_anomaly;
	angular_position -= 3.14159256;
	
	angular_position += t->eccentricity * -cos(angular_position);
	
	angle = (float)angular_position;
	
	return (float)-angle;
}

