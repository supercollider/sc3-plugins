#include "SC_PlugIn.h"

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

extern "C"
{
	void load(InterfaceTable *inTable);
	
	void Friction_Ctor(Friction* unit);
	void Friction_next(Friction *unit, int inNumSamples);
	
	void Crest_Ctor(Crest* unit);
	void Crest_next(Crest *unit, int inNumSamples);
	void Crest_Dtor(Crest* unit);
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
		result = (float)length * maxval / sum;
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

void load(InterfaceTable *inTable)
{
	ft = inTable;

	DefineSimpleUnit(Friction);
	DefineDtorUnit(Crest);
}
