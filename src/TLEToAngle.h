//TLEToAngle.h

//Calculate the angular position of a TLE

#ifndef _TLE_TO_ANGLE_H
#define _TLE_TO_ANGLE_H

#include <TLEType.h>

#include <linmath.h>

#include <iostream>
#include <cmath>
#include <ctime>

float TLEToAngle(tle_t *t, time_t current_time, float seconds);

#endif //_TLE_TO_ANGLE_H
