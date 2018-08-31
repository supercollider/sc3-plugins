/*
 *  AmbisonicUGens.cpp
 *  xSC3plugins
 *
 *  Created by Josh Parmenter on 2/4/05.
 *  Copyright 2005 __MyCompanyName__. All rights reserved.
 *
 */
/*
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

const double sqrt3div6 = sqrt(3.) * 0.1666666667;
const double sqrt3div2 = sqrt(3.) * 0.5;
const double rsqrt6 = 1. / sqrt(6.);
const double sqrt6div3 = sqrt(6.) * 0.3333333333;

static InterfaceTable *ft;


struct Dominate : public Unit
{
	float m_dominance;
};

struct Push : public Unit
{
	float m_push;
};

struct DistanceB : public Unit
{
	float m_distanceStart, m_yx1, m_yy1, m_yz1;
};

struct ProximityB : public Unit
{
	float m_distanceStart, m_yx1, m_yy1, m_yz1;
};

struct A2B: public Unit
{
};

extern "C"
{
	void Dominate_next(Dominate *unit, int inNumSamples);
	void Dominate_Ctor(Dominate* unit);

	void Push_next(Push *unit, int inNumSamples);
	void Push_Ctor(Push* unit);

	void DistanceB_next_k(DistanceB *unit, int inNumSamples);
	void DistanceB_next_a(DistanceB *unit, int inNumSamples);
	void DistanceB_Ctor(DistanceB* unit);

	void ProximityB_next_k(ProximityB *unit, int inNumSamples);
	void ProximityB_next_a(ProximityB *unit, int inNumSamples);
	void ProximityB_Ctor(ProximityB* unit);

	void A2B_Ctor(A2B* unit);
	void A2B_next_00(A2B *unit, int inNumSamples);
//	void A2B_next_01(A2B *unit, int inNumSamples);
//	void A2B_next_02(A2B *unit, int inNumSamples);
	void A2B_next_10(A2B *unit, int inNumSamples);
//	void A2B_next_11(A2B *unit, int inNumSamples);
//	void A2B_next_12(A2B *unit, int inNumSamples);
	void A2B_next_20(A2B *unit, int inNumSamples);
//	void A2B_next_21(A2B *unit, int inNumSamples);
//	void A2B_next_22(A2B *unit, int inNumSamples);
	void A2B_next_30(A2B *unit, int inNumSamples);
//	void A2B_next_31(A2B *unit, int inNumSamples);
//	void A2B_next_32(A2B *unit, int inNumSamples);
	void A2B_next_40(A2B *unit, int inNumSamples);
//	void A2B_next_41(A2B *unit, int inNumSamples);
//	void A2B_next_42(A2B *unit, int inNumSamples);
	void A2B_next_50(A2B *unit, int inNumSamples);
//	void A2B_next_51(A2B *unit, int inNumSamples);
//	void A2B_next_52(A2B *unit, int inNumSamples);
	void A2B_next_60(A2B *unit, int inNumSamples);
//	void A2B_next_61(A2B *unit, int inNumSamples);
//	void A2B_next_62(A2B *unit, int inNumSamples);
	void A2B_next_70(A2B *unit, int inNumSamples);
//	void A2B_next_71(A2B *unit, int inNumSamples);
//	void A2B_next_72(A2B *unit, int inNumSamples);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// DistanceB - 4 in 4 out- a or k rate distance vals

void DistanceB_Ctor(DistanceB* unit)
{
    unit->m_yx1 = 0.0f;
    unit->m_yy1 = 0.0f;
    unit->m_yz1 = 0.0f;
    if (INRATE(4) == calc_FullRate) {
	SETCALC(DistanceB_next_a);
	DistanceB_next_a(unit, 1);
	   } else {
	SETCALC(DistanceB_next_k);
	unit->m_distanceStart = IN0(4);
	DistanceB_next_k(unit, 1);
	};
}

void DistanceB_next_k(DistanceB *unit, int inNumSamples)
{
	float *Wout = ZOUT(0);
	float *Xout = ZOUT(1);
	float *Yout = ZOUT(2);
	float *Zout = ZOUT(3);

	float *Win = ZIN(0);
	float *Xin = ZIN(1);
	float *Yin = ZIN(2);
	float *Zin = ZIN(3);
	float distanceEnd = ZIN0(4);
	float distanceStart = unit->m_distanceStart;

	float distanceInc = CALCSLOPE(distanceEnd, distanceStart);

	float yx1 = unit->m_yx1;
	float yy1 = unit->m_yy1;
	float yz1 = unit->m_yz1;

	LOOP(inNumSamples,
	    float freq = 53.0 / distanceStart;
	    float wc = (twopi * freq) * SAMPLEDUR;
	    //	a0 = (1 + (wc.cos.neg * 2 + 2).sqrt).reciprocal;
	    float a0 = 1 / (sqrt((cos(wc) * -2) + 2) + 1);
	    float yx0 = ZXP(Xin) + a0 * yx1;
	    ZXP(Xout) = a0 * yx0 + (a0 * -1) * yx1;
	    yx1 = yx0;
	    float yy0 = ZXP(Yin) + a0 * yy1;
	    ZXP(Yout) = a0 * yy0 + (a0 * -1) * yy1;
	    yy1 = yy0;
	    float yz0 = ZXP(Zin) + a0 * yz1;
	    ZXP(Zout) = a0 * yz0 + (a0 * -1) * yz1;
	    yz1 = yz0;
	    // W is passed straight out
	    ZXP(Wout) = ZXP(Win);
	    distanceStart += distanceInc;
	)

	unit->m_yx1 = zapgremlins(yx1);
	unit->m_yy1 = zapgremlins(yy1);
	unit->m_yz1 = zapgremlins(yz1);
	unit->m_distanceStart = distanceEnd;

}

void DistanceB_next_a(DistanceB *unit, int inNumSamples)
{
	float *Wout = ZOUT(0);
	float *Xout = ZOUT(1);
	float *Yout = ZOUT(2);
	float *Zout = ZOUT(3);

	float *Win = ZIN(0);
	float *Xin = ZIN(1);
	float *Yin = ZIN(2);
	float *Zin = ZIN(3);
	float *distance = ZIN(4);

	float yx1 = unit->m_yx1;
	float yy1 = unit->m_yy1;
	float yz1 = unit->m_yz1;

	LOOP(inNumSamples,
	    float freq = 53.0 / ZXP(distance);
	    float wc = (twopi * freq) * SAMPLEDUR;
	    float a0 = 1 / (sqrt((cos(wc) * -2) + 2) + 1);
	    float yx0 = ZXP(Xin) + a0 * yx1;
	    ZXP(Xout) = a0 * yx0 + (a0 * -1) * yx1;
	    yx1 = yx0;
	    float yy0 = ZXP(Yin) + a0 * yy1;
	    ZXP(Yout) = a0 * yy0 + (a0 * -1) * yy1;
	    yy1 = yy0;
	    float yz0 = ZXP(Zin) + a0 * yz1;
	    ZXP(Zout) = a0 * yz0 + (a0 * -1) * yz1;
	    yz1 = yz0;
	    // W is passed straight out
	    ZXP(Wout) = ZXP(Win);
	)

	unit->m_yx1 = zapgremlins(yx1);
	unit->m_yy1 = zapgremlins(yy1);
	unit->m_yz1 = zapgremlins(yz1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// ProximityB - 4 in 4 out- a or k rate distance vals

void ProximityB_Ctor(ProximityB* unit)
{
    unit->m_yx1 = 0.0f;
    unit->m_yy1 = 0.0f;
    unit->m_yz1 = 0.0f;
    if (INRATE(4) == calc_FullRate) {
	SETCALC(ProximityB_next_a);
	ProximityB_next_a(unit, 1);
	   } else {
	SETCALC(ProximityB_next_k);
	unit->m_distanceStart = IN0(4);
	ProximityB_next_k(unit, 1);
	};
}

void ProximityB_next_k(ProximityB *unit, int inNumSamples)
{
	float *Wout = ZOUT(0);
	float *Xout = ZOUT(1);
	float *Yout = ZOUT(2);
	float *Zout = ZOUT(3);

	float *Win = ZIN(0);
	float *Xin = ZIN(1);
	float *Yin = ZIN(2);
	float *Zin = ZIN(3);
	float distanceEnd = ZIN0(4);
	float distanceStart = unit->m_distanceStart;

	float distanceInc = CALCSLOPE(distanceEnd, distanceStart);

	float yx1 = unit->m_yx1;
	float yy1 = unit->m_yy1;
	float yz1 = unit->m_yz1;

	LOOP(inNumSamples,
	    float freq = 53.0 / distanceStart;
	    float wc = (twopi * freq) * SAMPLEDUR;
	    //	a0 = 1 + (wc.cos.neg * 2 + 2).sqrt;
	    float a0 = 1 + sqrt((cos(wc) * -2) + 2);
	    float yx0 = ZXP(Xin) + 1.0 * yx1;
	    ZXP(Xout) = a0 * yx0 + -1.0 * yx1;
	    yx1 = yx0;
	    float yy0 = ZXP(Yin) + 1.0 * yy1;
	    ZXP(Yout) = a0 * yy0 + -1.0 * yy1;
	    yy1 = yy0;
	    float yz0 = ZXP(Zin) + 1.0 * yz1;
	    ZXP(Zout) = a0 * yz0 + -1.0 * yz1;
	    yz1 = yz0;
	    // W is passed straight out
	    ZXP(Wout) = ZXP(Win);
	    distanceStart += distanceInc;
	)

	unit->m_yx1 = zapgremlins(yx1);
	unit->m_yy1 = zapgremlins(yy1);
	unit->m_yz1 = zapgremlins(yz1);
	unit->m_distanceStart = distanceEnd;

}

void ProximityB_next_a(ProximityB *unit, int inNumSamples)
{
	float *Wout = ZOUT(0);
	float *Xout = ZOUT(1);
	float *Yout = ZOUT(2);
	float *Zout = ZOUT(3);

	float *Win = ZIN(0);
	float *Xin = ZIN(1);
	float *Yin = ZIN(2);
	float *Zin = ZIN(3);
	float *distance = ZIN(4);

	float yx1 = unit->m_yx1;
	float yy1 = unit->m_yy1;
	float yz1 = unit->m_yz1;

	LOOP(inNumSamples,
	    float freq = 53.0 / ZXP(distance);
	    float wc = (twopi * freq) * SAMPLEDUR;
	    float a0 = 1 + sqrt((cos(wc) * -2) + 2);
	    float yx0 = ZXP(Xin) + 1.0 * yx1;
	    ZXP(Xout) = a0 * yx0 + -1.0 * yx1;
	    yx1 = yx0;
	    float yy0 = ZXP(Yin) + 1.0 * yy1;
	    ZXP(Yout) = a0 * yy0 + -1.0 * yy1;
	    yy1 = yy0;
	    float yz0 = ZXP(Zin) + 1.0 * yz1;
	    ZXP(Zout) = a0 * yz0 + -1.0 * yz1;
	    yz1 = yz0;
	    // W is passed straight out
	    ZXP(Wout) = ZXP(Win);
	)

	unit->m_yx1 = zapgremlins(yx1);
	unit->m_yy1 = zapgremlins(yy1);
	unit->m_yz1 = zapgremlins(yz1);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
// A2B - a, b, c, d in, w, x, y, z out

void A2B_Ctor(A2B* unit)
{
	float orientation = ZIN0(4);
	float mode = ZIN0(5);
	// set-up the proper encoding function
	switch ((int)orientation) {
	    case 0 :
		switch ((int)mode) {
		    case 0: SETCALC(A2B_next_00); break;
//		    case 1: SETCALC(A2B_next_01); break;
//		    case 2: SETCALC(A2B_next_02); break;
		    default : break;
		    }
		break;
	    case 1 :
		switch ((int)mode) {
		    case 0: SETCALC(A2B_next_10); break;
//		    case 1: SETCALC(A2B_next_11); break;
//		    case 2: SETCALC(A2B_next_12); break;
		    default : break;
		    }
		break;
	    case 2 :
		switch ((int)mode) {
		    case 0: SETCALC(A2B_next_20); break;
//		    case 1: SETCALC(A2B_next_21); break;
//		    case 2: SETCALC(A2B_next_22); break;
		    default : break;
		    }
		break;
	    case 3 :
		switch ((int)mode) {
		    case 0: SETCALC(A2B_next_30); break;
//		    case 1: SETCALC(A2B_next_31); break;
//		    case 2: SETCALC(A2B_next_32); break;
		    default : break;
		    }
		break;
	    case 4 :
		switch ((int)mode) {
		    case 0: SETCALC(A2B_next_40); break;
//		    case 1: SETCALC(A2B_next_41); break;
//		    case 2: SETCALC(A2B_next_42); break;
		    default : break;
		    }
		break;
	    case 5 :
		switch ((int)mode) {
		    case 0: SETCALC(A2B_next_50); break;
//		    case 1: SETCALC(A2B_next_51); break;
//		    case 2: SETCALC(A2B_next_52); break;
		    default : break;
		    }
		break;
	    case 6 :
		switch ((int)mode) {
		    case 0: SETCALC(A2B_next_60); break;
//		    case 1: SETCALC(A2B_next_61); break;
//		    case 2: SETCALC(A2B_next_62); break;
		    default : break;
		    }
		break;
	    case 7 :
		switch ((int)mode) {
		    case 0: SETCALC(A2B_next_70); break;
//		    case 1: SETCALC(A2B_next_71); break;
//		    case 2: SETCALC(A2B_next_72); break;
		    default : break;
		    }
		break;
	    default : break;
	    }
}

#define SETUP_A2B \
    float* a_in = ZIN(0); \
    float* b_in = ZIN(1); \
    float* c_in = ZIN(2); \
    float* d_in = ZIN(3); \
    float* w_out = ZOUT(0); \
    float* x_out = ZOUT(1); \
    float* y_out = ZOUT(2); \
    float* z_out = ZOUT(3); \
    float a, b, c, d;

#define ASSIGN_ABCD \
    a = ZXP(a_in); \
    b = ZXP(b_in); \
    c = ZXP(c_in); \
    d = ZXP(d_in);

/* some useful SC constants
#ifndef __FP__
const double pi     = acos(-1.);
#endif
const double pi2    = pi * .5;
const double pi32   = pi * 1.5;
const double twopi  = pi * 2.;
const double rtwopi = 1. / twopi;
const double log001 = log(0.001);
const double log01  = log(0.01);
const double log1   = log(0.1);
const double rlog2  = 1./log(2.);
const double sqrt2  = sqrt(2.);
const double rsqrt2 = 1. / sqrt2;
*/

