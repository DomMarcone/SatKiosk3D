//TLEToMat4x4.cc

#include "TLEToMat4x4.h"

#include <TLEType.h>
#include <SGP.h>

#include <linmath.h>
#include <intrin_linmath.h>

#include <cmath>
#include <ctime>
#include <chrono>

void TLEToMat4x4(mat4x4 m, tle_t *t, std::chrono::system_clock::time_point current_time){
	mat4x4 radius, earth_tilt, periapsis, right_ascending_node, inclination, translate, temp;
	float foci, e;
	
	float period = 86400.f/t->mean_motion;
	
	float r;
	
	sgp4_t s;
	
	sgp4_solve_init(&s, t);
	sgp4_solve_at_time(&s, current_time);
	
	r = s.A0DP*XKMPER;
	
	mat4x4_identity(radius);
	mat4x4_identity(m);
	
	radius[0][0] = r;
	radius[2][2] = r*(1.0 - s.e);
	
	foci = sqrt(
		radius[0][0]*radius[0][0] -
		radius[2][2]*radius[2][2]
	);
	
	mat4x4_translate(translate, foci, 0.f, 0.f);
	/*
	//intrin_mat4x4_rotate_Z(earth_tilt, m, -EARTH_TILT);	
	//intrin_mat4x4_rotate_Y(right_ascending_node, earth_tilt, -t->right_ascension);
	intrin_mat4x4_rotate_Y(right_ascending_node, m, -t->right_ascension);
	intrin_mat4x4_rotate_Z(inclination, right_ascending_node, t->inclination);
	intrin_mat4x4_rotate_Y(periapsis, inclination, t->argument_of_periapsis);
	intrin_mat4x4_mul(temp, periapsis, translate);	
	intrin_mat4x4_mul(m, temp, radius);
	*/
	
	/* //SGP
	intrin_mat4x4_rotate_Y(right_ascending_node, m, s.lk - 3.f*3.14159265f/2.f);
	intrin_mat4x4_rotate_Z(inclination, right_ascending_node, s.ik);
	intrin_mat4x4_rotate_Y(periapsis, inclination, s.wso);
	intrin_mat4x4_mul(temp, periapsis, translate);	
	intrin_mat4x4_mul(m, temp, radius);
	*/
	
	//SGP4
	intrin_mat4x4_rotate_Y(right_ascending_node, m, s.XNODE - 3.14159265f/2.f);
	intrin_mat4x4_rotate_Z(inclination, right_ascending_node, s.ik);
	intrin_mat4x4_rotate_Y(periapsis, inclination, s.OMEGA);
	intrin_mat4x4_mul(temp, periapsis, translate);	
	intrin_mat4x4_mul(m, temp, radius);
}
