//TLEToMat4x4.cc

#include "TLEToMat4x4.h"

#include <TLEType.h>

#include <linmath.h>
#include <intrin_linmath.h>

#include <cmath>
#include <ctime>

void TLEToMat4x4(mat4x4 m, tle_t *t){
	mat4x4 radius, earth_tilt, periapsis, right_ascending_node, inclination, translate, temp;
	float foci, a, b;
	float eccentricity = (1.f - t->eccentricity);
	
	mat4x4_identity(radius);
	mat4x4_identity(m);
	
	b = 1.f/sqrt(
			t->eccentricity*t->eccentricity + 1.f
		);
		
	a = 1.f/b;
	
	radius[0][0] = t->radius * b;
	radius[2][2] = t->radius * a;
	
	foci = sqrt(
		radius[2][2]*radius[2][2] -
		radius[0][0]*radius[0][0]
	);
	
	mat4x4_translate(translate, 0.f, 0.f, foci);
	
	intrin_mat4x4_rotate_X(earth_tilt, m, -EARTH_TILT);	
	intrin_mat4x4_rotate_Y(right_ascending_node, earth_tilt, t->right_ascension - 3.14159265f/3.f);
	intrin_mat4x4_rotate_X(inclination, right_ascending_node, t->inclination);
	intrin_mat4x4_rotate_Y(periapsis, inclination, t->argument_of_periapsis);
	intrin_mat4x4_mul(temp, periapsis, translate);	
	intrin_mat4x4_mul(m, temp, radius);
}
