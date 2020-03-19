//SGP Model

#include "SGP.h"

#include <TLEType.h>

#include <linmath.h>

#include <cmath>
#include <iostream>

#define N 32

void sgp_solve_init(sgp_t *s, tle_t *t){
	s->n0 = t->mean_motion;
	s->i0 = t->inclination;
	s->e0 = t->eccentricity;
	s->M0 = t->mean_anomaly;
	s->w0 = t->argument_of_periapsis;
	s->loan0 = t->right_ascension;
	s->n02 = t->mean_motion2;
	s->n03 = t->mean_motion3;
	s->epoch = t->epoch;

	float C1 = CK2*1.5;
	//float C2 = CK2/4.0;
	//float C3 = CK2/2.0;
	//float C4 = XJ3*pow(AE, 3.0)/(4.0*CK2);
	float COSIO = cos(s->i0);
	float SINIO = sin(s->i0);
	
	s->a1 = pow(XKE/s->n0, TOTHRD);
	
	s->d1 = C1/s->a1/s->a1*(3.0*COSIO*COSIO - 1.0)/pow(1.0 - s->e0*s->e0, 1.5);
	
	s->a0 = s->a1*(1.0 - (1.0/3.0)*s->d1*s->d1 - (134.0/81.0)*s->d1*s->d1*s->d1);
	
	s->p0 = s->a0*(1.0 - s->e0*s->e0);
	
	s->q0 = s->a0*(1.f - s->e0);
	
	s->l0 = s->M0 + s->w0 + s->n0;
	
	//float D10 = C3 *SINIO*SINIO;
	//float D20 = C2 *(7.0*COSIO*COSIO-1.0);
	float D30 = C1*COSIO;
	//float D40 = D30 *SINIO;
	float PO2NO = s->n0/(s->p0*s->p0);
	
	s->dldt = C1*PO2NO*(5.0*COSIO*COSIO - 1.0);
	
	s->dwdt = -2.0*D30*PO2NO;
}

