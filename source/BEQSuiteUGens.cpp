
/*
 *  BEQSuite.cpp
 *  xSC3plugins
 *
 *  Created by Josh Parmenter on 6/20/05.
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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


#include "SC_PlugIn.h"

static InterfaceTable *ft;  

extern "C"
{
	void load(InterfaceTable *inTable);
}

struct BLowPass : public Unit
{
	float m_y1, m_y2, m_a0, m_a1, m_a2, m_b1, m_b2;
	float m_freq, m_rq;
};

extern "C"
{
	void BLowPass_next_kk(BLowPass *unit, int inNumSamples);
	void BLowPass_next_aa(BLowPass *unit, int inNumSamples);
	void BLowPass_Ctor(BLowPass* unit);
}


void BLowPass_Ctor(BLowPass* unit)
{	
	if ((INRATE(1) == calc_FullRate) && (INRATE(2) == calc_FullRate)) {
		SETCALC(BLowPass_next_aa);
		} else {
		SETCALC(BLowPass_next_kk);}
	float rq = unit->m_rq = ZIN0(2);
	float freq = unit->m_freq = ZIN0(1);
	float w0 = twopi * freq * SAMPLEDUR;
	float cosw0 = cos(w0);
	float i = 1. - cosw0;
	float alpha = sin(w0) * 0.5 * rq;
	float b0rz = 1. / (1. + alpha);
	float a0 = unit->m_a0 = i * 0.5 * b0rz;
	unit->m_a1 = i * b0rz;
	unit->m_a2 = a0;
	unit->m_b1 = cosw0 * 2. * b0rz;
	unit->m_b2 = (1. - alpha) * -b0rz;
	unit->m_y1 = 0.;

	unit->m_y2 = 0.;
	ClearUnitOutputs(unit, 1);
}
		
void BLowPass_next_aa(BLowPass *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float *freq = ZIN(1);
	float *rq = ZIN(2);

	float a0, a1, a2, b1, b2, w0, cosw0, i, alpha, b0rz;
	float y0, y1, y2;
	
	y1 = unit->m_y1;
	y2 = unit->m_y2;

	a0 = unit->m_a0;
	a1 = unit->m_a1;
	a2 = unit->m_a2;
	b1 = unit->m_b1;
	b2 = unit->m_b2;

	LOOP(unit->mRate->mFilterLoops,
		if ((unit->m_freq != ZXP(freq)) || (unit->m_rq != ZXP(rq))) {
		    w0 = twopi * ZXP(freq) * SAMPLEDUR; 
		    cosw0 = cos(w0); 
		    i = 1. - cosw0; 
		    alpha = sin(w0) * 0.5 * ZXP(rq); 
		    b0rz = 1. / (1. + alpha); 
		    a0 = i * 0.5 * b0rz; 
		    a1 = i * b0rz; 
		    a2 = a0; 
		    b1 = cosw0 * 2. * b0rz; 
		    b2 = (1. - alpha) * -b0rz; 
		    unit->m_freq = ZXP(freq); 
		    unit->m_rq = ZXP(rq);
		}
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
		unit->m_freq = ZXP(freq); 
		unit->m_rq = ZXP(rq);
	);
	
	unit->m_a0 = a0;
	unit->m_a1 = a1;
	unit->m_a2 = a2;
	unit->m_b1 = b1;
	unit->m_b2 = b2;	
	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}


void BLowPass_next_kk(BLowPass *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float nextfreq = ZIN0(1);
	float nextrq = ZIN0(2);

	float a0, a1, a2, b1, b2;
	float nexta0, nexta1, nexta2, nextb1, nextb2, nextw0, nextcosw0, nexti, nextalpha, nextb0rz;
	float y0;
	float y1 = unit->m_y1;
	float y2 = unit->m_y2;
	float a0slope, a1slope, a2slope, b1slope, b2slope;

	a0 = unit->m_a0;
	a1 = unit->m_a1;
	a2 = unit->m_a2;
	b1 = unit->m_b1;
	b2 = unit->m_b2;
	
	if ((unit->m_freq != nextfreq) || (unit->m_rq != nextrq)){
	    nextw0 = twopi * nextfreq * SAMPLEDUR; 
	    nextcosw0 = cos(nextw0); 
	    nexti = 1. - nextcosw0; 
	    nextalpha = sin(nextw0) * 0.5 * nextrq; 
	    nextb0rz = 1. / (1. + nextalpha); 
	    nexta0 = nexti * 0.5 * nextb0rz; 
	    nexta1 = nexti * nextb0rz; 
	    nexta2 = nexta0; 
	    nextb1 = nextcosw0 * 2. * nextb0rz; 
	    nextb2 = ((1. - nextalpha) * -nextb0rz); 
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
	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
	unit->m_a0 = a0;
	unit->m_a1 = a1;
	unit->m_a2 = a2;
	unit->m_b1 = b1;
	unit->m_b2 = b2;

}

struct BHiPass : public Unit
{
	float m_y1, m_y2, m_a0, m_a1, m_a2, m_b1, m_b2;
	float m_freq, m_rq;
};


extern "C"
{
	void BHiPass_next_kk(BHiPass *unit, int inNumSamples);
	void BHiPass_next_aa(BHiPass *unit, int inNumSamples);
	void BHiPass_Ctor(BHiPass* unit);
}

void BHiPass_Ctor(BHiPass* unit)
{	
	if ((INRATE(1) == calc_FullRate) && (INRATE(2) == calc_FullRate)) {
		SETCALC(BHiPass_next_aa);
		} else {
		SETCALC(BHiPass_next_kk);
		}
	float freq = unit->m_freq = ZIN0(1);
	float rq = unit->m_rq = ZIN0(2);
	float w0 = twopi * freq * SAMPLEDUR;
	float cosw0 = cos(w0);
	float i = 1. + cosw0;
	float alpha = sin(w0) * 0.5 * rq;
	float b0rz = 1. / (1. + alpha);
	float a0 = unit->m_a0 = i * 0.5 * b0rz;
	unit->m_a1 = -i * b0rz;
	unit->m_a2 = a0;
	unit->m_b1 = cosw0 * 2. * b0rz;
	unit->m_b2 = (1. - alpha) * -b0rz;
	unit->m_y1 = 0.;

	unit->m_y2 = 0.;
	ClearUnitOutputs(unit, 1);
}
		
void BHiPass_next_aa(BHiPass *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float *freq = ZIN(1);
	float *rq = ZIN(2);

	float a0, a1, a2, b1, b2, w0, cosw0, i, alpha, b0rz;
	float y0, y1, y2;
	
	y1 = unit->m_y1;
	y2 = unit->m_y2;

	a0 = unit->m_a0;
	a1 = unit->m_a1;
	a2 = unit->m_a2;
	b1 = unit->m_b1;
	b2 = unit->m_b2;

	LOOP(unit->mRate->mFilterLoops,
		if ((unit->m_freq != ZXP(freq)) || (unit->m_rq != ZXP(rq))) {
		    w0 = twopi * ZXP(freq) * SAMPLEDUR; 
		    cosw0 = cos(w0); 
		    i = 1. + cosw0; 
		    alpha = sin(w0) * 0.5 * ZXP(rq); 
		    b0rz = 1. / (1. + alpha); 
		    a0 = i * 0.5 * b0rz; 
		    a1 = -i * b0rz; 
		    a2 = a0; 
		    b1 = cosw0 * 2. * b0rz; 
		    b2 = (1. - alpha) * -b0rz; 
		    unit->m_freq = ZXP(freq); 
		    unit->m_rq = ZXP(rq);
		}
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
	
	unit->m_a0 = a0;
	unit->m_a1 = a1;
	unit->m_a2 = a2;
	unit->m_b1 = b1;
	unit->m_b2 = b2;	
	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}


void BHiPass_next_kk(BHiPass *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float nextfreq = ZIN0(1);
	float nextrq = ZIN0(2);

	float a0, a1, a2, b1, b2;
	float nexta0, nexta1, nexta2, nextb1, nextb2, nextw0, nextcosw0, nexti, nextalpha, nextb0rz;
	float y0;
	float y1 = unit->m_y1;
	float y2 = unit->m_y2;
	float a0slope, a1slope, a2slope, b1slope, b2slope;

	a0 = unit->m_a0;
	a1 = unit->m_a1;
	a2 = unit->m_a2;
	b1 = unit->m_b1;
	b2 = unit->m_b2;
	
	if ((unit->m_freq != nextfreq) || (unit->m_rq != nextrq)){
	    nextw0 = twopi * nextfreq * SAMPLEDUR; 
	    nextcosw0 = cos(nextw0); 
	    nexti = 1. + nextcosw0; 
	    nextalpha = sin(nextw0) * 0.5 * nextrq; 
	    nextb0rz = 1. / (1. + nextalpha); 
	    nexta0 = nexti * 0.5 * nextb0rz; 
	    nexta1 = -nexti * nextb0rz; 
	    nexta2 = nexta0; 
	    nextb1 = nextcosw0 * 2. * nextb0rz; 
	    nextb2 = (1. - nextalpha) * -nextb0rz; 
	    a0slope = (nexta0 - a0) * unit->mRate->mFilterSlope; //CALCSLOPE(nexta0, a0); 
	    a1slope = (nexta1 - a1) * unit->mRate->mFilterSlope; //CALCSLOPE(nexta1, a1); 
	    a2slope = (nexta2 - a2) * unit->mRate->mFilterSlope; //CALCSLOPE(nexta2, a2); 
	    b1slope = (nextb1 - b1) * unit->mRate->mFilterSlope; //CALCSLOPE(nextb1, b1); 
	    b2slope = (nextb2 - b2) * unit->mRate->mFilterSlope; //CALCSLOPE(nextb2, b2); 

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
	    unit->m_freq = nextfreq; 
	    unit->m_rq = nextrq;
	    unit->m_a0 = a0;
	    unit->m_a1 = a1;
	    unit->m_a2 = a2;
	    unit->m_b1 = b1;
	    unit->m_b2 = b2;
	    
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
	
	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);

}

/* BBandPass */

