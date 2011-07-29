/*
MdaUGens: UGens ported from Paul Kellett's VST plugins http://mda.smartelectronix.com/
MdaPiano ported to SC3 by Dan Stowell.

(c) Paul Kellett, Dan Stowell
The mda plug-ins are released under the MIT license or under the GPL
"either version 2 of the License, or (at your option) any later version".
*/
#include "SC_PlugIn.h"
#include "mdaPiano_sc3.h"
#include "mdaPianoData.h"

static InterfaceTable *ft;

struct MdaPiano : public Unit
{
	float prevgate;
	
	int32 note, cpos;
	
	VOICE voice;
	float *comb, cdep, width, trim;
	int32  cmax, size, sustain;
	float tune, fine, random, stretch;
	float muff, muffvel, sizevel, velsens, volume;
};

extern "C"
{
	void load(InterfaceTable *inTable);
	void MdaPiano_Ctor(MdaPiano* unit);
	void MdaPiano_next(MdaPiano *unit, int inNumSamples);
	void MdaPiano_Dtor(MdaPiano* unit);
};

//////////////////////////////////////////////////////////////////

void MdaPiano_Ctor(MdaPiano* unit)
{

	SETCALC(MdaPiano_next);

	//initialise...
	unit->prevgate = 0.f;
	unit->voice.env = 0.0f;
	unit->voice.dec = 0.99f; //all notes off
	unit->volume = 0.2f;
	unit->muff = 160.0f;
	unit->cpos = unit->sustain = 0;
	
	unit->comb = (float*)RTAlloc(unit->mWorld, 256 * sizeof(float));
	Clear(256, unit->comb);

	if(SAMPLERATE > 64000.0f) unit->cmax = 0xFF; else unit->cmax = 0x7F;

	// calculate one sample of output.
	MdaPiano_next(unit, 1);
}

void MdaPiano_Dtor(MdaPiano* unit)
{
	if(unit->comb)
		RTFree(unit->mWorld, unit->comb);
}