void A2B_next_00(A2B *unit, int inNumSamples)
{
    SETUP_A2B
    LOOP(inNumSamples,
	ASSIGN_ABCD

	ZXP(w_out) = (a + b + c + d) * 0.5;
	ZXP(x_out) = (a + b - c - d) * 0.5;
	ZXP(y_out) = (a - b - c + d) * 0.5;
	ZXP(z_out) = (a - b + c - d) * 0.5;
    );
}

void A2B_next_10(A2B *unit, int inNumSamples)
{
    SETUP_A2B
    LOOP(inNumSamples,
	ASSIGN_ABCD

	ZXP(w_out) = (a + b + c + d) * 0.5;
	ZXP(x_out) = (a + b - c - d) * 0.5;
	ZXP(y_out) = (a - b - c + d) * 0.5;
	ZXP(z_out) = (-a + b - c + d) * 0.5;
	)
}

void A2B_next_20(A2B *unit, int inNumSamples)
{
    SETUP_A2B
    LOOP(inNumSamples,
	ASSIGN_ABCD

	ZXP(w_out) = (a + b + c + d) * 0.5;
	ZXP(x_out) = (a + b - c - d) * 0.5;
	ZXP(y_out) = (a - b) * rsqrt2;
	ZXP(z_out) = (c - d) * rsqrt2;
	)
}

