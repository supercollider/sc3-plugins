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


struct Dneuromodule : public Unit
{
	int m_size;
	
	double *m_theta;
	double *m_x;
	double *m_weights;
	double *m_x_temp;
};

extern "C"
{
	void Dneuromodule_Ctor(Dneuromodule *unit);
	void Dneuromodule_Dtor(Dneuromodule *unit);
	void Dneuromodule_reset(Dneuromodule *unit, int inNumSamples);
	void Dneuromodule_end(Dneuromodule *unit);
	void Dneuromodule_next(Dneuromodule *unit, int inNumSamples);
	void Neuromodule_initInputs(Dneuromodule *unit, int size);
}


void Neuromodule_initInputs(Dneuromodule *unit, int size)
{
	int memsize = size * sizeof(double);
	int numWeights = size * size;
	
	
	unit->m_theta = (double*)RTAlloc(unit->mWorld, memsize);
	memset(unit->m_theta, 0, memsize);
	
	unit->m_x = (double*)RTAlloc(unit->mWorld, memsize);
	memset(unit->m_x, 0, memsize);
	
	unit->m_weights = (double*)RTAlloc(unit->mWorld, numWeights);
	memset(unit->m_weights, 0, numWeights);
	
	unit->m_x_temp = (double*)RTAlloc(unit->mWorld, memsize);
	memset(unit->m_x_temp, 0, memsize);

	
}

void Dneuromodule_next(Dneuromodule *unit, int inNumSamples)
{
	
	
	if(inNumSamples) {
		
		int size = unit->m_size;
		int numWeights = size * size;
		
		
		// printf("theta: ");
		
		// THETA
		for(int i=0; i < size; i++) {
			double val = (double) DEMANDINPUT_A(i + 1, inNumSamples);
			if(sc_isnan(val)) { Dneuromodule_end(unit); return; }
			//printf("%f ", val);
			unit->m_theta[i] = val;
		}
		
		// printf("weights: ");
		
		// WEIGHTS
		for(int i=0; i < numWeights; i++) {
			double val = (double) DEMANDINPUT_A(i + 1 + size + size, inNumSamples);
			if(sc_isnan(val)) { Dneuromodule_end(unit); return; }
		// 	printf("%f ", val);
			unit->m_weights[i] = val;
		}
		
		
		// keep normalized old values in a temporary array
		// so that we can overwrite the original directly
		
		for(int i=0; i < size; i++) {
			unit->m_x_temp[i] = std::tanh(unit->m_x[i]);
		}
		
		// iterate over all other values for each values and calcuate their contribution by weights.
		
		// printf("outputs: \n");
		for(int i=0; i < size; i++) {
			double xval;
			xval = unit->m_theta[i];
			for(int j=0; j < size; j++) {
				// printf("x = %f + %f * %f\n", xval, unit->m_weights[i * size + j], unit->m_x_temp[j]);
				xval += unit->m_weights[i * size + j] * unit->m_x_temp[j];
			}
			xval = zapgremlins(xval);
			unit->m_x[i] = xval;
			// printf("-> %f ", xval);
			OUT0(i) = (float) xval;
		}
		
		//printf("\n");

		
	} else {
		
		Dneuromodule_reset(unit, inNumSamples);
		
	}
	
	
}

void Dneuromodule_reset(Dneuromodule *unit, int inNumSamples)
{
	int size = unit->m_size;
	int size_end = size * size + 2 * size + 1;
	
	for(int i =	1; i < size_end; i++) {
		RESETINPUT(i);
	}
	// initialize x
	for(int i=0; i < size; i++) {
		double val = (double) IN(i + 1 + size)[inNumSamples];
		if(sc_isnan(val)) { Dneuromodule_end(unit); return; }
		// printf("%f ", val);
		unit->m_x[i] = val;
		OUT0(i) = val;
	}
}

void Dneuromodule_end(Dneuromodule *unit)
{
	
	for(int i =	0; i < unit->m_size; i++) {
		OUT0(i) = NAN;
	}
	
}


void Dneuromodule_Ctor(Dneuromodule *unit)
{
	
	SETCALC(Dneuromodule_next);
	unit->m_size = sc_max((int) IN0(0), 0);
	Neuromodule_initInputs(unit, unit->m_size);
	
	Dneuromodule_reset(unit, 0);
}

void Dneuromodule_Dtor(Dneuromodule *unit)
{
	RTFree(unit->mWorld, unit->m_theta);
	RTFree(unit->mWorld, unit->m_x);
	RTFree(unit->mWorld, unit->m_weights);
	RTFree(unit->mWorld, unit->m_x_temp);
}


//////////////////////////////////////////////////////



PluginLoad(TagSystem)
{
	ft = inTable;
	DefineDtorUnit(Dneuromodule);
	
}
