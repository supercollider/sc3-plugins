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
VOSIM implemented by Léon Spek
http://www.dendriet.nl
http://www.leonspek.tk
 */

#include "SC_PlugIn.h"
//#include <limits.h>

// InterfaceTable contains pointers to functions in the host (server).
static InterfaceTable *ft;

// declare struct to hold unit generator state
struct VOSIM : public Unit
{
	float m_vosim;
	float m_phase;
	float m_prevtrig;

	float m_nCycles;
	int m_numberCurCycle;

	float m_prevsine;
	float m_decay;
	float m_amp;

};

// declare unit generator functions
extern "C"
{
	void load(InterfaceTable *inTable);
	void VOSIM_next_a(VOSIM *unit, int inNumSamples);
	void VOSIM_next_aaak(VOSIM *unit, int inNumSamples);
	void VOSIM_next_aaka(VOSIM *unit, int inNumSamples);
	void VOSIM_next_akaa(VOSIM *unit, int inNumSamples);
	void VOSIM_next_aakk(VOSIM *unit, int inNumSamples);
	void VOSIM_next_akka(VOSIM *unit, int inNumSamples);
	void VOSIM_next_akak(VOSIM *unit, int inNumSamples);
	void VOSIM_next_akkc(VOSIM *unit, int inNumSamples);

	void VOSIM_Ctor(VOSIM* unit);
};



//////////////////////////////////////////////////////////////////

// Ctor is called to initialize the unit generator.
// It only executes once.

