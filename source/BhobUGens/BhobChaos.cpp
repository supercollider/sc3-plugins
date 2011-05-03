/* a set of chaos waveforms in which one equation maps to frequency, the other to amplitude.
this code is by bhob rainey, but owes a great debt to Lance Putnam for his chaos ugens, and to Nick Collins for his
FitzHughNagumo ugen
http://www.bhobrainey.net

SuperCollider real time audio synthesis system
    Copyright (c) 2002 James McCartney. All rights reserved.
	http://www.audiosynth.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#include "SC_PlugIn.h"
#define TWOPI 6.283185307179586
#define PI 3.141592653589793
#define RECPI 0.3183098861837907
#define RECTWOPI 0.1591549430918953
#define ONESIXTH 0.1666666666666667

static InterfaceTable *ft;

struct Henon2DN : public Unit
{
	float mfreqMul;
	double mx0, my0, mxn, myn, myclip, mxscale, myscale, mphase, mspeed;
};

struct Henon2DL : public Unit
{
	float mfreqMul;
	double mx0, my0, mxn, myn, mynm1, myclip, mxscale, myscale, mphase, mspeed;
};

struct Henon2DC : public Unit
{
	double mx0, my0, mxn, myn, myclip, mlevel, mxscale, myscale, mslope, mcurve;
	int32 mphase;
};

struct HenonTrig : public Unit
{
	float mfreqMul;
	double mx0, my0, mxn, myn, mxscale, myscale, mphase, mspeed;
};

struct Latoocarfian2DN : public Unit
{
	float mfreqMul;
	double mx0, my0, mxn, myn, myclip, mxscale, mphase, mspeed;
};

struct Latoocarfian2DL : public Unit
{
	float mfreqMul;
	double mx0, my0, mxn, myn, mynm1, myclip, mxscale, mphase, mspeed;
};

struct Latoocarfian2DC : public Unit
{
	double mx0, my0, mxn, myn, myclip, mlevel, mxscale, mslope, mcurve;
	int32 mphase;
};

struct LatoocarfianTrig : public Unit
{
	float mfreqMul;
	double mx0, my0, mxn, myn, mxscale, mphase, mspeed;
};

struct Lorenz2DN : public Unit
{
	float mfreqMul;
	double mx0, my0, mz0, mxn, myn, mzn, myclip, mxscale, myscale, mphase, mspeed;
};

struct Lorenz2DL : public Unit
{
	float mfreqMul;
	double mx0, my0, mz0, mxn, myn, mzn, mynm1, myclip, mxscale, myscale, mphase, mspeed;
};

struct Lorenz2DC : public Unit
{
	int32 mphase;
	double mx0, my0, mz0, mxn, myn, mzn, myclip, mxscale, myscale, mlevel, mslope, mcurve;
};

struct LorenzTrig : public Unit
{
	float mfreqMul;
	double mx0, my0, mz0, mxn, myn, mzn, mxscale, myscale, mphase, mspeed;
};

struct Standard2DN : public Unit
{
	float mfreqMul;
	double mx0, my0, mxn, myn, myclip, mscale, mphase, mspeed;
};

struct Standard2DL : public Unit
{
	float mfreqMul;
	double mx0, my0, mxn, myn, mynm1, myclip, mscale, mphase, mspeed;
};

struct Standard2DC : public Unit
{
	int32 mphase;
	double mx0, my0, mxn, myn, myclip, mscale, mlevel, mslope, mcurve;
};

struct StandardTrig : public Unit
{
	float mfreqMul;
	double mx0, my0, mxn, myn, mscale, mphase, mspeed;
};

struct Gbman2DN : public Unit
{
	float mfreqMul;
	double mx0, my0, mxn, myn, myclip, mscale, mphase, mspeed;
};

struct Gbman2DL : public Unit
{
	float mfreqMul;
	double mx0, my0, mxn, myn, mynm1, myclip, mscale, mphase, mspeed;
};

struct Gbman2DC : public Unit
{
	int32 mphase;
	double mx0, my0, mxn, myn, myclip, mscale, mslope, mcurve, mlevel;
};

struct GbmanTrig : public Unit
{
	float mfreqMul;
	double mx0, my0, mxn, myn, mscale, mphase, mspeed;
};

struct Fhn2DN : public Unit
{
	float mfreqMul;
	double mu0, mw0, mun, mwn, muclip, mphase, mspeed;
};

struct Fhn2DL : public Unit
{
	float mfreqMul;
	double mu0, mw0, mun, mwn, muclip, um1, mphase, mspeed;
};

struct Fhn2DC : public Unit
{
	int32 mphase;
	double mu0, mw0, mun, mwn, muclip, um1, mslope, mcurve, mlevel;
};

struct FhnTrig : public Unit
{
	float mfreqMul;
	double mu0, mw0, mun, mwn, mphase, mspeed;
};

extern "C"
{
	void load(InterfaceTable *inTable);

	void Henon2DN_Ctor(Henon2DN* unit);
	void Henon2DN_next(Henon2DN *unit, int inNumSamples);

	void Henon2DL_Ctor(Henon2DL* unit);
	void Henon2DL_next(Henon2DL *unit, int inNumSamples);

	void Henon2DC_Ctor(Henon2DC* unit);
	void Henon2DC_next(Henon2DC *unit, int inNumSamples);

	void HenonTrig_Ctor(HenonTrig* unit);
	void HenonTrig_next(HenonTrig *unit, int inNumSamples);

	void Latoocarfian2DN_Ctor(Latoocarfian2DN* unit);
	void Latoocarfian2DN_next(Latoocarfian2DN *unit, int inNumSamples);

	void Latoocarfian2DL_Ctor(Latoocarfian2DL* unit);
	void Latoocarfian2DL_next(Latoocarfian2DL *unit, int inNumSamples);

	void Latoocarfian2DC_Ctor(Latoocarfian2DC* unit);
	void Latoocarfian2DC_next(Latoocarfian2DC *unit, int inNumSamples);

	void LatoocarfianTrig_Ctor(LatoocarfianTrig* unit);
	void LatoocarfianTrig_next(LatoocarfianTrig *unit, int inNumSamples);

	void Lorenz2DN_Ctor(Lorenz2DN* unit);
	void Lorenz2DN_next(Lorenz2DN *unit, int inNumSamples);

	void Lorenz2DL_Ctor(Lorenz2DL* unit);
	void Lorenz2DL_next(Lorenz2DL *unit, int inNumSamples);

	void Lorenz2DC_Ctor(Lorenz2DC* unit);
	void Lorenz2DC_next(Lorenz2DC *unit, int inNumSamples);

	void LorenzTrig_Ctor(LorenzTrig* unit);
	void LorenzTrig_next(LorenzTrig *unit, int inNumSamples);

	void Standard2DN_Ctor(Standard2DN* unit);
	void Standard2DN_next(Standard2DN *unit, int inNumSamples);

	void Standard2DL_Ctor(Standard2DL* unit);
	void Standard2DL_next(Standard2DL *unit, int inNumSamples);

	void Standard2DC_Ctor(Standard2DC* unit);
	void Standard2DC_next(Standard2DC *unit, int inNumSamples);

	void StandardTrig_Ctor(StandardTrig* unit);
	void StandardTrig_next(StandardTrig *unit, int inNumSamples);

	void Gbman2DN_Ctor(Gbman2DN* unit);
	void Gbman2DN_next(Gbman2DN *unit, int inNumSamples);

	void Gbman2DL_Ctor(Gbman2DL* unit);
	void Gbman2DL_next(Gbman2DL *unit, int inNumSamples);

	void Gbman2DC_Ctor(Gbman2DC* unit);
	void Gbman2DC_next(Gbman2DC *unit, int inNumSamples);

	void GbmanTrig_Ctor(GbmanTrig* unit);
	void GbmanTrig_next(GbmanTrig *unit, int inNumSamples);

	void Fhn2DN_Ctor(Fhn2DN* unit);
	void Fhn2DN_next(Fhn2DN *unit, int inNumSamples);

	void Fhn2DL_Ctor(Fhn2DL* unit);
	void Fhn2DL_next(Fhn2DL *unit, int inNumSamples);

	void Fhn2DC_Ctor(Fhn2DC* unit);
	void Fhn2DC_next(Fhn2DC *unit, int inNumSamples);

	void FhnTrig_Ctor(FhnTrig* unit);
	void FhnTrig_next(FhnTrig *unit, int inNumSamples);
};

inline double mod2pi(double in) {
	const double lo = (double)0.;
	const double hi = TWOPI;

	if (in >= hi) {
		in -= hi;
		if (in < hi) return in;
	} else if (in < lo) {
		in += hi;
		if (in >= lo) return in;
	} else return in;

	return in - hi * (double)((int)(in * RECTWOPI));
}

float Mirroring (float lower, float upper, float in);
/*
float Mirroring (float lower, float upper, float in)
{
	if(in>upper || in<lower) {
		float range= (upper-lower);
		if(in<lower) in=(2*upper-lower)-in;
		in=fmod(in-upper, 2*range);
		if(in<range) in=upper-in;
		else in=in-(range);
	}
	return in;
}
*/

