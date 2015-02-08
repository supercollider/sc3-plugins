/*
	SuperCollider real time audio synthesis system
 Copyright (c) 2002 James McCartney. All rights reserved.
	http://www.audiosynth.com
 
 
	Neuromodules
	by Frank Pasemann and Julian Rohrhuber
 
 
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
#include <stdio.h>

#ifndef MAXFLOAT
# include <float.h>
# define MAXFLOAT FLT_MAX
#endif

static InterfaceTable *ft;

#define MAXCHANNELS 32

struct DNeuromodule2 : public Unit
{
	double x1, x2;
};

extern "C"
{
	void DNeuromodule2_Ctor(DNeuromodule2 *unit);
	void DNeuromodule2_reset(DNeuromodule2 *unit, int inNumSamples);
	void DNeuromodule2_end(DNeuromodule2 *unit);
	void DNeuromodule2_next(DNeuromodule2 *unit, int inNumSamples);
}


enum {
	theta1 = 0, theta2, x1, x2, w11, w12, w21, w22 // these seem to be unreliable.
};


void DNeuromodule2_next(DNeuromodule2 *unit, int inNumSamples)
{
	double x1, x2;
	
	if(inNumSamples) {
		double x1_limit = std::tanh(unit->x1);
		double x2_limit = std::tanh(unit->x2);
		
		/*
		 double theta1 = (double) DEMANDINPUT_A(theta1, inNumSamples);
		 double theta2 = (double) DEMANDINPUT_A(theta1, inNumSamples);
		 
		 double w11 = (double) DEMANDINPUT_A(w11, inNumSamples);
		 double w12 = (double) DEMANDINPUT_A(w12, inNumSamples);
		 double w21 = (double) DEMANDINPUT_A(w21, inNumSamples);
		 double w22 = (double) DEMANDINPUT_A(w22, inNumSamples);
		 */
		
		double theta1 = (double) DEMANDINPUT_A(0, inNumSamples);
		if(sc_isnan(theta1)) { DNeuromodule2_end(unit); return; }
		double theta2 = (double) DEMANDINPUT_A(1, inNumSamples);
		if(sc_isnan(theta2)) { DNeuromodule2_end(unit); return; }
		
		double w11 = (double) DEMANDINPUT_A(4, inNumSamples);
		if(sc_isnan(w11)) { DNeuromodule2_end(unit); return; }
		double w12 = (double) DEMANDINPUT_A(5, inNumSamples);
		if(sc_isnan(w12)) { DNeuromodule2_end(unit); return; }
		double w21 = (double) DEMANDINPUT_A(6, inNumSamples);
		if(sc_isnan(w21)) { DNeuromodule2_end(unit); return; }
		double w22 = (double) DEMANDINPUT_A(7, inNumSamples);
		if(sc_isnan(w22)) { DNeuromodule2_end(unit); return; }
		
		
		x1 = theta1 + w11 * x1_limit + w12 * x2_limit;
		x2 = theta2 + w21 * x1_limit + w22 * x2_limit;
		
		//	printf("x1 = %f  x2 = %f\n", x1, x2);
		//	printf("x1 = %f  x2 = %f, theta1 = %f, theat2 = %f, w11 = %f, w12 = %f, w21 = %f, w22 = %f\n", x1, x2, theta1, theta2, w11, w12, w21, w22);
		
		x1 = zapgremlins(x1);
		x2 = zapgremlins(x2);
		
		
		unit->x1 = x1;
		unit->x2 = x2;
		
		
		OUT0(0) = (float) x1;
		OUT0(1) = (float) x2;
		
	} else {
		
		DNeuromodule2_reset(unit, inNumSamples);
		
	}
	
	
}

void DNeuromodule2_reset(DNeuromodule2 *unit, int inNumSamples)
{
	float x1, x2;
	for(int i =	0; i < 8; i++) {
		RESETINPUT(i);
	}
	x1 = DEMANDINPUT_A(2, inNumSamples);
	x2 = DEMANDINPUT_A(3, inNumSamples);
	
	unit->x1 = (double) x1;
	unit->x2 = (double) x2;
	
	OUT0(0) = x1;
	OUT0(1) = x2;
	
	unit->x1 = (double) x1;
	unit->x2 = (double) x2;
	
}

void DNeuromodule2_end(DNeuromodule2 *unit)
{
	
	for(int i =	0; i < 2; i++) {
		OUT0(i) = NAN;
	}
	
}



void DNeuromodule2_Ctor(DNeuromodule2 *unit)
{
	
	SETCALC(DNeuromodule2_next);
	DNeuromodule2_reset(unit, 0);
}


//////////////////////////////////////////////////////



PluginLoad(TagSystem)
{
	ft = inTable;
	DefineSimpleUnit(DNeuromodule2);
	
}
