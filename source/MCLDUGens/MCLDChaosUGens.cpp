/*
 *  MCLDChaosUGens.cpp
 *  xSC3plugins
 *
 *  Chaos UGens by Dan Stowell.
 *  Partly based on Lance Putnam's chaos UGen work.
 *  Available under the terms of the GNU Public License (GPL).
 *
 */

#include "SC_PlugIn.h"
#define TWOPI 6.283185307179586
#define PI 3.141592653589793
#define RECPI 0.3183098861837907
#define RECTWOPI 0.1591549430918953
#define ONESIXTH 0.1666666666666667

// InterfaceTable contains pointers to functions in the host (server).
static InterfaceTable *ft;

struct NonLinear : public Unit {
	double x0, y0, xn, yn, xnm1, ynm1;
	float counter;
	//bool stable;
};

// declare struct to hold unit generator state
struct RosslerL : public NonLinear {
	double z0, zn, znm1, frac;
};
struct FincoSprottL : public NonLinear {
	double z0, zn, znm1, frac;
};
struct FincoSprottM : public NonLinear {
	double z0, zn, znm1, frac;
};
struct FincoSprottS : public NonLinear {
	double z0, zn, znm1, frac;
};

struct Perlin3 : Unit {
};

// declare unit generator functions
extern "C"
{
	void load(InterfaceTable *inTable);

	void RosslerL_next(RosslerL *unit, int inNumSamples);
	void RosslerL_Ctor(RosslerL *unit);

	void FincoSprottL_next(FincoSprottL *unit, int inNumSamples);
	void FincoSprottL_Ctor(FincoSprottL *unit);
	void FincoSprottM_next(FincoSprottM *unit, int inNumSamples);
	void FincoSprottM_Ctor(FincoSprottM *unit);
	void FincoSprottS_next(FincoSprottS *unit, int inNumSamples);
	void FincoSprottS_Ctor(FincoSprottS *unit);

	void Perlin3_next(Perlin3 *unit, int inNumSamples);
	void Perlin3_Ctor(Perlin3 *unit);
};

//////////////////////////////////////////////////////////////////