float Mirroring (float lower, float upper, float in)
{
	if(in>upper || in<lower) {
		if(in<lower) {
			float diff=lower - in;
			in = sc_min(upper, lower + diff);
		} else {
			float diff=in - upper;
			in = sc_max(lower, upper - diff);
		}
	}
	return in;
}

void Henon2DN_Ctor(Henon2DN *unit)
{
	unit->mx0=ZIN0(4);
	unit->my0=ZIN0(5);
	unit->mphase=1.f;
	unit->mspeed=100.f;
	unit->mxn=unit->mx0;
	unit->myn=unit->my0;
	unit->mxscale=0.77850360953955;
	unit->myscale=2.594;
	unit->mfreqMul=unit->mRate->mSampleDur;
	SETCALC(Henon2DN_next);
	ZOUT0(0) = 0.f;
}

void Henon2DN_next(Henon2DN *unit, int inNumSamples)
{
	float *out=ZOUT(0);
	float minf=ZIN0(0);
	float maxf=ZIN0(1);
	double a=ZIN0(2);
	double b=ZIN0(3);
	double x0=ZIN0(4);
	double y0=ZIN0(5);
	double xn=unit->mxn;
	double yn=unit->myn;
	double yclip=unit->myclip;
	double yscale=unit->myscale;
	double phase=unit->mphase;
	double speed=unit->mspeed;

	if (x0 != unit->mx0) unit->mx0=xn=x0;
	if (y0 != unit->my0) unit->my0=yn=y0;
	LOOP(inNumSamples,
		if (phase >= 1.f) {
			phase -= 1.f;
			double newx = yn + 1 - a * xn * xn;
			double rate = (newx * unit->mxscale + 1) * 0.5;
			rate=Mirroring(0.0, 1.0, rate);
			speed = (minf + ((maxf-minf)*rate)) * unit->mfreqMul;

			yn=b * xn;
			yclip=yn * yscale;
			yclip=Mirroring(-1.0, 1.0, yclip);
			xn=newx;
		}

		ZXP(out) = yclip;
		phase += speed;
	);

	unit->mphase=phase;
	unit->mspeed=speed;
	unit->mxn=xn;
	unit->myn=yn;
	unit->myclip=yclip;
}

void Henon2DL_Ctor(Henon2DL *unit)
{
	unit->mx0=ZIN0(4);
	unit->my0=ZIN0(5);
	unit->mphase=1.f;
	unit->mspeed=100.f;
	unit->mxn=unit->mx0;
	unit->myn=unit->my0;
	unit->mxscale=0.77850360953955;
	unit->myscale=2.594;
	unit->mfreqMul=unit->mRate->mSampleDur;
	SETCALC(Henon2DL_next);
	ZOUT0(0) = 0.f;
}

void Henon2DL_next(Henon2DL *unit, int inNumSamples)
{
	float *out=ZOUT(0);
	float minf=ZIN0(0);
	float maxf=ZIN0(1);
	double a=ZIN0(2);
	double b=ZIN0(3);
	double x0=ZIN0(4);
	double y0=ZIN0(5);
	double xn=unit->mxn;
	double yn=unit->myn;
	double yclip=unit->myclip;
	double ynm1=unit->mynm1;
	double yscale=unit->myscale;
	double phase=unit->mphase;
	double speed=unit->mspeed;

	if (x0 != unit->mx0) unit->mx0=xn=x0;
	if (y0 != unit->my0) unit->my0=yn=y0;
	LOOP(inNumSamples,
		if (phase >= 1.f) {
			phase -= 1.f;
			double newx = yn + 1 - a * xn * xn;
			double rate = (newx * unit->mxscale + 1) * 0.5;
			rate=Mirroring(0.0, 1.0, rate);
			speed = (minf + ((maxf-minf)*rate)) * unit->mfreqMul;

			ynm1=yclip;
			yn=b * xn;
			yclip=yn * yscale;
			yclip=Mirroring(-1.0, 1.0, yclip);
			xn=newx;
		}

		ZXP(out) = (1-phase) * ynm1 + phase * yclip;
		phase += speed;
	);

	unit->mphase=phase;
	unit->mspeed=speed;
	unit->mxn=xn;
	unit->myn=yn;
	unit->mynm1=ynm1;
	unit->myclip=yclip;
}

void Henon2DC_Ctor(Henon2DC *unit)
{
	unit->mx0=ZIN0(4);
	unit->my0=ZIN0(5);
	unit->mphase=0;
	unit->mslope=0.f;
	unit->mxn=unit->mx0;
	unit->myn=unit->my0;
	unit->mxscale=0.77850360953955;
	unit->myscale=2.594;
	unit->mlevel=unit->my0 * unit->myscale;
	SETCALC(Henon2DC_next);
	ZOUT0(0) = 0.f;
}

void Henon2DC_next(Henon2DC *unit, int inNumSamples)
{
	float *out=ZOUT(0);
	float minf=ZIN0(0);
	float maxf=ZIN0(1);
	double a=ZIN0(2);
	double b=ZIN0(3);
	double x0=ZIN0(4);
	double y0=ZIN0(5);
	double xn=unit->mxn;
	double yn=unit->myn;
	double yscale=unit->myscale;
	int32 phase=unit->mphase;
	double level=unit->mlevel;
	double slope=unit->mslope;
	double curve=unit->mcurve;

	int remain=inNumSamples;

	if (x0 != unit->mx0) unit->mx0=xn=x0;
	if (y0 != unit->my0) unit->my0=yn=y0;
	do {
		if (phase <= 0) {
			double newx = yn + 1 - a * xn * xn;
			double rate = (newx * unit->mxscale + 1) * 0.5;
			rate=Mirroring(0.0, 1.0, rate);
			float fphase = (minf + ((maxf-minf)*rate));
			phase = (int32)(unit->mRate->mSampleRate/sc_max(fphase, 0.001f));
			phase = sc_max(2, phase);

			double ynm1=unit->myclip;
			yn=b * xn;
			double yclip=yn * yscale;
			yclip=Mirroring(-1.0, 1.0, yclip);
			double midpnt=(ynm1 + yclip) * 0.5;
			unit->myclip=yclip;
			curve=2.f * (midpnt - level - phase * slope) / (phase * phase + phase);
			xn=newx;
		}
		int nsmps = sc_min(phase, remain);
		phase -= nsmps;
		remain -= nsmps;
		LOOP(nsmps,
			ZXP(out) = level;
			slope += curve;
			level += slope;
		);
	} while (remain);

	unit->mphase=phase;
	unit->mlevel=level;
	unit->mslope=slope;
	unit->mcurve=curve;
	unit->mxn=xn;
	unit->myn=yn;
}

