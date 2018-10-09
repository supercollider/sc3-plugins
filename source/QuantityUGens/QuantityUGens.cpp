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
#include <limits.h>

static InterfaceTable *ft;

// MovingSum/Average uses the underlying strategy of RunningSum
// but adds a variable window size.
struct MovingX : public Unit {
    int nsamps, maxSamps, head, tail, resetCounter;
    float msum, msum2;
    bool reset;
    float* msquares;
};

struct MovingSum : public MovingX
{};

struct MovingAverage : public MovingX
{};

extern "C"
{
    void MovingSum_Ctor(MovingSum *unit);
    void MovingSum_Dtor(MovingSum *unit);
    void MovingSum_next(MovingSum *unit, int inNumSamples);
    
    void MovingAverage_Ctor(MovingAverage *unit);
    void MovingAverage_Dtor(MovingAverage *unit);
    void MovingAverage_next(MovingAverage *unit, int inNumSamples);
    
    void MovingXCtorHelper(MovingX *unit, UnitCalcFunc calcFunc);
    void MovingX_next(MovingX *unit, int inNumSamples, bool avg);
}

void MovingSum_Ctor( MovingSum* unit )
{
    MovingXCtorHelper(unit, (UnitCalcFunc)&MovingSum_next);
}

void MovingAverage_Ctor( MovingAverage* unit )
{
    MovingXCtorHelper(unit, (UnitCalcFunc)&MovingAverage_next);
}

inline void MovingXCtorHelper( MovingX* unit, UnitCalcFunc calcFunc )
{
    unit->msquares = nullptr; // in case we error out before msquares is assigned
    
    if (ZIN0(2) < 1 || ZIN0(2) > std::numeric_limits<int>::max() || !std::isfinite(ZIN0(2))) {
        Print("MovingSum/Average Error:\n\t'maxsamp' argument must be >= 1, and within integer resolution.\n\tReceived: %f\n", ZIN0(2));
        SETCALC(*ClearUnitOutputs);
        ClearUnitOutputs(unit, 1);
        unit->mDone = true;
        return;
    }
    
    unit->mCalcFunc = calcFunc;
    
    unit->maxSamps  = (int) ZIN0(2);
    unit->nsamps    = sc_clip((int) ZIN0(1), 1, unit->maxSamps);
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
            Print("Failed to allocate memory for MovingSum/Average\n");
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
    if (unit->msquares) {
        RTFree(unit->mWorld, unit->msquares);
    }
}
void MovingAverage_Dtor(MovingAverage *unit)
{
    if (unit->msquares) {
        RTFree(unit->mWorld, unit->msquares);
    }
}


void MovingSum_next( MovingSum *unit, int inNumSamples )
{
    MovingX_next(unit, inNumSamples, false);
}
void MovingAverage_next( MovingAverage *unit, int inNumSamples )
{
    MovingX_next(unit, inNumSamples, true);
}

inline void MovingX_next( MovingX *unit, int inNumSamples, bool avg )
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
    
    newWinSize = sc_clip(newWinSize, 1, maxWinSize);
    
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
    DefineDtorUnit(MovingAverage);
}
