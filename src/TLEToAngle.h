//TLEToAngle.h

//Calculate the angular position of a TLE

#ifndef _TLE_TO_ANGLE_H
#define _TLE_TO_ANGLE_H

#include <TLEType.h>

#include <linmath.h>

#include <iostream>
#include <cmath>
#include <chrono>

float TLEToAngle(tle_t *t, std::chrono::system_clock::time_point current_time);

#endif //_TLE_TO_ANGLE_H