void HenonTrig_Ctor(HenonTrig *unit)
{
	unit->mx0=ZIN0(4);
	unit->my0=ZIN0(5);
	unit->mphase=1.f;
	unit->mspeed=100.f;
	unit->mxn=unit->mx0;
	unit->myn=unit->my0;
	unit->mxscale=0.77850360953955;
	unit->myscale=2.594;
	unit->mfreqMul=unit->mRate->mSampleDur;
	SETCALC(HenonTrig_next);
	ZOUT0(0) = 0.f;
}

void HenonTrig_next(HenonTrig *unit, int inNumSamples)
{
	float *out=ZOUT(0);
	float minf=ZIN0(0);
	float maxf=ZIN0(1);
	double a=ZIN0(2);
	double b=ZIN0(3);
	double x0=ZIN0(4);
	double y0=ZIN0(5);
	double xn=unit->mxn;
	double yn=unit->myn;
	double yscale=unit->myscale;
	double phase=unit->mphase;
	double speed=unit->mspeed;

	if (x0 != unit->mx0) unit->mx0=xn=x0;
	if (y0 != unit->my0) unit->my0=yn=y0;
	LOOP(inNumSamples,
		if (phase >= 1.f) {
			phase -= 1.f;
			double newx = yn + 1 - a * xn * xn;
			double rate = (newx * unit->mxscale + 1) * 0.5;
			rate=Mirroring(0.0, 1.0, rate);
			speed = (minf + ((maxf-minf)*rate)) * unit->mfreqMul;

			yn=b * xn;
			float yclip=(yn * yscale + 1) * 0.5;
			yclip=Mirroring(0.0, 1.0, yclip);
			xn=newx;
			ZXP(out) = yclip;
		} else ZXP(out) = 0.f;

		phase += speed;
	);

	unit->mphase=phase;
	unit->mspeed=speed;
	unit->mxn=xn;
	unit->myn=yn;
}

void Latoocarfian2DN_Ctor(Latoocarfian2DN *unit)
{
	unit->mx0=ZIN0(6);
	unit->my0=ZIN0(7);
	unit->mphase=1.f;
	unit->mspeed=100.f;
	unit->mxn=unit->mx0;
	unit->myn=unit->my0;
	unit->mxscale=0.70390617562334;
	unit->mfreqMul=unit->mRate->mSampleDur;
	SETCALC(Latoocarfian2DN_next);
	ZOUT0(0) = 0.f;
}

void Latoocarfian2DN_next(Latoocarfian2DN *unit, int inNumSamples)
{
	float *out=ZOUT(0);
	float minf=ZIN0(0);
	float maxf=ZIN0(1);
	double a=ZIN0(2);
	double b=ZIN0(3);
	double c=ZIN0(4);
	double d=ZIN0(5);
	double x0=ZIN0(6);
	double y0=ZIN0(7);
	double xn=unit->mxn;
	double yn=unit->myn;
	double yclip=unit->myclip;
	double phase=unit->mphase;
	double speed=unit->mspeed;

	if (x0 != unit->mx0) unit->mx0=xn=x0;
	if (y0 != unit->my0) unit->my0=yn=y0;
	LOOP(inNumSamples,
		if (phase >= 1.f) {
			phase -= 1.f;
			float newx = sin(b * yn) + c*sin(b * xn);
			double rate = (newx * unit->mxscale + 1) * 0.5;
			rate=Mirroring(0.0, 1.0, rate);
			speed = (minf + ((maxf-minf)*rate)) * unit->mfreqMul;

			yn = sin(a * xn) + d*sin(a * yn);
			yclip=yn * 0.72074001609432;
			yclip=Mirroring(-1.0, 1.0, yclip);
			xn=newx;
		}

		ZXP(out) = yclip;
		phase += speed;
	);

	unit->mphase=phase;
	unit->mspeed=speed;
	unit->mxn=xn;
	unit->myn=yn;
	unit->myclip=yclip;
}

void Latoocarfian2DL_Ctor(Latoocarfian2DL *unit)
{
	unit->mx0=ZIN0(6);
	unit->my0=ZIN0(7);
	unit->mphase=1.f;
	unit->mspeed=100.f;
	unit->mxn=unit->mx0;
	unit->myn=unit->my0;
	unit->mxscale=0.70390617562334;
	unit->mfreqMul=unit->mRate->mSampleDur;
	SETCALC(Latoocarfian2DL_next);
	ZOUT0(0) = 0.f;
}

void Latoocarfian2DL_next(Latoocarfian2DL *unit, int inNumSamples)
{
	float *out=ZOUT(0);
	float minf=ZIN0(0);
	float maxf=ZIN0(1);
	double a=ZIN0(2);
	double b=ZIN0(3);
	double c=ZIN0(4);
	double d=ZIN0(5);
	double x0=ZIN0(6);
	double y0=ZIN0(7);
	double xn=unit->mxn;
	double yn=unit->myn;
	double ynm1=unit->mynm1;
	double yclip=unit->myclip;
	double phase=unit->mphase;
	double speed=unit->mspeed;

	if (x0 != unit->mx0) unit->mx0=xn=x0;
	if (y0 != unit->my0) unit->my0=yn=y0;
	LOOP(inNumSamples,
		if (phase >= 1.f) {
			phase -= 1.f;
			float newx = sin(b * yn) + c*sin(b * xn);
			double rate = (newx * unit->mxscale + 1) * 0.5;
			rate=Mirroring(0.0, 1.0, rate);
			speed = (minf + ((maxf-minf)*rate)) * unit->mfreqMul;

			ynm1=yclip;
			yn = sin(a * xn) + d*sin(a * yn);
			yclip=yn * 0.72074001609432;
			yclip=Mirroring(-1.0, 1.0, yclip);
			xn=newx;
		}

		ZXP(out) = (1-phase) * ynm1 + phase * yclip;
		phase += speed;
	);

	unit->mphase=phase;
	unit->mspeed=speed;
	unit->mxn=xn;
	unit->myn=yn;
	unit->mynm1=ynm1;
	unit->myclip=yclip;
}

void Latoocarfian2DC_Ctor(Latoocarfian2DC *unit)
{
	unit->mx0=ZIN0(6);
	unit->my0=ZIN0(7);
	unit->mphase=0;
	unit->mslope=0.f;
	unit->mxn=unit->mx0;
	unit->myn=unit->my0;
	unit->mxscale=0.70390617562334;
	unit->mlevel=unit->myn;
	SETCALC(Latoocarfian2DC_next);
	ZOUT0(0) = 0.f;
}

