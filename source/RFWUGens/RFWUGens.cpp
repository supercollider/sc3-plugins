/*
 *  rfw-ugens.cpp
 *  Plugins
 *
 *  Created by Rob Watson on 17/December/2008.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 *  Git repository: https://github.com/rfwatson
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
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 */


#include "SC_PlugIn.h"

static InterfaceTable *ft;

#define ENVLEN 2000.0 // TODO modulate

struct SwitchDelay : public Unit  {
    float *buffer;
    float prev_samp, offset_start, offset_current;
    uint32 writepos, readpos, decaytime, bufsize, offset_timer;
    char crossfading;
};

struct AverageOutput : public Unit  {
    float average, prev_trig;
    uint32 count;
};

extern "C" {
	void SwitchDelay_next(SwitchDelay *unit, int inNumSamples);
	void SwitchDelay_Ctor(SwitchDelay* unit);
	void SwitchDelay_Dtor(SwitchDelay* unit);

	void AverageOutput_next(AverageOutput *unit, int inNumSamples);
	void AverageOutput_Ctor(AverageOutput* unit);
}


void SwitchDelay_Ctor( SwitchDelay* unit ) {
	RGen& rgen = *unit->mParent->mRGen;

    float *buffer;

    unit->bufsize = (uint32)(SAMPLERATE * ZIN0(5));
    buffer = unit->buffer = (float *)RTAlloc(unit->mWorld, unit->bufsize * sizeof(float));

    for(int i=0; i<unit->bufsize; ++i)
        *(buffer+i)=0.; // TODO use memset or something here

    unit->decaytime = (uint32)(ZIN0(3) * SAMPLERATE);
    unit->writepos = 0;
    unit->prev_samp = 0.;
    unit->offset_start = 0.;
    unit->offset_current = 0.;
    unit->offset_timer = ENVLEN;
    unit->crossfading = 0;
    unit->readpos = ((unit->bufsize - unit->decaytime) + unit->bufsize) % unit->bufsize;

	SETCALC(SwitchDelay_next);
}


void SwitchDelay_Dtor(SwitchDelay *unit) {
    RTFree(unit->mWorld, unit->buffer);
}

void SwitchDelay_next( SwitchDelay *unit, int inNumSamples ) {
    int i;
    float recval, readval, ratio;

    float *out = OUT(0);
    float *in = IN(0);
    float *buffer = unit->buffer;

    float drylevel = ZIN0(1);
    float wetlevel = ZIN0(2);
    float delayfactor = ZIN0(4);
    float prev_samp = unit->prev_samp;
    float offset_current = unit->offset_current;
    float offset_start = unit->offset_start;

    uint32 decaytime = (uint32)(ZIN0(3) * SAMPLERATE);
    uint32 bufsize = unit->bufsize;
    uint32 offset_timer = unit->offset_timer;
    uint32 writepos = unit->writepos;
    uint32 readpos = ((writepos - decaytime) + (bufsize)) % bufsize;

    char crossfading = unit->crossfading;

    if(decaytime != unit->decaytime) { // move the read pointer
        float newval, oldval, offset;

        newval = buffer[((readpos - decaytime) + bufsize) % bufsize];
        oldval = buffer[readpos] + offset_current; // adding the current offset means that we can modulate again mid-crossfade.
        offset = oldval - newval;

        crossfading = 1;
        offset_start = offset;
        offset_current = offset;
        offset_timer = ENVLEN;
    }

    // limit the delay multiplier to reasonable numbers
    if(delayfactor < 0.) delayfactor = 0.;
    if(delayfactor > 0.9) delayfactor = 0.9;

    for(i=0; i < inNumSamples; ++i) {
        recval = in[i];
        readval = buffer[readpos] + offset_current;

        recval = recval + (prev_samp * delayfactor);
        out[i] = (in[i] * drylevel) + (readval * wetlevel);

        buffer[writepos] = recval;
        prev_samp = readval;

        readpos = (readpos + 1) % bufsize;
        writepos = (writepos + 1) % bufsize;

        if(crossfading) {
            --offset_timer;

            if(offset_timer > 0.) { // still crossfading
                ratio = (offset_timer / ENVLEN);
                offset_current = offset_start * ratio;
            } else { // all done
                crossfading = 0;
                offset_current = 0.;
            }
        }
    }

    unit->crossfading = crossfading;
    unit->offset_start = offset_start;
    unit->offset_current = offset_current;
    unit->offset_timer = offset_timer;
    unit->decaytime = decaytime;
    unit->writepos = writepos;
    unit->readpos = readpos;
    unit->prev_samp = prev_samp;
}


void AverageOutput_Ctor( AverageOutput* unit ) {
    unit->average = 0.;
    unit->count = 0;
    unit->prev_trig = 0.;

	RGen& rgen = *unit->mParent->mRGen;

	SETCALC(AverageOutput_next);
}


void AverageOutput_next( AverageOutput *unit, int inNumSamples ) {
    int i;
    float *in = IN(0);
    float *out = ZOUT(0);
    float trig = ZIN0(1);
    float prev_trig = unit->prev_trig;
    double average = unit->average;
    uint32 count = unit->count;

    if(prev_trig <= 0. && trig > 0.) {
        average = 0.;
        count = 0;
    }

	for (i=0; i<inNumSamples; ++i) {
        average = ((count * average) + *(in+i)) / ++count;
        ZXP(out) = average;
	}

    unit->prev_trig = trig;
	  unit->count = count;
    unit->average = average;
}


PluginLoad(RFW)
{
	ft = inTable;
	DefineDtorUnit(SwitchDelay);
	DefineSimpleUnit(AverageOutput);
}
