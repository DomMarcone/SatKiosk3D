//EarthTransform.h
#ifndef _EARTH_TRANSFORM_H
#define _EARTH_TRANSFORM_H

#include <linmath.h>

#include <ctime>

#define EARTH_SIDEREAL_PERIOD_YEARS   1.0
#define EARTH_SIDEREAL_PERIOD_DAYS    365.25636
#define EARTH_SIDEREAL_PERIOD_HOURS   8766.15264
#define EARTH_SIDEREAL_PERIOD_MINUTES 525969.1584
#define EARTH_SIDEREAL_PERIOD_SECONDS 31558149.504

#define EARTH_SIDEREAL_DAY 86164.1

#define EARTH_TILT 0.4090470572f

//Reference time in UTC
#define MARCH_EQUINOX_YEAR   1970
#define MARCH_EQUINOX_MONTH  3
#define MARCH_EQUINOX_DAY    21
#define MARCH_EQUINOX_HOUR   1
#define MARCH_EQUINOX_MINUTE 56

#ifndef M_PI
#define M_PI 3.14159265359
#endif //M_PI

class EarthTransform {
private :

	float radius_poles, radius_equator;
	
	time_t march_equinox;
	time_t day_zero;
	double day_zero_offset;
	time_t current_time;
	
	float tilt;
	
	float rotation_offset;
	
	mat4x4 transform;
	vec3 sun_direction;
	
public :

	EarthTransform();
	~EarthTransform();
	
	void setCurrentTime(time_t ct);
	
	void setRotationOffset(float r);
	
	mat4x4 &getTransform();
	vec3 &getSunDirection();
};

#endif //_EARTH_TRANSFORM_H