void Latoocarfian2DC_next(Latoocarfian2DC *unit, int inNumSamples)
{
	float *out=ZOUT(0);
	float minf=ZIN0(0);
	float maxf=ZIN0(1);
	double a=ZIN0(2);
	double b=ZIN0(3);
	double c=ZIN0(4);
	double d=ZIN0(5);
	double x0=ZIN0(6);
	double y0=ZIN0(7);
	double xn=unit->mxn;
	double yn=unit->myn;
	double level=unit->mlevel;
	double slope=unit->mslope;
	double curve=unit->mcurve;
	int32 phase=unit->mphase;

	if (x0 != unit->mx0) unit->mx0=xn=x0;
	if (y0 != unit->my0) unit->my0=yn=y0;

	int remain = inNumSamples;
	do {
		if (phase <= 0) {
			float newx = sin(b * yn) + c*sin(b * xn);
			double rate = (newx * unit->mxscale + 1) * 0.5;
			rate=Mirroring(0.0, 1.0, rate);
			float fphase = (minf + ((maxf-minf)*rate));
			phase = (int32)(unit->mRate->mSampleRate/sc_max(fphase, 0.001f));
			phase = sc_max(2, phase);

			double ynm1=unit->myclip;
			yn = sin(a * xn) + d*sin(a * yn);
			double yclip=yn * 0.72074001609432;
			yclip=Mirroring(-1.0, 1.0, yclip);
			double midpnt=(ynm1 + yclip) * 0.5;
			unit->myclip=yclip;
			curve=2.f * (midpnt - level - phase * slope) / (phase * phase + phase);
			xn=newx;
		}

		int nsmps = sc_min(phase, remain);
		phase -= nsmps;
		remain -= nsmps;
		LOOP(nsmps,
			ZXP(out)=level;
			slope += curve;
			level += slope;
		);
	} while (remain);

	unit->mphase=phase;
	unit->mslope=slope;
	unit->mcurve=curve;
	unit->mlevel=level;
	unit->mxn=xn;
	unit->myn=yn;
}

void LatoocarfianTrig_Ctor(LatoocarfianTrig *unit)
{
	unit->mx0=ZIN0(6);
	unit->my0=ZIN0(7);
	unit->mphase=1.f;
	unit->mspeed=100.f;
	unit->mxn=unit->mx0;
	unit->myn=unit->my0;
	unit->mxscale=0.70390617562334;
	unit->mfreqMul=unit->mRate->mSampleDur;
	SETCALC(LatoocarfianTrig_next);
	ZOUT0(0) = 0.f;
}

void LatoocarfianTrig_next(LatoocarfianTrig *unit, int inNumSamples)
{
	float *out=ZOUT(0);
	float minf=ZIN0(0);
	float maxf=ZIN0(1);
	double a=ZIN0(2);
	double b=ZIN0(3);
	double c=ZIN0(4);
	double d=ZIN0(5);
	double x0=ZIN0(6);
	double y0=ZIN0(7);
	double xn=unit->mxn;
	double yn=unit->myn;
	double phase=unit->mphase;
	double speed=unit->mspeed;

	if (x0 != unit->mx0) unit->mx0=xn=x0;
	if (y0 != unit->my0) unit->my0=yn=y0;
	LOOP(inNumSamples,
		if (phase >= 1.f) {
			phase -= 1.f;
			float newx = sin(b * yn) + c*sin(b * xn);
			double rate = (newx * unit->mxscale + 1) * 0.5;
			rate=Mirroring(0.0, 1.0, rate);
			speed = (minf + ((maxf-minf)*rate)) * unit->mfreqMul;

			yn = sin(a * xn) + d*sin(a * yn);
			float yclip=(yn * 0.72074001609432 + 1) * 0.5;
			yclip=Mirroring(0.0, 1.0, yclip);
			xn=newx;
			ZXP(out) = yclip;
		} else ZXP(out) = 0.f;
		phase += speed;
	);

	unit->mphase=phase;
	unit->mspeed=speed;
	unit->mxn=xn;
	unit->myn=yn;
}

void Lorenz2DN_Ctor(Lorenz2DN *unit)
{
	unit->mx0=unit->mxn=ZIN0(6);
	unit->my0=unit->myn=ZIN0(7);
	unit->mz0=unit->mzn=ZIN0(8);
	unit->mphase=1.f;
	unit->mspeed=100.f;
	unit->mxscale=0.048269545768799;
	unit->myscale=0.035757929840258;
	unit->mfreqMul=unit->mRate->mSampleDur;
	SETCALC(Lorenz2DN_next);
	ZOUT0(0) = 0.f;
}

void Lorenz2DN_next(Lorenz2DN *unit, int inNumSamples)
{
	float *out=ZOUT(0);
	float minf=ZIN0(0);
	float maxf=ZIN0(1);
	double s=ZIN0(2);
	double r=ZIN0(3);
	double b=ZIN0(4);
	double h=ZIN0(5);
	double x0=ZIN0(6);
	double y0=ZIN0(7);
	double z0=ZIN0(8);
	double xn=unit->mxn;
	double yn=unit->myn;
	double zn=unit->mzn;
	double yclip=unit->myclip;
	double yscale=unit->myscale;
	double phase=unit->mphase;
	double speed=unit->mspeed;

	if (x0 != unit->mx0) unit->mx0=xn=x0;
	if (y0 != unit->my0) unit->my0=yn=y0;
	if (z0 != unit->mz0) unit->mz0=zn=z0;
	LOOP(inNumSamples,
		if (phase >= 1.f) {
			phase -= 1.f;
			float newx = xn + h * s * (yn - xn);
			double rate = (newx * unit->mxscale + 1) * 0.5;
			rate=Mirroring(0.0, 1.0, rate);
			speed = (minf + ((maxf-minf)*rate)) * unit->mfreqMul;

			float newy = yn + h * (-xn * zn + r * xn - yn);
			zn = zn + h * (xn * yn - b * zn);

			yclip=newy * yscale;
			yclip=Mirroring(-1.0, 1.0, yclip);
			xn=newx;
			yn=newy;
		}

		ZXP(out) = yclip;
		phase += speed;
	);

	unit->mphase=phase;
	unit->mspeed=speed;
	unit->mxn=xn;
	unit->myn=yn;
	unit->mzn=zn;
	unit->myclip=yclip;
}

void Lorenz2DL_Ctor(Lorenz2DL *unit)
{
	unit->mx0=unit->mxn=ZIN0(6);
	unit->my0=unit->myn=ZIN0(7);
	unit->mz0=unit->mzn=ZIN0(8);
	unit->mphase=1.f;
	unit->mspeed=100.f;
	unit->mxscale=0.048269545768799;
	unit->myscale=0.035757929840258;
	unit->mfreqMul=unit->mRate->mSampleDur;
	SETCALC(Lorenz2DL_next);
	ZOUT0(0) = 0.f;
}

void Lorenz2DL_next(Lorenz2DL *unit, int inNumSamples)
{
	float *out=ZOUT(0);
	float minf=ZIN0(0);
	float maxf=ZIN0(1);
	double s=ZIN0(2);
	double r=ZIN0(3);
	double b=ZIN0(4);
	double h=ZIN0(5);
	double x0=ZIN0(6);
	double y0=ZIN0(7);
	double z0=ZIN0(8);
	double xn=unit->mxn;
	double yn=unit->myn;
	double zn=unit->mzn;
	double ynm1=unit->mynm1;
	double yclip=unit->myclip;
	double yscale=unit->myscale;
	double phase=unit->mphase;
	double speed=unit->mspeed;

	if (x0 != unit->mx0) unit->mx0=xn=x0;
	if (y0 != unit->my0) unit->my0=yn=y0;
	if (z0 != unit->mz0) unit->mz0=zn=z0;
	LOOP(inNumSamples,
		if (phase >= 1.f) {
			phase -= 1.f;
			float newx = xn + h * s * (yn - xn);
			double rate = (newx * unit->mxscale + 1) * 0.5;
			rate=Mirroring(0.0, 1.0, rate);
			speed = (minf + ((maxf-minf)*rate)) * unit->mfreqMul;

			float newy = yn + h * (-xn * zn + r * xn - yn);
			zn = zn + h * (xn * yn - b * zn);

			ynm1=yclip;
			yclip=newy * yscale;
			yclip=Mirroring(-1.0, 1.0, yclip);
			xn=newx;
			yn=newy;
		}

		ZXP(out) = (1-phase) * ynm1 + phase * yclip;
		phase += speed;
	);

	unit->mphase=phase;
	unit->mspeed=speed;
	unit->mxn=xn;
	unit->myn=yn;
	unit->mzn=zn;
	unit->myclip=yclip;
	unit->mynm1=ynm1;
}

