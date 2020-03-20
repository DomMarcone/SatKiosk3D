//SGPToMat4x4.h
#ifndef _SGP_TO_MAT4X4_H
#define _SGP_TO_MAT4X4_H

#include <TLEType.h>
#include <SGP.h>

#include <linmath.h>
#include <intrin_linmath.h>

#include <cmath>

#define EARTH_TILT 0.4090470572f

void SGPToMat4x4(mat4x4 m, sgp4_t *s);

#endif //_SGP_TO_MAT4X4_H
