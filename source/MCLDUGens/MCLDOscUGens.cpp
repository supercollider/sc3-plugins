#include "SC_PlugIn.h"

static InterfaceTable *ft;

struct SawDPW : public Unit
{
	double mPhase; // phase of the oscillator, from -1 to 1.
	float mFreqMul; // a constant for multiplying frequency
	float mVal; // memory of prev value
	float mFreq; // memory of prev freq, just for recalculating scalefac
	float mScalefac;
};

extern "C"
{
	void load(InterfaceTable *inTable);
	void SawDPW_next_a(SawDPW *unit, int inNumSamples);
	void SawDPW_next_k(SawDPW *unit, int inNumSamples);
	void SawDPW_Ctor(SawDPW* unit);
};

void SawDPW_Ctor(SawDPW* unit)
{

	if (INRATE(0) == calc_FullRate) {
		SETCALC(SawDPW_next_a);
	} else {
		SETCALC(SawDPW_next_k);
	}

	unit->mFreqMul = 2.0 * SAMPLEDUR;
	unit->mVal = 0.f;
	unit->mFreq = 0.f;
	unit->mScalefac = 0.f;
	// get initial phase of oscillator
	unit->mPhase = IN0(1);
	SawDPW_next_a(unit, 1);
}

void SawDPW_next_a(SawDPW *unit, int inNumSamples)
{
	float *out = OUT(0);
	float *freq = IN(0);
	float freqmul = unit->mFreqMul;
	float val    = unit->mVal;
	double phase = unit->mPhase;
	float oldval;
	float prevfreq = unit->mFreq;
	float scalefac = unit->mScalefac;
	for (int i=0; i < inNumSamples; ++i)
	{
		// The "phase" is our modulo counter, -1 to 1
		phase += (*freq) * freqmul;
		if(phase >= 1.)  phase -= 2.;
		if(phase < -1.)  phase += 2.;
		// Square the phase to produce a parabolic wave
		oldval = val;
		val = (phase*phase);

		// Recalculate scale factor if needed
		if(*freq != prevfreq){
			prevfreq = *freq;
			scalefac = SAMPLERATE / (8.f * prevfreq * (1.f - prevfreq/SAMPLERATE));
		}

		// Differentiate and write the output, scaled to +-1
		out[i] = (val-oldval) * scalefac;
		prevfreq = *freq;
		++freq;
	}
	// store state back to the struct
	unit->mPhase = phase;
	unit->mVal   = val;
	unit->mFreq  = prevfreq;
	unit->mScalefac = scalefac;
}

void SawDPW_next_k(SawDPW *unit, int inNumSamples)
{
	float *out = OUT(0);
	float freq = IN0(0) * unit->mFreqMul;
	float val    = unit->mVal;
	double phase = unit->mPhase;
	float oldval;
	// Recalculate scale factor if needed
	float scalefac;
	if(freq != unit->mFreq){
		unit->mFreq = freq;
		scalefac = 1.f / (4.f * freq * (1.f - freq/SAMPLERATE));
		unit->mScalefac = scalefac;
	}else{
		scalefac = unit->mScalefac;
	}
	for (int i=0; i < inNumSamples; ++i)
	{
		// The "phase" is our modulo counter, -1 to 1
		phase += freq;
		if(phase >= 1.)  phase -= 2.;
		if(phase < -1.)  phase += 2.;
		// Square the phase to produce a parabolic wave
		oldval = val;
		val = (phase*phase);
		//Print("val %g, oldval %g\n", val, oldval);
		// Differentiate and write the output, scaled to +-1
		//out[i] = (val-oldval) * SAMPLERATE / (4.f * freq * (1.f - freq/SAMPLERATE)); // Differentiate and scale amplitude to +-1
		out[i] = (val-oldval) * scalefac; // Differentiate and scale amplitude to +-1
	}
	// store state back to the struct
	unit->mPhase = phase;
	unit->mVal   = val;
}

PluginLoad(MCLDOsc)
{
	ft = inTable;
	DefineSimpleUnit(SawDPW);
}