struct BBandPass : public Unit
{
	float m_y1, m_y2, m_a0, m_a1, m_a2, m_b1, m_b2;
	float m_freq, m_bw;
};

extern "C"
{
	void BBandPass_next_kk(BBandPass *unit, int inNumSamples);
	void BBandPass_next_aa(BBandPass *unit, int inNumSamples);
	void BBandPass_Ctor(BBandPass* unit);
}

void BBandPass_Ctor(BBandPass* unit)
{	
	if ((INRATE(1) == calc_FullRate) && (INRATE(2) == calc_FullRate)) {
		SETCALC(BBandPass_next_aa);
		} else {
		SETCALC(BBandPass_next_kk);}
	float freq = unit->m_freq = ZIN0(1);
	float bw = unit->m_bw = ZIN0(2);
	
	float w0 = twopi * freq * SAMPLEDUR;
	float alpha = sin(w0) * (sinh((0.34657359027997 * bw * w0) / sin(w0)));
	float b0rz = 1. / (1. + alpha); 
	float a0 = unit->m_a0 = alpha * b0rz; 
	unit->m_a1 = 0.0f;
	unit->m_a2 = -a0; 
	unit->m_b1 = cos(w0) * 2. * b0rz; 
	unit->m_b2 = (1. - alpha) * -b0rz; 

	unit->m_y1 = 0.;

	unit->m_y2 = 0.;
	ClearUnitOutputs(unit, 1);
}
		
