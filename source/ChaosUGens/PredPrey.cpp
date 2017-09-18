/*
 *  predprey.cpp
 *
 *  Implemented 2017 by Till Bovermann.
 *  Partly based on Lance Putnam's chaos UGens, 
 *  MCLDs ChaosUGens, and 
 *  Gary William Flake's Computational Beauty of Nature.
 * 
 *  Available under the terms of the GNU Public License (GPL).
 *
 */

#include "SC_PlugIn.h"
#define ONESIXTH 0.1666666666666667

// InterfaceTable contains pointers to functions in the host (server).
static InterfaceTable *ft;

// declare struct to hold unit generator state
struct ArneodoCoulletTresser : public Unit {
	double x0;
	double y0;
	double z0;
	double xn;
	double yn;
	double zn;
	double xnm1;
	double ynm1;
	double znm1;
	float  counter;
	double frac;
};

struct LotkaVolterra : public Unit {
	double x0;
	double y0;
	double xn;
	double yn;
	double xnm1;
	double ynm1;
	float  counter;
	double frac;
};


// declare unit generator functions
extern "C"
{
	void ArneodoCoulletTresser_next(ArneodoCoulletTresser *unit, int inNumSamples);
	void ArneodoCoulletTresser_Ctor(ArneodoCoulletTresser *unit);

	void LotkaVolterra_next(LotkaVolterra *unit, int inNumSamples);
	void LotkaVolterra_Ctor(LotkaVolterra *unit);
};

///////////////// ArneodoCoulletTresser /////////////////////////////////////////////////