void RosslerL_next(RosslerL *unit, int inNumSamples)
{
	float *xout = ZOUT(0);
	float *yout = ZOUT(1);
	float *zout = ZOUT(2);
	float freq = ZIN0(0);
	double a = ZIN0(1);
	double b = ZIN0(2);
	double c = ZIN0(3);
	double h = ZIN0(4);
	double x0 = ZIN0(5);
	double y0 = ZIN0(6);
	double z0 = ZIN0(7);

	double xn = unit->xn;
	double yn = unit->yn;
	double zn = unit->zn;
	float counter = unit->counter;
	double xnm1 = unit->xnm1;
	double ynm1 = unit->ynm1;
	double znm1 = unit->znm1;
	double frac = unit->frac;

	float samplesPerCycle;
	double slope;
	if(freq < unit->mRate->mSampleRate){
		samplesPerCycle = unit->mRate->mSampleRate / sc_max(freq, 0.001f);
		slope = 1.f / samplesPerCycle;
	}
	else {
		samplesPerCycle = 1.f;
		slope = 1.f;
	}

	if((unit->x0 != x0) || (unit->y0 != y0) || (unit->z0 != z0)){
		xnm1 = xn;
		ynm1 = yn;
		znm1 = zn;
		unit->x0 = xn = x0;
		unit->y0 = yn = y0;
		unit->z0 = zn = z0;
	}

	double dx = xn - xnm1;
	double dy = yn - ynm1;
	double dz = zn - znm1;

	for (int i=0; i<inNumSamples; ++i) {
		if(counter >= samplesPerCycle){
			counter -= samplesPerCycle;
			frac = 0.f;

			xnm1 = xn;
			ynm1 = yn;
			znm1 = zn;

			double k1x, k2x, k3x, k4x,
				k1y, k2y, k3y, k4y,
				k1z, k2z, k3z, k4z,
				kxHalf, kyHalf, kzHalf;

			// 4th order Runge-Kutta approximate solution for differential equations
			k1x = - (h * (ynm1 + znm1));
			k1y = h * (xnm1 + a * ynm1);
			k1z = h * (b + znm1 * (xnm1 - c));
			kxHalf = k1x * 0.5;
			kyHalf = k1y * 0.5;
			kzHalf = k1z * 0.5;

			k2x = - (h * (ynm1 + kyHalf + znm1 + kzHalf));
			k2y = h * (xnm1 + kxHalf + a * (ynm1 + kyHalf));
			k2z = h * (b + (znm1 + kzHalf) * (xnm1 + kxHalf - c));
			kxHalf = k2x * 0.5;
			kyHalf = k2y * 0.5;
			kzHalf = k2z * 0.5;

			k3x = - (h * (ynm1 + kyHalf + znm1 + kzHalf));
			k3y = h * (xnm1 + kxHalf + a * (ynm1 + kyHalf));
			k3z = h * (b + (znm1 + kzHalf) * (xnm1 + kxHalf - c));

			k4x = - (h * (ynm1 + k3y + znm1 + k3z));
			k4y = h * (xnm1 + k3x + a * (ynm1 + k3y));
			k4z = h * (b + (znm1 + k3z) * (xnm1 + k3x - c));

			xn = xn + (k1x + 2.0*(k2x + k3x) + k4x) * ONESIXTH;
			yn = yn + (k1y + 2.0*(k2y + k3y) + k4y) * ONESIXTH;
			zn = zn + (k1z + 2.0*(k2z + k3z) + k4z) * ONESIXTH;

			dx = xn - xnm1;
			dy = yn - ynm1;
			dz = zn - znm1;
		}
		counter++;
		ZXP(xout) = (xnm1 + dx * frac) * 0.5f;
		ZXP(yout) = (ynm1 + dy * frac) * 0.5f;
		ZXP(zout) = (znm1 + dz * frac) * 1.0f;
		frac += slope;
	}

	unit->xn = xn;
	unit->yn = yn;
	unit->zn = zn;
	unit->counter = counter;
	unit->xnm1 = xnm1;
	unit->ynm1 = ynm1;
	unit->znm1 = znm1;
	unit->frac = frac;
}

void RosslerL_Ctor(RosslerL* unit){
	SETCALC(RosslerL_next);

	unit->x0 = unit->xn = unit->xnm1 = ZIN0(5);
	unit->y0 = unit->yn = unit->ynm1 = ZIN0(6);
	unit->z0 = unit->zn = unit->znm1 = ZIN0(7);
	unit->counter = 0.f;
	unit->frac = 0.f;

	RosslerL_next(unit, 1);
}


//////////////////////////////////////////////////////////////////


