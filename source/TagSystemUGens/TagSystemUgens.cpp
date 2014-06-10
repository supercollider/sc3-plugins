/*
	SuperCollider real time audio synthesis system
    Copyright (c) 2002 James McCartney. All rights reserved.
	http://www.audiosynth.com


	Emil Post Tag Sytem UGens
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


struct DbufTag : public Unit
{
	float m_fbufnum;
	SndBuf *m_buf;
	int *m_rule_offsets;
	int *m_rule_lengths;
	int32 m_axiom_size; // all these int32 could be uint32, but sc_wrap would need support for this.
	int32 m_read_pos;
	int32 m_write_pos;
	int m_numRules;
};

struct Dtag : public Unit
{
	int *m_rule_offsets;
	int *m_rule_lengths;
	float *m_tape;
	int32 m_tape_size;
	int32 m_axiom_size;
	int32 m_read_pos;
	int32 m_write_pos;
	int m_numRules;
};


extern "C"
{
void DbufTag_Ctor(DbufTag *unit);
void DbufTag_Dtor(DbufTag *unit);
void DbufTag_next(DbufTag *unit, int inNumSamples);

void Dtag_Ctor(Dtag *unit);
void Dtag_Dtor(Dtag *unit);
void Dtag_next(Dtag *unit, int inNumSamples);

}

/////////////////////////////////////////////////////////////


#define CHECK_BUF \
	if (!bufData) { \
                unit->mDone = true; \
		ClearUnitOutputs(unit, inNumSamples); \
		return; \
	}


/////////////////////////////////////////////////////////////

enum {
	dtag_tape_param,
	dtag_deletion_number,
	dtag_recycle,
	dtag_mode,
	dtag_axiom_size,
	dtag_num_rules,
	dtag_argoffset
};



void DbufTag_initInputs(DbufTag *unit, int argOffset, int size)
{
	int memsize = size * sizeof(int);

	unit->m_rule_lengths = (int*)RTAlloc(unit->mWorld, memsize);
	memset(unit->m_rule_lengths, 0, memsize);

	unit->m_rule_offsets = (int*)RTAlloc(unit->mWorld, memsize);
	memset(unit->m_rule_offsets, 0, memsize);


	for(int i=0; i < size; i++) {
		unit->m_rule_lengths[i] = (int) IN0(i + argOffset); // drop first n args
	}

	// calculate positions
	int position = argOffset + size;
	for(int i=0; i < size; i++) {
		unit->m_rule_offsets[i] = position;
		position += unit->m_rule_lengths[i];
		// printf("m_rule_offsets[%d]: %d\n", i, unit->m_rule_offsets[i]);
	}
}


void DbufTag_reset(DbufTag *unit, int recycle, int inNumSamples)
{
	GET_BUF
	CHECK_BUF
	RESETINPUT(dtag_deletion_number); // reset deletion number

	// if axiom size exceeds buffer, omit part of the axiom.
	if(unit->m_axiom_size > (int) bufFrames) {
		 unit->m_axiom_size = (int) bufFrames;
	}

	if(recycle == 0) {
		// recycle = 0 - write axiom to buffer again.
		unit->m_read_pos = 0;
		unit->m_write_pos = (int32) unit->m_axiom_size;

		if(unit->m_write_pos >= bufFrames) {
				unit->m_write_pos = unit->m_write_pos % bufFrames;
		}
		// write axiom to tape
		for(int i = 0; i < unit->m_write_pos; i++) {
			bufData[i] = (float) DEMANDINPUT_A(dtag_argoffset + i, inNumSamples);
			// printf("axiom[%d] = %d\n", i, (int) bufData[i]);
		}
	} else if (recycle < 0) {
		// recycle < 0 - write_pos is left where it is, read_pos is offset by recycle value
			unit->m_read_pos = unit->m_write_pos + recycle;
			if(unit->m_read_pos < 0) {
				unit->m_read_pos = unit->m_read_pos % bufFrames;
			}

		} else {
			// recycle > 0 - read_pos is left where it is, write_pos is offset by recycle value
			unit->m_write_pos = unit->m_read_pos + recycle;
			if(unit->m_write_pos >= bufFrames) {
				unit->m_write_pos = unit->m_write_pos % bufFrames;
			}
			//printf("new write: %d, read: %d\n", unit->m_write_pos, unit->m_read_pos);
		}
}


void DbufTag_end(DbufTag *unit, int which_case, int inNumSamples) {

	int recycle = (int) DEMANDINPUT_A(dtag_recycle, inNumSamples);
	int mode = (int) IN0(dtag_mode);

	if(which_case == 0) {
		DbufTag_reset(unit, recycle, inNumSamples);
		if(mode == 4) {
			printf("tag system was reset externally.\n");
			if(recycle) { printf("recycling. axiom length: %d\n", recycle); }
		}
		return;
	}

	if((mode == 0) || (mode == which_case)) {
		if(recycle) {
			DbufTag_reset(unit, recycle, inNumSamples);
		} else {
			OUT0(0) = NAN;
		}
		return;
	}
	if(mode >= 4) {
		printf("tag system halt: ");
		if(which_case == 1) {
			printf("divergence too large (buffer filled up).\n");
		} else {
			printf("terminated (string empty)\n");
		}

		if(recycle) {

			printf("recycling. axiom length: %d\n", recycle);
			//printf("new axiom:\n"); // todo.
			DbufTag_reset(unit, recycle, inNumSamples);

			GET_BUF
			printf("new axiom (index %d..%d): ", unit->m_read_pos, unit->m_write_pos);
			int32 n = unit->m_write_pos - unit->m_read_pos;
			if(n < 0) { n = sc_wrap(n, 0, bufFrames - 1); }
			for(int32 i=0; i < n; i++) {
					int32 j = sc_wrap(unit->m_read_pos + i, 0, bufFrames - 1);
					printf("%d ", (int)bufData[j]);
			}
			printf("\n");

		} else {
			OUT0(0) = NAN;
		}
		return;
	}
	OUT0(0) = NAN;
}


void DbufTag_Ctor(DbufTag *unit)
{
	SETCALC(DbufTag_next);
	unit->m_fbufnum = -1e9f;
	unit->m_axiom_size = (int) IN0(dtag_axiom_size);
	unit->m_numRules = (int) IN0(dtag_num_rules);

	DbufTag_initInputs(unit, dtag_argoffset + unit->m_axiom_size, unit->m_numRules);
	DbufTag_reset(unit, 0, 1);

	OUT0(0) = 0.f;
}


void DbufTag_Dtor(DbufTag *unit)
{
	RTFree(unit->mWorld, unit->m_rule_lengths);
	RTFree(unit->mWorld, unit->m_rule_offsets);
}


void DbufTag_next(DbufTag *unit, int inNumSamples)
{

	GET_BUF
	CHECK_BUF


	int rule_length;
	int cur_rule_pos, next_rule_pos;
	int32 write_pos = unit->m_write_pos;
	int32 read_pos = unit->m_read_pos;

	float value = bufData[read_pos];
	int ruleIndex = (int) value;

	// verbose print mode
	if(!(IN0(dtag_mode) < 5.f)) {
		int max = bufFrames;
		if(max > 32) { max = 32; }
		for(int i=0; i<max; i++) {
			if(i == write_pos) { printf(">"); } else if (i == read_pos) { printf("|"); } else { printf(" "); }
			printf("%d", (int) bufData[i]);

		}
		printf("\n");
		printf("apply rule %d\n", ruleIndex);

	}

	//printf("writepos: %d readpos: %d rule_length: %d\n", unit->m_write_pos, unit->m_read_pos, rule_length);

	if (!inNumSamples) {
		DbufTag_end(unit, 0, inNumSamples);
		return;
	}

	int v = (int) DEMANDINPUT_A(dtag_deletion_number, inNumSamples);

	if(ruleIndex >= unit-> m_numRules || (ruleIndex < 0)) {
		// printf("no rule found for value %d\n", ruleIndex);
		OUT0(0) = NAN;  // no rule found
		return;
	} else {

		OUT0(0) = value;

		cur_rule_pos = unit->m_rule_offsets[ruleIndex];
		rule_length = unit->m_rule_lengths[ruleIndex];

		for(int i=0; i < rule_length; i++) {
			// copy production rule.
			bufData[write_pos] = DEMANDINPUT_A(cur_rule_pos + i, inNumSamples);
			write_pos += 1;
			if(write_pos == read_pos) {
				DbufTag_end(unit, 1, inNumSamples);
				return;
			}
			if(write_pos == bufFrames) {
				write_pos = 0;
			}
		}

		for(int j=0; j < v; j++) {
			read_pos += 1;
			if(write_pos == read_pos) {
				DbufTag_end(unit, 2, inNumSamples);
				return;
			}
			if(read_pos == bufFrames) {
				read_pos = 0;
			}
		}
		unit->m_write_pos = write_pos;
		unit->m_read_pos = read_pos;
	}

}


///////////////////////////////////////////////////////


void Dtag_initInputs(Dtag *unit, int argOffset, int size)
{

	unit->m_tape_size = (int32) IN0(dtag_tape_param);

	// make sure axiom isn't longer than the tape.
	if(unit->m_axiom_size > (int) unit->m_tape_size) {
		 unit->m_axiom_size = (int) unit->m_tape_size;
	}

	// allocate tape
	int32 memtapesize = (int32) unit->m_tape_size * sizeof(int);
	unit->m_tape = (float*)RTAlloc(unit->mWorld, memtapesize);
	memset(unit->m_tape, 0, memtapesize);


	// allocate offsets and lengths
	int memsize = size * sizeof(int);

	unit->m_rule_lengths = (int*)RTAlloc(unit->mWorld, memsize);
	memset(unit->m_rule_lengths, 0, memsize);

	unit->m_rule_offsets = (int*)RTAlloc(unit->mWorld, memsize);
	memset(unit->m_rule_offsets, 0, memsize);


	for(int i=0; i < size; i++) {
		unit->m_rule_lengths[i] = (int) IN0(i + argOffset); // drop first n args
	}

	// calculate positions
	int position = argOffset + size;
	for(int i=0; i < size; i++) {
		unit->m_rule_offsets[i] = position;
		position += unit->m_rule_lengths[i];
		// printf("m_rule_offsets[%d]: %d\n", i, unit->m_rule_offsets[i]);
	}
}


void Dtag_reset(Dtag *unit, int recycle, int inNumSamples)
{

	RESETINPUT(dtag_deletion_number); // reset deletion number
	// other than in DbufTag,  m_axiom_size doesn't have to be checked again here, since buffer is fix.
	if(recycle == 0) {
		// recycle = 0 - write axiom to buffer again.
		unit->m_read_pos = 0;
		unit->m_write_pos = (int32) unit->m_axiom_size;

		// write axiom to tape
		for(int i = 0; i < unit->m_axiom_size; i++) {
			unit->m_tape[i] = DEMANDINPUT_A(dtag_argoffset + i, inNumSamples);
		//	printf("axiom[%d] = %d\n", i, unit->m_tape[i]);
		}
	} else if (recycle < 0) {
			// recycle < 0 - write_pos is left where it is, read_pos is offset by recycle value
			if(unit->m_write_pos >= unit->m_tape_size) {
				unit->m_write_pos = unit->m_write_pos % unit->m_tape_size;
			}
			unit->m_read_pos = unit->m_write_pos + recycle;
			if(unit->m_read_pos < 0) {
				unit->m_read_pos = sc_wrap(unit->m_read_pos, 0, unit->m_tape_size - 1);
			}
		} else {
			// recycle > 0 - read_pos is left where it is, write_pos is offset by recycle value
			if(unit->m_read_pos >= unit->m_tape_size) {
				unit->m_read_pos = unit->m_read_pos % unit->m_tape_size;
			}
			unit->m_write_pos = unit->m_read_pos + recycle;
			if(unit->m_write_pos >= unit->m_tape_size) {
				unit->m_write_pos = unit->m_write_pos % unit->m_tape_size;
			}

		}
	}


void Dtag_end(Dtag *unit, int which_case, int inNumSamples) {

	int recycle = (int) DEMANDINPUT_A(dtag_recycle, inNumSamples);
	int mode = (int) IN0(dtag_mode);

	if(which_case == 0) {
		Dtag_reset(unit, recycle, inNumSamples);
		if(mode == 4) {
			printf("tag system was reset.\n");
			if(recycle) { printf("recycling. axiom length: %d\n", recycle); }
		}
		return;
	}

	if((mode == 0) || (mode == which_case)) {
		if(recycle) {
			Dtag_reset(unit, recycle, inNumSamples);
		} else {
			OUT0(0) = NAN;
		}
		return;
	}
	if(mode >= 4) {
		printf("tag system halt: ");
		if(which_case == 1) {
			printf("divergence too large (buffer filled up).\n");
		} else {
			printf("terminated (string empty)\n");
		}

		if(recycle) {
			printf("recycling. axiom length: %d\n", recycle);

			Dtag_reset(unit, recycle, inNumSamples);

			printf("new axiom (index %d..%d): ", unit->m_read_pos, unit->m_write_pos);
			int32 n = unit->m_write_pos - unit->m_read_pos;
			if(n < 0) { n = sc_wrap(n, 0, unit->m_tape_size - 1); }
			for(int32 i=0; i < n; i++) {
					int32 j = sc_wrap(unit->m_read_pos + i, 0, unit->m_tape_size - 1);
					printf("%d ", (int)unit->m_tape[j]);
			}
			printf("\n");

		} else {
			OUT0(0) = NAN;
		}
		return;
	}
	OUT0(0) = NAN;
}


void Dtag_Ctor(Dtag *unit)
{

	SETCALC(Dtag_next);

	unit->m_axiom_size = (int) IN0(dtag_axiom_size);
	unit->m_numRules = (int) IN0(dtag_num_rules);

	// initialise and reset
	Dtag_initInputs(unit, dtag_argoffset + unit->m_axiom_size, unit->m_numRules);
	Dtag_reset(unit, 0, 1);

	OUT0(0) = 0.f;
}


void Dtag_Dtor(Dtag *unit)
{
	RTFree(unit->mWorld, unit->m_rule_lengths);
	RTFree(unit->mWorld, unit->m_rule_offsets);
	RTFree(unit->mWorld, unit->m_tape);
}

void Dtag_next(Dtag *unit, int inNumSamples)
{

	int rule_length;
	int cur_rule_pos, next_rule_pos;

	int32 write_pos = unit->m_write_pos;
	int32 read_pos = unit->m_read_pos;
	int32 tape_size = unit->m_tape_size;
	float *tape = unit->m_tape;
	float tapeVal = tape[read_pos];
	int ruleIndex = (int) tapeVal;


		// verbose print mode
	if(!(IN0(dtag_mode) < 5.f)) {
		int max = (int) unit->m_tape_size;
		if(max > 32) { max = 32; }
		for(int i=0; i<max; i++) {
			if(i == write_pos) { printf(">"); } else if (i == read_pos) { printf("|"); } else { printf(" "); }
			printf("%d", (int) unit->m_tape[i]);

		}
		printf("\n");
		printf("apply rule %d\n", ruleIndex);
	}



	if (!inNumSamples) {
		Dtag_end(unit, 0, 1);
		return;
	}

	int v = (int) DEMANDINPUT_A(dtag_deletion_number, inNumSamples);

	//printf("ruleIndex: %d rulesize: %d\n", ruleIndex, unit->m_numRules);
	if(ruleIndex >= unit->m_numRules || (ruleIndex < 0)) {
		// printf("no rule found for value %d rulesize: %d\n", ruleIndex, unit->m_numRules);
		OUT0(0) = NAN;  // no rule found
		return;
	} else {
		OUT0(0) = tapeVal;
		cur_rule_pos = unit->m_rule_offsets[ruleIndex];
		rule_length = unit->m_rule_lengths[ruleIndex];

		for(int i=0; i < rule_length; i++) {
			tape[write_pos] = DEMANDINPUT_A(cur_rule_pos + i, inNumSamples);
			write_pos += 1;
			if(write_pos == read_pos) {
				Dtag_end(unit, 1, inNumSamples);
				return;
			}
			if(write_pos == tape_size) {
				write_pos = 0;
			}
		}
		for(int j=0; j < v; j++) {
			read_pos += 1;
			if(write_pos == read_pos) {
				Dtag_end(unit, 2, inNumSamples);
				return;
			}
			if(read_pos == tape_size) {
				read_pos = 0;
			}
		}
		unit->m_write_pos = write_pos;
		unit->m_read_pos = read_pos;
	}

}






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
	float m_count;
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
		outval = DEMANDINPUT_A(current_state_offset, inNumSamples); // this will have to switch behaviour, depending on slotRepeats
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
	unit->m_count = DEMANDINPUT_A(0, inNumSamples) - 1.f; // offset: first value is embedded below.

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
		outval = DEMANDINPUT_A(unit->m_current_state_offset, inNumSamples); // get first state, which is the packed termination state
		OUT0(0) = outval;
		// // printf("going to exit state (1): %d end\n", unit->m_current_state);
		// // printf("output: %f\n", outval);
		unit->m_end = 1;
		return;
	}

	// how many nextstates ?
	float size = (float) unit->m_nextstate_sizes[unit->m_current_state];

	// get random value and generate random offset (0..size)
	float rand = DEMANDINPUT_A(1, inNumSamples);
	choice = (int) sc_max(0.f, rand * size - 0.5f);

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
	outval = DEMANDINPUT_A(current_state_offset, inNumSamples);
	if(sc_isnan(outval)) {
			// // printf("(1) resetting input %d\n", current_state_offset);
			if(unit->m_end) {
				outval = NAN;
			} else {
				RESETINPUT(current_state_offset);
				outval = DEMANDINPUT_A(current_state_offset, inNumSamples);
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



PluginLoad(TagSystem)
{
	ft = inTable;
	DefineDtorUnit(DbufTag);
	DefineDtorUnit(Dtag);
	DefineDtorUnit(Dfsm);

}
