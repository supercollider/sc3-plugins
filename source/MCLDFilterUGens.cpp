#include "SC_PlugIn.h"

static InterfaceTable *ft;

struct Friction : public Unit
{
	float m_V, m_beltpos, m_x, m_dx;
};

extern "C"
{
	void load(InterfaceTable *inTable);
	void Friction_next(Friction *unit, int inNumSamples);
	void Friction_Ctor(Friction* unit);
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


//////////////////////////////////////////////////////////////////

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

void load(InterfaceTable *inTable)
{
	ft = inTable;

	DefineSimpleUnit(Friction);
}
