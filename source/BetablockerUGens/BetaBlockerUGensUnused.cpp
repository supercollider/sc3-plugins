#include "SC_Plugin.h"
#include <stdio.h>

#ifndef MAXFLOAT
# include <float.h>
# define MAXFLOAT FLT_MAX
#endif

//#include <unistd.h>
// randomer
#include <cstdlib>

#include "betablocker/machine.h"
#include "betablocker/thread.h"

// declare struct to hold unit generator state
struct Betablocker : public Unit
{
	Betablocker(void) : bblocker()
		{}

	~Betablocker(void)
		{}

	machine bblocker;
	u32 threadID;
};

struct DetaBlocker01 : public Unit
{
	DetaBlocker01(void) : bblocker()
		{}

	~DetaBlocker01(void)
		{}

	double m_repeats;
	int32 m_repeatCount;

	machine bblocker;
	u32 threadID;
};


static void Betablocker_Ctor(Betablocker* unit);
static void Betablocker_Dtor(Betablocker* unit);
static void Betablocker_next_k(Betablocker *unit, int inNumSamples);

void DetaBlocker01_Ctor(DetaBlocker01 *unit);
void DetaBlocker01_Dtor(DetaBlocker01 *unit);
void DetaBlocker01_next(DetaBlocker01 *unit, int inNumSamples);

// Ctor is called to initialize the unit generator.
// It only executes once.

// A Ctor usually does 3 things.
// 1. set the calculation function.
// 2. initialize the unit generator state variables.
// 3. calculate one sample of output.
void Betablocker_Ctor(Betablocker* unit)
{
	SETCALC(Betablocker_next_k);

		// 2. initialize the unit generator state variables.
		// initialize the machine
	new(unit) Betablocker();

	unit->threadID = unit->bblocker.add_thread(0);

	for(size_t i = 0; i < HEAP_SIZE; ++i)
	{
		unit->bblocker.poke(i, int(unit->mParent->mRGen->frand()*25));
	}
		// get initial phase of oscillator
		// unit.mPhase = IN0(1);

		// 3. calculate one sample of output.
	Betablocker_next_k(unit, 1);
}

void Betablocker_Dtor(Betablocker* unit)
{
	unit->bblocker.del_thread(0);
	// delete unit->bblocker;
	unit->~Betablocker();
}

// calculation function for a control rate frequency argument
void Betablocker_next_k(Betablocker *unit, int inNumSamples)
{
		// get the pointer to the output buffer
	float *out = OUT(0);

		// get the pointer to the input buffer
		// float *freq = IN(0);

		// get phase and freqmul constant from struct and store it in a
		// local variable.
		// The optimizer will cause them to be loaded it into a register.
	machine m = unit->bblocker;
	u32 threadID = unit->threadID;
		// perform a loop for the number of samples in the control period.
		// If this unit is audio rate then inNumSamples will be 64 or whatever
		// the block size is. If this unit is control rate then inNumSamples will
		// be 1.
	for (int i=0; i < inNumSamples; ++i)
	{
				// out must be written last for in place operation
		float z;
		m.run();
		z = (float) m.get_thread(threadID)->top() / 255.;

				// write the output
		out[i] = z;
	}

		// store the threadID back to the struct
	unit->threadID = threadID;
}

//////////////////////////////////////////////////////////////////////


void DetaBlocker01_Ctor(DetaBlocker01 *unit)
{
	SETCALC(DetaBlocker01_next);

	new(unit) DetaBlocker01();
	unit->threadID = unit->bblocker.add_thread(0);

	for(size_t i = 0; i < HEAP_SIZE; i++)
	{
		unit->bblocker.poke(i, int(unit->mParent->mRGen->frand()*25));
	}

	DetaBlocker01_next(unit, 0);
	OUT0(0) = 0.f;
}

void DetaBlocker01_Dtor(DetaBlocker01* unit)
{
	unit->bblocker.del_thread(0);
	unit->~DetaBlocker01();
	
}


void DetaBlocker01_next(DetaBlocker01 *unit, int inNumSamples)
{
	if (inNumSamples) {
		if (unit->m_repeats < 0.) {
			float x = DEMANDINPUT_A(0, inNumSamples);
			unit->m_repeats = sc_isnan(x) ? 0.f : floor(x + 0.5f);
		}
		if (unit->m_repeatCount >= unit->m_repeats) {
			OUT0(0) = NAN;
			return;
		}
		unit->m_repeatCount++;

		float z;
		machine m = unit->bblocker;
		u32 threadID = unit->threadID;
		m.run();
		z = (float) m.get_thread(threadID)->top() / 255.f;

		unit->threadID = threadID;

		// write the output
		OUT0(0) = z;
	} else {
		unit->m_repeats = -1.f;
		unit->m_repeatCount = 0;
	}
}

// the entry point is called by the host when the plug-in is loaded
PluginLoad(Betablocker)
{
	// InterfaceTable *inTable implicitly given as argument to the load function
	ft = inTable; // store pointer to InterfaceTable

	DefineDtorUnit(Betablocker);
	DefineDtorUnit(DetaBlocker01);
}