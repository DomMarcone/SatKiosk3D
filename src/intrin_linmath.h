//intrin_linmath.h
//some linmath.h functions, with intrinsics (SIMD)

#ifndef _INTRIN_LINMATH_H
#define _INTRIN_LINMATH_H

	#include <linmath.h>
	
	#if defined(__i386__) || defined(__x86_64__) || defined(_M_IX86) || defined(_M_X64) //Intel/AMD SIMD
		#define USE_INTRINSICS_INTEL
		
		#include <xmmintrin.h>
		#include <immintrin.h>
		#include <pmmintrin.h>
		
		#pragma message( "Using Intel SIMD" )
		
		//Intel specific codes
		LINMATH_H_FUNC void intrin_mat4x4_mul_intel(mat4x4 m, mat4x4 a, mat4x4 b){
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
		#define intrin_mat4x4_mul intrin_mat4x4_mul_intel
		
		LINMATH_H_FUNC void intrin_mat4x4_rotate_X_intel(mat4x4 m, mat4x4 a, float b){
			float s = sinf(b);
			float c = cosf(b);
			mat4x4 R = {
				{1.f, 0.f, 0.f, 0.f},
				{0.f,   c,   s, 0.f},
				{0.f,  -s,   c, 0.f},
				{0.f, 0.f, 0.f, 1.f}
			};
			intrin_mat4x4_mul_intel(m, a, R);
		}
		#define intrin_mat4x4_rotate_X intrin_mat4x4_rotate_X_intel

		LINMATH_H_FUNC void intrin_mat4x4_rotate_Y_intel(mat4x4 m, mat4x4 a, float b){
			float s = sinf(b);
			float c = cosf(b);
			mat4x4 R = {
				{   c, 0.f,   s, 0.f},
				{ 0.f, 1.f, 0.f, 0.f},
				{  -s, 0.f,   c, 0.f},
				{ 0.f, 0.f, 0.f, 1.f}
			};
			intrin_mat4x4_mul_intel(m, a, R);
		}
		#define intrin_mat4x4_rotate_Y intrin_mat4x4_rotate_Y_intel

		LINMATH_H_FUNC void intrin_mat4x4_rotate_Z_intel(mat4x4 m, mat4x4 a, float b){
			float s = sinf(b);
			float c = cosf(b);
			mat4x4 R = {
				{   c,   s, 0.f, 0.f},
				{  -s,   c, 0.f, 0.f},
				{ 0.f, 0.f, 1.f, 0.f},
				{ 0.f, 0.f, 0.f, 1.f}
			};
			intrin_mat4x4_mul_intel(m, a, R);
		}
		#define intrin_mat4x4_rotate_Z intrin_mat4x4_rotate_Z_intel
		
	#elif defined(__arm__) || defined(_M_ARM) //ARM SIMD
		#define USE_INTRINSICS_ARM
		
		#include <arm_neon.h>
		
		#pragma message( "Using ARM SIMD" )
		
		//TODO : these functions
		#define intrin_mat4x4_mul mat4x4_mul
		#define intrin_mat4x4_rotate_X mat4x4_rotate_X
		#define intrin_mat4x4_rotate_Y mat4x4_rotate_Y
		#define intrin_mat4x4_rotate_Z mat4x4_rotate_Z
		
	#else //No SIMD
		
		#define intrin_mat4x4_mul mat4x4_mul
		#define intrin_mat4x4_rotate_X mat4x4_rotate_X
		#define intrin_mat4x4_rotate_Y mat4x4_rotate_Y
		#define intrin_mat4x4_rotate_Z mat4x4_rotate_Z
		
	#endif //Intrinsics

#endif //_INTRIN_LINMATH_H
