/*
 *  RMEQSuiteUGens.cpp
 *  xSC3plugins
 *
 *  Created by Josh Parmenter on 7/20/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
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

const double sqrt3 = sqrt(3.);
const double sqrt3div6 = sqrt(3.) * 0.1666666667;
const double sqrt3div2 = sqrt(3.) * 0.5;
const double rsqrt6 = 1. / sqrt(6.);
const double sqrt6div3 = sqrt(6.) * 0.3333333333;

static InterfaceTable *ft;

extern "C"
{
	void load(InterfaceTable *inTable);
}

/* Allpass2 */

struct Allpass2 : public Unit
{
	double m_y1, m_y2, m_a0, m_a1, m_a2, m_b1, m_b2;
	float m_freq, m_rq;
};

extern "C"
{
	void Allpass2_next_kk(Allpass2 *unit, int inNumSamples);
	void Allpass2_next_aa(Allpass2 *unit, int inNumSamples);
	void Allpass2_Ctor(Allpass2* unit);
}

void Allpass2_Ctor(Allpass2* unit)
{
	if ((INRATE(1) == calc_FullRate) && (INRATE(2) == calc_FullRate)) {
		SETCALC(Allpass2_next_aa);
		} else {
		SETCALC(Allpass2_next_kk);
		}
	float freq = unit->m_freq = ZIN0(1);
	float rq = unit->m_rq = ZIN0(2);

	double wc = twopi * (double)freq * SAMPLEDUR;
	double y = cos(wc) * -1.;
	double tanw = tan(0.5 * wc * (double)rq);
	double a0 = unit->m_a0 = (1. - tanw) / (1. + tanw);
	double a1 = unit->m_a1 = y * (1. + a0);
	unit->m_a2 = 1.;
	unit->m_b1 = -a1;
	unit->m_b2 = -a0;

	unit->m_y1 = 0.;

	unit->m_y2 = 0.;
	ClearUnitOutputs(unit, 1);
}

#define CHECK_ALLPASS_INPUTS \
    curfreq = ZXP(freq); \
    currq = ZXP(rq); \
    if ((unit->m_freq != curfreq) || (unit->m_rq != currq)) { \
	wc = twopi * (double)curfreq * SAMPLEDUR; \
	y = cos(wc) * -1.; \
	tanw = tan(0.5 * wc * (double)currq); \
	a0 = unit->m_a0 = (1. - tanw) / (1. + tanw); \
	a1 = y * (1. + a0); \
	a2 = 1.; \
	b1 = -a1; \
	b2 = -a0; \
	unit->m_freq = curfreq; \
	unit->m_rq = currq; \
    } \


void Allpass2_next_aa(Allpass2 *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float *freq = ZIN(1);
	float *rq = ZIN(2);

	double a0, a1, a2, b1, b2, wc, y, tanw;
	double y0, y1, y2;
	float curfreq, currq;

	y1 = unit->m_y1;
	y2 = unit->m_y2;

	a0 = unit->m_a0;
	a1 = unit->m_a1;
	a2 = unit->m_a2;
	b1 = unit->m_b1;
	b2 = unit->m_b2;

	LOOP(unit->mRate->mFilterLoops,

		CHECK_ALLPASS_INPUTS

		y0 = ZXP(in) + b1 * y1 + b2 * y2;
		ZXP(out) = a0 * y0 + a1 * y1 + a2 * y2;

		CHECK_ALLPASS_INPUTS

		y2 = ZXP(in) + b1 * y0 + b2 * y1;
		ZXP(out) = a0 * y2 + a1 * y0 + a2 * y1;

		CHECK_ALLPASS_INPUTS

		y1 = ZXP(in) + b1 * y2 + b2 * y0;
		ZXP(out) = a0 * y1 + a1 * y2 + a2 * y0;
	);
	LOOP(unit->mRate->mFilterRemain,

		CHECK_ALLPASS_INPUTS

		y0 = ZXP(in) + b1 * y1 + b2 * y2;
		ZXP(out) = a0 * y0 + a1 * y1 + a2 * y2;
		y2 = y1;
		y1 = y0;
	);

	unit->m_a0 = a0;
	unit->m_a1 = a1;
	unit->m_a2 = a2;
	unit->m_b1 = b1;
	unit->m_b2 = b2;
	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}