void ArneodoCoulletTresser_next(ArneodoCoulletTresser *unit, int inNumSamples)
{
	float *xout = ZOUT(0);
	float *yout = ZOUT(1);
	float *zout = ZOUT(2);
	float  freq = ZIN0(0);
	double alpha = ZIN0(1);
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

	// reset if start values change
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
			k1x = h * (xnm1 * (1.1 - xnm1 / 2 - ynm1 / 2 - znm1 / 10));
			k1y = h * (ynm1 * (-0.5 + xnm1 / 2 + ynm1 / 10 - znm1 / 10));
			k1z = h * (znm1 * (alpha + 0.2 - alpha * xnm1 - ynm1 / 10 - znm1 / 10));
			kxHalf = k1x * 0.5;
			kyHalf = k1y * 0.5;
			kzHalf = k1z * 0.5;

			k2x = h * ((xnm1 + kxHalf) * (1.1 - (xnm1 + kxHalf) / 2 - (ynm1 + kyHalf) / 2 - (znm1 + kzHalf) / 10));
			k2y = h * ((ynm1 + kyHalf) * (-0.5 + (xnm1 + kxHalf) / 2 + (ynm1 + kyHalf) / 10 - (znm1 + kzHalf) / 10));
			k2z = h * ((znm1 + kzHalf) * (alpha + 0.2 - alpha * (xnm1 + kxHalf) - (ynm1 + kyHalf) / 10 - (znm1 + kzHalf) / 10));
			kxHalf = k2x * 0.5;
			kyHalf = k2y * 0.5;
			kzHalf = k2z * 0.5;

			k3x = h * ((xnm1 + kxHalf) * (1.1 - (xnm1 + kxHalf) / 2 - (ynm1 + kyHalf) / 2 - (znm1 + kzHalf) / 10));
			k3y = h * ((ynm1 + kyHalf) * (-0.5 + (xnm1 + kxHalf) / 2 + (ynm1 + kyHalf) / 10 - (znm1 + kzHalf) / 10));
			k3z = h * ((znm1 + kzHalf) * (alpha + 0.2 - alpha * (xnm1 + kxHalf) - (ynm1 + kyHalf) / 10 - (znm1 + kzHalf) / 10));

			k4x = h * ((xnm1 + k3x) * (1.1 - (xnm1 + k3x) / 2 - (ynm1 + k3y) / 2 - (znm1 + k3z) / 10));
			k4y = h * ((ynm1 + k3y) * (-0.5 + (xnm1 + k3x) / 2 + (ynm1 + k3y) / 10 - (znm1 + k3z) / 10));
			k4z = h * ((znm1 + k3z) * (alpha + 0.2 - alpha * (xnm1 + k3x) - (ynm1 + k3y) / 10 - (znm1 + k3z) / 10));


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

void ArneodoCoulletTresser_Ctor(ArneodoCoulletTresser* unit){
	SETCALC(ArneodoCoulletTresser_next);

	unit->x0 = unit->xn = unit->xnm1 = ZIN0(3);
	unit->y0 = unit->yn = unit->ynm1 = ZIN0(4);
	unit->z0 = unit->zn = unit->znm1 = ZIN0(5);
	unit->counter = 0.f;
	unit->frac = 0.f;

	ArneodoCoulletTresser_next(unit, 1);
}


///////////////// LotkaVolterra /////////////////////////////////////////////////

void LotkaVolterra_next(LotkaVolterra *unit, int inNumSamples)
{
	float *xout = ZOUT(0);
	float *yout = ZOUT(1);
	float  freq = ZIN0(0);
	double a = ZIN0(1);
	double b = ZIN0(2);
	double c = ZIN0(3);
	double d = ZIN0(4);
	double h = ZIN0(5);
	double x0 = ZIN0(6);
	double y0 = ZIN0(7);

	double xn = unit->xn;
	double yn = unit->yn;
	float counter = unit->counter;
	double xnm1 = unit->xnm1;
	double ynm1 = unit->ynm1;
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

	// reset if start values change
	if((unit->x0 != x0) || (unit->y0 != y0)){
		xnm1 = xn;
		ynm1 = yn;
		unit->x0 = xn = x0;
		unit->y0 = yn = y0;
	}

	double dx = xn - xnm1;
	double dy = yn - ynm1;

	for (int i=0; i<inNumSamples; ++i) {
		if(counter >= samplesPerCycle){
			counter -= samplesPerCycle;
			frac = 0.f;

			xnm1 = xn;
			ynm1 = yn;

			double k1x, k2x, k3x, k4x,
				k1y, k2y, k3y, k4y,
				kxHalf, kyHalf;

			// 4th order Runge-Kutta approximate solution for differential equations
			k1x = h * (xnm1 * (a - b * ynm1));
			k1y = h * (ynm1 * (c * xnm1 - d));
			kxHalf = k1x * 0.5;
			kyHalf = k1y * 0.5;

			k2x = h * ((xnm1 + kxHalf) * (a - b * (ynm1 + kyHalf)));
			k2y = h * ((ynm1 + kyHalf) * (c * (xnm1 + kxHalf) - d));

			kxHalf = k2x * 0.5;
			kyHalf = k2y * 0.5;

			k3x = h * ((xnm1 + kxHalf) * (a - b * (ynm1 + kyHalf)));
			k3y = h * ((ynm1 + kyHalf) * (c * (xnm1 + kxHalf) - d));

			k4x = h * ((xnm1 + k3x) * (a - b * (ynm1 + k3y)));
			k4y = h * ((ynm1 + k3y) * (c * (xnm1 + k3x) - d));

			xn = xn + (k1x + 2.0*(k2x + k3x) + k4x) * ONESIXTH;
			yn = yn + (k1y + 2.0*(k2y + k3y) + k4y) * ONESIXTH;

			dx = xn - xnm1;
			dy = yn - ynm1;
		}
		counter++;
		ZXP(xout) = (xnm1 + dx * frac) * 0.5f;
		ZXP(yout) = (ynm1 + dy * frac) * 0.5f;
		frac += slope;
	}

	unit->xn = xn;
	unit->yn = yn;
	unit->counter = counter;
	unit->xnm1 = xnm1;
	unit->ynm1 = ynm1;
	unit->frac = frac;
}

void LotkaVolterra_Ctor(LotkaVolterra* unit){
	SETCALC(LotkaVolterra_next);

	unit->x0 = unit->xn = unit->xnm1 = ZIN0(6);
	unit->y0 = unit->yn = unit->ynm1 = ZIN0(7);
	unit->counter = 0.f;
	unit->frac = 0.f;

	LotkaVolterra_next(unit, 1);
}


//////////////////////////////////////////////////////////////////


// the load function is called by the host when the plug-in is loaded
PluginLoad(Chaos)
{
	ft = inTable;

	DefineSimpleUnit(ArneodoCoulletTresser);
	DefineSimpleUnit(LotkaVolterra);
}

////////////////////////////////////////////////////////////////////