void FincoSprottL_next(FincoSprottL *unit, int inNumSamples)
{
	float *xout = ZOUT(0);
	float *yout = ZOUT(1);
	float *zout = ZOUT(2);
	float freq = ZIN0(0);
	double a = ZIN0(1);
	double h = ZIN0(2);
	double x0 = ZIN0(3);
	double y0 = ZIN0(4);
	double z0 = ZIN0(5);

	double xn = unit->xn;
	double yn = unit->yn;
	double zn = unit->zn;
	float counter = unit->counter;
	double xnm1 = unit->xnm1;
	double ynm1 = unit->ynm1;
	double znm1 = unit->znm1;
	double frac = unit->frac;

	float samplesPerCycle;
	double slope;
	if(freq < unit->mRate->mSampleRate){
		samplesPerCycle = unit->mRate->mSampleRate / sc_max(freq, 0.001f);
		slope = 1.f / samplesPerCycle;
	}
	else {
		samplesPerCycle = 1.f;
		slope = 1.f;
	}

	if((unit->x0 != x0) || (unit->y0 != y0) || (unit->z0 != z0)){
		xnm1 = xn;
		ynm1 = yn;
		znm1 = zn;
		unit->x0 = xn = x0;
		unit->y0 = yn = y0;
		unit->z0 = zn = z0;
	}

	double dx = xn - xnm1;
	double dy = yn - ynm1;
	double dz = zn - znm1;

	for (int i=0; i<inNumSamples; ++i) {
		if(counter >= samplesPerCycle){
			counter -= samplesPerCycle;
			frac = 0.f;

			xnm1 = xn;
			ynm1 = yn;
			znm1 = zn;

			double k1x, k2x, k3x, k4x,
				k1y, k2y, k3y, k4y,
				k1z, k2z, k3z, k4z,
				kxHalf, kyHalf, kzHalf;

			// 4th order Runge-Kutta approximate solution for differential equations

			k1x = h * (ynm1 + znm1);
			k1y = h * (a * sc_abs(xnm1) - ynm1);
			k1z = h * (1.0 - xnm1);
			kxHalf = k1x * 0.5;
			kyHalf = k1y * 0.5;
			kzHalf = k1z * 0.5;

			k2x = h * (ynm1 + kyHalf + znm1 + kzHalf);
			k2y = h * (a * sc_abs(xnm1 + kxHalf) - ynm1 - kyHalf);
			k2z = h * (1.0 - xnm1 - kxHalf);
			kxHalf = k2x * 0.5;
			kyHalf = k2y * 0.5;
			kzHalf = k2z * 0.5;

			k3x = h * (ynm1 + kyHalf + znm1 + kzHalf);
			k3y = h * (a * sc_abs(xnm1 + kxHalf) - ynm1 - kyHalf);
			k3z = h * (1.0 - xnm1 - kxHalf);

			k4x = h * (ynm1 + k3y + znm1 + k3z);
			k4y = h * (a * sc_abs(xnm1 + k3x) - ynm1 - k3y);
			k4z = h * (1.0 - xnm1 - k3x);

			xn = xn + (k1x + 2.0*(k2x + k3x) + k4x) * ONESIXTH;
			yn = yn + (k1y + 2.0*(k2y + k3y) + k4y) * ONESIXTH;
			zn = zn + (k1z + 2.0*(k2z + k3z) + k4z) * ONESIXTH;

			dx = xn - xnm1;
			dy = yn - ynm1;
			dz = zn - znm1;
		}
		counter++;
		ZXP(xout) = (xnm1 + dx * frac) * 0.5f;
		ZXP(yout) = (ynm1 + dy * frac) * 0.5f;
		ZXP(zout) = (znm1 + dz * frac) * 1.0f;
		frac += slope;
	}

	unit->xn = xn;
	unit->yn = yn;
	unit->zn = zn;
	unit->counter = counter;
	unit->xnm1 = xnm1;
	unit->ynm1 = ynm1;
	unit->znm1 = znm1;
	unit->frac = frac;
}

void FincoSprottL_Ctor(FincoSprottL* unit){
	SETCALC(FincoSprottL_next);

	unit->x0 = unit->xn = unit->xnm1 = ZIN0(3);
	unit->y0 = unit->yn = unit->ynm1 = ZIN0(4);
	unit->z0 = unit->zn = unit->znm1 = ZIN0(5);
	unit->counter = 0.f;
	unit->frac = 0.f;

	FincoSprottL_next(unit, 1);
}

//////////////////////////////////////////////////////////////////