void Allpass2_next_kk(Allpass2 *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float nextfreq = ZIN0(1);
	float nextrq = ZIN0(2);

	double a0, a1, a2, b1, b2;
	double nexta0, nexta1, nexta2, nextb1, nextb2, nextwc, nexty, nexttanw;
	double y0;
	double y1 = unit->m_y1;
	double y2 = unit->m_y2;
	double a0slope, a1slope, a2slope, b1slope, b2slope;

	a0 = unit->m_a0;
	a1 = unit->m_a1;
	a2 = unit->m_a2;
	b1 = unit->m_b1;
	b2 = unit->m_b2;
	if ((unit->m_freq != nextfreq) || (unit->m_rq != nextrq)){

	    nextwc = twopi * (double)nextfreq * SAMPLEDUR;
	    nexty = cos(nextwc) * -1.;
	    nexttanw = tan(0.5 * nextwc * (double)nextrq);
	    nexta0 = unit->m_a0 = (1. - nexttanw) / (1. + nexttanw);
	    nexta1 = nexty * (1. + a0);
	    nexta2 = 1.;
	    nextb1 = -a1;
	    nextb2 = -a0;

	    a0slope = (nexta0 - a0) * unit->mRate->mFilterSlope;
	    a1slope = (nexta1 - a1) * unit->mRate->mFilterSlope;
	    a2slope = (nexta2 - a2) * unit->mRate->mFilterSlope;
	    b1slope = (nextb1 - b1) * unit->mRate->mFilterSlope;
	    b2slope = (nextb2 - b2) * unit->mRate->mFilterSlope;
	    unit->m_freq = nextfreq;
	    unit->m_rq = nextrq;
	    LOOP(unit->mRate->mFilterLoops,
		y0 = ZXP(in) + b1 * y1 + b2 * y2;
		ZXP(out) = a0 * y0 + a1 * y1 + a2 * y2;

		y2 = ZXP(in) + b1 * y0 + b2 * y1;
		ZXP(out) = a0 * y2 + a1 * y0 + a2 * y1;

		y1 = ZXP(in) + b1 * y2 + b2 * y0;
		ZXP(out) = a0 * y1 + a1 * y2 + a2 * y0;
		a0 += a0slope;
		a1 += a1slope;
		a2 += a2slope;
		b1 += b1slope;
		b2 += b2slope;
	    );
	    LOOP(unit->mRate->mFilterRemain,
		y0 = ZXP(in) + b1 * y1 + b2 * y2;
		ZXP(out) = a0 * y0 + a1 * y1 + a2 * y2;
		y2 = y1;
		y1 = y0;
	    );
	    } else {
	    LOOP(unit->mRate->mFilterLoops,
		    y0 = ZXP(in) + b1 * y1 + b2 * y2;
		    ZXP(out) = a0 * y0 + a1 * y1 + a2 * y2;

		    y2 = ZXP(in) + b1 * y0 + b2 * y1;
		    ZXP(out) = a0 * y2 + a1 * y0 + a2 * y1;

		    y1 = ZXP(in) + b1 * y2 + b2 * y0;
		    ZXP(out) = a0 * y1 + a1 * y2 + a2 * y0;
	    );
	    LOOP(unit->mRate->mFilterRemain,
		    y0 = ZXP(in) + b1 * y1 + b2 * y2;
		    ZXP(out) = a0 * y0 + a1 * y1 + a2 * y2;
		    y2 = y1;
		    y1 = y0;
	    );
	    }

	unit->m_a0 = a0;
	unit->m_a1 = a1;
	unit->m_a2 = a2;
	unit->m_b1 = b1;
	unit->m_b2 = b2;
	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);

}

/* Allpass1 */

struct Allpass1 : public Unit
{
	double m_y1, m_a0, m_a1, m_b1;
	float m_freq;
};

extern "C"
{
	void Allpass1_next_k(Allpass1 *unit, int inNumSamples);
	void Allpass1_next_a(Allpass1 *unit, int inNumSamples);
	void Allpass1_Ctor(Allpass1 *unit);
}

void Allpass1_Ctor(Allpass1* unit)
{
	if (INRATE(1) == calc_FullRate) {
		SETCALC(Allpass1_next_a);
	    } else {
		SETCALC(Allpass1_next_k);
	    }

	unit->m_freq = ZIN0(1);
	unit->m_y1 = 0.f;
	double wc = pi * (double)unit->m_freq * SAMPLEDUR;
	double b1 = (1. - wc) / (1. + wc);
	unit->m_a0 = -b1;
	unit->m_a1 = 1.f;
	unit->m_b1 = b1;
	ZOUT0(0) = 0.f;
}

void Allpass1_next_a(Allpass1* unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float *freq = ZIN(1);
	float curfreq;
	double wc, a0, a1, b1;
	a0 = unit->m_a0;
	a1 = unit->m_a1;
	b1 = unit->m_b1;

	double y1 = unit->m_y1;
	LOOP(inNumSamples,

		if(unit->m_freq != (curfreq = ZXP(freq))){
		    wc = pi * (double)curfreq * SAMPLEDUR;
		    unit->m_b1 = b1 = (1. - wc) / (1. + wc);
		    unit->m_a0 = a0 = -b1;
		    }

		double y0 = ZXP(in) + b1 * y1;
		ZXP(out) = a0 * y0 + a1 * y1;
		y1 = y0;
	);
	unit->m_y1 = zapgremlins(y1);

}

