/*
	SuperCollider real time audio synthesis system
    Copyright (c) 2002 James McCartney. All rights reserved.
	http://www.audiosynth.com
	
	
	Symbolic Machines
	by Julian Rohrhuber


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

#ifndef MAXFLOAT
# include <float.h>
# define MAXFLOAT FLT_MAX
#endif

static InterfaceTable *ft;

#define MAXCHANNELS 32


struct Dfsm : public Unit
{
	//int m_repeats;
	int m_num_sizes;
	int m_num_states;
	int m_state_offset;
	int *m_nextstate_indices;
	int *m_nextstate_sizes;
	int m_current_state;
	int m_current_state_offset;
	int m_count;
	int m_end;
};


extern "C"
{
void load(InterfaceTable *inTable);


void Dfsm_Ctor(Dfsm *unit);
void Dfsm_Dtor(Dfsm *unit);
void Dfsm_next(Dfsm *unit, int inNumSamples);

};



//////////////////////////////////////////////////////////////////////////////////////////////////

void Dfsm_reset(Dfsm *unit) {
		unit->m_current_state = 0;
		unit->m_end = 0;
		unit->m_count = 0.f;
		for(int i = 0; i < unit->m_num_states; i++) {
			// // printf("reset input[%d]\n", unit->m_state_offset + i);
			RESETINPUT(unit->m_state_offset + i);
		}
}

void Dfsm_next(Dfsm *unit, int inNumSamples)
{
	int current_state_offset, state_offset;
	int choice;
	int index_index;
	int next_index;
	float outval;
	
	// // printf("\n\n\n ------- \n");
	
	////////////////////// reset /////////////////////////
	// current_state is the internal state, including exit / enty pair.
	// i.e. first rule is pair [exit, entrance]
	
	if (!inNumSamples) {
		Dfsm_reset(unit);
		// printf("resetting\n");
	}
	
	// get some member state
	state_offset = unit->m_state_offset;
	current_state_offset = unit->m_current_state_offset;
	
	////////////////////// embedding mode /////////////////////////
	if(unit->m_count > 0.f) {
		outval = DEMANDINPUT(current_state_offset); // this will have to switch behaviour, depending on slotRepeats
		if(sc_isnan(outval)) {
			
			if(unit->m_end) {
					// exit state last value. end.
					OUT0(0) = NAN;
					unit->m_end = 0;
					unit->m_count = 0.f;
					// // printf("output: NAN to end stream\n");
					return;
			} else {	
				
				// other state last value
				// // printf("(1) resetting input %d\n", current_state_offset);
				RESETINPUT(current_state_offset);
			};
			
		} else {
			// embed current value of current state
			// // printf("outval: %f\n", outval);
			OUT0(0) = outval;
			unit->m_count --;
			return;
		}
	}
	
	////////////////////// init count /////////////////////////
		
	// get new count
	unit->m_count = DEMANDINPUT(0) - 1.f; // offset: first value is embedded below.
		
	if(sc_isnan(unit->m_count)) {  // terminate and reset
					RESETINPUT(0);
					OUT0(0) = NAN;
					unit->m_end = 0;
					unit->m_count = 0.f;
					// // printf("output: NAN to end stream\n");
					return;
	};
		

	
	////////////////////// finding next state /////////////////////////
	
	
	
	if(unit->m_current_state >= unit->m_num_states) {
		unit->m_current_state_offset = state_offset;
		outval = DEMANDINPUT(unit->m_current_state_offset); // get first state, which is the packed termination state
		OUT0(0) = outval;
		// // printf("going to exit state (1): %d end\n", unit->m_current_state);
		// // printf("output: %f\n", outval);
		unit->m_end = 1;
		return;
	}
	
	// how many nextstates ?
	float size = (float) unit->m_nextstate_sizes[unit->m_current_state];
	
	// get random value and generate random offset (0..size)
	float rand = DEMANDINPUT(1);
	choice = (int) sc_max(0.f, rand * size - 0.5);
	
	// look up the nextstate index
	index_index = unit->m_nextstate_indices[unit->m_current_state] + choice; // we'll need to limit this /0..1/
	
	// get the next state index from the input, add one for offset: first rule is pair [exit, entrance]
	next_index = IN0(index_index) + 1;
	unit->m_current_state= next_index;
	
	current_state_offset = state_offset + next_index;
	
	// exit
	if(next_index >= unit->m_num_states) {
		current_state_offset = state_offset; // get first state, which is the packed termination state
		// // printf("going to exit state (2): %d end\n", next_index);
		unit->m_end = 1;
	} 
	
	
	// get first value
	outval = DEMANDINPUT(current_state_offset);
	if(sc_isnan(outval)) {
			// // printf("(1) resetting input %d\n", current_state_offset);
			if(unit->m_end) {
				outval = NAN;
			} else {
				RESETINPUT(current_state_offset);
				outval = DEMANDINPUT(current_state_offset);
			}
	}
	OUT0(0) = outval;
	
	// set member state
	unit->m_current_state_offset = current_state_offset;
	
	 // printf("indexindex: %d choice: %d, previndex: %d nextindex: %d outval index: %d\n", index_index, choice, unit->m_current_state, next_index, state_offset + next_index);
	// // printf("outval: %f\n", outval);
}



void Dfsm_Ctor(Dfsm *unit)
{
	SETCALC(Dfsm_next);
	
	int numStates = (int)IN0(2);
	unit->m_num_states = numStates;
	
	// keep state size information
	int memNumSize = numStates * sizeof(int);
	unit->m_nextstate_sizes = (int*)RTAlloc(unit->mWorld, memNumSize);
	memset(unit->m_nextstate_sizes, 0, memNumSize);
	
	for(int i = 0; i <  numStates; i++) {
		unit->m_nextstate_sizes[i] = (int) IN0(3 + i);
	}
	
	
	// keep state index information
	unit->m_state_offset = 3 + numStates;
	int nextOffset = unit->m_state_offset + numStates;
	
	int memNumIndices = numStates * sizeof(int);
	unit->m_nextstate_indices = (int*)RTAlloc(unit->mWorld, memNumIndices);
	memset(unit->m_nextstate_indices, 0, memNumIndices);
	
	for(int i = 0; i <  numStates; i++) {
		unit->m_nextstate_indices[i] = nextOffset;
		nextOffset = nextOffset + unit->m_nextstate_sizes[i];
	}
	
	// reset
	unit->m_current_state = 0;
	unit->m_end = 0;
	unit->m_count = 0.f;
	OUT0(0) = 0.f;
}


void Dfsm_Dtor(Dfsm *unit)
{	
	RTFree(unit->mWorld, unit->m_nextstate_indices);
	RTFree(unit->mWorld, unit->m_nextstate_sizes);
}


//////////////////////////////////////////////////////



void load(InterfaceTable *inTable)
{
	ft = inTable;
	DefineDtorUnit(Dfsm);
}
