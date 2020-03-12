//TLEToMat4x4.cc

#include "TLEToMat4x4.h"

#include <TLEType.h>

#include <linmath.h>

#include <cmath>
#include <ctime>

#if defined(__i386__) || defined(__x86_64__) || defined(_M_IX86) || defined(_M_X64)
#define USE_INTRINSICS
#endif

#ifdef USE_INTRINSICS
	#include <xmmintrin.h>
	#include <immintrin.h>
	#include <pmmintrin.h>

	inline void intrin_mat4x4_mul(mat4x4 m, mat4x4 a, mat4x4 b){
		__m128 av[4], bv[4], cv, dv;
		
		int c, r;
		for(int i=0; i<4; ++i){
			av[i] = _mm_set_ps(a[3][i], a[2][i], a[1][i], a[0][i]);
			bv[i] = _mm_load_ps(b[i]);
		}
		
		for(c=0; c<4; ++c)
			for(r=0; r<4; ++r){
				cv = _mm_mul_ps(av[r], bv[c]);
				dv = _mm_hadd_ps(cv, cv);
				cv = _mm_hadd_ps(dv, dv);
				_mm_store_ss(&m[c][r], cv);
			}
	}
#endif //USE_INTRINSICS

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
	
	mat4x4_rotate_Z(earth_tilt, m, EARTH_TILT/2.f);	
	mat4x4_rotate_Y(right_ascending_node, earth_tilt, -t->right_ascension - 3.14159265f/2.f);
	mat4x4_rotate_X(inclination, right_ascending_node, -t->inclination);
	mat4x4_rotate_Y(periapsis, inclination, -t->argument_of_periapsis);
	#ifdef USE_INTRINSICS
	intrin_mat4x4_mul(temp, periapsis, translate);	
	intrin_mat4x4_mul(m, temp, radius);
	#else //no USE_INTRINSICS
	mat4x4_mul(temp, periapsis, translate);	
	mat4x4_mul(m, temp, radius);
	#endif //USE_INTRINSICS
	
}