void Allpass1_next_k(Allpass1* unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float next_freq = ZIN0(1);

	double y1 = unit->m_y1;
	double a0 = unit->m_a0;
	double a1 = unit->m_a1;
	double b1 = unit->m_b1;
	double a0_slope, b1_slope;

	if(next_freq != unit->m_freq){
	    double wc = pi * (double)next_freq * SAMPLEDUR;
	    double next_b1 = (1. - wc) / (1. + wc);
	    double next_a0 = -next_b1;
	    a0_slope = CALCSLOPE(next_a0, a0);
	    b1_slope = CALCSLOPE(next_b1, b1);
	    } else {
	    a0_slope = 0.;
	    b1_slope = 0.;
	    }

	LOOP(inNumSamples,
		double y0 = ZXP(in) + b1 * y1;
		ZXP(out) = a0 * y0 + a1 * y1;
		y1 = y0;

		a0 += a0_slope;
		b1 += b1_slope;
	);

	unit->m_freq = next_freq;
	unit->m_y1 = zapgremlins(y1);
	unit->m_a0 = a0;
	unit->m_b1 = b1;

}

/* RMEQ */

struct RMEQ : public Unit
{
	double m_y1, m_y2, m_a0, m_a1, m_a2, m_b1, m_b2;
	float m_freq, m_rq, m_k, m_ksign, m_kabs;
};

extern "C"
{
	void RMEQ_next_kkk(RMEQ *unit, int inNumSamples);
	void RMEQ_next_aaa(RMEQ *unit, int inNumSamples);
	void RMEQ_Ctor(RMEQ* unit);
}

#define CHECK_K_A \
    nextk = ZXP(k); \
    if(nextk != unit->m_k){ \
	if(nextk >= 0.) \
	    ksign = 1.;  \
	    else  \
	    ksign = -1.; \
	kabs = fabs(nextk); \
	unit->m_k = nextk; \
	}

#define CHECK_K_K \
    if(kslope != 0.) { \
	if(curk >= 0.) \
	    ksign = 1.;  \
	    else  \
	    ksign = -1.; \
	kabs = fabs(curk); \
	}

void RMEQ_Ctor(RMEQ* unit)
{
	if ((INRATE(1) == calc_FullRate) && (INRATE(2) == calc_FullRate) && (INRATE(3) == calc_FullRate)) {
		SETCALC(RMEQ_next_aaa);
		} else {
		SETCALC(RMEQ_next_kkk);
		}
	float freq = unit->m_freq = ZIN0(1);
	float rq = unit->m_rq = ZIN0(2);
	float k = unit->m_k = ZIN0(3);
	unit->m_kabs = fabs(k);
	if(k >= 0.) unit->m_ksign = 1.;
	    else
	    unit->m_ksign = -1.;
	double wc = twopi * (double)freq * SAMPLEDUR;
	double y = cos(wc) * -1.;
	double tanw = tan(0.5 * wc * (double)rq);
	double a0 = unit->m_a0 = (1. - tanw) / (1. + tanw);
	double a1 = unit->m_a1 = y * (1. + a0);
	unit->m_a2 = 1.;
	unit->m_b1 = -a1;
	unit->m_b2 = -a0;

	unit->m_y1 = 0.;

	unit->m_y2 = 0.;
	ClearUnitOutputs(unit, 1);
}

void RMEQ_next_aaa(RMEQ *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float *freq = ZIN(1);
	float *rq = ZIN(2);
	float *k = ZIN(3);

	double a0, a1, a2, b1, b2, wc, y, tanw;
	double y0, y1, y2, allpass;
	float curfreq, currq, nextk, ksign, kabs, curin;

	y1 = unit->m_y1;
	y2 = unit->m_y2;

	a0 = unit->m_a0;
	a1 = unit->m_a1;
	a2 = unit->m_a2;
	b1 = unit->m_b1;
	b2 = unit->m_b2;
	ksign = unit->m_ksign;
	kabs = unit->m_kabs;


	LOOP(unit->mRate->mFilterLoops,
		CHECK_ALLPASS_INPUTS
		CHECK_K_A

		y0 = (curin = ZXP(in)) + b1 * y1 + b2 * y2;
		allpass = (a0 * y0 + a1 * y1 + a2 * y2) * ksign;
		ZXP(out) = 0.5 * (curin + allpass + (kabs * (curin - allpass)));

		CHECK_ALLPASS_INPUTS
		CHECK_K_A

		y2 = (curin = ZXP(in)) + b1 * y0 + b2 * y1;
		allpass = (a0 * y2 + a1 * y0 + a2 * y1) * ksign;
		ZXP(out) = 0.5 * (curin + allpass + (kabs * (curin - allpass)));

		CHECK_ALLPASS_INPUTS
		CHECK_K_A

		y1 = (curin = ZXP(in)) + b1 * y2 + b2 * y0;
		allpass = (a0 * y1 + a1 * y2 + a2 * y0) * ksign;
		ZXP(out) = 0.5 * (curin + allpass + (kabs * (curin - allpass)));
	);
	LOOP(unit->mRate->mFilterRemain,
		CHECK_ALLPASS_INPUTS
		CHECK_K_A

		y0 = (curin = ZXP(in)) + b1 * y1 + b2 * y2;
		allpass = (a0 * y0 + a1 * y1 + a2 * y2) * ksign;
		ZXP(out) = 0.5 * (curin + allpass + (kabs * (curin - allpass)));

		y2 = y1;
		y1 = y0;
	);

	unit->m_k = nextk;
	unit->m_kabs = kabs;
	unit->m_ksign = ksign;

	unit->m_a0 = a0;
	unit->m_a1 = a1;
	unit->m_a2 = a2;
	unit->m_b1 = b1;
	unit->m_b2 = b2;
	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}


