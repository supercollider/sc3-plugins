#include "SC_PlugIn.h"
// #include <stdio.h>

// #ifndef MAXFLOAT
// # include <float.h>
// # define MAXFLOAT FLT_MAX
// #endif

// InterfaceTable contains pointers to functions in the host (server).
static InterfaceTable *ft;
typedef unsigned int uint;
// declare struct to hold unit generator state
struct DNoiseRing : public Unit
{
	uint state;
};


// declare unit generator functions

void DNoiseRing_Ctor(DNoiseRing *unit);
void DNoiseRing_Dtor(DNoiseRing *unit);
void DNoiseRing_next(DNoiseRing *unit, int inNumSamples);

//////////////// DNoiseRing //////////////////////////////////////////////


void DNoiseRing_Ctor(DNoiseRing *unit)
{
	SETCALC(DNoiseRing_next);

	unit->state = 0;

	DNoiseRing_next(unit, 0);
	// printf("initial unit state: %d,\n", unit->state);
	OUT0(0) = 0.f;
}

void DNoiseRing_Dtor(DNoiseRing *unit)
{
	// unit->~DNoiseRing();
}


void DNoiseRing_next(DNoiseRing *unit, int inNumSamples)
{
	float change    =        DEMANDINPUT_A(0, inNumSamples);
	float chance    =        DEMANDINPUT_A(1, inNumSamples);
	uint shift      = (uint) DEMANDINPUT_A(2, inNumSamples);
	uint numBits    = (uint) DEMANDINPUT_A(3, inNumSamples);
	uint initialState = (uint) DEMANDINPUT_A(4, inNumSamples);

	// get random state
	RGET
	
	if (inNumSamples) {// calc one sample
		uint x = unit->state;
		
		// bitwise rotate right
		uint lsbs = x & ((1 << shift) - 1);
		x = (x >> shift) | (lsbs << (numBits-shift));
		
		float coin = frand(s1, s2, s3);
		if(coin < change){
			float flipUp = frand(s1, s2, s3);
			if (flipUp < chance){
				// set bit0 to 1
				x |= 1;
			} else {
				// set bit0 to 0
				x &= ~1;
			}
		};
		unit->state = x;
		
		OUT0(0) = (float) x;
	} else {
		unit->state = initialState; //trand(s1, s2, s3);
		RESETINPUT(0);
		RESETINPUT(1);
		RESETINPUT(2);
		RESETINPUT(3);
		// do not reset the initialState slot
	}
	// save random state
	RPUT
}



//////////////////////////////
PluginLoad(NoiseRing)
{
	// InterfaceTable *inTable implicitly given as argument to load function
	ft = inTable;
	DefineDtorUnit(DNoiseRing);
}