/*
This code (c) Dan Stowell, published under the GPL, v2 or later
*/

#include "SC_PlugIn.h"
#define TWOPI 6.28318530717952646f

static InterfaceTable *ft;

struct Friction : public Unit
{
	float m_V, m_beltpos, m_x, m_dx;
};

struct Crest : public Unit
{
	float *m_circbuf;
	unsigned int m_circbufpos;
	unsigned int m_length;
	float m_result;
	bool m_notfullyet;
	int m_realNumSamples;
};

struct Goertzel : public Unit
{
	unsigned int m_size, m_pos, m_realNumSamples;

	// Constants
	float m_w, m_cosine, m_sine;
	float m_coeff;
	unsigned int m_numParallel, m_whichNext, *m_checkpoints;
	// State variables
	float *m_q2, *m_q1;
	// Output variables
	float m_real, m_imag;
};

extern "C"
{
	void Friction_Ctor(Friction* unit);
	void Friction_next(Friction *unit, int inNumSamples);

	void Crest_Ctor(Crest* unit);
	void Crest_next(Crest *unit, int inNumSamples);
	void Crest_Dtor(Crest* unit);

	void Goertzel_Ctor(Goertzel* unit);
	void Goertzel_next_1(Goertzel *unit, int inNumSamples);
	void Goertzel_next_multi(Goertzel *unit, int inNumSamples);
	void Goertzel_Dtor(Goertzel* unit);
};

//////////////////////////////////////////////////////////////////

void Friction_Ctor(Friction* unit)
{

	SETCALC(Friction_next);

	// initialize the unit generator state variables.
	unit->m_V = 0.f;
	unit->m_beltpos = 0.f;
	unit->m_x = 0.f;
	unit->m_dx = 0.f;

	// calculate one sample of output.
	Friction_next(unit, 1);
}


void Friction_next(Friction *unit, int inNumSamples)
{
	float *out = OUT(0);

	float *in = IN(0);

	// Control-rate parameters
	float friction	= ZIN0(1);
	float spring	= ZIN0(2);
	float damp		= ZIN0(3);
	float mass		= ZIN0(4);
	float beltmass	= ZIN0(5);

	// Retrive state
	float beltpos = unit->m_beltpos;
	float V = unit->m_V;
	float x = unit->m_x;
	float dx = unit->m_dx;

	// vars used in the loop
	float F_N, relspeed, F_f, drivingforce, F_s, F, ddx, oldbeltpos, oldV, beltaccn;
	bool sticking;

	// The normal force is just due to the weight of the object
	F_N = mass * 9.81f;
	float frictimesF_N = (friction * F_N);

	for (int i=0; i < inNumSamples; ++i)
	{
		oldbeltpos = beltpos;
		oldV = V;

		beltpos = in[i];
		V = beltpos - oldbeltpos;
		beltaccn = V - oldV;

		// Calc the kinetic friction force
		relspeed = dx - V;
		if(relspeed==0.f){
			 F_f = 0.f; // No kinetic friction when no relative motion
		} else if (relspeed > 0.f){
			 F_f = frictimesF_N;
		} else {
			 F_f = 0.f - frictimesF_N;
		}

		drivingforce = beltaccn * beltmass;

		// Calc the nonfriction force that would occur if moving along with the belt
		F_s = drivingforce - (damp * V) - (spring * x);


		// Decide if we're sticking or not
		sticking = sc_abs(F_s) < frictimesF_N;

		// NOW TO UPDATE THE MASS'S POSITION.
		// If sticking it's easy. Mass speed == belt speed
		if(sticking){
			 dx = V;
		} else {
			 // Based on eq (5)
			 F = F_s - F_f;
			 ddx = F / mass;
			 dx += ddx;
		}

		x += dx;

		// write the output
		out[i] = x;
	}

	// Store state
	unit->m_beltpos = beltpos;
	unit->m_V  = V;
	unit->m_x  = x;
	unit->m_dx  = dx;
}

//////////////////////////////////////////////////////////////////

void Crest_Ctor(Crest* unit)
{
	SETCALC(Crest_next);

	unsigned int length = (unsigned int)ZIN0(1); // Fixed number of items to store in the ring buffer
	if(length==0)
		length=1; // ...because things would get painful in the stupid scenario of length 0

	unit->m_circbuf = (float*)RTAlloc(unit->mWorld, length * sizeof(float));
	unit->m_circbuf[0] = ZIN0(0); // Load first sample in...
	unit->m_circbufpos = 0U;
	unit->m_length = length;
	unit->m_notfullyet = true; // Only after first filled do we scan the whole buffer. Otherwise we scan up to and including the current position.

	if (INRATE(0) == calc_FullRate) {
	unit->m_realNumSamples = unit->mWorld->mFullRate.mBufLength;
	} else {
		unit->m_realNumSamples = 1;
	}

	ZOUT0(0) = unit->m_result = 1.f; // Necessarily 1 at first since only 1 sample received. No need to run calc func
}


