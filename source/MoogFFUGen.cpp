/**
"MoogFF" - Moog VCF digital implementation.
As described in the paper entitled
"Preserving the Digital Structure of the Moog VCF"
by Federico Fontana
appeared in the Proc. ICMC07, Copenhagen, 25-31 August 2007

Original Java code Copyright F. Fontana - August 2007
federico.fontana@univr.it

Ported to C++ for SuperCollider by Dan Stowell - August 2007
http://www.mcld.co.uk/
*/

#include "SC_PlugIn.h"

#define PI 3.1415926535898f

static InterfaceTable *ft;

struct MoogFF : public Unit
{
	float m_freq, m_b0, m_a1; // Resonant freq and corresponding vals; stored because we need to compare against prev vals
	double m_wcD;
	
	double m_T; // 1/SAMPLEFREQ
	float m_s1, m_s2, m_s3, m_s4; // 1st order filter states
};

extern "C"
{
	void load(InterfaceTable *inTable);
	void MoogFF_next(MoogFF *unit, int inNumSamples);
	void MoogFF_Ctor(MoogFF* unit);
};

//////////////////////////////////////////////////////////////////

void MoogFF_Ctor(MoogFF* unit)
{

	SETCALC(MoogFF_next);

	// initialize the unit generator state variables.
	unit->m_freq = -10000.3f; // Force the freq to update on first run
	unit->m_T = 1.0 / SAMPLERATE;
	unit->m_s1 = 0.f;
	unit->m_s2 = 0.f;
	unit->m_s3 = 0.f;
	unit->m_s4 = 0.f;
	
	// calculate one sample of output.
	MoogFF_next(unit, 1);
}


//////////////////////////////////////////////////////////////////

void MoogFF_next(MoogFF *unit, int inNumSamples)
{
	float *out = OUT(0);
	
	float *in = IN(0);
	float k = IN0(2);
	k = k > 4.f? 4.f : (k<0.f ? 0.f : k);
	
	// Load state from the struct
	float s1 = unit->m_s1;
	float s2 = unit->m_s2;
	float s3 = unit->m_s3;
	float s4 = unit->m_s4;
	float freq = unit->m_freq;///
	double T = unit->m_T;
	
	// Reset filter state if requested
	if(IN0(3)>0){
		s1 = s2 = s3 = s4 = 0.f;
	}
	
	double wcD=unit->m_wcD;
	float a1 = unit->m_a1, b0 = unit->m_b0; // Filter coefficient parameters
	float o, u; // System's null response, loop input
	float past, future;
	for (int i=0; i < inNumSamples; ++i)
	{
		// Update filter coefficients, but only if freq changes since it involves some expensive operations
		if(freq != IN0(1)){
			freq = IN0(1);
			//Print("Updated freq to %g\n", freq);
			wcD = 2.0 * tan(T * PI * freq) * SAMPLERATE;
			if(wcD<0) 
				wcD = 0; // Protect against negative cutoff freq
			double TwcD = T*wcD; 
			b0 = (float)(TwcD/(TwcD + 2.));
			a1 = (float)((TwcD - 2.)/(TwcD + 2.));
		}
		
		// compute loop values
		o = s4 + b0*(s3 + b0*(s2 + b0*s1));
		out[i] = (b0*b0*b0*b0*in[i] + o)/(1. + b0*b0*b0*b0*k);
		u = in[i] - k*out[i];

		// update 1st order filter states
		past = u;
		future = b0*past + s1;
		s1 = b0*past - a1*future;

		past = future;
		future = b0*past + s2;
		s2 = b0*past - a1*future;

		past = future;
		future = b0*past + s3;
		s3 = b0*past - a1*future;

		s4 = b0*future - a1*out[i];
	}

	// Store state
	unit->m_freq = freq;
	unit->m_b0 = b0;
	unit->m_a1 = a1;
	unit->m_wcD = wcD;
	unit->m_s1 = s1;
	unit->m_s2 = s2;
	unit->m_s3 = s3;
	unit->m_s4 = s4;
}

//////////////////////////////////////////////////////////////////

void load(InterfaceTable *inTable)
{
	ft = inTable;

	DefineSimpleCantAliasUnit(MoogFF);
}
