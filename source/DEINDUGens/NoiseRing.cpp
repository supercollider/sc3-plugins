#include "SC_PlugIn.h"
// #include <stdio.h>

// #ifndef MAXFLOAT
// # include <float.h>
// # define MAXFLOAT FLT_MAX
// #endif

//#include <unistd.h>
// randomer
//#include <cstdlib>



// InterfaceTable contains pointers to functions in the host (server).
static InterfaceTable *ft;

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
	printf("initial unit state: %d,\n", unit->state);
	OUT0(0) = 0.f;
}

void DNoiseRing_Dtor(DNoiseRing *unit)
{
	// unit->~DNoiseRing();
	
}


void DNoiseRing_next(DNoiseRing *unit, int inNumSamples)
{
	// printf("in next 0 \n");
	float change = DEMANDINPUT_A(0, inNumSamples);
	float chance = DEMANDINPUT_A(1, inNumSamples);
	uint shift = (uint) DEMANDINPUT_A(2, inNumSamples);
	uint numBits = (uint) DEMANDINPUT_A(3, inNumSamples);

	// printf("in next 1\n");
	RGET
	// printf("in next 2\n");
	
	
	if (inNumSamples) {// calc one sample
		// printf("\tin calc\n");
		uint x = unit->state;
		
		// bitwise rotate right			
		uint lsbs = x & ((1 << shift) - 1);
		x = (x >> shift) | (lsbs << (numBits-shift));
		
		float coin = frand(s1, s2, s3);
		if(coin < change){
			// printf("change! %f < %f\n", coin, change);

			float flipUp = frand(s1, s2, s3);
			// printf("\t%f < %f: ", flipUp, chance);
			if (flipUp < chance){
				// printf("11111\n");

				// set bit0 to 1
				x |= 1;
			} else {
				// printf("00000\n");

				// set bit0 to 0
				x &= ~1;
			}
		};
		unit->state = x;
		
		OUT0(0) = (float) x;
	} else {
		printf("\tin else\n");
		unit->state = trand(s1, s2, s3);
		RESETINPUT(0);
		RESETINPUT(1);
		RESETINPUT(2);
		RESETINPUT(3);
	}
	RPUT
}



//////////////////////////////

// the entry point is called by the host when the plug-in is loaded
PluginLoad(NoiseRing)
{
	// InterfaceTable *inTable implicitly given as argument to the load function
	ft = inTable; // store pointer to InterfaceTable
	DefineDtorUnit(DNoiseRing);
}