void Lorenz2DC_Ctor(Lorenz2DC *unit)
{
	unit->mx0=unit->mxn=ZIN0(6);
	unit->my0=unit->myn=ZIN0(7);
	unit->mz0=unit->mzn=ZIN0(8);
	unit->mphase=0;
	unit->mslope=0.f;
	unit->mxscale=0.048269545768799;
	unit->myscale=0.035757929840258;
	unit->mlevel=unit->my0 * unit->myscale;
	SETCALC(Lorenz2DC_next);
	ZOUT0(0) = 0.f;
}

void Lorenz2DC_next(Lorenz2DC *unit, int inNumSamples)
{
	float *out=ZOUT(0);
	float minf=ZIN0(0);
	float maxf=ZIN0(1);
	double s=ZIN0(2);
	double r=ZIN0(3);
	double b=ZIN0(4);
	double h=ZIN0(5);
	double x0=ZIN0(6);
	double y0=ZIN0(7);
	double z0=ZIN0(8);
	double xn=unit->mxn;
	double yn=unit->myn;
	double zn=unit->mzn;
	double yscale=unit->myscale;
	int32 phase=unit->mphase;
	double slope=unit->mslope;
	double level=unit->mlevel;
	double curve=unit->mcurve;

	if (x0 != unit->mx0) unit->mx0=xn=x0;
	if (y0 != unit->my0) unit->my0=yn=y0;
	if (z0 != unit->mz0) unit->mz0=zn=z0;

	int remain=inNumSamples;
	do {
		if (phase <= 0) {
			float newx = xn + h * s * (yn - xn);
			double rate = (newx * unit->mxscale + 1) * 0.5;
			rate=Mirroring(0.0, 1.0, rate);
			float fphase = (minf + ((maxf-minf)*rate));
			phase = (int32)(unit->mRate->mSampleRate / sc_max(fphase, 0.001f));
			phase = sc_max(2, phase);

			float newy = yn + h * (-xn * zn + r * xn - yn);
			zn = zn + h * (xn * yn - b * zn);

			double ynm1=unit->myclip;
			double yclip=newy * yscale;
			yclip=Mirroring(-1.0, 1.0, yclip);
			double midpnt=(ynm1 + yclip) * 0.5;
			unit->myclip=yclip;
			curve=2.f * (midpnt - level - phase * slope) / (phase * phase + phase);
			xn=newx;
			yn=newy;
		}

		int nsmps=sc_min(phase, remain);
		phase -= nsmps;
		remain -= nsmps;
		LOOP(nsmps,
			ZXP(out)=level;
			slope += curve;
			level += slope;
		);
	} while (remain);

	unit->mphase=phase;
	unit->mslope=slope;
	unit->mcurve=curve;
	unit->mlevel=level;
	unit->mxn=xn;
	unit->myn=yn;
	unit->mzn=zn;
}

void LorenzTrig_Ctor(LorenzTrig *unit)
{
	unit->mx0=unit->mxn=ZIN0(6);
	unit->my0=unit->myn=ZIN0(7);
	unit->mz0=unit->mzn=ZIN0(8);
	unit->mphase=1.f;
	unit->mspeed=100.f;
	unit->mxscale=0.048269545768799;
	unit->myscale=0.035757929840258;
	unit->mfreqMul=unit->mRate->mSampleDur;
	SETCALC(LorenzTrig_next);
	ZOUT0(0) = 0.f;
}

void LorenzTrig_next(LorenzTrig *unit, int inNumSamples)
{
	float *out=ZOUT(0);
	float minf=ZIN0(0);
	float maxf=ZIN0(1);
	double s=ZIN0(2);
	double r=ZIN0(3);
	double b=ZIN0(4);
	double h=ZIN0(5);
	double x0=ZIN0(6);
	double y0=ZIN0(7);
	double z0=ZIN0(8);
	double xn=unit->mxn;
	double yn=unit->myn;
	double zn=unit->mzn;
	double yscale=unit->myscale;
	double phase=unit->mphase;
	double speed=unit->mspeed;

	if (x0 != unit->mx0) unit->mx0=xn=x0;
	if (y0 != unit->my0) unit->my0=yn=y0;
	if (z0 != unit->mz0) unit->mz0=zn=z0;
	LOOP(inNumSamples,
		if (phase >= 1.f) {
			phase -= 1.f;
			float newx = xn + h * s * (yn - xn);
			double rate = (newx * unit->mxscale + 1) * 0.5;
			rate=Mirroring(0.0, 1.0, rate);
			speed = (minf + ((maxf-minf)*rate)) * unit->mfreqMul;

			float newy = yn + h * (-xn * zn + r * xn - yn);
			zn = zn + h * (xn * yn - b * zn);

			float yclip=(newy * yscale + 1) * 0.5;
			yclip=Mirroring(0.0, 1.0, yclip);
			xn=newx;
			yn=newy;
			ZXP(out) = yclip;
		} else ZXP(out) = 0.f;
		phase += speed;
	);

	unit->mphase=phase;
	unit->mspeed=speed;
	unit->mxn=xn;
	unit->myn=yn;
	unit->mzn=zn;
}

void Standard2DN_Ctor(Standard2DN *unit)
{
	unit->mx0=ZIN0(3);
	unit->my0=ZIN0(4);
	unit->mphase=1.f;
	unit->mspeed=100.f;
	unit->mxn=unit->mx0;
	unit->myn=unit->my0;
	unit->mscale=0.1591549430919;
	unit->mfreqMul=unit->mRate->mSampleDur;
	SETCALC(Standard2DN_next);
	ZOUT0(0) = 0.f;
}

void Standard2DN_next(Standard2DN *unit, int inNumSamples)
{
	float *out=ZOUT(0);
	float minf=ZIN0(0);
	float maxf=ZIN0(1);
	double k=ZIN0(2);
	double x0=ZIN0(3);
	double y0=ZIN0(4);
	double xn=unit->mxn;
	double yn=unit->myn;
	double yclip=unit->myclip;
	double scale=unit->mscale;
	double phase=unit->mphase;
	double speed=unit->mspeed;

	if (x0 != unit->mx0) unit->mx0=xn=x0;
	if (y0 != unit->my0) unit->my0=yn=y0;
	LOOP(inNumSamples,
		if (phase >= 1.f) {
			phase -= 1.f;
			yn = (yn + k * sin(xn));
			yn = mod2pi(yn);
			xn = (xn + yn);
			xn = mod2pi(xn);
			double rate = xn * scale;
			rate=Mirroring(0.0, 1.0, rate);
			speed = (minf + ((maxf-minf)*rate)) * unit->mfreqMul;

			yclip=yn * scale * 2.0 - 1.0;
			yclip=Mirroring(-1.0, 1.0, yclip);
		}

		ZXP(out) = yclip;
		phase += speed;
	);

	unit->mphase=phase;
	unit->mspeed=speed;
	unit->mxn=xn;
	unit->myn=yn;
	unit->myclip=yclip;
}

void Standard2DL_Ctor(Standard2DL *unit)
{
	unit->mx0=ZIN0(3);
	unit->my0=ZIN0(4);
	unit->mphase=1.f;
	unit->mspeed=100.f;
	unit->mxn=unit->mx0;
	unit->myn=unit->my0;
	unit->mscale=0.1591549430919;
	unit->mfreqMul=unit->mRate->mSampleDur;
	SETCALC(Standard2DL_next);
	ZOUT0(0) = 0.f;
}

