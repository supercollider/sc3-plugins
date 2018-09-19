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


/*
 
 UGens to aid in capturing audio and soundfield "quantities":
 
 MovingSum
 
 Authors:
 Michael McCrea, 2018, mtm5@uw.edu
 
*/

#include "SC_PlugIn.h"

static InterfaceTable *ft;


// MovingSum uses the underlying strategy of RunningSum
// but adds a variable window size and a flag to average the output.
struct MovingSum : public Unit {
    int nsamps, maxSamps, head, tail, resetCounter;
    float msum, msum2;
    bool reset;
    float* msquares;
};

extern "C"
{
    void MovingSum_Ctor(MovingSum *unit);
    void MovingSum_Dtor(MovingSum *unit);
    void MovingSum_next(MovingSum *unit, int inNumSamples);
}

void MovingSum_Ctor( MovingSum* unit )
{
    if ((int) ZIN0(2) == 0) {
        printf("MovingSum Error: maxSamps initialized to 0.\n");
        SETCALC(*ClearUnitOutputs);
        unit->mDone = true;
        return;
    }

    SETCALC(MovingSum_next);

    unit->maxSamps  = (int) ZIN0(2);
    unit->nsamps    = sc_max(1, sc_min((int) ZIN0(1), unit->maxSamps)); // clip(1, maxSamps)
    unit->msum      = 0.0f;
    unit->msum2     = 0.0f;
    unit->resetCounter = 0;
    unit->head      = 0;    // first write position
    unit->tail      = unit->maxSamps - unit->nsamps;
    unit->reset     = false;
    unit->msquares  = (float*)RTAlloc(unit->mWorld, unit->maxSamps * sizeof(float));

    if (unit->msquares == nullptr) {
        SETCALC(*ClearUnitOutputs);
        ClearUnitOutputs(unit, 1);
        if (unit->mWorld->mVerbosity > -2) {
            printf("Failed to allocate memory for MovingSum\n");
        }
        return;
    }

    // zero the summing buffer
    for (int i=0; i < unit->maxSamps; ++i)
        unit->msquares[i] = 0.f;

    ZOUT0(0) = 0.f;
}

void MovingSum_Dtor(MovingSum *unit)
{
    RTFree(unit->mWorld, unit->msquares);
}

void MovingSum_next( MovingSum *unit, int inNumSamples )
{
    float *in   = ZIN(0);
    float *out  = ZOUT(0);
    float *data = unit->msquares;
	
	int newWinSize  = (int) ZIN0(1);  // number of samples to sum
    int prevWinSize = unit->nsamps;   // keep track of previous block's window size
	int curWinSize  = unit->nsamps;   // keep track of window size as it ramps to new size
	int maxWinSize  = unit->maxSamps;
	
	int head = unit->head;            // current write index in the rolling buffer
    int tail = unit->tail;            // current tail  index in the rolling buffer
	
    float sum = unit->msum;
    float sum2 = unit->msum2;         // modeled after RunningSum - thanks to Ross Bencina
	
	int resetCounter = unit->resetCounter; // trigger sum<>sum2 swap
	bool winSizeChanged = false;
	float sampSlope = 0.0;
	bool avg = ZIN0(3) > 0.0;         // output average flag
	
    newWinSize = sc_max(1, sc_min(newWinSize, maxWinSize)); // clamp [1, maxWinSize]
	
	if (newWinSize != prevWinSize) {
		winSizeChanged = true;
		sampSlope = CALCSLOPE( (float)newWinSize, prevWinSize );
	}
	
	for (int i = 0; i < inNumSamples; ++i) {
		
		// handle change in summing window size
		if (winSizeChanged) {
			int steps = prevWinSize + (int)(sampSlope * (i+1)) - curWinSize;
			
			if (steps > 0) { // window grows
				for (int j = 0; j < steps; ++j) {
					tail--;
					if (tail < 0)
						tail += maxWinSize; // wrap
					
					sum += data[tail];
					curWinSize++;
					
					if (resetCounter == curWinSize) {
						sum = sum2;
						resetCounter = 0;
						sum2 = 0.0;
					}
				}
			}
			
			if (steps < 0) { // window shrinks
				for (int j = 0; j < abs(steps); ++j) {
					sum -= data[tail];
					tail++;
					if (tail == maxWinSize)
						tail = 0; // wrap
					
					curWinSize--;
					if (resetCounter == curWinSize) {
						sum = sum2;
						resetCounter = 0;
						sum2 = 0.0;
					}
				}
			}
				
		}
		
		// remove last buffer sample
		sum -= data[tail];
		
		// add and store new input sample
		float next = ZXP(in);
		data[head]= next;
		sum += next;
		sum2 += next;
		
		// write out
		if (avg) {
			ZXP(out) = sum / curWinSize;
		} else {
			ZXP(out) = sum;
		}
		
		// increment and wrap the head and tail indices
		head++;
		if (head == maxWinSize)
			head = 0;
		
		tail++;
		if (tail == maxWinSize)
			tail = 0;
		
		resetCounter++;
		if (resetCounter == curWinSize) {
			sum = sum2;
			resetCounter = 0;
			sum2 = 0.0;
		}
		
		unit->nsamps = newWinSize;
		unit->resetCounter = resetCounter;
		unit->head  = head;
		unit->tail  = tail;
		unit->msum  = sum;
		unit->msum2 = sum2;
	}
}

PluginLoad(QuantityUGens)
{
	ft = inTable; // store pointer to InterfaceTable
	
    DefineDtorUnit(MovingSum);
}