// A Ctor usually does 3 things.
// 1. set the calculation function.
// 2. initialize the unit generator state variables.
// 3. calculate one sample of output.
void VOSIM_Ctor(VOSIM* unit)
{
	// 1. set the calculation function.
	if (INRATE(1) == calc_FullRate ){
		if(INRATE(3) == calc_FullRate){
			if(INRATE(2) == calc_FullRate){
			SETCALC(VOSIM_next_a);
			//unit->m_nCycles = IN0(2);
			//unit->m_decay = IN0(3);

			}else{ SETCALC(VOSIM_next_aaka);}
			}else if(INRATE(2) == calc_FullRate)
			{SETCALC(VOSIM_next_aaak);}
			else{SETCALC(VOSIM_next_aakk);}
		}else if (INRATE(3) == calc_FullRate){
			if(INRATE(2) == calc_FullRate){
				SETCALC(VOSIM_next_akaa);
				}else{ SETCALC(VOSIM_next_akka);}
			}else if(INRATE(2) == calc_FullRate){
					SETCALC(VOSIM_next_akak);
					}else{
					SETCALC(VOSIM_next_akkc);
					}

	//SETCALC(VOSIM_next_akkc);

	// 2. initialize the unit generator state variables.
	// get initial phase of oscillator
	unit->m_phase = 0.f;
	unit->m_prevtrig = 0.f;
	unit->m_nCycles = IN0(2);
	unit->m_numberCurCycle = 0;
	unit->m_decay = IN0(3);
	unit->m_amp = 1.f;

	// 3. calculate one sample of output.
	//VOSIM_next_k(unit, 1);
}
//////////////////////////////////////////////////////////////////
void VOSIM_next_a(VOSIM *unit, int inNumSamples)
{

	// get the pointer to the output buffer
	float *out = OUT(0);

	// get the pointer to the input buffer
	float *trig = IN(0);
	float *freq = IN(1);
	float *nCycles = IN(2);
	float *nDecay = IN(3);


	float vosim = unit->m_vosim;
	float phase = unit->m_phase;
	float prevtrig = unit->m_prevtrig;
	float numberCycles = unit->m_nCycles;

	int number = unit->m_numberCurCycle;
	float prevsine = unit->m_prevsine;

	float decay = unit->m_decay;
	float amp = unit->m_amp;




	// perform a loop for the number of samples in the control period.
	// If this unit is audio rate then inNumSamples will be 64 or whatever
	// the block size is. If this unit is control rate then inNumSamples will
	// be 1.
	for (int i=0; i < inNumSamples; ++i)
	{
		float phaseinc = freq[i] * 2.f * pi / SAMPLERATE;
		float z = vosim;
		float trigin = trig[i];


		if(phase > 0 && number <= numberCycles ){
			float sine = sin(phase);
			vosim = (sine * sine) * amp;

			if(prevsine >= 0.f && sine <= 0.f){
			number += 1;
			amp = amp * decay;
				}

			if(prevsine <= 0.f && sine >= 0.f){
			number += 1;
			amp = amp * decay;
				}

			prevsine = sine;

			phase = phase + phaseinc;

		}else if(trigin > 0.f && prevtrig <= 0.f){
			numberCycles = nCycles[i];
			decay = nDecay[i];
			amp = 1.f;
			number = 0;

			float sine = sin(phase);

			vosim = (sine * sine) * amp;

			prevsine = sine;

			phase = phase + phaseinc;
		}else if(number >= numberCycles){
			phase = 0;
			//vosim = 0.f;


			}
		prevtrig = trigin;

		// write the output
		out[i] = z;
	}

	unit->m_vosim = vosim;
	// store the phase back to the struct
	unit->m_phase = phase;
	unit->m_prevtrig = prevtrig;
	unit->m_nCycles = numberCycles;
	unit->m_prevsine = prevsine;
	unit->m_numberCurCycle = number;
	unit->m_decay = decay;
	unit->m_amp = amp;
}
//////////////////////////////////////////////////////////////////
void VOSIM_next_aaak(VOSIM *unit, int inNumSamples)
{
	// get the pointer to the output buffer
	float *out = OUT(0);

	// get the pointer to the input buffer
	float *trig = IN(0);
	float *freq = IN(1);
	float *nCycles = IN(2);
	float nDecay = IN0(3);


	float vosim = unit->m_vosim;
	float phase = unit->m_phase;
	float prevtrig = unit->m_prevtrig;
	float numberCycles = unit->m_nCycles;

	int number = unit->m_numberCurCycle;
	float prevsine = unit->m_prevsine;

	float decay = unit->m_decay;
	float amp = unit->m_amp;




	// perform a loop for the number of samples in the control period.
	// If this unit is audio rate then inNumSamples will be 64 or whatever
	// the block size is. If this unit is control rate then inNumSamples will
	// be 1.
	for (int i=0; i < inNumSamples; ++i)
	{
		float phaseinc = freq[i] * 2.f * pi / SAMPLERATE;
		float z = vosim;
		float trigin = trig[i];


		if(phase > 0 && number <= numberCycles ){
			float sine = sin(phase);
			vosim = (sine * sine) * amp;

			if(prevsine >= 0.f && sine <= 0.f){
			number += 1;
			amp = amp * decay;
				}

			if(prevsine <= 0.f && sine >= 0.f){
			number += 1;
			amp = amp * decay;
				}

			prevsine = sine;

			phase = phase + phaseinc;

		}else if(trigin > 0.f && prevtrig <= 0.f){
			numberCycles = nCycles[i];
			decay = nDecay;
			amp = 1.f;
			number = 0;

			float sine = sin(phase);

			vosim = (sine * sine) * amp;

			prevsine = sine;

			phase = phase + phaseinc;
		}else if(number >= numberCycles){
			phase = 0;
			//vosim = 0.f;


			}
		prevtrig = trigin;

		// write the output
		out[i] = z;
	}

	unit->m_vosim = vosim;
	// store the phase back to the struct
	unit->m_phase = phase;
	unit->m_prevtrig = prevtrig;
	unit->m_nCycles = numberCycles;
	unit->m_prevsine = prevsine;
	unit->m_numberCurCycle = number;
	unit->m_decay = decay;
	unit->m_amp = amp;
}
/////////////////////////////////////////////////////////////
void VOSIM_next_aaka(VOSIM *unit, int inNumSamples)
{
// get the pointer to the output buffer
	float *out = OUT(0);

	// get the pointer to the input buffer
	float *trig = IN(0);
	float *freq = IN(1);
	float nCycles = IN0(2);
	float *nDecay = IN(3);


	float vosim = unit->m_vosim;
	float phase = unit->m_phase;
	float prevtrig = unit->m_prevtrig;
	float numberCycles = unit->m_nCycles;

	int number = unit->m_numberCurCycle;
	float prevsine = unit->m_prevsine;

	float decay = unit->m_decay;
	float amp = unit->m_amp;




	// perform a loop for the number of samples in the control period.
	// If this unit is audio rate then inNumSamples will be 64 or whatever
	// the block size is. If this unit is control rate then inNumSamples will
	// be 1.
	for (int i=0; i < inNumSamples; ++i)
	{
		float phaseinc = freq[i] * 2.f * pi / SAMPLERATE;
		float z = vosim;
		float trigin = trig[i];


		if(phase > 0 && number <= numberCycles ){
			float sine = sin(phase);
			vosim = (sine * sine) * amp;

			if(prevsine >= 0.f && sine <= 0.f){
			number += 1;
			amp = amp * decay;
				}

			if(prevsine <= 0.f && sine >= 0.f){
			number += 1;
			amp = amp * decay;
				}

			prevsine = sine;

			phase = phase + phaseinc;

		}else if(trigin > 0.f && prevtrig <= 0.f){
			numberCycles = nCycles;
			decay = nDecay[i];
			amp = 1.f;
			number = 0;

			float sine = sin(phase);

			vosim = (sine * sine) * amp;

			prevsine = sine;

			phase = phase + phaseinc;
		}else if(number >= numberCycles){
			phase = 0;
			//vosim = 0.f;


			}
		prevtrig = trigin;

		// write the output
		out[i] = z;
	}

	unit->m_vosim = vosim;
	// store the phase back to the struct
	unit->m_phase = phase;
	unit->m_prevtrig = prevtrig;
	unit->m_nCycles = numberCycles;
	unit->m_prevsine = prevsine;
	unit->m_numberCurCycle = number;
	unit->m_decay = decay;
	unit->m_amp = amp;
}
/////////////////////////////////////////////////////////////
void VOSIM_next_akaa(VOSIM *unit, int inNumSamples)
{
	// get the pointer to the output buffer
	float *out = OUT(0);

	// get the pointer to the input buffer
	float *trig = IN(0);
	float freq = IN0(1);
	float *nCycles = IN(2);
	float *nDecay = IN(3);


	float vosim = unit->m_vosim;
	float phase = unit->m_phase;
	float prevtrig = unit->m_prevtrig;
	float numberCycles = unit->m_nCycles;

	int number = unit->m_numberCurCycle;
	float prevsine = unit->m_prevsine;

	float decay = unit->m_decay;
	float amp = unit->m_amp;

	float phaseinc = freq * 2.f * pi / SAMPLERATE;


	// perform a loop for the number of samples in the control period.
	// If this unit is audio rate then inNumSamples will be 64 or whatever
	// the block size is. If this unit is control rate then inNumSamples will
	// be 1.
	for (int i=0; i < inNumSamples; ++i)
	{

		float z = vosim;
		float trigin = trig[i];


		if(phase > 0 && number <= numberCycles ){
			float sine = sin(phase);
			vosim = (sine * sine) * amp;

			if(prevsine >= 0.f && sine <= 0.f){
			number += 1;
			amp = amp * decay;
				}

			if(prevsine <= 0.f && sine >= 0.f){
			number += 1;
			amp = amp * decay;
				}

			prevsine = sine;

			phase = phase + phaseinc;

		}else if(trigin > 0.f && prevtrig <= 0.f){
			numberCycles = nCycles[i];
			decay = nDecay[i];
			amp = 1.f;
			number = 0;

			float sine = sin(phase);

			vosim = (sine * sine) * amp;

			prevsine = sine;

			phase = phase + phaseinc;
		}else if(number >= numberCycles){
			phase = 0;
			//vosim = 0.f;


			}
		prevtrig = trigin;

		// write the output
		out[i] = z;
	}

	unit->m_vosim = vosim;
	// store the phase back to the struct
	unit->m_phase = phase;
	unit->m_prevtrig = prevtrig;
	unit->m_nCycles = numberCycles;
	unit->m_prevsine = prevsine;
	unit->m_numberCurCycle = number;
	unit->m_decay = decay;
	unit->m_amp = amp;
}
/////////////////////////////////////////////////////////////
void VOSIM_next_aakk(VOSIM *unit, int inNumSamples)
{
	// get the pointer to the output buffer
	float *out = OUT(0);

	// get the pointer to the input buffer
	float *trig = IN(0);
	float *freq = IN(1);
	float nCycles = IN0(2);
	float nDecay = IN0(3);


	float vosim = unit->m_vosim;
	float phase = unit->m_phase;
	float prevtrig = unit->m_prevtrig;
	float numberCycles = unit->m_nCycles;

	int number = unit->m_numberCurCycle;
	float prevsine = unit->m_prevsine;

	float decay = unit->m_decay;
	float amp = unit->m_amp;




	// perform a loop for the number of samples in the control period.
	// If this unit is audio rate then inNumSamples will be 64 or whatever
	// the block size is. If this unit is control rate then inNumSamples will
	// be 1.
	for (int i=0; i < inNumSamples; ++i)
	{

		float z = vosim;
		float trigin = trig[i];
		float phaseinc = freq[i] * 2.f * pi / SAMPLERATE;

		if(phase > 0 && number <= numberCycles ){
			float sine = sin(phase);
			vosim = (sine * sine) * amp;

			if(prevsine >= 0.f && sine <= 0.f){
			number += 1;
			amp = amp * decay;
				}

			if(prevsine <= 0.f && sine >= 0.f){
			number += 1;
			amp = amp * decay;
				}

			prevsine = sine;

			phase = phase + phaseinc;

		}else if(trigin > 0.f && prevtrig <= 0.f){
			numberCycles = nCycles;
			decay = nDecay;
			amp = 1.f;
			number = 0;

			float sine = sin(phase);

			vosim = (sine * sine) * amp;

			prevsine = sine;

			phase = phase + phaseinc;
		}else if(number >= numberCycles){
			phase = 0;
			//vosim = 0.f;


			}
		prevtrig = trigin;

		// write the output
		out[i] = z;
	}

	unit->m_vosim = vosim;
	// store the phase back to the struct
	unit->m_phase = phase;
	unit->m_prevtrig = prevtrig;
	unit->m_nCycles = numberCycles;
	unit->m_prevsine = prevsine;
	unit->m_numberCurCycle = number;
	unit->m_decay = decay;
	unit->m_amp = amp;
}
/////////////////////////////////////////////////////////////
void VOSIM_next_akka(VOSIM *unit, int inNumSamples)
{
	// get the pointer to the output buffer
	float *out = OUT(0);

	// get the pointer to the input buffer
	float *trig = IN(0);
	float freq = IN0(1);
	float nCycles = IN0(2);
	float *nDecay = IN(3);


	float vosim = unit->m_vosim;
	float phase = unit->m_phase;
	float prevtrig = unit->m_prevtrig;
	float numberCycles = unit->m_nCycles;

	int number = unit->m_numberCurCycle;
	float prevsine = unit->m_prevsine;

	float decay = unit->m_decay;
	float amp = unit->m_amp;

	float phaseinc = freq * 2.f * pi / SAMPLERATE;


	// perform a loop for the number of samples in the control period.
	// If this unit is audio rate then inNumSamples will be 64 or whatever
	// the block size is. If this unit is control rate then inNumSamples will
	// be 1.
	for (int i=0; i < inNumSamples; ++i)
	{

		float z = vosim;
		float trigin = trig[i];


		if(phase > 0 && number <= numberCycles ){
			float sine = sin(phase);
			vosim = (sine * sine) * amp;

			if(prevsine >= 0.f && sine <= 0.f){
			number += 1;
			amp = amp * decay;
				}

			if(prevsine <= 0.f && sine >= 0.f){
			number += 1;
			amp = amp * decay;
				}

			prevsine = sine;

			phase = phase + phaseinc;

		}else if(trigin > 0.f && prevtrig <= 0.f){
			numberCycles = nCycles;
			decay = nDecay[i];
			amp = 1.f;
			number = 0;

			float sine = sin(phase);

			vosim = (sine * sine) * amp;

			prevsine = sine;

			phase = phase + phaseinc;
		}else if(number >= numberCycles){
			phase = 0;
			//vosim = 0.f;


			}
		prevtrig = trigin;

		// write the output
		out[i] = z;
	}

	unit->m_vosim = vosim;
	// store the phase back to the struct
	unit->m_phase = phase;
	unit->m_prevtrig = prevtrig;
	unit->m_nCycles = numberCycles;
	unit->m_prevsine = prevsine;
	unit->m_numberCurCycle = number;
	unit->m_decay = decay;
	unit->m_amp = amp;
}
/////////////////////////////////////////////////////////////
void VOSIM_next_akak(VOSIM *unit, int inNumSamples)
{
	// get the pointer to the output buffer
	float *out = OUT(0);

	// get the pointer to the input buffer
	float *trig = IN(0);
	float freq = IN0(1);
	float *nCycles = IN(2);
	float nDecay = IN0(3);


	float vosim = unit->m_vosim;
	float phase = unit->m_phase;
	float prevtrig = unit->m_prevtrig;
	float numberCycles = unit->m_nCycles;

	int number = unit->m_numberCurCycle;
	float prevsine = unit->m_prevsine;

	float decay = unit->m_decay;
	float amp = unit->m_amp;

	float phaseinc = freq * 2.f * pi / SAMPLERATE;


	// perform a loop for the number of samples in the control period.
	// If this unit is audio rate then inNumSamples will be 64 or whatever
	// the block size is. If this unit is control rate then inNumSamples will
	// be 1.
	for (int i=0; i < inNumSamples; ++i)
	{

		float z = vosim;
		float trigin = trig[i];


		if(phase > 0 && number <= numberCycles ){
			float sine = sin(phase);
			vosim = (sine * sine) * amp;

			if(prevsine >= 0.f && sine <= 0.f){
			number += 1;
			amp = amp * decay;
				}

			if(prevsine <= 0.f && sine >= 0.f){
			number += 1;
			amp = amp * decay;
				}

			prevsine = sine;

			phase = phase + phaseinc;

		}else if(trigin > 0.f && prevtrig <= 0.f){
			numberCycles = nCycles[i];
			decay = nDecay;
			amp = 1.f;
			number = 0;

			float sine = sin(phase);

			vosim = (sine * sine) * amp;

			prevsine = sine;

			phase = phase + phaseinc;
		}else if(number >= numberCycles){
			phase = 0;
			//vosim = 0.f;


			}
		prevtrig = trigin;

		// write the output
		out[i] = z;
	}

	unit->m_vosim = vosim;
	// store the phase back to the struct
	unit->m_phase = phase;
	unit->m_prevtrig = prevtrig;
	unit->m_nCycles = numberCycles;
	unit->m_prevsine = prevsine;
	unit->m_numberCurCycle = number;
	unit->m_decay = decay;
	unit->m_amp = amp;
}