void BBandPass_next_aa(BBandPass *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float *freq = ZIN(1);
	float *bw = ZIN(2);

	float a0, a1, a2, b1, b2, w0, alpha, b0rz;
	float y0, y1, y2;
	
	y1 = unit->m_y1;
	y2 = unit->m_y2;

	a0 = unit->m_a0;
	a1 = unit->m_a1;
	a2 = unit->m_a2;
	b1 = unit->m_b1;
	b2 = unit->m_b2;

		
	LOOP(unit->mRate->mFilterLoops,
		if ((unit->m_freq != ZXP(freq)) || (unit->m_bw != ZXP(bw))) {
		    w0 = twopi * ZXP(freq) * SAMPLEDUR;
		    alpha = sin(w0) * (sinh((0.34657359027997 * ZXP(bw) * w0) / sin(w0)));
		    b0rz = 1. / (1. + alpha); 
		    a0 = alpha * b0rz; 
		    a1 = 0.0f;
		    a2 = -a0; 
		    b1 = cos(w0) * 2. * b0rz; 
		    b2 = (1. - alpha) * -b0rz; 
		    unit->m_freq = ZXP(freq); 
		    unit->m_bw = ZXP(bw);
		}
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
		unit->m_freq = ZXP(freq); 
		unit->m_bw = ZXP(bw);
	);
	
	unit->m_a0 = a0;
	unit->m_a1 = a1;
	unit->m_a2 = a2;
	unit->m_b1 = b1;
	unit->m_b2 = b2;	
	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}