void A2B_next_30(A2B *unit, int inNumSamples)
{
    SETUP_A2B
    LOOP(inNumSamples,
	ASSIGN_ABCD

	ZXP(w_out) = (a + b + c + d) * 0.5;
	ZXP(x_out) = (a + b - c - d) * 0.5;
	ZXP(y_out) = (-c + d) * rsqrt2;
	ZXP(z_out) = (a - b) * rsqrt2;
	)
}


void A2B_next_40(A2B *unit, int inNumSamples)
{
    SETUP_A2B
    LOOP(inNumSamples,
	ASSIGN_ABCD

	ZXP(w_out) = (a + b + c + d) * 0.5;
	ZXP(x_out) = (a * sqrt3div2) + ((-b - c - d) * sqrt3div6);
	ZXP(y_out) = (-b + c) * rsqrt2;
	ZXP(z_out) = ((b + c) * rsqrt6) - (d * sqrt6div3);
	)
}

void A2B_next_50(A2B *unit, int inNumSamples)
{
    SETUP_A2B

    LOOP(inNumSamples,
	ASSIGN_ABCD

	ZXP(w_out) = (a + b + c + d) * 0.5;
	ZXP(x_out) = ((a + b + c) * sqrt3div6) - (d * sqrt3div2);
	ZXP(y_out) = (a - b) * rsqrt2;
	ZXP(z_out) = ((-a - b) * rsqrt6) - (c * sqrt6div3);
	)
}