void Crest_next(Crest* unit, int inNumSamples)
{
	float *in = ZIN(0);
	float gate = ZIN0(1);

	// Get state and instance variables from the struct
	float* circbuf = unit->m_circbuf;
	int circbufpos = unit->m_circbufpos;
	int length = unit->m_length;
	float result = unit->m_result;
	bool notfullyet = unit->m_notfullyet;
	int realNumSamples = unit->m_realNumSamples;

	LOOP(realNumSamples,
		// Always add to the ringbuf, even if we're not calculating
		circbuf[circbufpos++] = std::fabs(ZXP(in));
		if(circbufpos == length){
			circbufpos = 0U;
			if(notfullyet){
				notfullyet = unit->m_notfullyet = false;
			}
		}
	);

	if(gate){
		// crest = (samples.abs.max) / (samples.abs.mean).
		// crest = N * (samples.abs.max) / (samples.abs.sum).
		// Already performed abs when storing data.
		float maxval=0.f, sum=0.f;
		unsigned int limit = notfullyet ? circbufpos : length;
		//Print("Crest calculating over %u samples (full length %u. mBufLength %u, realNumSamples %u)\n", limit, length, unit->mBufLength, realNumSamples);
		for(unsigned int i=0U; i < limit; ++i){
			sum += circbuf[i];
			if(maxval < circbuf[i])
				maxval = circbuf[i];
		}
		result = sum==0.f  ?  1.f  :  (float)length * maxval / sum;
	}

	ZOUT0(0) = unit->m_result = result;
	// Store state variables back
	unit->m_circbufpos = circbufpos;
	unit->m_result = result;
}
void Crest_Dtor(Crest* unit)
{
	if(unit->m_circbuf)
		RTFree(unit->mWorld, unit->m_circbuf);
}


//////////////////////////////////////////////////////////////////
// Goertzel Algorithm, following
// http://www.embedded.com/story/OEG20020819S0057

void Goertzel_Ctor(Goertzel* unit)
{
	// initialize the unit generator state variables.
	unsigned int size = (int)ZIN0(1);
	float hopf = ZIN0(3); // a ratio, should be 0<hopf<=1
	unsigned int hop = (unsigned int)std::ceil(hopf * (float)size); // in samples

	double srate;
	if (INRATE(0) == calc_FullRate) {
	unit->m_realNumSamples = unit->mWorld->mFullRate.mBufLength;
		srate = unit->mWorld->mFullRate.mSampleRate;

		// Ensure size is a multiple of block size (if audio rate)
		size = unit->m_realNumSamples * (unsigned int)std::ceil(size / (float)unit->m_realNumSamples);
		hop  = unit->m_realNumSamples * (unsigned int)std::ceil(hop  / (float)unit->m_realNumSamples);
	} else {
		unit->m_realNumSamples = 1;
		srate = unit->mWorld->mBufRate.mSampleRate;
	}

	unsigned int numParallel = size / hop;

	//printf("Goertzel: size %u, hop %u; so %u in parallel\n", size, hop, numParallel);

	if(numParallel == 1)
		SETCALC(Goertzel_next_1);
	else
		SETCALC(Goertzel_next_multi);

	float freq = ZIN0(2);

	// Precomputed constants
	float floatN  = (float)size;
	int    k      = (int)(0.5 + floatN * freq / srate);
	double w      = (TWOPI * k)/floatN;
	double cosine = std::cos(w);
	double sine   = std::sin(w);
	double coeff  = 2. * cosine;

	unit->m_size   = size;
	unit->m_cosine = cosine;
	unit->m_sine   = sine;
	unit->m_coeff  = coeff;

	// Values re the round-robin approach to "overlap"
	unit->m_numParallel = numParallel;
	unit->m_whichNext = 0;

	unit->m_q1 = (float*)RTAlloc(unit->mWorld, numParallel * sizeof(float));
	unit->m_q2 = (float*)RTAlloc(unit->mWorld, numParallel * sizeof(float));
	unit->m_checkpoints = (unsigned int*)RTAlloc(unit->mWorld, numParallel * sizeof(unsigned int));
	for(unsigned int i=0; i<numParallel; ++i){
		unit->m_q1[i] = 0.f;
		unit->m_q2[i] = 0.f;
		// The "checkpoints" are the points in the (theoretical) "buffer" at which we output-and-reset each goertzel stream
		unit->m_checkpoints[i] = (i+1) * hop;
	}
	unit->m_real = 0.f;
	unit->m_imag = 0.f;

	unit->m_pos = 0;

	// calculate one sample of output.
	ZOUT0(0) = 0.f;
}

