/**
Getenv UGen (c) Dan Stowell 2007
Grab environment variables (numeric ones please!) for use in synth graphs.
*/

#ifdef _MSC_VER
#include "wintime.h"
#else
#include <sys/time.h>
#endif
#include <ctime>
#include "SC_PlugIn.h"

static InterfaceTable *ft;

struct Getenv : public Unit
{
	int m_key;
	char* m_key_string;
	float m_value;
};

struct Clockmus : public Unit
{
	timeval tp;
};

extern "C"
{
	void Getenv_Ctor(Getenv* unit);
	void Getenv_next(Getenv *unit, int inNumSamples);
	void Getenv_Dtor(Getenv* unit);

	void Clockmus_Ctor(Clockmus* unit);
	void Clockmus_next(Clockmus *unit, int inNumSamples);
};

//////////////////////////////////////////////////////////////////

void Getenv_Ctor(Getenv* unit)
{

	SETCALC(Getenv_next);

	unit->m_key = IN0(0); // number of chars in the key string
	// Now allocate and load the key into a null-terminated string
	unit->m_key_string = (char*)RTAlloc(unit->mWorld, ((int)unit->m_key + 1) * sizeof(char));
	for(int i = 0; i < (int)unit->m_key; i++){
		unit->m_key_string[i] = (char)IN0(2+i);
		};
	unit->m_key_string[(int)unit->m_key] = '\0';

	int verbosity = unit->mWorld->mVerbosity;

	if(verbosity >= 1){
		Print("Getenv: key is %s\n", unit->m_key_string);
	}

	char *envstr = getenv(unit->m_key_string);

	if(envstr==NULL){
		if(verbosity >= 0){
			Print("Getenv warning: string is NULL (env var %s not set)\n", unit->m_key_string);
		}
		unit->m_value = IN0(1); // defaultval
	}else{
		unit->m_value = atof(envstr);
		if(verbosity >= 1){
			Print("Getenv: string value is %s\n", envstr);
			//Print("Getenv: float value is %g\n", unit->m_value);
		}
	}

	// calculate one sample of output.
	Getenv_next(unit, 1);
}

void Getenv_next(Getenv *unit, int inNumSamples)
{
	OUT0(0) = unit->m_value;
}

void Getenv_Dtor(Getenv* unit)
{
	RTFree(unit->mWorld, unit->m_key_string);
}

//////////////////////////////////////////////////////////////////

void Clockmus_Ctor(Clockmus* unit)
{
	SETCALC(Clockmus_next);
	Clockmus_next(unit, 1);
}

void Clockmus_next(Clockmus *unit, int inNumSamples)
{
	gettimeofday(&(unit->tp), NULL);
	OUT0(0) = (float) unit->tp.tv_usec;
}

//////////////////////////////////////////////////////////////////

PluginLoad(MCLDGetEnv)
{
	ft = inTable;

	DefineDtorUnit(Getenv);
	DefineSimpleUnit(Clockmus);
}
