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
	double mAng;
};

// declare unit generator functions
static void ComplexRes_next_ak(ComplexRes *unit, int inNumSamples);
static void ComplexRes_next_aa(ComplexRes *unit, int inNumSamples);
static void ComplexRes_next_kk(ComplexRes *unit, int inNumSamples);
static void ComplexRes_next_ka(ComplexRes *unit, int inNumSamples);
static void ComplexRes_Ctor(ComplexRes* unit);


//////////////////////////////////////////////////////////////////
void ComplexRes_Ctor(ComplexRes* unit)
{
	// 1. set the calculation function.
	if (INRATE(1) == calc_FullRate) {
		// if the frequency argument is audio rate
		if (INRATE(2) == calc_FullRate) {
			SETCALC(ComplexRes_next_aa);
		} else {
			SETCALC(ComplexRes_next_ak);
		}
	} else {
		// if the frequency argument is control rate (or a scalar).
		if (INRATE(2) == calc_FullRate) {
			SETCALC(ComplexRes_next_ka);
		} else {
			SETCALC(ComplexRes_next_kk);
		}		
	}

	// 2. initialize the unit generator state variables.
	unit->mDecay = IN0(2);
	unit->mRes = exp(-1.0/(unit->mDecay * SAMPLERATE)); // Filter resonance coefficient (0...1)
	unit->mX = 0.0; // First state (real part)
	unit->mY = 0.0; // Second state (imaginary part)
	unit->mFreq = IN0(1);
	unit->mNormCoeff = (1.0-unit->mRes*unit->mRes)/unit->mRes;
	unit->mcoeffX = unit->mRes * cos(TWOPI * unit->mFreq / SAMPLERATE);
	unit->mcoeffY = unit->mRes * sin(TWOPI * unit->mFreq / SAMPLERATE);
	// unit->mNormCoeff = uninitializedControl;
	// unit->mcoeffX = uninitializedControl;
	// unit->mcoeffY = uninitializedControl;

	// 3. calculate one sample of output.
	ComplexRes_next_kk(unit, 1);
}


//////////////////////////////////////////////////////////////////
// audio rate frequency and audio rate decay
void ComplexRes_next_aa(ComplexRes *unit, int inNumSamples)
{
	float *out = OUT(0);

	float *in = IN(0);
	float *freq = IN(1);
	float *decay = IN(2);
	double oldX = unit->mX;
	double oldY = unit->mY;
	double res,ang,coeffX,coeffY,X,Y,normCoeff;
	
	for (int i=0; i < inNumSamples; ++i)
	{
		res = exp(-1.0/(decay[i]*SAMPLERATE));
		normCoeff = (1.0-res*res)/res;
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
// audio rate frequency and control rate decay
void ComplexRes_next_ak(ComplexRes *unit, int inNumSamples)
{
	float *out = OUT(0);

	float *in = IN(0);
	float *freq = IN(1);
	float decay = IN0(2);
	double oldX = unit->mX;
	double oldY = unit->mY;
	double res,ang,coeffX,coeffY,X,Y,normCoeff;
	
	// update params
	if (decay != unit->mDecay){
		res = exp(-1.0/((decay+unit->mDecay)*0.5*SAMPLERATE));
		normCoeff = (1.0-res*res)/res;
		unit->mDecay = decay;
		unit->mRes = exp(-1.0/(decay*SAMPLERATE));
		unit->mNormCoeff = normCoeff;
	} else {
		res = unit->mRes;
		normCoeff = unit->mNormCoeff;
	};

	// render signal
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

	// store states back to struct
	unit->mX = zapgremlins(oldX);
	unit->mY = zapgremlins(oldY);
}

//////////////////////////////////////////////////////////////////
// control rate frequency and audio rate decay
void ComplexRes_next_ka(ComplexRes *unit, int inNumSamples)
{
	float *out = OUT(0);

	float *in = IN(0);
	float freq = IN0(1);
	float *decay = IN(2);
	double oldX = unit->mX;
	double oldY = unit->mY;
	double res,ang,coeffX,coeffY,X,Y,normCoeff;
	
	// update params
	if (freq != unit->mFreq){
		ang = (freq+unit->mFreq)*0.5 * TWOPI / SAMPLERATE;
		unit->mFreq = freq;
		unit->mAng = freq * TWOPI / SAMPLERATE;
	} else {
		ang = unit->mAng;
	};

	// render signal
	for (int i=0; i < inNumSamples; ++i)
	{
		res = exp(-1.0/(decay[i]*SAMPLERATE));
		normCoeff = (1.0-res*res)/res;
		coeffX = res*cos(ang);
		coeffY = res*sin(ang);
		X = coeffX*oldX - coeffY*oldY + in[i];
		Y = coeffY*oldX + coeffX*oldY;
		out[i] = Y*normCoeff;
		
		oldX = X;
		oldY = Y;
	}

	// store states back to struct
	unit->mX = zapgremlins(oldX);
	unit->mY = zapgremlins(oldY);
	
}



//////////////////////////////////////////////////////////////////
// control rate frequency and control rate decay
void ComplexRes_next_kk(ComplexRes *unit, int inNumSamples)
{
	float *out = OUT(0);

	float *in = IN(0);
	float freq = IN0(1);
	float decay = IN0(2);
	double oldX = unit->mX;
	double oldY = unit->mY;
	double res,ang,coeffX,coeffY,X,Y,normCoeff;
	
	// update params
	if (decay != unit->mDecay || freq != unit->mFreq){
		res = exp(-1.0/((decay+unit->mDecay)*0.5*SAMPLERATE));
		normCoeff = (1.0-res*res)/res;
		ang = (freq+unit->mFreq)*0.5 * TWOPI / SAMPLERATE;
		coeffX = res*cos(ang);
		coeffY = res*sin(ang);
		unit->mDecay = decay; // If the parameters have changed, store them back in the struct
		unit->mRes = exp(-1.0/((decay)*SAMPLERATE));
		unit->mFreq = freq;
		unit->mAng =  freq * TWOPI / SAMPLERATE;
		unit->mcoeffX = res*cos(unit->mAng);
		unit->mcoeffY = res*sin(unit->mAng);
		unit->mNormCoeff = normCoeff;
	} else {
		res = unit->mRes;
		coeffX = unit->mcoeffX;
		coeffY = unit->mcoeffY;
		normCoeff = unit->mNormCoeff;
		ang = unit->mAng;
	};

	// render signal
	for (int i=0; i < inNumSamples; ++i)
	{
		X = coeffX*oldX - coeffY*oldY + in[i];
		Y = coeffY*oldX + coeffX*oldY;
		out[i] = Y*normCoeff;
		oldX = X;
		oldY = Y;
	}

	// store states back to struct
	unit->mX = zapgremlins(oldX);
	unit->mY = zapgremlins(oldY);
	
}


// the entry point is called by host when plug-in is loaded
PluginLoad(ComplexRes)
{
	// InterfaceTable *inTable implicitly given as argument to load function
	ft = inTable; // store pointer to InterfaceTable

	DefineSimpleUnit(ComplexRes);
}