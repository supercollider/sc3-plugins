/*
	MCLD Poll-based units for SuperCollider.
    Copyright (c) 2010 Dan Stowell. All rights reserved.

    Incorporating some code from SuperCollider3 "Poll" UGen, (c) SC developers.

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
#include <algorithm>            /* for std::min and std::max */

static InterfaceTable *ft;

//////////////////////////////////////////////////////////////////////////////////////////////////

struct TextVU : public Unit
{
	int m_samplesRemain, m_intervalSamples;
	float m_trig;
	float m_lastTextVU, m_id;
	char *m_id_string;
	bool m_mayprint;

	size_t m_width;
	float *m_cutoffs;
	char *m_vustring;
	float m_maxinepoch;
	size_t m_maxever;
};

extern "C"
{
	void TextVU_Ctor(TextVU* unit);
	void TextVU_next_kk(TextVU *unit, int inNumSamples);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void TextVU_Ctor(TextVU* unit)
{
	SETCALC(TextVU_next_kk);

	unit->m_trig = IN0(0);
	unit->m_id = IN0(4); // number of chars in the id string
	unit->m_id_string = (char*)RTAlloc(unit->mWorld, ((int)unit->m_id + 1) * sizeof(char));
	Print("TextVU: string length %g\n", unit->m_id);
	for(int i = 0; i < (int)unit->m_id; i++){
		unit->m_id_string[i] = (char)IN0(5+i);
	};
	unit->m_id_string[(int)unit->m_id] = '\0';

	size_t width = (size_t)std::max(IN0(2), 2.f);
	unit->m_width = width;
	// Now we want to initialise the set of cutoffs, where the first is -60dB and last is 0dB.
	unit->m_cutoffs = (float*)RTAlloc(unit->mWorld, width * sizeof(float));
	unit->m_vustring  = (char*)RTAlloc(unit->mWorld, (width+1) * sizeof(char));
	unit->m_vustring[width] = 0;
	float cutstep = 60.f / (float)(width-1);
	float db = -60.f;
	for(size_t i=0; i<width; ++i){
		// calc cutoffs in amplitude from the db vals
		unit->m_cutoffs[i] = sc_dbamp(db);
		//Print("cutoff %i: %g\n", i, unit->m_cutoffs[i]);
		db += cutstep;
	}
	unit->m_maxinepoch = 0.f;
	unit->m_maxever = 0;

	unit->m_mayprint = unit->mWorld->mVerbosity >= 0;

	TextVU_next_kk(unit, 1);
}

void TextVU_Dtor(TextVU* unit)
{
	if(unit->m_id_string) RTFree(unit->mWorld, unit->m_id_string);
	if(unit->m_cutoffs)   RTFree(unit->mWorld, unit->m_cutoffs);
	if(unit->m_vustring)   RTFree(unit->mWorld, unit->m_vustring);
}

void TextVU_next_kk(TextVU *unit, int inNumSamples){
	float in = IN0(1);
	float trig = IN0(0);
	float maxinepoch = unit->m_maxinepoch;
	size_t maxever = unit->m_maxever;
	size_t width = unit->m_width;
	float* cutoffs = unit->m_cutoffs;
	char* vustring = unit->m_vustring;
	if(IN0(3)){
		maxinepoch = 0.f;
		maxever = 0;
	}
	maxinepoch = sc_max(maxinepoch, in);
	if((unit->m_trig <= 0.f) && (trig > 0.f)){
		if(unit->m_mayprint){

			// convert the input value to a meter position, using the precalced thresholds
			size_t index = 0;
			for(size_t i=0; i<width; ++i){
				if(maxinepoch < cutoffs[i]){
					if(maxever==i){
						vustring[i] = '|';
					}else{
						vustring[i] = '-';
					}
				}else{
					index = i; // this will get pushed up until it stops at the peak
					vustring[i] = 'X';
				}
			}

			// note the best ever
			if(index > maxever){
				maxever = index;
			}
			Print("%s: %s\n", unit->m_id_string, vustring);
			maxinepoch = 0.f;
		}
	}
	unit->m_trig = trig;
	unit->m_maxever = maxever;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

PluginLoad(MCLDPoll)
{
	ft = inTable;

	DefineDtorUnit(TextVU);
}