void Standard2DL_next(Standard2DL *unit, int inNumSamples)
{
	float *out=ZOUT(0);
	float minf=ZIN0(0);
	float maxf=ZIN0(1);
	double k=ZIN0(2);
	double x0=ZIN0(3);
	double y0=ZIN0(4);
	double xn=unit->mxn;
	double yn=unit->myn;
	double ynm1=unit->mynm1;
	double yclip=unit->myclip;
	double scale=unit->mscale;
	double phase=unit->mphase;
	double speed=unit->mspeed;

	if (x0 != unit->mx0) unit->mx0=xn=x0;
	if (y0 != unit->my0) unit->my0=yn=y0;
	LOOP(inNumSamples,
		if (phase >= 1.f) {
			phase -= 1.f;
			yn = (yn + k * sin(xn));
			yn = mod2pi(yn);
			xn = (xn + yn);
			xn = mod2pi(xn);
			double rate = xn * scale;
			rate=Mirroring(0.0, 1.0, rate);
			speed = (minf + ((maxf-minf)*rate)) * unit->mfreqMul;

			ynm1=yclip;
			yclip=yn * scale * 2.0 - 1.0;
			yclip=Mirroring(-1.0, 1.0, yclip);
		}

		ZXP(out) = (1-phase) * ynm1 + phase * yclip;
		phase += speed;
	);

	unit->mphase=phase;
	unit->mspeed=speed;
	unit->mxn=xn;
	unit->myn=yn;
	unit->mynm1=ynm1;
	unit->myclip=yclip;
}

void Standard2DC_Ctor(Standard2DC *unit)
{
	unit->mx0=ZIN0(3);
	unit->my0=ZIN0(4);
	unit->mphase=0;
	unit->mslope=0.f;
	unit->mxn=unit->mx0;
	unit->myn=unit->my0;
	unit->mscale=0.1591549430919;
	unit->mlevel=unit->my0 * unit->mscale;
	SETCALC(Standard2DC_next);
	ZOUT0(0) = 0.f;
}

void Standard2DC_next(Standard2DC *unit, int inNumSamples)
{
	float *out=ZOUT(0);
	float minf=ZIN0(0);
	float maxf=ZIN0(1);
	double k=ZIN0(2);
	double x0=ZIN0(3);
	double y0=ZIN0(4);
	double xn=unit->mxn;
	double yn=unit->myn;
	double scale=unit->mscale;
	int32 phase=unit->mphase;
	double level=unit->mlevel;
	double slope=unit->mslope;
	double curve=unit->mcurve;

	if (x0 != unit->mx0) unit->mx0=xn=x0;
	if (y0 != unit->my0) unit->my0=yn=y0;

	int remain=inNumSamples;
	do {
		if (phase <= 0) {
			yn = (yn + k * sin(xn));
			yn = mod2pi(yn);
			xn = (xn + yn);
			xn = mod2pi(xn);
			double rate = xn * scale;
			rate=Mirroring(0.0, 1.0, rate);
			float fphase = (minf + ((maxf-minf)*rate));
			phase = (int32)(unit->mRate->mSampleRate/sc_max(fphase, 0.001f));
			phase = sc_max(2, phase);

			double ynm1=unit->myclip;
			double yclip=yn * scale * 2.0 - 1.0;
			yclip=Mirroring(-1.0, 1.0, yclip);
			unit->myclip=yclip;
			double midpnt = (ynm1 + yclip) * 0.5;
			curve=2.f * (midpnt - level - phase * slope) / (phase * phase + phase);
		}

		int nsmps = sc_min(phase, remain);
		phase -= nsmps;
		remain -= nsmps;
		LOOP(nsmps,
			ZXP(out)=level;
			slope += curve;
			level += slope;
		);
	} while (remain);

	unit->mphase=phase;
	unit->mslope=slope;
	unit->mcurve=curve;
	unit->mlevel=level;
	unit->mxn=xn;
	unit->myn=yn;
}

void StandardTrig_Ctor(StandardTrig *unit)
{
	unit->mx0=ZIN0(3);
	unit->my0=ZIN0(4);
	unit->mphase=1.f;
	unit->mspeed=100.f;
	unit->mxn=unit->mx0;
	unit->myn=unit->my0;
	unit->mscale=0.1591549430919;
	unit->mfreqMul=unit->mRate->mSampleDur;
	SETCALC(StandardTrig_next);
	ZOUT0(0) = 0.f;
}

void StandardTrig_next(StandardTrig *unit, int inNumSamples)
{
	float *out=ZOUT(0);
	float minf=ZIN0(0);
	float maxf=ZIN0(1);
	double k=ZIN0(2);
	double x0=ZIN0(3);
	double y0=ZIN0(4);
	double xn=unit->mxn;
	double yn=unit->myn;
	double scale=unit->mscale;
	double phase=unit->mphase;
	double speed=unit->mspeed;

	if (x0 != unit->mx0) unit->mx0=xn=x0;
	if (y0 != unit->my0) unit->my0=yn=y0;
	LOOP(inNumSamples,
		if (phase >= 1.f) {
			phase -= 1.f;
			yn = (yn + k * sin(xn));
			yn = mod2pi(yn);
			xn = (xn + yn);
			xn = mod2pi(xn);
			double rate = xn * scale;
			rate=Mirroring(0.0, 1.0, rate);
			speed = (minf + ((maxf-minf)*rate)) * unit->mfreqMul;

			float yclip=yn * scale;
			yclip=Mirroring(0.0, 1.0, yclip);
			ZXP(out) = yclip;
		} else ZXP(out) = 0.f;

		phase += speed;
	);

	unit->mphase=phase;
	unit->mspeed=speed;
	unit->mxn=xn;
	unit->myn=yn;
}

void Gbman2DN_Ctor(Gbman2DN *unit)
{
	unit->mx0=ZIN0(2);
	unit->my0=ZIN0(3);
	unit->mphase=1.f;
	unit->mspeed=100.f;
	unit->mxn=unit->mx0;
	unit->myn=unit->my0;
	unit->mscale=0.125;
	unit->mfreqMul=unit->mRate->mSampleDur;
	SETCALC(Gbman2DN_next);
	ZOUT0(0) = 0.f;
}

void Gbman2DN_next(Gbman2DN *unit, int inNumSamples)
{
	float *out=ZOUT(0);
	float minf=ZIN0(0);
	float maxf=ZIN0(1);
	double x0=ZIN0(2);
	double y0=ZIN0(3);
	double xn=unit->mxn;
	double yn=unit->myn;
	double yclip=unit->myclip;
	double scale=unit->mscale;
	double phase=unit->mphase;
	double speed=unit->mspeed;

	if (x0 != unit->mx0) unit->mx0=xn=x0;
	if (y0 != unit->my0) unit->my0=yn=y0;
	LOOP(inNumSamples,
		if (phase >= 1.f) {
			phase -= 1.f;
			float last_x=xn;
			if (last_x < 0.f) xn = 1 - yn - last_x;
			else xn = 1 - yn + last_x;
			double rate = (xn * scale + 1) * 0.5;
			rate=Mirroring(0.0, 1.0, rate);
			speed = (minf + ((maxf-minf)*rate)) * unit->mfreqMul;

			yn=last_x;
			yclip=yn * scale;
			yclip=Mirroring(-1.0, 1.0, yclip);
		}

		ZXP(out) = yclip;
		phase += speed;
	);

	unit->mphase=phase;
	unit->mspeed=speed;
	unit->mxn=xn;
	unit->myn=yn;
	unit->myclip=yclip;
}

void Gbman2DL_Ctor(Gbman2DL *unit)
{
	unit->mx0=ZIN0(2);
	unit->my0=ZIN0(3);
	unit->mphase=1.f;
	unit->mspeed=100.f;
	unit->mxn=unit->mx0;
	unit->myn=unit->my0;
	unit->mscale=0.125;
	unit->mfreqMul=unit->mRate->mSampleDur;
	SETCALC(Gbman2DL_next);
	ZOUT0(0) = 0.f;
}

