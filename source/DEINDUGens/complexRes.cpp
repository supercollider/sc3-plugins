#include "SC_PlugIn.h"
#define TWOPI 6.283185307179586
#define PI 3.141592653589793

// InterfaceTable contains pointers to functions in the host (server).
static InterfaceTable *ft;

// declare struct to hold unit generator state
struct ComplexRes : public Unit
{
	double mFreq;
	double mcoeffX;
	double mcoeffY;
	double mDecay; // Exponential decay time constant in seconds
	double mRes; // Filter resonance coefficient (0...1)
	double mX; // First state (real part)
	double mY; // Second state (imaginary part)
	double mNormCoeff; // Normalisation gain
};

// declare unit generator functions
static void ComplexRes_next_a(ComplexRes *unit, int inNumSamples);
static void ComplexRes_next_k(ComplexRes *unit, int inNumSamples);
static void ComplexRes_Ctor(ComplexRes* unit);


//////////////////////////////////////////////////////////////////

// Ctor is called to initialize the unit generator.
// It only executes once.

// A Ctor usually does 3 things.
// 1. set the calculation function.
// 2. initialize the unit generator state variables.
// 3. calculate one sample of output.
void ComplexRes_Ctor(ComplexRes* unit)
{
	// 1. set the calculation function.
	if (INRATE(1) == calc_FullRate) {
		// if the frequency argument is audio rate
		SETCALC(ComplexRes_next_a);
	} else {
		// if the frequency argument is control rate (or a scalar).
		SETCALC(ComplexRes_next_k);
	}

	// 2. initialize the unit generator state variables.
	unit->mDecay = 0.1;
	unit->mRes = exp(-1.0/(unit->mDecay * SAMPLERATE)); // Filter resonance coefficient (0...1)
	unit->mX = 0.0; // First state (real part)
	unit->mY = 0.0; // Second state (imaginary part)
	unit->mFreq = 10;
	unit->mNormCoeff = 1/(2*(1-unit->mRes));
	unit->mcoeffX = unit->mRes * cos(TWOPI * unit->mFreq / SAMPLERATE);
	unit->mcoeffY = unit->mRes * sin(TWOPI * unit->mFreq / SAMPLERATE);

	// 3. calculate one sample of output.
	ComplexRes_next_k(unit, 1);
}


//////////////////////////////////////////////////////////////////

// The calculation function executes once per control period
// which is typically 64 samples.

// calculation function for an audio rate frequency argument
void ComplexRes_next_a(ComplexRes *unit, int inNumSamples)
{
	// get the pointer to the output buffer
	float *out = OUT(0);

	// get the pointer to the input buffer
	float *in = IN(0);
	float *freq = IN(1);
	float decay = IN0(2);
	double oldX = unit->mX;
	double oldY = unit->mY;
	double res,ang,coeffX,coeffY,X,Y,normCoeff;
	
	if (decay != unit->mDecay){
		res = exp(-1.0/(decay*SAMPLERATE));
		normCoeff = 1/(2*(1-res));
		unit->mDecay = decay;
		unit->mRes = res;
		unit->mNormCoeff = normCoeff;
		
	} else {
		res = unit->mRes;
		normCoeff = unit->mNormCoeff;
	};
	// perform a loop for the number of samples in the control period.
	// If this unit is audio rate then inNumSamples will be 64 or whatever
	// the block size is. If this unit is control rate then inNumSamples will
	// be 1.
	for (int i=0; i < inNumSamples; ++i)
	{
		ang = (freq[i]/SAMPLERATE)*TWOPI;
		coeffX = res*cos(ang);
		coeffY = res*sin(ang);
		
		X = coeffX*oldX - coeffY*oldY + in[i];
		Y = coeffY*oldX + coeffX*oldY;
		out[i] = Y * normCoeff;
		
		oldX = X;
		oldY = Y;
	}

	// store the states back to the struct
	unit->mX = zapgremlins(oldX);
	unit->mY = zapgremlins(oldY);
}

//////////////////////////////////////////////////////////////////

// calculation function for a control rate frequency argument
void ComplexRes_next_k(ComplexRes *unit, int inNumSamples)
{
	// get the pointer to the output buffer
	float *out = OUT(0);

	// get the pointer to the input buffer
	float *in = IN(0);
	float freq = IN0(1);
	float decay = IN0(2);
	double oldX = unit->mX;
	double oldY = unit->mY;
	double res,ang,coeffX,coeffY,X,Y,normCoeff;
	
	if (decay != unit->mDecay || freq != unit->mFreq){
		res = exp(-1.0/(decay*SAMPLERATE));
		normCoeff = 1/(2*(1-res));
		ang = freq * TWOPI / SAMPLERATE;
		coeffX = res*cos(ang);
		coeffY = res*sin(ang);
		unit->mDecay = decay; // If the parameters have changed, store them back in the struct
		unit->mRes = res;
		unit->mFreq = freq;
		unit->mcoeffX = coeffX;
		unit->mcoeffY = coeffY;
		unit->mNormCoeff = normCoeff;
	} else {
		 res = unit->mRes;
		 coeffX = unit->mcoeffX;
		 coeffY = unit->mcoeffY;
		 normCoeff = unit->mNormCoeff;
	};
	// perform a loop for the number of samples in the control period.
	// If this unit is audio rate then inNumSamples will be 64 or whatever
	// the block size is. If this unit is control rate then inNumSamples will
	// be 1.
	for (int i=0; i < inNumSamples; ++i)
	{
		X = coeffX*oldX - coeffY*oldY + in[i];
		Y = coeffY*oldX + coeffX*oldY;
		out[i] = Y*normCoeff;
		
		oldX = X;
		oldY = Y;
	}

	// store the states back to the struct
	unit->mX = zapgremlins(oldX);
	unit->mY = zapgremlins(oldY);
	
}


// the entry point is called by the host when the plug-in is loaded
PluginLoad(ComplexRes)
{
	// InterfaceTable *inTable implicitly given as argument to the load function
	ft = inTable; // store pointer to InterfaceTable

	DefineSimpleUnit(ComplexRes);
}