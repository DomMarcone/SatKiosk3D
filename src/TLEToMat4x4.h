//TLEToMat4x4.h
#ifndef _TLE_TO_MAT4X4_H
#define _TLE_TO_MAT4X4_H

#include <TLEType.h>

#include <linmath.h>

#include <cmath>

#define EARTH_TILT 0.4090470572f

void TLEToMat4x4(mat4x4 m, tle_t *t);

#endif //_TLE_TO_MAT4X4_H