void RMEQ_next_kkk(RMEQ *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float nextfreq = ZIN0(1);
	float nextrq = ZIN0(2);
	float nextk = ZIN0(3);

	double a0, a1, a2, b1, b2;
	double nexta0, nexta1, nexta2, nextb1, nextb2, nextwc, nexty, nexttanw;
	double y0;
	double y1 = unit->m_y1;
	double y2 = unit->m_y2;
	double a0slope, a1slope, a2slope, b1slope, b2slope;
	float ksign, kabs, curin, curk, kslope, allpass;


	a0 = unit->m_a0;
	a1 = unit->m_a1;
	a2 = unit->m_a2;
	b1 = unit->m_b1;
	b2 = unit->m_b2;
	ksign = unit->m_ksign;
	kabs = unit->m_kabs;
	curk = unit->m_k;

	if(curk != nextk) {
	    kslope = CALCSLOPE(nextk, curk);
	    } else {
	    kslope = 0.;
	    }

	if ((unit->m_freq != nextfreq) || (unit->m_rq != nextrq)){

	    nextwc = twopi * (double)nextfreq * SAMPLEDUR;
	    nexty = cos(nextwc) * -1.;
	    nexttanw = tan(0.5 * nextwc * (double)nextrq);
	    nexta0 = unit->m_a0 = (1. - nexttanw) / (1. + nexttanw);
	    nexta1 = nexty * (1. + a0);
	    nexta2 = 1.;
	    nextb1 = -a1;
	    nextb2 = -a0;

	    a0slope = (nexta0 - a0) * unit->mRate->mFilterSlope;
	    a1slope = (nexta1 - a1) * unit->mRate->mFilterSlope;
	    a2slope = (nexta2 - a2) * unit->mRate->mFilterSlope;
	    b1slope = (nextb1 - b1) * unit->mRate->mFilterSlope;
	    b2slope = (nextb2 - b2) * unit->mRate->mFilterSlope;

	    kslope = CALCSLOPE(nextk, curk);

	    unit->m_freq = nextfreq;
	    unit->m_rq = nextrq;

	    LOOP(unit->mRate->mFilterLoops,
		y0 = (curin = ZXP(in)) + b1 * y1 + b2 * y2;
		allpass = (a0 * y0 + a1 * y1 + a2 * y2) * ksign;
		ZXP(out) = 0.5 * (curin + allpass + (kabs * (curin - allpass)));

		curk += kslope;
		CHECK_K_K

		y2 = (curin = ZXP(in)) + b1 * y0 + b2 * y1;
		allpass = (a0 * y2 + a1 * y0 + a2 * y1) * ksign;
		ZXP(out) = 0.5 * (curin + allpass + (kabs * (curin - allpass)));

		curk += kslope;
		CHECK_K_K

		y1 = (curin = ZXP(in)) + b1 * y2 + b2 * y0;
		allpass = (a0 * y1 + a1 * y2 + a2 * y0) * ksign;
		ZXP(out) = 0.5 * (curin + allpass + (kabs * (curin - allpass)));

		curk += kslope;
		CHECK_K_K

		a0 += a0slope;
		a1 += a1slope;
		a2 += a2slope;
		b1 += b1slope;
		b2 += b2slope;
	    );
	    LOOP(unit->mRate->mFilterRemain,
		y0 = (curin = ZXP(in)) + b1 * y1 + b2 * y2;
		allpass = (a0 * y0 + a1 * y1 + a2 * y2) * ksign;
		ZXP(out) = 0.5 * (curin + allpass + (kabs * (curin - allpass)));

		curk += kslope;
		CHECK_K_K

		y2 = y1;
		y1 = y0;
	    );
	    } else {
	    LOOP(unit->mRate->mFilterLoops,
		    y0 = (curin = ZXP(in)) + b1 * y1 + b2 * y2;
		    allpass = (a0 * y0 + a1 * y1 + a2 * y2) * ksign;
		    ZXP(out) = 0.5 * (curin + allpass + (kabs * (curin - allpass)));

		    curk += kslope;
		    CHECK_K_K

		    y2 = (curin = ZXP(in)) + b1 * y0 + b2 * y1;
		    allpass = (a0 * y2 + a1 * y0 + a2 * y1) * ksign;
		    ZXP(out) = 0.5 * (curin + allpass + (kabs * (curin - allpass)));

		    curk += kslope;
		    CHECK_K_K

		    y1 = (curin = ZXP(in)) + b1 * y2 + b2 * y0;
		    allpass = (a0 * y1 + a1 * y2 + a2 * y0) * ksign;
		    ZXP(out) = 0.5 * (curin + allpass + (kabs * (curin - allpass)));

		    curk += kslope;
		    CHECK_K_K
	    );
	    LOOP(unit->mRate->mFilterRemain,
		    y0 = (curin = ZXP(in)) + b1 * y1 + b2 * y2;
		    allpass = (a0 * y0 + a1 * y1 + a2 * y2) * ksign;
		    ZXP(out) = 0.5 * (curin + allpass + (kabs * (curin - allpass)));

		    curk += kslope;
		    CHECK_K_K

		    y2 = y1;
		    y1 = y0;
	    );
	    }

	unit->m_k = curk;
	unit->m_ksign = ksign;
	unit->m_kabs = kabs;
	unit->m_a0 = a0;
	unit->m_a1 = a1;
	unit->m_a2 = a2;
	unit->m_b1 = b1;
	unit->m_b2 = b2;
	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);

}

