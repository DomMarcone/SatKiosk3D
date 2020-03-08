//GPSConvert.h
#ifndef _GPS_CONVERT_H
#define _GPS_CONVERT_H

#include "GPSTypes.h"

//sea level to the Earth's core
#define SEA_LEVEL 6371.f


void to_gps(gps_t *target, xyz_t source);

void to_xyz(xyz_t target, gps_t *source);


#endif //_GPS_CONVERT_H