////////////////////////////////////////////////////////////

// The calculation function executes once per control period
// which is typically 64 samples.

// calculation function for an audio rate frequency argument
void VOSIM_next_akkc(VOSIM *unit, int inNumSamples)
{
	// get the pointer to the output buffer
	float *out = OUT(0);

	// get the pointer to the input buffer
	float *trig = IN(0);
	float freq = IN0(1);
	float nCycles = IN0(2);
	float nDecay = IN0(3);


	float vosim = unit->m_vosim;
	float phase = unit->m_phase;
	float prevtrig = unit->m_prevtrig;
	float numberCycles = unit->m_nCycles;

	int number = unit->m_numberCurCycle;
	float prevsine = unit->m_prevsine;

	float decay = unit->m_decay;
	float amp = unit->m_amp;

	float phaseinc = freq * 2.f * pi / SAMPLERATE;


	// perform a loop for the number of samples in the control period.
	// If this unit is audio rate then inNumSamples will be 64 or whatever
	// the block size is. If this unit is control rate then inNumSamples will
	// be 1.
	for (int i=0; i < inNumSamples; ++i)
	{

		float z = vosim;
		float trigin = trig[i];


		if(phase > 0 && number <= numberCycles ){
			float sine = sin(phase);
			vosim = (sine * sine) * amp;

			if(prevsine >= 0.f && sine <= 0.f){
			number += 1;
			amp = amp * decay;
				}

			if(prevsine <= 0.f && sine >= 0.f){
			number += 1;
			amp = amp * decay;
				}

			prevsine = sine;

			phase = phase + phaseinc;

		}else if(trigin > 0.f && prevtrig <= 0.f){
			numberCycles = nCycles;
			decay = nDecay;
			amp = 1.f;
			number = 0;

			float sine = sin(phase);

			vosim = (sine * sine) * amp;

			prevsine = sine;

			phase = phase + phaseinc;
		}else if(number >= numberCycles){
			phase = 0;
			//vosim = 0.f;


			}
		prevtrig = trigin;

		// write the output
		out[i] = z;
	}

	unit->m_vosim = vosim;
	// store the phase back to the struct
	unit->m_phase = phase;
	unit->m_prevtrig = prevtrig;
	unit->m_nCycles = numberCycles;
	unit->m_prevsine = prevsine;
	unit->m_numberCurCycle = number;
	unit->m_decay = decay;
	unit->m_amp = amp;
}

//////////////////////////////////////////////////////////////////

// the load function is called by the host when the plug-in is loaded
PluginLoad(VOSIM)
{
	ft = inTable;

	DefineSimpleUnit(VOSIM);
}