/* RMShelf */

/* Allpass1 */

struct RMShelf : public Unit
{
	double m_y1, m_a0, m_a1, m_b1;
	float m_freq, m_k, m_kabs, m_ksign;
};

extern "C"
{
	void RMShelf_next_k(RMShelf *unit, int inNumSamples);
	void RMShelf_next_a(RMShelf *unit, int inNumSamples);
	void RMShelf_Ctor(RMShelf *unit);
}

void RMShelf_Ctor(RMShelf* unit)
{
	if ((INRATE(1) == calc_FullRate) && (INRATE(2) == calc_FullRate)) {
		SETCALC(RMShelf_next_a);
	    } else {
		SETCALC(RMShelf_next_k);
	    }

	unit->m_freq = ZIN0(1);
	unit->m_y1 = 0.f;
	double wc = pi * (double)unit->m_freq * SAMPLEDUR;
	double b1 = (1. - wc) / (1. + wc);
	unit->m_a0 = -b1;
	unit->m_a1 = 1.f;
	unit->m_b1 = b1;
	unit->m_k = ZIN0(2);
	unit->m_kabs = fabs(unit->m_k);
	if(unit->m_k >=0)
	    unit->m_ksign = 1.;
	    else
	    unit->m_ksign = -1.;
	ZOUT0(0) = 0.f;
}


void RMShelf_next_a(RMShelf* unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float *freq = ZIN(1);
	float *k = ZIN(2);
	float curfreq;
	double wc, a0, a1, b1;
	float ksign, kabs, allpass, curin, curk;

	a0 = unit->m_a0;
	a1 = unit->m_a1;
	b1 = unit->m_b1;

	ksign = unit->m_ksign;
	kabs = unit->m_kabs;

	double y1 = unit->m_y1;
	LOOP(inNumSamples,

		if(unit->m_k != (curk = ZXP(k))){
		    if(curk >= 0){
			ksign = 1.;
			} else {
			ksign = -1.;
			}
		    kabs = fabs(curk);
		    unit->m_k = curk;
		    }

		if(unit->m_freq != (curfreq = ZXP(freq))){
		    wc = pi * (double)curfreq * SAMPLEDUR;
		    unit->m_b1 = b1 = (1. - wc) / (1. + wc);
		    unit->m_a0 = a0 = -b1;
		    }

		double y0 = (curin = ZXP(in)) + b1 * y1;
		allpass = (a0 * y0 + a1 * y1) * ksign;
		ZXP(out) = 0.5 * (curin + allpass + (kabs * (curin - allpass)));
		y1 = y0;
	);
	unit->m_y1 = zapgremlins(y1);
	unit->m_ksign = ksign;
	unit->m_kabs = kabs;

}

