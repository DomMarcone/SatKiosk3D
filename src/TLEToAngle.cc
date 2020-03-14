//TLEToAngle.cc

//Calculate the angular position of a TLE

#include "TLEToAngle.h"

#include <TLEType.h>

#include <linmath.h>

#include <iostream>
#include <cmath>
#include <ctime>

//Requires further testing and debugging.
float TLEToAngle(tle_t *t, std::chrono::system_clock::time_point current_time){
	float angle;
	double period = 86400.0/t->mean_motion, ma_seconds = (t->mean_anomaly/360.0)/period ;
	std::chrono::duration<double> elapsed_seconds = current_time - t->epoch;
	double angular_position = fmod(
		elapsed_seconds.count() - ma_seconds, 
		period
	)/period;
	
	angular_position *= 3.14159265*2.0;
	angular_position -= 3.14159256/2.0;
	
	angular_position += t->eccentricity * -cos(angular_position);
	
	angle = (float)angular_position;
	
	return (float)-angle;
}