void Gbman2DL_next(Gbman2DL *unit, int inNumSamples)
{
	float *out=ZOUT(0);
	float minf=ZIN0(0);
	float maxf=ZIN0(1);
	double x0=ZIN0(2);
	double y0=ZIN0(3);
	double xn=unit->mxn;
	double yn=unit->myn;
	double yclip=unit->myclip;
	double ynm1=unit->mynm1;
	double scale=unit->mscale;
	double phase=unit->mphase;
	double speed=unit->mspeed;

	if (x0 != unit->mx0) unit->mx0=xn=x0;
	if (y0 != unit->my0) unit->my0=yn=y0;
	LOOP(inNumSamples,
		if (phase >= 1.f) {
			phase -= 1.f;
			float last_x=xn;
			if (last_x < 0.f) xn = 1 - yn - last_x;
			else xn = 1 - yn + last_x;
			double rate = (xn * scale + 1) * 0.5;
			rate=Mirroring(0.0, 1.0, rate);
			speed = (minf + ((maxf-minf)*rate)) * unit->mfreqMul;

			ynm1=yclip;
			yn=last_x;
			yclip=yn * scale;
			yclip=Mirroring(-1.0, 1.0, yclip);
		}

		ZXP(out) = (1-phase) * ynm1 + phase * yclip;
		phase += speed;
	);

	unit->mphase=phase;
	unit->mspeed=speed;
	unit->mxn=xn;
	unit->myn=yn;
	unit->mynm1=ynm1;
	unit->myclip=yclip;
}

void Gbman2DC_Ctor(Gbman2DC *unit)
{
	unit->mx0=ZIN0(2);
	unit->my0=ZIN0(3);
	unit->mphase=0;
	unit->mslope=0.f;
	unit->mxn=unit->mx0;
	unit->myn=unit->my0;
	unit->mscale=0.125;
	unit->mlevel=unit->my0 * unit->mscale;
	SETCALC(Gbman2DC_next);
	ZOUT0(0) = 0.f;
}

void Gbman2DC_next(Gbman2DC *unit, int inNumSamples)
{
	float *out=ZOUT(0);
	float minf=ZIN0(0);
	float maxf=ZIN0(1);
	double x0=ZIN0(2);
	double y0=ZIN0(3);
	double xn=unit->mxn;
	double yn=unit->myn;
	double scale=unit->mscale;
	int32 phase=unit->mphase;
	double slope=unit->mslope;
	double curve=unit->mcurve;
	double level=unit->mlevel;

	if (x0 != unit->mx0) unit->mx0=xn=x0;
	if (y0 != unit->my0) unit->my0=yn=y0;

	int remain=inNumSamples;
	do {
		if (phase <= 0) {
			float last_x=xn;
			if (last_x < 0.f) xn = 1 - yn - last_x;
			else xn = 1 - yn + last_x;
			double rate = (xn * scale + 1) * 0.5;
			rate=Mirroring(0.0, 1.0, rate);
			float fphase = (minf + ((maxf-minf)*rate));
			phase = (int32)(unit->mRate->mSampleRate/sc_max(fphase, 0.001f));
			phase = sc_max(2, phase);

			double ynm1=unit->myclip;
			yn=last_x;
			double yclip=yn * scale;
			yclip=Mirroring(-1.0, 1.0, yclip);
			unit->myclip=yclip;
			double midpnt = (ynm1 + yclip) * 0.5;
			curve = 2.f * (midpnt - level - phase * slope) / (phase * phase + phase);
		}

		int nsmps = sc_min(phase, remain);
		phase -= nsmps;
		remain -= nsmps;
		LOOP(nsmps,
			ZXP(out)=level;
			slope += curve;
			level += slope;
		);
	} while (remain);

	unit->mphase=phase;
	unit->mslope=slope;
	unit->mcurve=curve;
	unit->mlevel=level;
	unit->mxn=xn;
	unit->myn=yn;
}

void GbmanTrig_Ctor(GbmanTrig *unit)
{
	unit->mx0=ZIN0(2);
	unit->my0=ZIN0(3);
	unit->mphase=1.f;
	unit->mspeed=100.f;
	unit->mxn=unit->mx0;
	unit->myn=unit->my0;
	unit->mscale=0.125;
	unit->mfreqMul=unit->mRate->mSampleDur;
	SETCALC(GbmanTrig_next);
	ZOUT0(0) = 0.f;
}

void GbmanTrig_next(GbmanTrig *unit, int inNumSamples)
{
	float *out=ZOUT(0);
	float minf=ZIN0(0);
	float maxf=ZIN0(1);
	double x0=ZIN0(2);
	double y0=ZIN0(3);
	double xn=unit->mxn;
	double yn=unit->myn;
	double scale=unit->mscale;
	double phase=unit->mphase;
	double speed=unit->mspeed;

	if (x0 != unit->mx0) unit->mx0=xn=x0;
	if (y0 != unit->my0) unit->my0=yn=y0;
	LOOP(inNumSamples,
		if (phase >= 1.f) {
			phase -= 1.f;
			float last_x=xn;
			if (last_x < 0.f) xn = 1 - yn - last_x;
			else xn = 1 - yn + last_x;
			double rate = (xn * scale + 1) * 0.5;
			rate=Mirroring(0.0, 1.0, rate);
			speed = (minf + ((maxf-minf)*rate)) * unit->mfreqMul;

			yn=last_x;
			float yclip=(yn * scale + 1) * 0.5;
			yclip=Mirroring(0.0, 1.0, yclip);
			ZXP(out) = yclip;
		} else ZXP(out) = 0.f;

		phase += speed;
	);

	unit->mphase=phase;
	unit->mspeed=speed;
	unit->mxn=xn;
	unit->myn=yn;
}

void Fhn2DN_Ctor(Fhn2DN* unit) {

	unit->mun=unit->mu0=ZIN0(7);
	unit->mwn=unit->mw0=ZIN0(8);
	unit->mphase=1.f;
	unit->mspeed=100.f;
	unit->mfreqMul=unit->mRate->mSampleDur;

	SETCALC(Fhn2DN_next);
	ZOUT0(0) = 0.f;
}


void Fhn2DN_next(Fhn2DN *unit, int inNumSamples) {

	float *out = ZOUT(0);

	double phase=unit->mphase;
	double speed=unit->mspeed;

	double u=unit->mun;
	double w=unit->mwn;
	double uclip=unit->muclip;
	double u0=ZIN0(7);
	double w0=ZIN0(8);

	if (u0 != unit->mu0) u=u0;
	if (w0 != unit->mw0) w=w0;

	LOOP(inNumSamples,
		if (phase >= 1.f) {
			phase -= 1.f;

			double minf=ZIN0(0);
			double maxf=ZIN0(1);
			double urate=ZIN0(2);
			double wrate=ZIN0(3);
			double b0=ZIN0(4);
			double b1=ZIN0(5);
			double i=ZIN0(6);
			double dudt=urate*(u-(0.33333*u*u*u)-w+i);
			double dwdt=wrate*(b0+b1*u-w);

			u+=dudt;
			w+=dwdt;

			double rate=(w * 0.5 + 1) * 0.5;
			rate=sc_max(rate, 0.0);
			speed=(minf + ((maxf-minf)*rate)) * unit->mfreqMul;

			if ((u>1.0) || (u<-1.0)) u=fabs(fmod((u-1.0),4.0)-2.0)-1.0;
			uclip=u * 0.3;
			unit->muclip=uclip;
		}

		ZXP(out) = uclip;
		phase += speed;
	);

	unit->mun=u;
	unit->mwn=w;
	unit->mphase=phase;
	unit->mspeed=speed;
}

