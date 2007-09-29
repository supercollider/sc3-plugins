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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "SC_PlugIn.h"

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
void load(InterfaceTable *inTable);


void DbufTag_Ctor(DbufTag *unit);
void DbufTag_Dtor(DbufTag *unit);
void DbufTag_next(DbufTag *unit, int inNumSamples);

void Dtag_Ctor(Dtag *unit);
void Dtag_Dtor(Dtag *unit);
void Dtag_next(Dtag *unit, int inNumSamples);

};

/////////////////////////////////////////////////////////////


#define GET_BUF \
	float fbufnum  = ZIN0(0); \
	if (fbufnum != unit->m_fbufnum) { \
		uint32 bufnum = (int)fbufnum; \
		World *world = unit->mWorld; \
		if (bufnum >= world->mNumSndBufs) bufnum = 0; \
		unit->m_fbufnum = fbufnum; \
		unit->m_buf = world->mSndBufs + bufnum; \
	} \
	SndBuf *buf = unit->m_buf; \
	float *bufData __attribute__((__unused__)) = buf->data; \
	uint32 bufChannels __attribute__((__unused__)) = buf->channels; \
	uint32 bufSamples __attribute__((__unused__)) = buf->samples; \
	uint32 bufFrames = buf->frames; \
	int mask __attribute__((__unused__)) = buf->mask; \
	int guardFrame __attribute__((__unused__)) = bufFrames - 2; 

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
			bufData[i] = (float) DEMANDINPUT(dtag_argoffset + i);
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
			//printf("new write: %ld, read: %ld\n", unit->m_write_pos, unit->m_read_pos);
		}
}


void DbufTag_end(DbufTag *unit, int which_case, int inNumSamples) {
	
	int recycle = (int) DEMANDINPUT(dtag_recycle);
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
			printf("new axiom (index %ld..%ld): ", unit->m_read_pos, unit->m_write_pos);
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
	
	//printf("writepos: %d readpos: %ld rule_length: %ld\n", unit->m_write_pos, unit->m_read_pos, rule_length);

	if (!inNumSamples) {
		DbufTag_end(unit, 0, inNumSamples);
		return;
	}
	
	int v = (int) DEMANDINPUT(dtag_deletion_number);
	
	if(ruleIndex >= unit-> m_numRules || (ruleIndex < 0)) {
		// printf("no rule found for value %ld\n", ruleIndex);
		OUT0(0) = NAN;  // no rule found
		return;
	} else {
	
		OUT0(0) = value;
		
		cur_rule_pos = unit->m_rule_offsets[ruleIndex];
		rule_length = unit->m_rule_lengths[ruleIndex];
		
		for(int i=0; i < rule_length; i++) {
			// copy production rule.
			bufData[write_pos] = DEMANDINPUT(cur_rule_pos + i);
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
		// printf("m_rule_offsets[%ld]: %ld\n", i, unit->m_rule_offsets[i]);
	}
}


void Dtag_reset(Dtag *unit, int recycle)
{
	
	RESETINPUT(dtag_deletion_number); // reset deletion number
	// other than in DbufTag,  m_axiom_size doesn't have to be checked again here, since buffer is fix.
	if(recycle == 0) {
		// recycle = 0 - write axiom to buffer again.
		unit->m_read_pos = 0;
		unit->m_write_pos = (int32) unit->m_axiom_size;
		
		// write axiom to tape
		for(int i = 0; i < unit->m_axiom_size; i++) {
			unit->m_tape[i] = DEMANDINPUT(dtag_argoffset + i);
		//	printf("axiom[%ld] = %ld\n", i, unit->m_tape[i]);
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


void Dtag_end(Dtag *unit, int which_case) {
	
	int recycle = (int) DEMANDINPUT(dtag_recycle);
	int mode = (int) IN0(dtag_mode);
	
	if(which_case == 0) {
		Dtag_reset(unit, recycle);
		if(mode == 4) { 
			printf("tag system was reset.\n");
			if(recycle) { printf("recycling. axiom length: %ld\n", recycle); }
		}
		return;
	}
	
	if((mode == 0) || (mode == which_case)) {
		if(recycle) {
			Dtag_reset(unit, recycle);
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
			printf("recycling. axiom length: %ld\n", recycle);
			
			Dtag_reset(unit, recycle);
			
			printf("new axiom (index %ld..%ld): ", unit->m_read_pos, unit->m_write_pos);
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
	Dtag_reset(unit, 0);
	
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
		Dtag_end(unit, 0);
		return;
	}
	
	int v = (int) DEMANDINPUT(dtag_deletion_number);
	
	//printf("ruleIndex: %d rulesize: %ld\n", ruleIndex, unit->m_numRules);
	if(ruleIndex >= unit->m_numRules || (ruleIndex < 0)) {
		// printf("no rule found for value %d rulesize: %ld\n", ruleIndex, unit->m_numRules);
		OUT0(0) = NAN;  // no rule found
		return;
	} else {
		OUT0(0) = tapeVal;
		cur_rule_pos = unit->m_rule_offsets[ruleIndex];
		rule_length = unit->m_rule_lengths[ruleIndex];
		
		for(int i=0; i < rule_length; i++) {
			tape[write_pos] = DEMANDINPUT(cur_rule_pos + i);
			write_pos += 1;
			if(write_pos == read_pos) {
				Dtag_end(unit, 1);
				return;
			}
			if(write_pos == tape_size) {
				write_pos = 0;
			}
		}
		for(int j=0; j < v; j++) {
			read_pos += 1;
			if(write_pos == read_pos) {
				Dtag_end(unit, 2);
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

//////////////////////////////////////////////////////



void load(InterfaceTable *inTable)
{
	ft = inTable;
	DefineDtorUnit(DbufTag);
	DefineDtorUnit(Dtag);
	
}
