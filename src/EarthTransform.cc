//EarthTransform.cc

#include "EarthTransform.h"

#include <linmath.h>

#include <ctime>
#include <iostream>

EarthTransform::EarthTransform(){
	struct tm reference_equinox, reference_zero;
	
	reference_equinox.tm_sec   = 0;
	reference_equinox.tm_min   = MARCH_EQUINOX_MINUTE;
	reference_equinox.tm_hour  = MARCH_EQUINOX_HOUR;
	reference_equinox.tm_mday  = MARCH_EQUINOX_DAY;
	reference_equinox.tm_mon   = MARCH_EQUINOX_MONTH;
	reference_equinox.tm_year  = MARCH_EQUINOX_YEAR-1900;
	reference_equinox.tm_isdst = 0;
	
	
	reference_zero.tm_sec = 0;
	reference_zero.tm_min = 0;
	reference_zero.tm_hour = 0;
	reference_zero.tm_mday = MARCH_EQUINOX_DAY;
	reference_zero.tm_mon = MARCH_EQUINOX_MONTH;
	reference_zero.tm_year = MARCH_EQUINOX_YEAR-1900;
	reference_zero.tm_isdst = 0;
	
	
	march_equinox = mktime(&reference_equinox);
	day_zero = mktime(&reference_zero);
	day_zero_offset = difftime(march_equinox, day_zero);
	
	rotation_offset = 0.f;
	
	tilt = EARTH_TILT;
	
	mat4x4_identity(transform);
}

EarthTransform::~EarthTransform(){
	//Nothing to do yet
}

void EarthTransform::setCurrentTime(time_t ct){
	current_time = ct;
}

void EarthTransform::setRotationOffset(float r){
	rotation_offset = r;
}

mat4x4 &EarthTransform::getTransform(){
	double earth_rotation = fmod(
		difftime(
			current_time, 
			day_zero
		)/* + day_zero_offset*/, 
		EARTH_SIDEREAL_DAY
	)/EARTH_SIDEREAL_DAY;
	
	earth_rotation = 1.0 - earth_rotation;
	
	earth_rotation *= M_PI*2.0;
	earth_rotation += rotation_offset;
	
	mat4x4 rotate_tilt;
	mat4x4_identity(transform);
	
	mat4x4_rotate_Z(rotate_tilt, transform, tilt/2.f);
	mat4x4_rotate_Y(transform, rotate_tilt, (float)earth_rotation);
	
	return transform;
}

vec3 &EarthTransform::getSunDirection(){
	double earth_orbital_postion = fmod(
		difftime(
			current_time,
			march_equinox
		), 
		EARTH_SIDEREAL_PERIOD_SECONDS
	)/EARTH_SIDEREAL_PERIOD_SECONDS;
		
	//Rotate prograde
	earth_orbital_postion = 1.0 - earth_orbital_postion;
	
	earth_orbital_postion *= M_PI*2.0;
	
	
	sun_direction[0] = (float)cos(earth_orbital_postion);
	sun_direction[1] = 0.f;
	sun_direction[2] = (float)sin(earth_orbital_postion);
	
	return sun_direction;
}