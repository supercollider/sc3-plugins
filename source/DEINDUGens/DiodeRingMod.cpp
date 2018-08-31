//-----------------------------------------------------
// name: "DiodeRingMog"
// version: "1.0"
// author: "Julian Parker & Till Bovermann"
// license: "GPL2+"
// copyright: "(c) Julian Parker & Till Bovermann 2013"
//-----------------------------------------------------

#include "SC_PlugIn.h"
#define TWOPI 6.283185307179586
#define PI 3.141592653589793

// InterfaceTable contains pointers to functions in the host (server).
static InterfaceTable *ft;

// declare struct to hold unit generator state
struct DiodeRingMod : public Unit
{
	double v_b;
	double v_l;
	double h;
	double diodeLinCoeff;
	double diodeCurveCoeff;
};

// declare unit generator functions
static void DiodeRingMod_next_aa(DiodeRingMod *unit, int inNumSamples);

static void DiodeRingMod_Ctor(DiodeRingMod* unit);

//static float DiodeTransfer(float input);


float DiodeTransfer(float input, float v_b,float v_l,float h, float diodeCurveCoeff, float diodeLinCoeff)
{
	float output = 0;
	
	if (input > v_l){
		output = h*input + diodeLinCoeff;
	}
	else if(input > v_b){
		output =diodeCurveCoeff*pow(input-v_b, 2);
	}
	return output;
}

//////////////////////////////////////////////////////////////////

// Ctor is called to initialize the unit generator.
// It only executes once.

// A Ctor usually does 3 things.
// 1. set the calculation function.
// 2. initialize the unit generator state variables.
// 3. calculate one sample of output.
void DiodeRingMod_Ctor(DiodeRingMod* unit)
{
	// 1. set the calculation function.
	SETCALC(DiodeRingMod_next_aa);

	unit->v_b = 0.2;
	unit->v_l = 0.6;
	unit->h = 1.0;
	unit->diodeCurveCoeff = unit->h/(2*(unit->v_l-unit->v_b));

	unit->diodeLinCoeff = unit->diodeCurveCoeff * pow(unit->v_l - unit->v_b, 2) - unit->h * unit->v_l;
	// 2. initialize the unit generator state variables.


	// 3. calculate one sample of output.
	DiodeRingMod_next_aa(unit, 1);
}


//////////////////////////////////////////////////////////////////

// The calculation function executes once per control period
// which is typically 64 samples.


// calculation function for an audio rate frequency argument and audio rate decay argument
void DiodeRingMod_next_aa(DiodeRingMod *unit, int inNumSamples)
{
	// get the pointer to the output buffer
	float *out = OUT(0);

	// get the pointer to the input buffer
	float *carrier = IN(0);
	float *modulator = IN(1);
	double h = unit->h;
	double v_b = unit->v_b;
	double v_l = unit->v_l;
	double diodeCurveCoeff = unit->diodeCurveCoeff;
	double diodeLinCoeff = unit->diodeLinCoeff;
	
	// perform a loop for the number of samples in the control period.
	// If this unit is audio rate then inNumSamples will be 64 or whatever
	// the block size is. If this unit is control rate then inNumSamples will
	// be 1.
	for (int i=0; i < inNumSamples; ++i)
	{
		float in1 = carrier[i]+0.5*modulator[i];
		float in2 = carrier[i]-0.5*modulator[i];
		float out1 = DiodeTransfer(in1,v_b,v_l,h,diodeCurveCoeff,diodeLinCoeff)+DiodeTransfer(-in1,v_b,v_l,h,diodeCurveCoeff,diodeLinCoeff);
		float out2 = DiodeTransfer(in2,v_b,v_l,h,diodeCurveCoeff,diodeLinCoeff)+DiodeTransfer(-in2,v_b,v_l,h,diodeCurveCoeff,diodeLinCoeff);
		out[i] = out1 - out2;

	}

	// store the states back to the struct

}

//////////////////////////////////////////////////////////////////




// the entry point is called by the host when the plug-in is loaded
PluginLoad(DiodeRingMod)
{
	// InterfaceTable *inTable implicitly given as argument to the load function
	ft = inTable; // store pointer to InterfaceTable

	DefineSimpleUnit(DiodeRingMod);
}