void A2B_next_60(A2B *unit, int inNumSamples)
{
    SETUP_A2B

    LOOP(inNumSamples,
	ASSIGN_ABCD

	ZXP(w_out) = (a + b + c + d) * 0.5;
	ZXP(x_out) = (a * sqrt3div2) + ((-b - c - d) * sqrt3div6);
	ZXP(y_out) = (-c + d) * rsqrt2;
	ZXP(z_out) = (b * sqrt6div3) + ((-c - d) * rsqrt6);
	)
}

void A2B_next_70(A2B *unit, int inNumSamples)
{
    SETUP_A2B

    LOOP(inNumSamples,
	ASSIGN_ABCD

	ZXP(w_out) = (a + b + c + d) * 0.5;
	ZXP(x_out) = ((a + b + c) * sqrt3div6) - (d * sqrt3div2);
	ZXP(y_out) = (a - b) * rsqrt2;
	ZXP(z_out) = ((a + b) * rsqrt6) + (c * sqrt6div3);
	)
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
// Dominante - 4 in 4 out

void Dominate_Ctor(Dominate* unit)
{
	SETCALC(Dominate_next);
	Dominate_next(unit, 1);
	unit->m_dominance = ZIN0(4) * pi2;
}

// four channels in, four channels out...
// regardless... call in w, x, y, z and out a, b, c, d
void Dominate_next(Dominate *unit, int inNumSamples)
{
	float *Wout = ZOUT(0);
	float *Xout = ZOUT(1);
	float *Yout = ZOUT(2);
	float *Zout = ZOUT(3);

	float *Win = ZIN(0);
	float *Xin = ZIN(1);
	float *Yin = ZIN(2);
	float *Zin = ZIN(3);
	float dominanceEnd = ZIN0(4); // dom * 0.5pi
	float dominance = unit->m_dominance;

	float cosDom, sinDom;
	float w, x, y, z;

	dominanceEnd = dominanceEnd * (float)pi2;

	float dominance_slope = CALCSLOPE(dominanceEnd, dominance);

	LOOP(inNumSamples,

	    cosDom = cos(dominance);
	    sinDom = sin(dominance);

	    w = ZXP(Win);
	    x = ZXP(Xin);
	    y = ZXP(Yin);
	    z = ZXP(Zin);

	    ZXP(Wout) = w + (sqrt2 * sinDom * w);
	    ZXP(Xout) = (rsqrt2 * sinDom * x) + x;
	    ZXP(Yout) = y * cosDom;
	    ZXP(Zout) = z * cosDom;

	    dominance += dominance_slope;
	    );

	unit->m_dominance = dominanceEnd;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Push - 4 in 4 out

void Push_Ctor(Push* unit)
{
	SETCALC(Push_next);
	Push_next(unit, 1);
	unit->m_push = ZIN0(4) * pi2;
}

// four channels in, four channels out...
// regardless... call in w, x, y, z and out a, b, c, d
void Push_next(Push *unit, int inNumSamples)
{
	float *Wout = ZOUT(0);
	float *Xout = ZOUT(1);
	float *Yout = ZOUT(2);
	float *Zout = ZOUT(3);

	float *Win = ZIN(0);
	float *Xin = ZIN(1);
	float *Yin = ZIN(2);
	float *Zin = ZIN(3);
	float pushEnd = ZIN0(4); // dom * 0.5pi
	float push = unit->m_push;

	float piover4 = pi * 0.25;
	float cosPushFac, sinPushFac, cosPush;
	float w, x, y, z;

	pushEnd = pushEnd * (float)pi2;

	float push_slope = CALCSLOPE(pushEnd, push);

	LOOP(inNumSamples,

	    cosPushFac = cos(piover4 - push);
	    sinPushFac = sin(piover4 - push);
	    cosPush = cos(push);

	    w = ZXP(Win);
	    x = ZXP(Xin);
	    y = ZXP(Yin);
	    z = ZXP(Zin);

	    ZXP(Wout) = ((1 + (sqrt2 * cosPushFac)) * w) + ((sqrt2 - (2 * sinPushFac)) * w);
	    ZXP(Xout) = ((rsqrt2 - cosPushFac) * x) + ((1 + (sqrt2 * sinPushFac)) * x);
	    ZXP(Yout) = y * cosPush;
	    ZXP(Zout) = z * cosPush;

	    push += push_slope;
	    );

	unit->m_push = pushEnd;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

PluginLoad(Ambisonic)
{
	ft = inTable;

	DefineSimpleUnit(Dominate);
	DefineSimpleUnit(Push);
	//DefineSimpleUnit(A2B);
	DefineSimpleUnit(DistanceB);
	DefineSimpleUnit(ProximityB);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
