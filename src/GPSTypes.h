//GPSTypes.h
#ifndef _GPS_T_H
#define _GPS_T_H

//gps coordinate type
typedef struct gps_t{
	float longitude;
	float latitude;
	float altitude;
}gps_t;

//cartesian coordinate type
typedef float xyz_t[3];

#endif //_GPS_T_H