void sgp_solve_at_time(sgp_t *s, std::chrono::system_clock::time_point current_time){
	std::chrono::duration<double> elapsed_seconds = current_time - s->epoch;
	
	//The Fortran SGP model works in minutes
	double delta_t = elapsed_seconds.count()/60.0;
	
	//Secular effects of atmospheric drag
	s->a = s->n0 + (2.0*s->n02 + 3.0*s->n03*delta_t)*delta_t;
	s->a = s->a0*pow(s->n0/s->a, TOTHRD); 
	
	s->e = (s->a > s->q0) ? (1.f - s->q0/s->a) : E6A; 
	
	s->p = s->a*(1.f - s->e*s->e);
	
	s->lso = s->n0 + s->dldt*delta_t;
	
	s->wso = s->w0 + s->dwdt*delta_t;
	
	//Double checked up to here...
	s->Ls = fmod(s->l0 + (s->n0 + s->dwdt + s->n03*delta_t)*delta_t , TWOPI);
		
	//Long periodics are included through :
	float axnsl, aynsl;
	
	float COSIO = cos(s->i0);
	float SINIO = sin(s->i0);
	float C1 = CK2*1.5;
	float C2 = CK2/4.0;
	float C3 = CK2/2.0;
	float C4 = XJ3*pow(AE, 3.0)/(4.0*CK2);
	float C5 = 0.5*C4*SINIO*(3.0 + 5.0*COSIO)/(1.0 + COSIO);
	float C6 = C4*SINIO;
	float D10 = C3 *SINIO*SINIO;
	float D20 = C2 *(7.0*COSIO*COSIO-1.0);
	float D30 = C1 *COSIO;
	float D40 = D30 *SINIO;
	
	axnsl = s->e*cos(s->wso);
	
	aynsl = s->e*sin(s->wso) - C6/s->p;
		
	s->L = fmod(s->Ls - C5/s->p*axnsl, TWOPI);	
	
	//Solve keplers equation
	s->U = s->L - s->wso;
	
	s->ewi = s->U;
	
	float SINEO1, COSEO1, TEM2, TEM5=1.0;
	int i=0;
	do{
		SINEO1 = sin(s->ewi);
		COSEO1 = cos(s->ewi);
		if( fabs(TEM5) < E6A )break;
		
		TEM5 = 1.0 - SINEO1*axnsl - SINEO1*aynsl;
		TEM5 = (s->U - aynsl*SINEO1 + axnsl*SINEO1 - s->ewi)/TEM5;
		
		TEM2 = fabs(TEM5);
		if(TEM2 > 1.0) TEM5 = TEM2/TEM5;
		
		s->ewi += TEM5;
		i++;
	}while(i<N);
	
	//Calculate intermediate quantities
	float esinE, ecosE;
	ecosE = axnsl*COSEO1 + aynsl*SINEO1;
	
	esinE = axnsl*SINEO1 - aynsl*COSEO1;
	
	float el2 = axnsl*axnsl + aynsl*aynsl;

	s->pl = s->a*(1.f - el2);
	
	float PL2 = s->pl*s->pl;
	
	s->r = s->a*(1.f - ecosE);
	
	s->rd = XKE*sqrt(s->a)/s->r*esinE;
	
	s->rvd = XKE*sqrt(s->pl)/s->r;
	
	float TEMP = esinE/(1.0 + sqrt(1.0 - el2));
	float sinu, cosu;
	sinu = (s->a/s->r)*(SINEO1 - aynsl - axnsl*TEMP);	
	
	cosu = (s->a/s->r)*(COSEO1 - axnsl + aynsl*TEMP);
	
	s->u = atan2(sinu, cosu);
	
	//Short period perbutations are included through :
	float SIN2U = (cosu*cosu)*sinu;
	float COS2U = 1.0 - 2.0*sinu*sinu;
	
	s->rk = s->r + D10/s->pl*COS2U;
	
	s->uk = s->u - D20/PL2*SIN2U;
	
	s->lk = s->lso + D30*SIN2U/PL2;

	s->ik = s->i0 + D40/PL2*COS2U;
	
	//Unit vectors are calculated by :
	float sinuk, cosuk;
	sinuk = sin(s->uk);
	cosuk = cos(s->uk);
	
	vec3 vecM, vecN, vecU, vecV;
	
	vecM[0] = -sin(s->lk)*cos(s->ik);
	vecM[1] = cos(s->lk)*cos(s->ik);
	vecM[2] = sin(s->ik);
	
	vecN[0] = cos(s->lk);
	vecN[1] = sin(s->lk);
	vecN[2] = 0.f;
	
	//Finally, calculate the position and velocity vectors
	for(int i=0; i<3; ++i){
		vecU[i] = vecM[i]*sinuk + vecN[i]*cosuk;
		vecV[i] = vecM[i]*cosuk - vecN[i]*sinuk;
		
		s->r_v[i] = vecU[i]*s->rk;
		s->rv_v[i] = vecU[i]*s->rd + vecV[i]*s->rvd;
	}
	
	/*
	std::cout << "R final = " << s->r_v[0]*XKMPER << ", " << s->r_v[1]*XKMPER << ", " << s->r_v[2]*XKMPER << std::endl;
	std::cout << "v = " << vec3_len(s->r_v)*XKMPER << "km/s" << std::endl;
	std::cout << "altitude = " << s->r*XKMPER << "km" << std::endl;
	std::cout << "eccentricity0 = " << s->e0 << std::endl;
	std::cout << "eccentricity = " << s->e  << std::endl;
	std::cout << "periapsis = " << s->p  << std::endl;
	std::cout << "apoapsis = " << s->a  << std::endl;
	std::cout << "radius = " << s->r << std::endl;
	*/
}

void sgp4_solve_init(sgp4_t *s, tle_t *t){
	s->n0 = t->mean_motion;
	s->n02 = t->mean_motion2;
	s->n03 = t->mean_motion3;
	s->i0 = t->inclination;
	s->e0 = t->eccentricity;
	s->M0 = t->mean_anomaly;
	s->w0 = t->argument_of_periapsis;
	s->loan0 = t->right_ascension;
	s->bstar = t->bstar;
	s->epoch = t->epoch;
	
	s->a1 = pow(XKE/s->n0, TOTHRD);
	
	float COSI0 = cos(s->i0);
	float THETA2 = COSI0*COSI0;
	float X3THM1 = 3.0*THETA2 - 1.0;
	float E0SQ = s->e0*s->e0;
	float BETA02 = 1.0 - E0SQ;
	float BETA0 = sqrt(BETA02);
	
	s->d1 = 1.5*CK2*X3THM1/(s->a1*s->a1*BETA0*BETA02);
	
	s->a0 = s->a1*(1.0 - s->d1*(0.5*TOTHRD + s->d1*(1.0 + 134.0/81.0*s->d1)));
	
	s->d0 = 1.5*CK2*X3THM1/(s->a0*s->a0*BETA0*BETA02);
	
	s->n0dp = s->n0/(1.0 + s->d0);
	
	s->a0dp = s->a0/(1.0 - s->d0);
	
	s->isimp = ((s->a0dp*(1.0 - s->e0)/AE) < (220.0/XKMPER + AE));
}