//////////////////////////////////////////////////////////////////
// Optimised version if no overlap (can avoid some looping, some array indexing, etc):
void Goertzel_next_1(Goertzel *unit, int wrongNumSamples)
{
	unsigned int realNumSamples = unit->m_realNumSamples;

	float* in = IN(0);

	float cosine = unit->m_cosine;
	float sine   = unit->m_sine;
	float coeff  = unit->m_coeff;
	unsigned int pos         = unit->m_pos;
	unsigned int size        = unit->m_size;
	float q1 = unit->m_q1[0];
	float q2 = unit->m_q2[0];
	float real   = unit->m_real;
	float imag   = unit->m_imag;

	float q0;
	for(unsigned int i=0; i<realNumSamples; ++i){
		q0 = coeff * q1 - q2 + in[i];
		q2 = q1;
		q1 = q0;
		++pos;
	}

	if(pos == size){
		// We've reached the end of a block, let's find values
		real = (q1 - q2 * cosine);
		imag = (q2 * sine);
		//Print("Reached block boundary. cosine %g, sine %g, coeff %g, size %u, q0 %g, q1 %g, q2 %g\n", cosine, sine, coeff, size, q0, q1, q2);

		// reset:
		q1 = q2 = 0.f;
		pos = 0;
	}
	// Output values
	ZOUT0(0) = real;
	ZOUT0(1) = imag;

	// Store state
	unit->m_q1[0] = q1;
	unit->m_q2[0] = q2;
	unit->m_pos = pos;
	unit->m_real = real;
	unit->m_imag = imag;
}

void Goertzel_next_multi(Goertzel *unit, int wrongNumSamples)
{
	unsigned int realNumSamples = unit->m_realNumSamples;

	float* in = IN(0);

	float cosine = unit->m_cosine;
	float sine   = unit->m_sine;
	float coeff  = unit->m_coeff;
	unsigned int pos         = unit->m_pos;
	unsigned int size        = unit->m_size;
	unsigned int whichNext   = unit->m_whichNext;
	unsigned int numParallel = unit->m_numParallel;
	unsigned int checkpoint  = unit->m_checkpoints[whichNext];
	float *q1 = unit->m_q1;
	float *q2 = unit->m_q2;
	float real   = unit->m_real;
	float imag   = unit->m_imag;

	float q0;
	for(unsigned int i=0; i<realNumSamples; ++i){
		for(unsigned int j=0; j<numParallel; ++j){
			q0 = coeff * q1[j] - q2[j] + in[i];
			q2[j] = q1[j];
			q1[j] = q0;
		}
		++pos;
	}

	if(pos == checkpoint){
		// We've reached the end of a block for our currently-selected stream, let's find values
		real = (q1[whichNext] - q2[whichNext] * cosine);
		imag = (q2[whichNext] * sine);
		//Print("Reached block boundary. cosine %g, sine %g, coeff %g, size %u, q0 %g, q1 %g, q2 %g\n", cosine, sine, coeff, size, q0, q1, q2);

		// reset:
		q1[whichNext] = q2[whichNext] = 0.;
		if(pos==size){
			pos = 0;
		}
		++whichNext;
		if(whichNext == numParallel){
			whichNext = 0;
		}
		unit->m_whichNext = whichNext;
	}
	// Output values
	ZOUT0(0) = real;
	ZOUT0(1) = imag;

	// Store state
	unit->m_pos = pos;
	unit->m_real = real;
	unit->m_imag = imag;
}

void Goertzel_Dtor(Goertzel* unit)
{
	if(unit->m_q1){
		RTFree(unit->mWorld, unit->m_q1);
		RTFree(unit->mWorld, unit->m_q2);
		RTFree(unit->mWorld, unit->m_checkpoints);
	}
}

//////////////////////////////////////////////////////////////////

PluginLoad(MCLDFilter)
{
	ft = inTable;

	DefineSimpleUnit(Friction);
	DefineDtorUnit(Crest);
	DefineDtorUnit(Goertzel);
}