void FincoSprottM_next(FincoSprottM *unit, int inNumSamples)
{
	float *xout = ZOUT(0);
	float *yout = ZOUT(1);
	float *zout = ZOUT(2);
	float freq = ZIN0(0);
	double a = ZIN0(1);
	double b = ZIN0(2);
	double h = ZIN0(3);
	double x0 = ZIN0(4);
	double y0 = ZIN0(5);
	double z0 = ZIN0(6);

	double xn = unit->xn;
	double yn = unit->yn;
	double zn = unit->zn;
	float counter = unit->counter;
	double xnm1 = unit->xnm1;
	double ynm1 = unit->ynm1;
	double znm1 = unit->znm1;
	double frac = unit->frac;

	float samplesPerCycle;
	double slope;
	if(freq < unit->mRate->mSampleRate){
		samplesPerCycle = unit->mRate->mSampleRate / sc_max(freq, 0.001f);
		slope = 1.f / samplesPerCycle;
	}
	else {
		samplesPerCycle = 1.f;
		slope = 1.f;
	}

	if((unit->x0 != x0) || (unit->y0 != y0) || (unit->z0 != z0)){
		xnm1 = xn;
		ynm1 = yn;
		znm1 = zn;
		unit->x0 = xn = x0;
		unit->y0 = yn = y0;
		unit->z0 = zn = z0;
	}

	double dx = xn - xnm1;
	double dy = yn - ynm1;
	double dz = zn - znm1;

	for (int i=0; i<inNumSamples; ++i) {
		if(counter >= samplesPerCycle){
			counter -= samplesPerCycle;
			frac = 0.f;

			xnm1 = xn;
			ynm1 = yn;
			znm1 = zn;

			double k1x, k2x, k3x, k4x,
				k1y, k2y, k3y, k4y,
				k1z, k2z, k3z, k4z,
				kxHalf, kyHalf, kzHalf;

			// 4th order Runge-Kutta approximate solution for differential equations

			k1x = h * (0.0 - znm1);
			k1y = h * (a * sc_abs(xnm1) - ynm1);
			k1z = h * (1.0 + b * xnm1 + ynm1);
			kxHalf = k1x * 0.5;
			kyHalf = k1y * 0.5;
			kzHalf = k1z * 0.5;

			k2x = h * (0.0 - znm1 - kzHalf);
			k2y = h * (a * sc_abs(xnm1 + kxHalf) - ynm1 - kyHalf);
			k2z = h * (1.0 + b * (xnm1 + kxHalf) + ynm1 + kyHalf);
			kxHalf = k2x * 0.5;
			kyHalf = k2y * 0.5;
			kzHalf = k2z * 0.5;

			k3x = h * (0.0 - znm1 - kzHalf);
			k3y = h * (a * sc_abs(xnm1 + kxHalf) - ynm1 - kyHalf);
			k3z = h * (1.0 + b * (xnm1 + kxHalf) + ynm1 + kyHalf);

			k4x = h * (0.0 - znm1 - k3z);
			k4y = h * (a * sc_abs(xnm1 + k3x) - ynm1 - k3y);
			k4z = h * (1.0 + b * (xnm1 + k3x) + ynm1 + k3y);

			xn = xn + (k1x + 2.0*(k2x + k3x) + k4x) * ONESIXTH;
			yn = yn + (k1y + 2.0*(k2y + k3y) + k4y) * ONESIXTH;
			zn = zn + (k1z + 2.0*(k2z + k3z) + k4z) * ONESIXTH;

			dx = xn - xnm1;
			dy = yn - ynm1;
			dz = zn - znm1;
		}
		counter++;
		ZXP(xout) = (xnm1 + dx * frac) * 0.5f;
		ZXP(yout) = (ynm1 + dy * frac) * 0.5f;
		ZXP(zout) = (znm1 + dz * frac) * 1.0f;
		frac += slope;
	}

	unit->xn = xn;
	unit->yn = yn;
	unit->zn = zn;
	unit->counter = counter;
	unit->xnm1 = xnm1;
	unit->ynm1 = ynm1;
	unit->znm1 = znm1;
	unit->frac = frac;
}

void FincoSprottM_Ctor(FincoSprottM* unit){
	SETCALC(FincoSprottM_next);

	unit->x0 = unit->xn = unit->xnm1 = ZIN0(3);
	unit->y0 = unit->yn = unit->ynm1 = ZIN0(4);
	unit->z0 = unit->zn = unit->znm1 = ZIN0(5);
	unit->counter = 0.f;
	unit->frac = 0.f;

	FincoSprottM_next(unit, 1);
}

//////////////////////////////////////////////////////////////////