// calculation function for a control rate frequency argument
void MdaPiano_next(MdaPiano *unit, int inNumSamples)
{
	// get the pointer to the output buffer
	float *out0 = OUT(0);
	float *out1 = OUT(1);

	// grab some input controls
	float freq = IN0(0);
	float gate = IN0(1);
	int32 velocity = (int32)IN0(2);
	float param[12];
	param[ 0] = IN0( 3); // Decay
	param[ 1] = IN0( 4); // Release
	param[ 2] = IN0( 5); // Hardness
	param[ 3] = IN0( 6); // Vel>Hard
	param[ 4] = IN0( 7); // Muffle
	param[ 5] = IN0( 8); // Vel>Muff
	param[ 6] = IN0( 9); // Vel Curve
	param[ 7] = IN0(10); // Stereo
//NOT USED HERE	param[ 8]  // Max Poly
	param[ 9] = IN0(11); // Tune
	param[10] = IN0(12); // Random
	param[11] = IN0(13); // Stretch
	bool sustain = IN0(14) > 0.f;
	
	// get state from struct.
	VOICE *V = &unit->voice;
	
	float prevgate = unit->prevgate;
	
	int32 size    = unit->size;
	float sizevel = unit->sizevel;
	float muffvel = unit->muffvel;
	float velsens = unit->velsens;
	float fine    = unit->fine;
	float random  = unit->random;
	float stretch = unit->stretch;
	float cdep    = unit->cdep;
	float trim    = unit->trim;
	float width   = unit->width;
	
	float *comb   = unit->comb;
	
	int32 cpos    = unit->cpos;
	int32 note    = unit->note;
		
	if(!(prevgate>0.f) && (gate>0.f)){
		float l;
		int32 k, s;
		
		// NOTE ON
		note = sc_cpsmidi(freq);
		if(!(note>0)) note = 0; // Added & phrased to guard against NaNs, should also catch negs
		unit->note = note;
		
		// Update state from arguments:
		unit->size    = size    = (int32)(12.0f * param[2] - 6.0f);
		unit->sizevel = sizevel = 0.12f * param[3];
		unit->muffvel = muffvel = param[5] * param[5] * 5.0f;
		
		velsens = 1.0f + param[6] + param[6];
		if(param[6] < 0.25f)
			velsens -= 0.75f - 3.0f * param[6];
		unit->velsens = velsens;
		
		unit->fine    = fine = param[9] - 0.5f;
		unit->random  = random = 0.077f * param[10] * param[10];
		unit->stretch = stretch = 0.000434f * (param[11] - 0.5f);
		
		unit->cdep    = cdep = param[7] * param[7];
		unit->trim    = trim = 1.50f - 0.79f * cdep;
		width = 0.04f * param[7];
		if(width > 0.03f)
			width = 0.03f;
		unit->width   = width;
		
		// mdaPiano::noteOn setup stuff:
		k = (note - 60) * (note - 60);
		l = fine + random * ((float)(k % 13) - 6.5f);  //random & fine tune
		if(note > 60) l += stretch * (float)k; //stretch
		
		s = size;
		if(velocity > 40) s += (int32)(sizevel * (float)(velocity - 40));
		
		k = 0;
		while(note > (kgrp[k].high + s)) k++;  //find keygroup
		
		l += (float)(note - kgrp[k].root); //pitch
		l = 22050.0f * SAMPLEDUR * (float)exp(0.05776226505 * l);
		V->delta = (int32)(65536.0f * l);
		V->frac = 0;
		V->pos = kgrp[k].pos;
		V->end = kgrp[k].end;
		V->loop = kgrp[k].loop;
		
		V->env = (0.5f + velsens) * (float)pow(0.0078f * velocity, velsens); //velocity
		
		l = 50.0f + param[4] * param[4] * unit->muff + muffvel * (float)(velocity - 64); //muffle
		if(l < (55.0f + 0.25f * (float)note)) l = 55.0f + 0.25f * (float)note;
		if(l > 210.0f) l = 210.0f;
		V->ff = l * l * SAMPLEDUR;
		V->f0 = V->f1 = 0.0f;
		
		V->note = note;
		if(note <  12) note = 12;
		if(note > 108) note = 108;
		l = unit->volume * trim;
		V->outr = l + l * width * (float)(note - 60);
		V->outl = l + l - V->outr;
		
		if(note < 44) note = 44; //limit max decay length
		l = 2.0f * param[0];
		if(l < 1.0f) l += 0.25f - 0.5f * param[0];
		V->dec = (float)exp(-SAMPLEDUR * exp(-0.6 + 0.033 * (double)note - l));
		
		
	}else if((prevgate>0.f) && !(gate>0.f)){
	
		// NOTE OFF
		if(sustain==0)
		{
			if(note < 94 || note == sustain) //no release on highest notes
				V->dec = (float)exp(-SAMPLEDUR * exp(2.0 + 0.017 * (double)note - 2.0 * param[1]));
		}
		else V->note = SUSTAIN;
		
	}
	

	float x, l, r;
	int32 i;
	
	// now generate the audio
	for (int iter=0; iter < inNumSamples; ++iter)
	{
		l = r = 0.f;
		
		V->frac += V->delta;  //integer-based linear interpolation
        V->pos += V->frac >> 16;
        V->frac &= 0xFFFF;
        if(V->pos > V->end) V->pos -= V->loop;
		i = pianoData[V->pos] + ((V->frac * (pianoData[V->pos + 1] - pianoData[V->pos])) >> 16);
		x = V->env * (float)i / 32768.0f;

        V->env = V->env * V->dec;  //envelope
        V->f0 += V->ff * (x + V->f1 - V->f0);  //muffle filter
        V->f1 = x;

        l += V->outl * V->f0;
        r += V->outr * V->f0;
        
		comb[cpos] = l + r;
		++cpos &= unit->cmax;
		x = cdep * comb[cpos];  //stereo simulator

		*out0++ = l + x;
		*out1++ = r - x;
	}

	// store state back to the struct
	unit->prevgate = gate;
	unit->cpos = cpos;
}

////////////////////////////////////////////////////////////////////

int dummy;

// the load function is called by the host when the plug-in is loaded
PluginLoad(Mda)
{
	// touch every sample to force the buffer into physical ram
	for (size_t i = 0; i != sizeof(pianoData)/sizeof(short); ++i)
		dummy += pianoData[i];

	ft = inTable;

	DefineDtorUnit(MdaPiano);
}