void BBandPass_next_kk(BBandPass *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float nextfreq = ZIN0(1);
	float nextbw = ZIN0(2);

	float a0, a1, a2, b1, b2;
	float nexta0, nexta1, nexta2, nextb1, nextb2, nextw0, nextalpha, nextb0rz;
	float y0;
	float y1 = unit->m_y1;
	float y2 = unit->m_y2;
	float a0slope, a1slope, a2slope, b1slope, b2slope;

	a0 = unit->m_a0;
	a1 = unit->m_a1;
	a2 = unit->m_a2;
	b1 = unit->m_b1;
	b2 = unit->m_b2;	
	if ((unit->m_freq != nextfreq) || (unit->m_bw != nextbw)){
	    nextw0 = twopi * nextfreq * SAMPLEDUR; 
	    nextalpha = sin(nextw0) * (sinh((0.34657359027997 * nextbw * nextw0) / sin(nextw0))); 
	    nextb0rz = 1. / (1. + nextalpha); 
	    nexta0 = nextalpha * nextb0rz; 
	    nexta1 = 0.; 
	    nexta2 = -nexta0; 
	    nextb1 = cos(nextw0) * 2. * nextb0rz; 
	    nextb2 = ((1. - nextalpha) * -nextb0rz); 
	    a0slope = (nexta0 - a0) * unit->mRate->mFilterSlope; //CALCSLOPE(nexta0, a0); 
	    a1slope = (nexta1 - a1) * unit->mRate->mFilterSlope; //CALCSLOPE(nexta1, a1); 
	    a2slope = (nexta2 - a2) * unit->mRate->mFilterSlope; //CALCSLOPE(nexta2, a2); 
	    b1slope = (nextb1 - b1) * unit->mRate->mFilterSlope; //CALCSLOPE(nextb1, b1); 
	    b2slope = (nextb2 - b2) * unit->mRate->mFilterSlope; //CALCSLOPE(nextb2, b2); 
	    unit->m_freq = nextfreq; 
	    unit->m_bw = nextbw;
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

/* BBandStop */
struct BBandStop : public Unit
{
	float m_y1, m_y2, m_a0, m_a1, m_a2, m_b1, m_b2;
	float m_freq, m_bw;
};

extern "C"
{
	void BBandStop_next_kk(BBandStop *unit, int inNumSamples);
	void BBandStop_next_aa(BBandStop *unit, int inNumSamples);
	void BBandStop_Ctor(BBandStop* unit);
}

void BBandStop_Ctor(BBandStop* unit)
{	
	if ((INRATE(1) == calc_FullRate) && (INRATE(2) == calc_FullRate)) {
		SETCALC(BBandStop_next_aa);
		} else {
		SETCALC(BBandStop_next_kk);
	    }
	float freq = unit->m_freq = ZIN0(1);
	float bw = unit->m_bw = ZIN0(2);
	
	float w0 = twopi * freq * SAMPLEDUR;
	float alpha = sin(w0) * (sinh((0.34657359027997 * bw * w0) / sin(w0)));
	float b0rz = 1. / (1. + alpha);
	float b1 = unit->m_b1 = 2. * b0rz * cos(w0); 
	unit->m_a0 = b0rz; 
	unit->m_a1 = -b1;
	unit->m_a2 = b0rz; 
	unit->m_b2 = (1. - alpha) * -b0rz; 

	unit->m_y1 = 0.;

	unit->m_y2 = 0.;
	ClearUnitOutputs(unit, 1);
}
		
void BBandStop_next_aa(BBandStop *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float *freq = ZIN(1);
	float *bw = ZIN(2);

	float a0, a1, a2, b1, b2, w0, alpha, b0rz;
	float y0, y1, y2;
	
	y1 = unit->m_y1;
	y2 = unit->m_y2;

	a0 = unit->m_a0;
	a1 = unit->m_a1;
	a2 = unit->m_a2;
	b1 = unit->m_b1;
	b2 = unit->m_b2;

		
	LOOP(unit->mRate->mFilterLoops,
		if ((unit->m_freq != ZXP(freq)) || (unit->m_bw != ZXP(bw))) {
		    w0 = twopi * ZXP(freq) * SAMPLEDUR;
		    alpha = sin(w0) * (sinh((0.34657359027997 * ZXP(bw) * w0) / sin(w0)));
		    b0rz = 1. / (1. + alpha);
		    b1 = 2. * b0rz * cos(w0); 
		    a0 = b0rz; 
		    a1 = -b1;
		    a2 = b0rz; 
		    b2 = (1. - alpha) * -b0rz; 
		    unit->m_freq = ZXP(freq); 
		    unit->m_bw = ZXP(bw);
		}
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
		unit->m_freq = ZXP(freq); 
		unit->m_bw = ZXP(bw);		
	);
	
	unit->m_a0 = a0;
	unit->m_a1 = a1;
	unit->m_a2 = a2;
	unit->m_b1 = b1;
	unit->m_b2 = b2;	
	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}


void BBandStop_next_kk(BBandStop *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float nextfreq = ZIN0(1);
	float nextbw = ZIN0(2);

	float a0, a1, a2, b1, b2;
	float nexta0, nexta1, nexta2, nextb1, nextb2, nextw0, nextalpha, nextb0rz;
	float y0;
	float y1 = unit->m_y1;
	float y2 = unit->m_y2;
	float a0slope, a1slope, a2slope, b1slope, b2slope;

	a0 = unit->m_a0;
	a1 = unit->m_a1;
	a2 = unit->m_a2;
	b1 = unit->m_b1;
	b2 = unit->m_b2;

	if ((unit->m_freq != nextfreq) || (unit->m_bw != nextbw)){
	    nextw0 = twopi * nextfreq * SAMPLEDUR; 
	    nextalpha = sin(nextw0) * (sinh((0.34657359027997 * nextbw * nextw0) / sin(nextw0))); 
	    nextb0rz = 1. / (1. + nextalpha); 
	    nextb1 = cos(nextw0) * 2. * nextb0rz; 
	    nexta0 = nextb0rz; 
	    nexta1 = -nextb1; 
	    nexta2 = nextb0rz; 
	    nextb2 = ((1. - nextalpha) * -nextb0rz); 
	    a0slope = (nexta0 - a0) * unit->mRate->mFilterSlope; //CALCSLOPE(nexta0, a0); 
	    a1slope = (nexta1 - a1) * unit->mRate->mFilterSlope; //CALCSLOPE(nexta1, a1); 
	    a2slope = (nexta2 - a2) * unit->mRate->mFilterSlope; //CALCSLOPE(nexta2, a2); 
	    b1slope = (nextb1 - b1) * unit->mRate->mFilterSlope; //CALCSLOPE(nextb1, b1); 
	    b2slope = (nextb2 - b2) * unit->mRate->mFilterSlope; //CALCSLOPE(nextb2, b2); 
	    unit->m_freq = nextfreq; 
	    unit->m_bw = nextbw;
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
/* BPeakEQ */
struct BPeakEQ : public Unit
{
	float m_y1, m_y2, m_a0, m_a1, m_a2, m_b1, m_b2;
	float m_freq, m_rq, m_db;
};

extern "C"
{
	void BPeakEQ_next_kkk(BPeakEQ *unit, int inNumSamples);
	void BPeakEQ_next_aaa(BPeakEQ *unit, int inNumSamples);
	void BPeakEQ_Ctor(BPeakEQ* unit);
}

void BPeakEQ_Ctor(BPeakEQ* unit)
{	
	if ((INRATE(1) == calc_FullRate) && (INRATE(2) == calc_FullRate) && (INRATE(3) == calc_FullRate)) {
		    SETCALC(BPeakEQ_next_aaa);
		    } else {
		    SETCALC(BPeakEQ_next_kkk);
		    }
	
	float freq = unit->m_freq = ZIN0(1);
	float rq = unit->m_rq = ZIN0(2);
	float db = unit->m_db = ZIN0(3);
	float a = pow(10., db/40.);
	float w0 = twopi * freq * SAMPLEDUR;
	float alpha = sin(w0) * 0.5 * rq;
	float b0rz = 1. / (1. + (alpha / a));
	float b1 = unit->m_b1 = 2. * b0rz * cos(w0); 
	unit->m_a0 = (1. + (alpha * a)) * b0rz; 
	unit->m_a1 = -b1;
	unit->m_a2 = (1. - (alpha * a)) * b0rz; 
	unit->m_b2 = (1. - (alpha / a)) * -b0rz; 

	unit->m_y1 = 0.;
	unit->m_y2 = 0.;
	ClearUnitOutputs(unit, 1);
}

void BPeakEQ_next_aaa(BPeakEQ *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float *freq = ZIN(1);
	float *rq = ZIN(2);
	float *db = ZIN(3);

	float a0, a1, a2, b1, b2, a, w0, alpha, b0rz;
	float y0, y1, y2;
	
	y1 = unit->m_y1;
	y2 = unit->m_y2;

	a0 = unit->m_a0;
	a1 = unit->m_a1;
	a2 = unit->m_a2;
	b1 = unit->m_b1;
	b2 = unit->m_b2;

		
	LOOP(unit->mRate->mFilterLoops,
		if ((unit->m_freq != ZXP(freq)) || (unit->m_rq != ZXP(rq)) || (unit->m_db != ZXP(db))) {
		    a = pow(10., ZXP(db)/40.);
		    w0 = twopi * ZXP(freq) * SAMPLEDUR;
		    alpha = sin(w0) * 0.5 * ZXP(rq);
		    b0rz = 1. / (1. + (alpha / a));
		    b1 = 2. * b0rz * cos(w0); 
		    a0 = (1. + (alpha * a)) * b0rz; 
		    a1 = -b1;
		    a2 = (1. - (alpha * a)) * b0rz; 
		    b2 = (1. - (alpha / a)) * -b0rz; 
		    unit->m_freq = ZXP(freq);
		    unit->m_rq = ZXP(rq);
		    unit->m_db = ZXP(db);
		}
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
		unit->m_freq = ZXP(freq);
		unit->m_rq = ZXP(rq);
		unit->m_db = ZXP(db);
	);
	
	unit->m_a0 = a0;
	unit->m_a1 = a1;
	unit->m_a2 = a2;
	unit->m_b1 = b1;
	unit->m_b2 = b2;	
	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}


void BPeakEQ_next_kkk(BPeakEQ *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float nextfreq = ZIN0(1);
	float nextrq = ZIN0(2);
	float nextdb = ZIN0(3);

	float a0, a1, a2, b1, b2, a, w0, alpha, b0rz;
	float y0, y1, y2, a0slope, a1slope, a2slope, b1slope, b2slope;
	
	y1 = unit->m_y1;
	y2 = unit->m_y2;

	a0 = unit->m_a0;
	a1 = unit->m_a1;
	a2 = unit->m_a2;
	b1 = unit->m_b1;
	b2 = unit->m_b2;

	if ((unit->m_freq != nextfreq) || (unit->m_rq != nextrq) || (unit->m_db != nextdb)) {
	    a = pow(10., nextdb/40.);
	    w0 = twopi * nextfreq * SAMPLEDUR;
	    alpha = sin(w0) * 0.5 * nextrq;
	    b0rz = 1. / (1. + (alpha / a));
	    float nextb1 = 2. * b0rz * cos(w0); 
	    float nexta0 = (1. + (alpha * a)) * b0rz; 
	    float nexta1 = -nextb1;
	    float nexta2 = (1. - (alpha * a)) * b0rz; 
	    float nextb2 = (1. - (alpha / a)) * -b0rz;
	    a0slope = (nexta0 - a0) * unit->mRate->mFilterSlope; //CALCSLOPE(nexta0, a0); 
	    a1slope = (nexta1 - a1) * unit->mRate->mFilterSlope; //CALCSLOPE(nexta1, a1); 
	    a2slope = (nexta2 - a2) * unit->mRate->mFilterSlope; //CALCSLOPE(nexta2, a2); 
	    b1slope = (nextb1 - b1) * unit->mRate->mFilterSlope; //CALCSLOPE(nextb1, b1); 
	    b2slope = (nextb2 - b2) * unit->mRate->mFilterSlope; //CALCSLOPE(nextb2, b2);
	    unit->m_freq = nextfreq;
	    unit->m_db = nextdb;
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

/* BAllPass */
struct BAllPass : public Unit
{
	float m_y1, m_y2, m_a0, m_a1, m_a2, m_b1, m_b2;
	float m_freq, m_rq;
};

extern "C"
{
	void BAllPass_next_kk(BAllPass *unit, int inNumSamples);
	void BAllPass_next_aa(BAllPass *unit, int inNumSamples);
	void BAllPass_Ctor(BAllPass* unit);
}

void BAllPass_Ctor(BAllPass* unit)
{	
	if ((INRATE(1) == calc_FullRate) && (INRATE(2) == calc_FullRate)) {
		SETCALC(BAllPass_next_aa); 
		} else {
		SETCALC(BAllPass_next_kk);
		}
		
	float freq = unit->m_freq = ZIN0(1);
	float rq = unit->m_rq = ZIN0(2);

	float w0 = twopi * freq * SAMPLEDUR;
	float alpha = sin(w0) * 0.5 * rq;
	float b0rz = 1. / (1. + alpha);
	float a0 = unit->m_a0 = (1. - alpha) * b0rz;
	float b1 = unit->m_b1 = 2. * b0rz * cos(w0);  
	unit->m_a1 = -b1;
	unit->m_a2 = 1.; 
	unit->m_b2 = -a0; 

	unit->m_y1 = 0.;

	unit->m_y2 = 0.;
	ClearUnitOutputs(unit, 1);
}
		
void BAllPass_next_aa(BAllPass *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float *freq = ZIN(1);
	float *rq = ZIN(2);

	float a0, a1, a2, b1, b2, w0, alpha, b0rz;
	float y0, y1, y2;
	
	y1 = unit->m_y1;
	y2 = unit->m_y2;

	a0 = unit->m_a0;
	a1 = unit->m_a1;
	a2 = unit->m_a2;
	b1 = unit->m_b1;
	b2 = unit->m_b2;
		
	LOOP(unit->mRate->mFilterLoops,
		if ((unit->m_freq != ZXP(freq)) || (unit->m_rq != ZXP(rq))) {
		    w0 = twopi * ZXP(freq) * SAMPLEDUR;
		    alpha = sin(w0) * ZXP(rq) * 0.5;
		    b0rz = 1. / (1. + alpha);
		    b1 = 2. * b0rz * cos(w0); 
		    a0 = (1. - alpha) * b0rz; 
		    a1 = -b1;
		    a2 = 1.; 
		    b2 = -a0; 
		    unit->m_freq = ZXP(freq); 
		    unit->m_rq = ZXP(rq);
		}
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
		unit->m_freq = ZXP(freq); 
		unit->m_rq = ZXP(rq);
	);
	
	unit->m_a0 = a0;
	unit->m_a1 = a1;
	unit->m_a2 = a2;
	unit->m_b1 = b1;
	unit->m_b2 = b2;	
	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}


void BAllPass_next_kk(BAllPass *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float nextfreq = ZIN0(1);
	float nextrq = ZIN0(2);

	float a0, a1, a2, b1, b2;
	float nexta0, nexta1, nexta2, nextb1, nextb2, nextw0, nextalpha, nextb0rz;
	float y0;
	float y1 = unit->m_y1;
	float y2 = unit->m_y2;
	float a0slope, a1slope, a2slope, b1slope, b2slope;

	a0 = unit->m_a0;
	a1 = unit->m_a1;
	a2 = unit->m_a2;
	b1 = unit->m_b1;
	b2 = unit->m_b2;
		    
	if ((unit->m_freq != nextfreq) || (unit->m_rq != nextrq)){
	    nextw0 = twopi * nextfreq * SAMPLEDUR; 
	    nextalpha = sin(nextw0) * 0.5 * nextrq; 
	    nextb0rz = 1. / (1. + nextalpha); 
	    nextb1 = cos(nextw0) * 2. * nextb0rz; 
	    nexta0 = (1. - nextalpha) * nextb0rz; 
	    nexta1 = -nextb1; 
	    nexta2 = 1.; 
	    nextb2 = -nexta0; 
	    a0slope = CALCSLOPE(nexta0, a0); 
	    a1slope = CALCSLOPE(nexta1, a1); 
	    a2slope = CALCSLOPE(nexta2, a2); 
	    b1slope = CALCSLOPE(nextb1, b1); 
	    b2slope = CALCSLOPE(nextb2, b2); 
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


/* BLowShelf */
struct BLowShelf : public Unit
{
	float m_y1, m_y2, m_a0, m_a1, m_a2, m_b1, m_b2;
	float m_freq, m_rs, m_db;
};

extern "C"
{
	void BLowShelf_next_kkk(BLowShelf *unit, int inNumSamples);
	void BLowShelf_next_aaa(BLowShelf *unit, int inNumSamples);
	void BLowShelf_Ctor(BLowShelf* unit);
}

void BLowShelf_Ctor(BLowShelf* unit)
{	
	if ((INRATE(1) == calc_FullRate) && (INRATE(2) == calc_FullRate) && (INRATE(3) == calc_FullRate)) {
		    SETCALC(BLowShelf_next_aaa);
		    } else {
		    SETCALC(BLowShelf_next_kkk);
		    }
	
	float freq = unit->m_freq = ZIN0(1);
	float rs = unit->m_rs = ZIN0(2);
	float db = unit->m_db = ZIN0(3);
	float a = pow(10., db/40.);
	float w0 = twopi * freq * SAMPLEDUR;
	float cosw0 = cos(w0);
	float sinw0 = sin(w0);
	float alpha = sinw0 * 0.5 * sqrt((a + (1/a)) * (rs - 1) + 2.);
	float i = (a + 1.) * cosw0;
	float j = (a - 1.) * cosw0;
	float k = 2. * sqrt(a) * alpha;
	float b0rz = 1. / ((a + 1.) + j + k);
	unit->m_a0 = a * (( a + 1.) - j + k) * b0rz;
	unit->m_a1 = 2. * a * ((a - 1.) - i) * b0rz;
	unit->m_a2 = a * ((a + 1.) - j - k) * b0rz;
	unit->m_b1 = 2. * ((a - 1.) + i) * b0rz;
	unit->m_b2 = ((a + 1.) + j - k) * -b0rz; 

	unit->m_y1 = 0.;

	unit->m_y2 = 0.;
	ClearUnitOutputs(unit, 1);
}

void BLowShelf_next_aaa(BLowShelf *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float *freq = ZIN(1);
	float *rs = ZIN(2);
	float *db = ZIN(3);

	float a0, a1, a2, b1, b2, a, w0, cosw0, sinw0, alpha, i, j, k, b0rz;
	float y0, y1, y2;
	
	y1 = unit->m_y1;
	y2 = unit->m_y2;

	a0 = unit->m_a0;
	a1 = unit->m_a1;
	a2 = unit->m_a2;
	b1 = unit->m_b1;
	b2 = unit->m_b2;
	
	LOOP(unit->mRate->mFilterLoops,
		if ((unit->m_freq != ZXP(freq)) || (unit->m_rs != ZXP(rs)) || (unit->m_db != ZXP(db))) {
		    a = pow(10., ZXP(db)/40.);
		    w0 = twopi * ZXP(freq) * SAMPLEDUR;
		    sinw0 = sin(w0);
		    cosw0 = cos(w0);
		    alpha = sinw0 * 0.5 * sqrt((a + (1/a)) * (ZXP(rs) - 1) + 2.);
		    i = (a + 1.) * cosw0;
		    j = (a - 1.) * cosw0;
		    k = 2. * sqrt(a) * alpha;
		    b0rz = 1. / ((a + 1.) + j + k);		    
		    a0 = a * (( a + 1.) - j + k) * b0rz;
		    a1 = 2. * a * ((a - 1.) - i) * b0rz;
		    a2 = a * ((a + 1.) - j - k) * b0rz;
		    b1 = 2. * ((a - 1.) + i) * b0rz;
		    b2 = ((a + 1.) + j - k) * -b0rz; 
		    unit->m_freq = ZXP(freq);
		    unit->m_rs = ZXP(rs);
		    unit->m_db = ZXP(db);
		}
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
		unit->m_freq = ZXP(freq);
		unit->m_rs = ZXP(rs);
		unit->m_db = ZXP(db);
	);
	
	unit->m_a0 = a0;
	unit->m_a1 = a1;
	unit->m_a2 = a2;
	unit->m_b1 = b1;
	unit->m_b2 = b2;	
	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}

void BLowShelf_next_kkk(BLowShelf *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float nextfreq = ZIN0(1);
	float nextrs = ZIN0(2);
	float nextdb = ZIN0(3);

	float a0, a1, a2, b1, b2, a, w0, cosw0, sinw0, alpha, i, j, k, b0rz;
	float y0, y1, y2, a0slope, a1slope, a2slope, b1slope, b2slope;
	
	y1 = unit->m_y1;
	y2 = unit->m_y2;

	a0 = unit->m_a0;
	a1 = unit->m_a1;
	a2 = unit->m_a2;
	b1 = unit->m_b1;
	b2 = unit->m_b2;

	if ((unit->m_freq != nextfreq) || (unit->m_rs != nextrs) || (unit->m_db != nextdb)) {
	    a = pow(10., nextdb/40.);
	    w0 = twopi * nextfreq * SAMPLEDUR;
	    sinw0 = sin(w0);
	    cosw0 = cos(w0);
	    alpha = sinw0 * 0.5 * sqrt((a + (1/a)) * (nextrs - 1) + 2.);
	    i = (a + 1.) * cosw0;
	    j = (a - 1.) * cosw0;
	    k = 2. * sqrt(a) * alpha;
	    b0rz = 1. / ((a + 1.) + j + k);
	    float nexta0 = a * (( a + 1.) - j + k) * b0rz;
	    float nexta1 = 2. * a * ((a - 1.) - i) * b0rz;
	    float nexta2 = a * ((a + 1.) - j - k) * b0rz;
	    float nextb1 = 2. * ((a - 1.) + i) * b0rz; 
	    float nextb2 = ((a + 1.) + j - k) * -b0rz;
	    a0slope = (nexta0 - a0) * unit->mRate->mFilterSlope; //CALCSLOPE(nexta0, a0); 
	    a1slope = (nexta1 - a1) * unit->mRate->mFilterSlope; //CALCSLOPE(nexta1, a1); 
	    a2slope = (nexta2 - a2) * unit->mRate->mFilterSlope; //CALCSLOPE(nexta2, a2); 
	    b1slope = (nextb1 - b1) * unit->mRate->mFilterSlope; //CALCSLOPE(nextb1, b1); 
	    b2slope = (nextb2 - b2) * unit->mRate->mFilterSlope; //CALCSLOPE(nextb2, b2);
	    unit->m_freq = nextfreq;
	    unit->m_db = nextdb;
	    unit->m_rs = nextrs;
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

/* BHiShelf */
struct BHiShelf : public Unit
{
	float m_y1, m_y2, m_a0, m_a1, m_a2, m_b1, m_b2;
	float m_freq, m_rs, m_db;
};

extern "C"
{
	void BHiShelf_next_kkk(BHiShelf *unit, int inNumSamples);
	void BHiShelf_next_aaa(BHiShelf *unit, int inNumSamples);
	void BHiShelf_Ctor(BHiShelf* unit);
}

void BHiShelf_Ctor(BHiShelf* unit)
{	
	if ((INRATE(1) == calc_FullRate) && (INRATE(2) == calc_FullRate) && (INRATE(3) == calc_FullRate)) {
		    SETCALC(BHiShelf_next_aaa);
		    } else {
		    SETCALC(BHiShelf_next_kkk);
		    }
		    
	float freq = unit->m_freq = ZIN0(1);
	float rs = unit->m_rs = ZIN0(2);
	float db = unit->m_db = ZIN0(3);
	float a = pow(10., db/40.);
	float w0 = twopi * freq * SAMPLEDUR;
	float cosw0 = cos(w0);
	float sinw0 = sin(w0);
	float alpha = sinw0 * 0.5 * sqrt((a + (1/a)) * (rs - 1) + 2.);
	float i = (a + 1.) * cosw0;
	float j = (a - 1.) * cosw0;
	float k = 2. * sqrt(a) * alpha;

	float b0rz = 1. / ((a + 1.) - j + k);
	unit->m_a0 = a * (( a + 1.) + j + k) * b0rz;
	unit->m_a1 = -2. * a * ((a - 1.) + i) * b0rz;
	unit->m_a2 = a * ((a + 1.) + j - k) * b0rz;
	unit->m_b1 = -2. * ((a - 1.) - i) * b0rz;
	unit->m_b2 = ((a + 1.) - j - k) * -b0rz; 

	unit->m_y1 = 0.;

	unit->m_y2 = 0.;
	ClearUnitOutputs(unit, 1);
}

void BHiShelf_next_aaa(BHiShelf *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float *freq = ZIN(1);
	float *rs = ZIN(2);
	float *db = ZIN(3);

	float a0, a1, a2, b1, b2, a, w0, cosw0, sinw0, alpha, i, j, k, b0rz;
	float y0, y1, y2;
	
	y1 = unit->m_y1;
	y2 = unit->m_y2;

	a0 = unit->m_a0;
	a1 = unit->m_a1;
	a2 = unit->m_a2;
	b1 = unit->m_b1;
	b2 = unit->m_b2;
		
	LOOP(unit->mRate->mFilterLoops,
		if ((unit->m_freq != ZXP(freq)) || (unit->m_rs != ZXP(rs)) || (unit->m_db != ZXP(db))) {
		    a = pow(10., ZXP(db)/40.);
		    w0 = twopi * ZXP(freq) * SAMPLEDUR;
		    sinw0 = sin(w0);
		    cosw0 = cos(w0);
		    alpha = sinw0 * 0.5 * sqrt((a + (1/a)) * (ZXP(rs) - 1) + 2.);
		    i = (a + 1.) * cosw0;
		    j = (a - 1.) * cosw0;
		    k = 2. * sqrt(a) * alpha;
		    b0rz = 1. / ((a + 1.) - j + k);		    
		    a0 = a * (( a + 1.) + j + k) * b0rz;
		    a1 = -2. * a * ((a - 1.) + i) * b0rz;
		    a2 = a * ((a + 1.) + j - k) * b0rz;
		    b1 = -2. * ((a - 1.) - i) * b0rz;
		    b2 = ((a + 1.) - j - k) * -b0rz; 
		    unit->m_freq = ZXP(freq);
		    unit->m_rs = ZXP(rs);
		    unit->m_db = ZXP(db);
		}
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
		unit->m_freq = ZXP(freq);
		unit->m_rs = ZXP(rs);
		unit->m_db = ZXP(db);
	);
	
	unit->m_a0 = a0;
	unit->m_a1 = a1;
	unit->m_a2 = a2;
	unit->m_b1 = b1;
	unit->m_b2 = b2;	
	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}

void BHiShelf_next_kkk(BHiShelf *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float nextfreq = ZIN0(1);
	float nextrs = ZIN0(2);
	float nextdb = ZIN0(3);

	float a0, a1, a2, b1, b2, a, w0, cosw0, sinw0, alpha, i, j, k, b0rz;
	float y0, y1, y2, a0slope, a1slope, a2slope, b1slope, b2slope;
	
	y1 = unit->m_y1;
	y2 = unit->m_y2;

	a0 = unit->m_a0;
	a1 = unit->m_a1;
	a2 = unit->m_a2;
	b1 = unit->m_b1;
	b2 = unit->m_b2;

	if ((unit->m_freq != nextfreq) || (unit->m_rs != nextrs) || (unit->m_db != nextdb)) {
	    a = pow(10., nextdb/40.);
	    w0 = twopi * nextfreq * SAMPLEDUR;
	    sinw0 = sin(w0);
	    cosw0 = cos(w0);
	    alpha = sinw0 * 0.5 * sqrt((a + (1/a)) * (nextrs - 1) + 2.);
	    i = (a + 1.) * cosw0;
	    j = (a - 1.) * cosw0;
	    k = 2. * sqrt(a) * alpha;
	    b0rz = 1. / ((a + 1.) - j + k);		    
	    float nexta0 = a * (( a + 1.) + j + k) * b0rz;
	    float nexta1 = -2. * a * ((a - 1.) + i) * b0rz;
	    float nexta2 = a * ((a + 1.) + j - k) * b0rz;
	    float nextb1 = -2. * ((a - 1.) - i) * b0rz;
	    float nextb2 = ((a + 1.) - j - k) * -b0rz; 
	    a0slope = (nexta0 - a0) * unit->mRate->mFilterSlope; //CALCSLOPE(nexta0, a0); 
	    a1slope = (nexta1 - a1) * unit->mRate->mFilterSlope; //CALCSLOPE(nexta1, a1); 
	    a2slope = (nexta2 - a2) * unit->mRate->mFilterSlope; //CALCSLOPE(nexta2, a2); 
	    b1slope = (nextb1 - b1) * unit->mRate->mFilterSlope; //CALCSLOPE(nextb1, b1); 
	    b2slope = (nextb2 - b2) * unit->mRate->mFilterSlope; //CALCSLOPE(nextb2, b2); 
	    unit->m_freq = nextfreq;
	    unit->m_db = nextdb;
	    unit->m_rs = nextrs;
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

////////////////////////////////////////////////////////////////////////////////////////////////////////

void load(InterfaceTable *inTable)
{
	ft = inTable;
	
	/* BEQSuite UGens */
	DefineSimpleUnit(BLowPass);
	DefineSimpleUnit(BHiPass);
	DefineSimpleUnit(BBandPass);
	DefineSimpleUnit(BBandStop);
	DefineSimpleUnit(BPeakEQ);
	DefineSimpleUnit(BAllPass);
	DefineSimpleUnit(BLowShelf);
	DefineSimpleUnit(BHiShelf);

}

//////////////////////////////////////////////////////////////////////////////////////////////////