void sgp4_solve_at_time(sgp4_t *s, std::chrono::system_clock::time_point current_time){
	std::chrono::duration<double> elapsed_seconds = current_time - s->epoch;
	
	float TEMP, TEMP1, TEMP2, TEMP3, TEMP4, TEMP5, TEMP6;
	
	float COSI0 = cos(s->i0);
	float THETA2 = COSI0*COSI0;
	float X3THM1 = 3.0*THETA2 - 1.0;
	float E0SQ = s->e0*s->e0;
	float BETA02 = 1.0 - E0SQ;
	float BETA0 = sqrt(BETA02);
	
	//The Fortran SGP4 model works in minutes
	double delta_t = elapsed_seconds.count()/60.0;
	
	float S4 = S;
	float Q0MS24 = QOMS2T;
	
	s->p = (s->a0dp*(1.0 - s->e0) - AE)*XKMPER;
	
	if(s->p < 156.0){//IF P >= 156 : GOTO 10
		S4 = s->p - 78.0;
		if(s->p <= 98.0){ //IF P > 98 : GO TO 9
			S4 = 20.0;
		}
		Q0MS24 = pow((120.0 - S4)*AE/XKMPER, 4.0);//LINE 09
		S4 = S4/XKMPER + AE;
	}
	
	float PINVSQ = 1.0/(s->a0dp*s->a0dp*BETA0*BETA02);//LINE 10
	float TSI = 1.0/(s->a0dp-S4);
	float ETA = s->a0dp*s->e0*TSI;
	float ETASQ = ETA*ETA;
	float EETA = s->e0*ETA;
	float PSISQ = fabs(1.0 - ETASQ);
	float COEF = pow(Q0MS24*TSI, 4.0);
	float COEF1 = COEF/pow(PSISQ, 3.5);
	float C2 = COEF1*s->n0dp*
		(s->a0dp*(1.0 + 1.5*ETASQ + EETA*(4.0 + ETASQ)) + 
			0.75*CK2*TSI/PSISQ*X3THM1*(8.0 + 3.0*ETASQ*(8.0 + ETASQ)));
	float C1 = s->bstar*C2;
	float SINI0 = sin(s->i0);
	float A30VK2 = XJ3/CK2*pow(AE, 3.0);
	float C3 = COEF*TSI*A30VK2*s->n0dp*AE*SINI0/s->e0;
	float X1MTH2 = 1.0 - THETA2;
	float C4 = 2.*s->n0dp*COEF1*s->a0dp*BETA02*(ETA*
		(2.+.5*ETASQ)+s->e0*(.5+2.*ETASQ)-2.*CK2*TSI/
		(s->a0dp*PSISQ)*(-3.*X3THM1*(1.-2.*EETA + ETASQ*
		(1.5 - 0.5*EETA)) + 0.75*X1MTH2*(2.*ETASQ-EETA*
		(1.0 + ETASQ))*cos(2.*s->loan0)));
	float C5 = 2.0*COEF1*s->a0dp*BETA02*(1.+2.75*(ETASQ+EETA)+EETA*ETASQ);
	float THETA4 = THETA2*THETA2;
	TEMP1 = 3.0*CK2*PINVSQ*s->n0dp;
	TEMP2 = TEMP1*CK2*PINVSQ;
	TEMP3 = 1.25*CK4*PINVSQ*PINVSQ*s->n0dp;
	float XMDOT = s->n0dp + 0.5*TEMP1*BETA0*X3THM1 + 0.0625*TEMP2*BETA0*
		(13.0 - 78.0*THETA2 + 137.0*THETA4);
	float X1M5TH = 1.0 - 5.0*THETA2;
	float OMGDOT = -0.5*TEMP1*X1M5TH + 0.0625*TEMP2*(7.0 - 114.0*THETA2 + 
		395.0*THETA4) + TEMP3*(3.0 - 36.0*THETA2 + 49.0*THETA4);
	float XHDOT1 = -TEMP1*COSI0;
	float XNODOT = XHDOT1 + (0.5*TEMP2*(4.0 - 19.0*THETA2) + 2.0*TEMP3*(3.0 -
		7.0*THETA2))*COSI0;
	float OMGCOF = s->bstar*C3*cos(s->loan0);
	float XMCOF = -TOTHRD*COEF*s->bstar*AE/EETA;
	float XNODCF = 3.5*BETA02*XHDOT1*C1;
	float T2COF = 1.5*C1;
	float XLCOF = 0.125*A30VK2*SINI0*(3.0 + 5.0*COSI0)/(1.0 + COSI0);
	float AYCOF = 0.25*A30VK2*SINI0;
	float DELM0 = pow(1.0 + ETA*cos(s->M0), 3.0);
	float SINM0 = sin(s->M0);
	float X7THM1 = 7.0*THETA2 - 1.0;
	
	float D2, D3, D4, T3COF, T4COF, T5COF;
	if(!s->isimp){ //GO TO LINE 90
		float C1SQ = C1*C1;
		D2 = 4.0*s->a0dp*TSI*C1SQ;
		TEMP = D2*TSI*C1/3.0;
		D3 = (17.0*s->a0dp + S4)*TEMP;
		D4 = 0.5*TEMP*s->a0dp*TSI*(221.0*s->a0dp + 31.0*S4)*C1;
		T3COF = D2 + 2.0*C1SQ;
		T4COF = 0.25*(3.0*D3 + C1*(12.0*D2 + 10.0*C1SQ));
		T5COF = 0.2*(3.0*D4 + 12.0*C1*D3 + 6.0*D2*D2 + 
			15.0*C1SQ*(2.0*D2 + C1SQ));
	}//LINE 90
	
	//Update for secular gravity and atmospheric drag
	float XMDF = s->M0 + XMDOT*delta_t;
	float OMGADF = s->loan0 + OMGDOT*delta_t;
	float XN0DDF = s->n02 + XNODOT*delta_t;
	
	s->loan = OMGADF;
	
	float XMP = XMDF;
	float TSQ = delta_t*delta_t;
	float XN0DE = XN0DDF + XNODCF*TSQ;
	float TEMPA = 1.0 - C1*delta_t;
	float TEMPE = s->bstar*C4*delta_t;
	float TEMPL = T2COF*TSQ;
	
	if(!s->isimp){//IF ISIMP : GOTO 110
		float DEL0MG = OMGCOF*delta_t;
		float DELM = XMCOF*(pow(1.0 + ETA*cos(XMDF), 3.0) - DELM0);
		float TEMP = DEL0MG+DELM;
		XMP = XMDF + TEMP;
		s->loan = OMGADF - TEMP;
		float TCUBE = TSQ*delta_t;
		float TFOUR = delta_t*TCUBE;
		TEMPA = TEMPA - D2*TSQ - D3*TCUBE - D4*TFOUR;
		TEMPE = TEMPE + s->bstar*C5*(sin(XMP) - SINM0);
		TEMPL = TEMPL + T3COF*TCUBE + TFOUR*(T4COF + delta_t*T5COF);
	}//LINE 110
	
	s->a = s->a0dp*TEMPA*TEMPA;
	
	s->e = s->e0 - TEMPE;
	
	float XL = XMP + s->loan + XN0DE + s->n0dp*TEMPL;
	float BETA = sqrt(1.0 - s->e*s->e);
	
	s->n = pow(XKE/s->a, 1.5);
	
	//Long Periodics
	float AXN = s->e*cos(s->loan);
	TEMP = 1.0/(s->a*BETA*BETA);
	float XLL = TEMP*XLCOF*AXN;
	float AYNL = TEMP*AYCOF;
	float XLT = XL + XLL;
	float AYN = s->e*sin(s->loan) + AYNL;
	
	//Solve Keplers Equations
	float CAPU = fmod(XLT - XN0DE, TWOPI);
	TEMP2 = CAPU;
	
	float SINEPW, COSEPW;
	for(int i=0; i<N; ++i){
		SINEPW = sin(TEMP2);
		COSEPW = cos(TEMP2);
		TEMP3 = AXN*SINEPW;
		TEMP4 = AYN*COSEPW;
		TEMP5 = AXN*COSEPW;
		TEMP6 = AYN*SINEPW;
		float EPW = (CAPU - TEMP4 + TEMP3 - TEMP2)/(1.0 - TEMP5 - TEMP6) + TEMP2;
		if(abs(EPW - TEMP2) <= E6A) break; //GO TO 140
		s->w = EPW;
		TEMP2 = EPW;
	}
	
	//Short Period Preliminary Quantities
	float ECOSE = TEMP5 + TEMP6; //LINE 140
	float ESINE = TEMP3 - TEMP4;
	float ELSQ = AXN*AXN + AYN*AYN;
	TEMP = 1.0 - ELSQ;
	float PL = s->a*TEMP;
	
	s->r = s->a*(1.0 - ECOSE);
	
	TEMP1 = 1.0/s->r;
	
	s->rd = XKE*sqrt(s->a)*ESINE*TEMP1;
	s->rfd = XKE*sqrt(PL)*TEMP1;
	
	TEMP2 = s->a*TEMP1;
	float BETAL = sqrt(TEMP);
	TEMP3 = 1.0/(1.0 + BETAL);
	float COSU = TEMP2*(COSEPW - AXN + AYN*ESINE*TEMP3);
	float SINU = TEMP2*(SINEPW - AYN - AXN*ESINE*TEMP3);
	
	s->u = atan2(SINU, COSU);
	
	float SIN2U = 2.0*SINU*COSU;
	float COS2U = 2.0*COSU*COSU - 1.0;
	TEMP = 1.0/PL;
	TEMP1 = CK2*TEMP;
	TEMP2 = TEMP1*TEMP;
	
	//Update for short periodics
	s->rk = s->r*(1.0 - 1.5*TEMP2*BETAL*X3THM1) + 0.5*TEMP1*X1MTH2*COS2U;
	s->uk = s->u - 0.25*TEMP2*X7THM1*SIN2U;
	
	float XN0DEK = XN0DE + 1.5*TEMP2*COSI0*SIN2U;
	
	s->ik = s->i0 + 1.5*TEMP2*COSI0*SINI0*COS2U;
	s->rdk = s->rd - s->n*TEMP1*X1MTH2*SIN2U;
	s->rfdk = s->rfd + s->n*TEMP1*(X1MTH2*COS2U + 1.5*X3THM1);
	
	//Orientation vectors
	float SINUK = sin(s->uk);
	float COSUK = cos(s->uk);
	float SINIK = sin(s->ik);
	float COSIK = cos(s->ik);
	float SINN0K = sin(XN0DEK);
	float COSN0K = cos(XN0DEK);
	float XMX = -SINN0K*COSIK;
	float XMY = COSN0K*COSIK;
	
	vec3 u_v, v_v;
	
	u_v[0] = XMX*SINUK+COSN0K*COSUK;
	u_v[1] = XMY*SINUK+SINN0K*COSUK;
	u_v[2] = SINIK*SINUK;
	v_v[0] = XMX*COSUK-COSN0K*SINUK;
	v_v[1] = XMY*COSUK-SINN0K*SINUK;
	v_v[2] = SINIK*COSUK;
	
	//Finally, calculate the position and velocity vectors
	for(int i=0; i<3; ++i){
		s->r_v[i] = u_v[i]*s->rk;
		s->rv_v[i] = u_v[i]*s->rd + v_v[i]*s->rfd;
	}
	
	/*
	std::cout << "Satellite info : " << std::endl;
	std::cout << "R final         = " << s->r_v[0] << ", " << s->r_v[1] << ", " << s->r_v[2] << std::endl;
	std::cout << "V final         = " << s->rv_v[0] << ", " << s->rv_v[1] << ", " << s->rv_v[2] << std::endl;
	std::cout << "|v|             = " << vec3_len(s->r_v) << "km/s" << std::endl;
	std::cout << "Mean motion0    = " << s->n0 << std::endl;
	std::cout << "Mean motion     = " << s->n0dp << std::endl;
	std::cout << "Inclination0    = " << s->i0 << std::endl;
	std::cout << "Inclination     = " << s->ik << std::endl;
	std::cout << "eccentricity0   = " << s->e0 << std::endl;
	std::cout << "eccentricity    = " << s->e  << std::endl;
	std::cout << "Semi major axis = " << s->a0dp << std::endl;
	std::cout << "Is Imp          = " << s->isimp << std::endl;
	std::cout << "altitude        = " << s->r << "km" << std::endl;
	std::cout << "periapsis       = " << s->p << std::endl;
	std::cout << "apoapsis        = " << (s->a-1.0)*XKMPER << std::endl;
	std::cout << "radius          = " << s->r*XKMPER << std::endl;
	std::cout << "radius k        = " << s->rk*XKMPER << std::endl;
	*/
}