void RMShelf_next_k(RMShelf* unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float next_freq = ZIN0(1);
	float next_k = ZIN0(2);

	double y1 = unit->m_y1;
	double a0 = unit->m_a0;
	double a1 = unit->m_a1;
	double b1 = unit->m_b1;
	double a0_slope, b1_slope;

	float k = unit->m_k;
	float ksign = unit->m_ksign;
	float kabs = unit->m_kabs;
	float allpass, curin, kslope;

	if(next_freq != unit->m_freq){
	    double wc = pi * (double)next_freq * SAMPLEDUR;
	    double next_b1 = (1. - wc) / (1. + wc);
	    double next_a0 = -next_b1;
	    a0_slope = CALCSLOPE(next_a0, a0);
	    b1_slope = CALCSLOPE(next_b1, b1);
	    } else {
	    a0_slope = 0.;
	    b1_slope = 0.;
	    }

	if(next_k != unit->m_k){
	    kslope = CALCSLOPE(next_k, k);
	    } else {
	    kslope = 0.;
	    }

	LOOP(inNumSamples,
		double y0 = (curin = ZXP(in)) + b1 * y1;
		allpass = (a0 * y0 + a1 * y1) * ksign;
		ZXP(out) = 0.5 * (curin + allpass + (kabs * (curin - allpass)));

		if(next_k != unit->m_k){
		    if(k >= 0.)
			ksign = 1.;
			else
			ksign = -1;
		    kabs = fabs(k);
		    }

		y1 = y0;

		a0 += a0_slope;
		b1 += b1_slope;
		k += kslope;
	);

	unit->m_freq = next_freq;
	unit->m_y1 = zapgremlins(y1);
	unit->m_a0 = a0;
	unit->m_b1 = b1;

	if(next_k != unit->m_k){
	    unit->m_k = next_k;
	    unit->m_ksign = ksign;
	    unit->m_kabs = kabs;
	    }

}

/* RMShelf2 */


struct RMShelf2 : public Unit
{
	double m_y1, m_y1_2, m_a0, m_a1, m_b1;
	float m_freq, m_k, m_ksign, m_kabs;
};

extern "C"
{
	void RMShelf2_next_k(RMShelf2 *unit, int inNumSamples);
	void RMShelf2_next_a(RMShelf2 *unit, int inNumSamples);
	void RMShelf2_Ctor(RMShelf2 *unit);
}

void RMShelf2_Ctor(RMShelf2* unit)
{
	if (INRATE(1) == calc_FullRate) {
		SETCALC(RMShelf2_next_a);
	    } else {
		SETCALC(RMShelf2_next_k);
	    }

	unit->m_freq = ZIN0(1);
	unit->m_k = ZIN0(2);
	if(unit->m_k >=0)
	    unit->m_ksign = 1.0;
	    else
	    unit->m_ksign = -1.0;
	unit->m_kabs = fabs(unit->m_k);
	unit->m_y1 = 0.f;
	unit->m_y1_2 = 0.f;
	double wc = pi * (double)unit->m_freq * SAMPLEDUR;
	double b1 = (1. - wc) / (1. + wc);
	unit->m_a0 = -b1;
	unit->m_a1 = 1.f;
	unit->m_b1 = b1;
	ZOUT0(0) = 0.f;
}

void RMShelf2_next_a(RMShelf2* unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float *freq = ZIN(1);
	float *k = ZIN(2);
	float curfreq, curk, ksign, kabs, sum, curin, allpass1, allpass2;
	double wc, a0, a1, b1;
	a0 = unit->m_a0;
	a1 = unit->m_a1;
	b1 = unit->m_b1;
	ksign = unit->m_ksign;
	kabs = unit->m_kabs;

	double y1 = unit->m_y1;
	double y1_2 = unit->m_y1_2;

	LOOP(inNumSamples,

		if(unit->m_freq != (curfreq = ZXP(freq))){
		    wc = pi * (double)curfreq * SAMPLEDUR;
		    b1 = (1. - wc) / (1. + wc);
		    a0 = -b1;
		    }

		if(unit->m_k != (curk = ZXP(k))){
		    if(curk >= 0.)
			ksign = 1.;
			else
			ksign = -1.;
		    kabs = fabs(curk);
		    unit->m_k = curk;
		    }

		double y0 = (curin = ZXP(in)) + b1 * y1;
		allpass1 = a0 * y0 + a1 * y1;
		y1 = y0;

		double y0_2 = allpass1 + b1 * y1_2;
		allpass2 = a0 * y0_2 + a1 * y1_2;
		y1_2 = y0_2;

		sum = 0.5 * ksign * (curin + allpass2);

		ZXP(out) = (0.5 * (allpass1 + sum + (kabs * (allpass1 - sum))));
	);
	unit->m_y1 = zapgremlins(y1);
	unit->m_y1_2 = zapgremlins(y1_2);
	unit->m_a0 = a0;
	unit->m_b1 = b1;
	unit->m_ksign = ksign;
	unit->m_kabs = kabs;

}

