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



struct DNeuromodule3 : public Unit
{
	
};


extern "C"
{

void DNeuromodule3_Ctor(DNeuromodule3 *unit);
void DNeuromodule3_next(DNeuromodule3 *unit, int inNumSamples);

}


void DNeuromodule3_reset(DNeuromodule3 *unit, int recycle, int inNumSamples)
{


}


void DNeuromodule3_end(DNeuromodule3 *unit, int which_case, int inNumSamples) {
	OUT0(0) = NAN;
}


void DNeuromodule3_Ctor(DNeuromodule3 *unit)
{

	SETCALC(DNeuromodule3_next);

	for(i = 0; i < 3; i++;) {
		OUT0(i) = (float) i; // test
	}
}


void DNeuromodule3_next(DNeuromodule3 *unit, int inNumSamples)
{

	
	//  DEMANDINPUT_A(dtag_deletion_number, inNumSamples);
	//		OUT0(0) = NAN;  // no rule found
	//		OUT0(0) = tapeVal;
	

}

//////////////////////////////////////////////////////



PluginLoad(TagSystem)
{
	ft = inTable;
	DefineSimpleUnit(DNeuromodule3);

}
