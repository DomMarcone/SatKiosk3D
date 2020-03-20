//SGP.h 
#ifndef _SGP_H
#define _SGP_H

#include <cmath>

#include <TLEType.h>

#include <linmath.h>

#include <cmath>
#include <chrono>
#include <iostream>

//From the SGP Model
#define CK2    5.413080e-4
#define CK4    0.62098875e-6
#define E6A    1.0e-6
#define QOMS2T 1.88027916e-9
#define S      1.01222928
#define TOTHRD 0.66666667
#define XJ3    -0.253881e-5
#define XKE    0.743669161e-1
#define XKMPER 6378.135
#define XMNPDA 1440.0
#define AE     1.0 //In terms of earths radii
#define DE2RA  0.174532925e-1
#define PI     3.14159265
#define PIO2   1.57079633
#define TWOPI  6.2831853
#define X3PIO2 4.71238898

typedef struct sgp_t {
	//TLE coefficients
	float n0, i0, e0, M0, w0, loan0, n02, n03;
	std::chrono::system_clock::time_point epoch;
	
	//Initial, non-time dependent
	float a1, d1, a0, p0, q0, l0, dldt, dwdt;
		
	//Time dependents, results
	float a, e, p, lso, wso, Ls, L, U, ewi, pl,
		r, rd, rvd, u, rk, uk, lk, ik;
	vec3 r_v, rv_v;
	
} sgp_t;

typedef struct sgp4_t {
	float n0, i0, e0, XM0, OMEGAO, XNODE0, n02, n03, bstar;//TLE coefficients
	std::chrono::system_clock::time_point epoch;
	
	//Initial non-time dependent
	float A1, D1, A0, D0, XN0DP, A0DP;
	bool isimp;
	
	//Time dependents, results
	float P, XNODE, a, e, n, OMEGA, r, rd, rfd, u, rk, uk, ik, rdk, rfdk;
	vec3 r_v, rv_v;
} sgp4_t;

void sgp_solve_init(sgp_t *s, tle_t *t);
void sgp_solve_at_time(sgp_t *s, std::chrono::system_clock::time_point current_time);

void sgp4_solve_init(sgp4_t *s, tle_t *t);
void sgp4_solve_at_time(sgp4_t *s, std::chrono::system_clock::time_point current_time);

#endif //_SGP_H