void RMShelf2_next_k(RMShelf2* unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float next_freq = ZIN0(1);
	float next_k = ZIN0(2);

	double y1 = unit->m_y1;
	double y1_2 = unit->m_y1_2;
	float k = unit->m_k;
	float ksign = unit->m_ksign;
	float kabs = unit->m_kabs;

	double a0 = unit->m_a0;
	double a1 = unit->m_a1;
	double b1 = unit->m_b1;
	double a0_slope, b1_slope;
	double y0, y0_2;
	float kslope, allpass1, allpass2, sum, curin;

	if(next_freq != unit->m_freq){
	    double wc = pi * (double)next_freq * SAMPLEDUR;
	    double next_b1 = (1. - wc) / (1. + wc);
	    double next_a0 = -next_b1;
	    a0_slope = CALCSLOPE(next_a0, a0);
	    b1_slope = CALCSLOPE(next_b1, b1);
	    } else {
	    a0_slope = 0.;
	    b1_slope = 0.;
	    }

	if(next_k != k){
	    kslope = CALCSLOPE(next_k, k);
	    } else {
	    kslope = 0.;
	    }

	LOOP(inNumSamples,
		y0 = (curin = ZXP(in)) + b1 * y1;
		allpass1 = a0 * y0 + a1 * y1;
		y1 = y0;

		y0_2 = allpass1 + b1 * y1_2;
		allpass2 = a0 * y0_2 + a1 * y1_2;
		y1_2 = y0_2;

		sum = 0.5 * (curin + allpass2) * ksign;

		ZXP(out) = (0.5 * (allpass1 + sum + (kabs * (allpass1 - sum))));

		a0 += a0_slope;
		b1 += b1_slope;

		if(next_k != unit->m_k){
		    k += kslope;
		    if(k >= 0)
			ksign = 1.;
			else
			ksign = -1.;
		    kabs = fabs(k);
		    }
	);

	unit->m_freq = next_freq;
	unit->m_y1 = zapgremlins(y1);
	unit->m_y1_2 = zapgremlins(y1_2);
	unit->m_a0 = a0;
	unit->m_b1 = b1;
	if(next_k != unit->m_k){
	    unit->m_k = k;
	    unit->m_kabs = kabs;
	    unit->m_ksign = ksign;
	    }

}


/* Spreader */

struct Spreader : public Unit
{
	double* m_y1;
	double* m_a0;
//	double* m_a1;
	double* m_b1;
	float* m_freq;
	int m_numFilts;
	float m_theta, m_theta2, m_cosTheta2, m_sinTheta2, m_nSinTheta2;
};

extern "C"
{
	void Spreader_next_k(Spreader *unit, int inNumSamples);
	void Spreader_next_a(Spreader *unit, int inNumSamples);
	void Spreader_Ctor(Spreader *unit);
}

void Spreader_Ctor(Spreader* unit)
{
	if (INRATE(1) == calc_FullRate) {
		SETCALC(Spreader_next_a);
	    } else {
		SETCALC(Spreader_next_k);
	    }
	unit->m_theta = ZIN0(1);
	unit->m_theta2 = unit->m_theta * 0.5;
	unit->m_cosTheta2 = cos(unit->m_theta2);
	unit->m_sinTheta2 = sin(unit->m_theta2);
	unit->m_nSinTheta2 = unit->m_sinTheta2 * -1.;

	float filtsPerOct = ZIN0(2);
	float rFiltsPerOct = 1. / filtsPerOct;
	unit->m_numFilts = (int)filtsPerOct * 10;
	float freq = 0.0;
	double wc;

	unit->m_freq = (float*)RTAlloc(unit->mWorld, unit->m_numFilts * sizeof(float));
	unit->m_y1 = (double*)RTAlloc(unit->mWorld, unit->m_numFilts * sizeof(double));
	unit->m_a0 = (double*)RTAlloc(unit->mWorld, unit->m_numFilts * sizeof(double));
	unit->m_b1 = (double*)RTAlloc(unit->mWorld, unit->m_numFilts * sizeof(double));

	for(int i = 0; i < unit->m_numFilts; i ++){
	    unit->m_y1[i] = 0.0;
	    unit->m_freq[i] = sc_octcps(freq);
	    freq += rFiltsPerOct;
	    wc = pi * (double)(unit->m_freq[i]) * SAMPLEDUR;
	    unit->m_b1[i] = (1. - wc) / (1. + wc);
	    unit->m_a0[i] = -unit->m_b1[i];
	    }

	ZOUT0(0) = 0.f;
	ZOUT0(1) = 0.f;
}