void FincoSprottS_next(FincoSprottS *unit, int inNumSamples)
{
	float *xout = ZOUT(0);
	float *yout = ZOUT(1);
	float *zout = ZOUT(2);
	float freq = ZIN0(0);
	double a = ZIN0(1);
	double b = ZIN0(2);
	double h = ZIN0(3);
	double x0 = ZIN0(4);
	double y0 = ZIN0(5);
	double z0 = ZIN0(6);

	double xn = unit->xn;
	double yn = unit->yn;
	double zn = unit->zn;
	float counter = unit->counter;
	double xnm1 = unit->xnm1;
	double ynm1 = unit->ynm1;
	double znm1 = unit->znm1;
	double frac = unit->frac;

	float samplesPerCycle;
	double slope;
	if(freq < unit->mRate->mSampleRate){
		samplesPerCycle = unit->mRate->mSampleRate / sc_max(freq, 0.001f);
		slope = 1.f / samplesPerCycle;
	}
	else {
		samplesPerCycle = 1.f;
		slope = 1.f;
	}

	if((unit->x0 != x0) || (unit->y0 != y0) || (unit->z0 != z0)){
		xnm1 = xn;
		ynm1 = yn;
		znm1 = zn;
		unit->x0 = xn = x0;
		unit->y0 = yn = y0;
		unit->z0 = zn = z0;
	}

	double dx = xn - xnm1;
	double dy = yn - ynm1;
	double dz = zn - znm1;

	for (int i=0; i<inNumSamples; ++i) {
		if(counter >= samplesPerCycle){
			counter -= samplesPerCycle;
			frac = 0.f;

			xnm1 = xn;
			ynm1 = yn;
			znm1 = zn;

			double k1x, k2x, k3x, k4x,
				k1y, k2y, k3y, k4y,
				k1z, k2z, k3z, k4z,
				kxHalf, kyHalf, kzHalf;

			// 4th order Runge-Kutta approximate solution for differential equations

			k1x = h * (0.0 - (xnm1 + a * ynm1));
			k1y = h * (xnm1 + b * sc_abs(znm1));
			k1z = h * (xnm1 + 1.0);
			kxHalf = k1x * 0.5;
			kyHalf = k1y * 0.5;
			kzHalf = k1z * 0.5;

			k2x = h * (0.0 - (xnm1 + kxHalf + a * (ynm1 + kyHalf)));
			k2y = h * (xnm1 + kxHalf + b * sc_abs(znm1 + kzHalf));
			k2z = h * (xnm1 + kxHalf + 1.0);
			kxHalf = k2x * 0.5;
			kyHalf = k2y * 0.5;
			kzHalf = k2z * 0.5;

			k3x = h * (0.0 - (xnm1 + kxHalf + a * (ynm1 + kyHalf)));
			k3y = h * (xnm1 + kxHalf + b * sc_abs(znm1 + kzHalf));
			k3z = h * (xnm1 + kxHalf + 1.0);

			k4x = h * (0.0 - (xnm1 + k3x + a * (ynm1 + k3y)));
			k4y = h * (xnm1 + k3x + b * sc_abs(znm1 + k3z));
			k4z = h * (xnm1 + k3x + 1.0);

			xn = xn + (k1x + 2.0*(k2x + k3x) + k4x) * ONESIXTH;
			yn = yn + (k1y + 2.0*(k2y + k3y) + k4y) * ONESIXTH;
			zn = zn + (k1z + 2.0*(k2z + k3z) + k4z) * ONESIXTH;

			dx = xn - xnm1;
			dy = yn - ynm1;
			dz = zn - znm1;
		}
		counter++;
		ZXP(xout) = (xnm1 + dx * frac) * 0.5f;
		ZXP(yout) = (ynm1 + dy * frac) * 0.5f;
		ZXP(zout) = (znm1 + dz * frac) * 1.0f;
		frac += slope;
	}

	unit->xn = xn;
	unit->yn = yn;
	unit->zn = zn;
	unit->counter = counter;
	unit->xnm1 = xnm1;
	unit->ynm1 = ynm1;
	unit->znm1 = znm1;
	unit->frac = frac;
}