void Fhn2DL_Ctor(Fhn2DL* unit) {

	unit->mun=unit->mu0=ZIN0(7);
	unit->mwn=unit->mw0=ZIN0(8);
	unit->mphase=1.f;
	unit->mspeed=100.f;
	unit->mfreqMul=unit->mRate->mSampleDur;

	SETCALC(Fhn2DL_next);
	ZOUT0(0) = 0.f;
}


void Fhn2DL_next(Fhn2DL *unit, int inNumSamples) {

	float *out = ZOUT(0);

	double phase=unit->mphase;
	double speed=unit->mspeed;

	double u=unit->mun;
	double uclip=unit->muclip;
	double um1=unit->um1;
	double w=unit->mwn;
	double u0=ZIN0(7);
	double w0=ZIN0(8);

	if (u0 != unit->mu0) u=u0;
	if (w0 != unit->mw0) w=w0;

	LOOP(inNumSamples,
		if (phase >= 1.f) {
			phase -= 1.f;

			double minf=ZIN0(0);
			double maxf=ZIN0(1);
			double urate=ZIN0(2);
			double wrate=ZIN0(3);
			double b0=ZIN0(4);
			double b1=ZIN0(5);
			double i=ZIN0(6);
			double dudt=urate*(u-(0.33333*u*u*u)-w+i);
			double dwdt=wrate*(b0+b1*u-w);

			um1=uclip;
			u+=dudt;
			w+=dwdt;

			double rate=(w * 0.5 + 1) * 0.5;
			rate=sc_max(rate, 0.0);
			speed=(minf + ((maxf-minf)*rate)) * unit->mfreqMul;

			if ((u>1.0) || (u<-1.0)) u=fabs(fmod((u-1.0),4.0)-2.0)-1.0;
			uclip=u * 0.3;
			unit->muclip=uclip;
		}

		ZXP(out) = (1-phase) * um1 + phase * uclip;
		phase += speed;
	);

	unit->mun=u;
	unit->um1=um1;
	unit->mwn=w;
	unit->mphase=phase;
	unit->mspeed=speed;
}

void Fhn2DC_Ctor(Fhn2DC* unit) {

	unit->mun=unit->mu0=ZIN0(7);
	unit->mwn=unit->mw0=ZIN0(8);
	unit->mphase=0;
	unit->mslope=0.f;
	unit->mlevel=unit->mw0;

	SETCALC(Fhn2DC_next);
	ZOUT0(0) = 0.f;
}


void Fhn2DC_next(Fhn2DC *unit, int inNumSamples) {

	float *out = ZOUT(0);

	int32 phase=unit->mphase;

	double u=unit->mun;
	double um1=unit->um1;
	double w=unit->mwn;
	double u0=ZIN0(7);
	double w0=ZIN0(8);
	double level=unit->mlevel;
	double slope=unit->mslope;
	double curve=unit->mcurve;

	if (u0 != unit->mu0) u=u0;
	if (w0 != unit->mw0) w=w0;
	int remain=inNumSamples;
	do {
		if (phase <= 0) {
			double minf=ZIN0(0);
			double maxf=ZIN0(1);
			double urate=ZIN0(2);
			double wrate=ZIN0(3);
			double b0=ZIN0(4);
			double b1=ZIN0(5);
			double i=ZIN0(6);
			double dudt=urate*(u-(0.33333*u*u*u)-w+i);
			double dwdt=wrate*(b0+b1*u-w);

			um1=unit->muclip;
			u+=dudt;
			w+=dwdt;

			double rate=(w * 0.5 + 1) * 0.5;
			rate=sc_max(rate, 0.);
			float fphase=(minf + ((maxf-minf)*rate));
			phase = (int32)(unit->mRate->mSampleRate/sc_max(fphase, 0.001f));
			phase = sc_max(2, phase);

			if ((u>1.0) || (u<-1.0)) u=fabs(fmod((u-1.0),4.0)-2.0)-1.0;
			unit->muclip=u * 0.3;
			double midpnt=(um1 + unit->muclip) * 0.5;
			curve=2.f * (midpnt - level - phase * slope) / (phase * phase + phase);
		}

		int nsmps=sc_min(phase, remain);
		phase -= nsmps;
		remain -= nsmps;
		LOOP(nsmps,
			ZXP(out)=level;
			slope += curve;
			level += slope;
		);
	} while (remain);

	unit->mun=u;
	unit->mwn=w;
	unit->mphase=phase;
	unit->mslope=slope;
	unit->mcurve=curve;
	unit->mlevel=level;
}

void FhnTrig_Ctor(FhnTrig* unit) {

	unit->mun=unit->mu0=ZIN0(7);
	unit->mwn=unit->mw0=ZIN0(8);
	unit->mphase=1.f;
	unit->mspeed=100.f;
	unit->mfreqMul=unit->mRate->mSampleDur;

	SETCALC(FhnTrig_next);
	ZOUT0(0) = 0.f;
}


void FhnTrig_next(FhnTrig *unit, int inNumSamples) {

	float *out = ZOUT(0);

	double phase=unit->mphase;
	double speed=unit->mspeed;

	double u=unit->mun;
	double w=unit->mwn;
	double u0=ZIN0(7);
	double w0=ZIN0(8);

	if (u0 != unit->mu0) u=u0;
	if (w0 != unit->mw0) w=w0;

	LOOP(inNumSamples,
		if (phase >= 1.f) {
			phase -= 1.f;

			double minf=ZIN0(0);
			double maxf=ZIN0(1);
			double urate=ZIN0(2);
			double wrate=ZIN0(3);
			double b0=ZIN0(4);
			double b1=ZIN0(5);
			double i=ZIN0(6);
			double dudt=urate*(u-(0.33333*u*u*u)-w+i);
			double dwdt=wrate*(b0+b1*u-w);

			u+=dudt;
			w+=dwdt;

			double rate=(w * 0.5 + 1) * 0.5;
			rate=sc_max(rate, 0.0);
			speed=(minf + ((maxf-minf)*rate)) * unit->mfreqMul;

			if ((u>1.0) || (u<-1.0)) u=fabs(fmod((u-1.0),4.0)-2.0)-1.0;
			ZXP(out) = (u * 0.3 + 1) * 0.5;
		} else {
			ZXP(out) = 0.f;
		}
		phase += speed;
	);

	unit->mun=u;
	unit->mwn=w;
	unit->mphase=phase;
	unit->mspeed=speed;
}

PluginLoad(BhobChaos)
{
	ft = inTable;

	DefineSimpleUnit(Henon2DN);
	DefineSimpleUnit(Henon2DL);
	DefineSimpleUnit(Henon2DC);
	DefineSimpleUnit(HenonTrig);
	DefineSimpleUnit(Latoocarfian2DN);
	DefineSimpleUnit(Latoocarfian2DL);
	DefineSimpleUnit(Latoocarfian2DC);
	DefineSimpleUnit(LatoocarfianTrig);
	DefineSimpleUnit(Lorenz2DN);
	DefineSimpleUnit(Lorenz2DL);
	DefineSimpleUnit(Lorenz2DC);
	DefineSimpleUnit(LorenzTrig);
	DefineSimpleUnit(Standard2DN);
	DefineSimpleUnit(Standard2DL);
	DefineSimpleUnit(Standard2DC);
	DefineSimpleUnit(StandardTrig);
	DefineSimpleUnit(Gbman2DN);
	DefineSimpleUnit(Gbman2DL);
	DefineSimpleUnit(Gbman2DC);
	DefineSimpleUnit(GbmanTrig);
	DefineSimpleUnit(Fhn2DN);
	DefineSimpleUnit(Fhn2DL);
	DefineSimpleUnit(Fhn2DC);
	DefineSimpleUnit(FhnTrig);
}