void Spreader_Dtor(Spreader* unit)
{
	RTFree(unit->mWorld, unit->m_freq);
	RTFree(unit->mWorld, unit->m_y1);
	RTFree(unit->mWorld, unit->m_a0);
	RTFree(unit->mWorld, unit->m_b1);
}
void Spreader_next_a(Spreader* unit, int inNumSamples)
{
	float *outl = OUT(0);
	float *outr = OUT(1);
	float *in = IN(0);
	float *theta = IN(1);
	float sig;
	float curIn, cosIn, curTheta, cosTheta2, sinTheta2, theta2;

	cosTheta2 = unit->m_cosTheta2;
	sinTheta2 = unit->m_sinTheta2;

	for(int j = 0; j < inNumSamples; j++){
		sig = curIn = in[j];
		if((curTheta = theta[j]) != unit->m_theta){
		    unit->m_theta = curTheta;
		    theta2 = curTheta * 0.5;
		    unit->m_cosTheta2 = cos(theta2);
		    unit->m_sinTheta2 = sin(theta2);
		    }
		for(int i = 0; i < unit->m_numFilts; i ++){
		    float y0 = sig + unit->m_b1[i] * unit->m_y1[i];
		    sig = unit->m_a0[i] * y0 + 1. * unit->m_y1[i];
		    unit->m_y1[i] = y0;
		    }

		cosIn = (unit->m_cosTheta2 * curIn);
		outl[j] = cosIn + (unit->m_sinTheta2 * sig);
		outr[j] = cosIn + (-1. * unit->m_sinTheta2 * sig);

	    };
	for(int i = 0; i < unit->m_numFilts; i ++){
	    unit->m_y1[i] = zapgremlins(unit->m_y1[i]);
	    }

}

void Spreader_next_k(Spreader* unit, int inNumSamples)
{
	float *outl = OUT(0);
	float *outr = OUT(1);
	float *in = IN(0);
	float theta = IN0(1);
	float sig;
	float curIn, cosIn, curTheta, cosTheta2, sinTheta2, theta2, nSinTheta2;

	cosTheta2 = unit->m_cosTheta2;
	sinTheta2 = unit->m_sinTheta2;
	nSinTheta2 = unit->m_nSinTheta2;

	if(theta != unit->m_theta){
	    unit->m_theta = curTheta;
	    theta2 = curTheta * 0.5;
	    unit->m_cosTheta2 = cos(theta2);
	    unit->m_sinTheta2 = sin(theta2);
	    unit->m_nSinTheta2 = unit->m_sinTheta2 * -1.;
	    float cosTheta2Slope = CALCSLOPE(unit->m_cosTheta2, cosTheta2);
	    float sinTheta2Slope = CALCSLOPE(unit->m_sinTheta2, sinTheta2);
	    float nSinTheta2Slope = CALCSLOPE(unit->m_nSinTheta2, nSinTheta2);

	    for(int j = 0; j < inNumSamples; j++){
		    sig = curIn = in[j];

		    for(int i = 0; i < unit->m_numFilts; i ++){
			double y0 = sig + unit->m_b1[i] * unit->m_y1[i];
			sig = unit->m_a0[i] * y0 + 1. * unit->m_y1[i];
			unit->m_y1[i] = y0;
			}

		    cosIn = (cosTheta2 * curIn);
		    outl[j] = cosIn + (sinTheta2 * sig);
		    outr[j] = cosIn + (nSinTheta2 * sig);

		    cosTheta2 += cosTheta2Slope;
		    sinTheta2 += sinTheta2Slope;
		    nSinTheta2 += nSinTheta2Slope;
		};

	    } else {

	    for(int j = 0; j < inNumSamples; j++){
		    sig = curIn = in[j];

		    for(int i = 0; i < unit->m_numFilts; i ++){
			double y0 = sig + unit->m_b1[i] * unit->m_y1[i];
			sig = unit->m_a0[i] * y0 + 1. * unit->m_y1[i];
			unit->m_y1[i] = y0;
			}

		    cosIn = (cosTheta2 * curIn);
		    outl[j] = cosIn + (sinTheta2 * sig);
		    outr[j] = cosIn + (nSinTheta2 * sig);
		};
	    }

	for(int i = 0; i < unit->m_numFilts; i ++){
	    unit->m_y1[i] = zapgremlins(unit->m_y1[i]);
	    }

}


////////////////////////////////////////////////////////////////////////////////////////////////////////

PluginLoad(RMEQSuite)
{
	ft = inTable;

	DefineSimpleUnit(Allpass2);
	DefineSimpleUnit(Allpass1);
	DefineSimpleUnit(RMEQ);
	DefineSimpleUnit(RMShelf);
	DefineSimpleUnit(RMShelf2);
	DefineDtorCantAliasUnit(Spreader);
/*
#define DefineDelayUnit(name) \
    (*ft->fDefineUnit)(#name, sizeof(name), (UnitCtorFunc)&name##_Ctor, \
    (UnitDtorFunc)&DelayUnit_Dtor, 0);

	DefineDelayUnit(CombLP);
*/
}