void FincoSprottS_Ctor(FincoSprottS* unit){
	SETCALC(FincoSprottS_next);

	unit->x0 = unit->xn = unit->xnm1 = ZIN0(3);
	unit->y0 = unit->yn = unit->ynm1 = ZIN0(4);
	unit->z0 = unit->zn = unit->znm1 = ZIN0(5);
	unit->counter = 0.f;
	unit->frac = 0.f;

	FincoSprottS_next(unit, 1);
}

//////////////////////////////////////////////////////////////////

// Perlin noise isn't chaos, I know. But I'm adding it to this collection anyway.
// Based on java code by Ken Perlin, published at http://mrl.nyu.edu/~perlin/noise/
static int p[512], permutation[256] = { 151,160,137,91,90,15,
131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
};

static double fade(double t) { return t * t * t * (t * (t * 6. - 15.) + 10.); }
static double lerp(double t, double a, double b) { return a + t * (b - a); }
static double grad(int hash, double x, double y, double z) {
  int h = hash & 15;                      // CONVERT LO 4 BITS OF HASH CODE
  double u = h<8 ? x : y,                 // INTO 12 GRADIENT DIRECTIONS.
		 v = h<4 ? y : h==12||h==14 ? x : z;
  return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
}
double perlin_noise(double x, double y, double z) {
	int X = (int)std::floor(x) & 255,                  // FIND UNIT CUBE THAT
	  Y = (int)std::floor(y) & 255,                  // CONTAINS POINT.
	  Z = (int)std::floor(z) & 255;
	x -= std::floor(x);                                // FIND RELATIVE X,Y,Z
	y -= std::floor(y);                                // OF POINT IN CUBE.
	z -= std::floor(z);
	double u = fade(x),                                // COMPUTE FADE CURVES
		 v = fade(y),                                // FOR EACH OF X,Y,Z.
		 w = fade(z);
	int A = p[X  ]+Y, AA = p[A]+Z, AB = p[A+1]+Z,      // HASH COORDINATES OF
	  B = p[X+1]+Y, BA = p[B]+Z, BB = p[B+1]+Z;      // THE 8 CUBE CORNERS,

	return lerp(w, lerp(v, lerp(u, grad(p[AA  ], x  , y  , z   ),  // AND ADD
								 grad(p[BA  ], x-1, y  , z   )), // BLENDED
						 lerp(u, grad(p[AB  ], x  , y-1, z   ),  // RESULTS
								 grad(p[BB  ], x-1, y-1, z   ))),// FROM  8
				 lerp(v, lerp(u, grad(p[AA+1], x  , y  , z-1 ),  // CORNERS
								 grad(p[BA+1], x-1, y  , z-1 )), // OF CUBE
						 lerp(u, grad(p[AB+1], x  , y-1, z-1 ),
								 grad(p[BB+1], x-1, y-1, z-1 ))));
}



void Perlin3_next(Perlin3 *unit, int inNumSamples)
{
	float *x = IN(0), *y = IN(1), *z = IN(2), *out = OUT(0);
	for (int i=0; i<inNumSamples; ++i) {
		out[i] = perlin_noise(x[i], y[i], z[i]);
		//printf("result @[%g, %g, %g] is %g\n", x[i], y[i], z[i], out[i]);
	}
}

void Perlin3_Ctor(Perlin3* unit){
	SETCALC(Perlin3_next);
	Perlin3_next(unit, 1);
}

//////////////////////////////////////////////////////////////////


// the load function is called by the host when the plug-in is loaded
PluginLoad(MCLDChaos)
{
	ft = inTable;

	// perlin noise init
	for (int i=0; i < 256 ; i++) p[256+i] = p[i] = permutation[i];

	DefineSimpleUnit(RosslerL);
	DefineSimpleUnit(FincoSprottL);
	DefineSimpleUnit(FincoSprottM);
	DefineSimpleUnit(FincoSprottS);
	DefineSimpleUnit(Perlin3);
}

////////////////////////////////////////////////////////////////////
