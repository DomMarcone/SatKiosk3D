//intrin_linmath.h
//some linmath.h functions, with intrinsics (SIMD)

#ifndef _INTRIN_LINMATH_H
#define _INTRIN_LINMATH_H

	#include <linmath.h>
	
	#if defined(__i386__) || defined(__x86_64__) || defined(_M_IX86) || defined(_M_X64)
		#define USE_INTRINSICS_INTEL
		
		#include <xmmintrin.h>
		#include <immintrin.h>
		#include <pmmintrin.h>
		
		#warning "Using Intel SIMD"
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
		
	#elif defined(__arm__) || defined(_M_ARM) 
		#define USE_INTRINSICS_ARM
		
		#include <arm_neon.h>
		
		#warning "Using ARM SIMD"
		#pragma message( "Using ARM SIMD" )
		
		//TODO : these functions
		#define intrin_mat4x4_mul mat4x4_mul
		
	#else
		
	
		#define intrin_mat4x4_mul mat4x4_mul
		
	#endif //Intrinsics

#endif //_INTRIN_LINMATH_H
