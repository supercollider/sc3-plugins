#include "SC_PlugIn.h"

static InterfaceTable *ft;

struct InsideOut : public Unit
{
};

struct WaveLoss : public Unit
{
	bool m_on;
	int m_pos;
	float m_prevval;
};

struct Squiz : public Unit
{
	float *m_buf;
	int m_buflen;
	int m_writepos;
	float m_readpos;
	float m_prevval;
	int m_zcsofar;
};

// declare unit generator functions
extern "C"
{
	void load(InterfaceTable *inTable);

	void InsideOut_next(InsideOut *unit, int inNumSamples);
	void InsideOut_Ctor(InsideOut* unit);

	void WaveLoss_next(WaveLoss *unit, int inNumSamples);
	void WaveLoss_Ctor(WaveLoss* unit);

	void Squiz_Ctor(Squiz* unit);
	void Squiz_next(Squiz *unit, int inNumSamples);
	void Squiz_Dtor(Squiz* unit);
};

//////////////////////////////////////////////////////////////////

void InsideOut_Ctor(InsideOut* unit)
{
	SETCALC(InsideOut_next);
	InsideOut_next(unit, 1);
}

void InsideOut_next(InsideOut *unit, int inNumSamples)
{
	float *in = ZIN(0);
	float *out = ZOUT(0);

	float val;
	for (int i=0; i < inNumSamples; ++i)
	{
		val = ZXP(in);
		if(val>0.f)
			ZXP(out) = 1.0f - val;
		else if(val<0.f)
			ZXP(out) = - 1.0f - val;
		else
			ZXP(out) = 0.f;
	}
}

//////////////////////////////////////////////////////////////////

void WaveLoss_Ctor(WaveLoss* unit)
{
	SETCALC(WaveLoss_next);

	unit->m_on = true;
	unit->m_pos = 0;
	unit->m_prevval = 0.f;

	WaveLoss_next(unit, 1);
}

void WaveLoss_next(WaveLoss *unit, int inNumSamples)
{
	float *in = ZIN(0);
	float *out = ZOUT(0);

	bool on = unit->m_on; // Whether the current wave segment is being output
	int pos = unit->m_pos; // Which "number" of wave segment we're on
	float prevval = unit->m_prevval; // Used for checking for positivegoing zero crossings

	int drop  = (int)ZIN0(1);
	int outof = (int)ZIN0(2);
	int mode  = (int)ZIN0(3);

	float curval;
	for (int i=0; i < inNumSamples; ++i)
	{
		curval = ZXP(in);
		// If positive-going zero-crossing, this is a new segment.
		if(prevval<0.f && curval>=0.f){

			if(++pos >= outof){
				pos = 0;
			}
			if(mode == 2){ // Random
				on = (frand(unit->mParent->mRGen->s1, unit->mParent->mRGen->s2, unit->mParent->mRGen->s3)
					>= ((float)drop / (float)outof));
			}else{ // Nonrandom
				on = (pos >= drop);
			}

		}

		//Print("prevval: %g, curval: %g, on: %i, drop: %i, outof: %i, mode: %i\n", prevval, curval, on, drop, outof, mode);

		// Now simply output... or don't...
		ZXP(out) = on ? curval : 0.f;
		prevval = curval;
	}

	// Store state
	unit->m_on = on;
	unit->m_pos = pos;
	unit->m_prevval = prevval;
}

//////////////////////////////////////////////////////////////////

void Squiz_Ctor(Squiz* unit)
{
	SETCALC(Squiz_next);

	unit->m_buflen = (int)(ZIN0(3) * SAMPLERATE);
	unit->m_writepos     = 0;
	unit->m_readpos     = 0.f;
	unit->m_prevval = 0.f;
	unit->m_zcsofar = 0;

	unit->m_buf = (float*) RTAlloc(unit->mWorld, unit->m_buflen * sizeof(float));
	memset(unit->m_buf, 0, unit->m_buflen * sizeof(float));

	Squiz_next(unit, 1);
}

void Squiz_next(Squiz *unit, int inNumSamples)
{
	float *in = ZIN(0);
	float *out = ZOUT(0);

	float *buf = unit->m_buf;
	int buflen = unit->m_buflen;

	float ratio = sc_min(sc_max(ZIN0(1), 1.0f), (float)buflen); // pitch ratio; also === the sample-by-sample readback increment
	int zcperchunk = (int)ZIN0(2);

	int writepos = unit->m_writepos;
	float prevval = unit->m_prevval; // Used for checking for positivegoing zero crossings
	float readpos = unit->m_readpos; // Where in the buffer we're reading back from. Float value for accuracy, cast to uninterpolated int position though.
	int zcsofar = unit->m_zcsofar;

	float curval, outval;
	int readposi;
	for (int i=0; i < inNumSamples; ++i)
	{
		// First we read from the buffer
		readposi = (int)readpos;
		if(readposi >= buflen){
			outval = 0.f;
		}else{
			outval = buf[readposi];
			// postincrement the play-head position
			readpos += ratio;
		}

		// Next we write to the buffer
		curval = ZXP(in);
		writepos++;

		// If positive-going zero-crossing (or if buffer full), this is a new segment.
		//Print("zcsofar: %i\n", zcsofar);
		if((writepos==buflen) || (prevval<0.f && curval>=0.f && (++zcsofar >= zcperchunk))){
			writepos = 0;
			readpos = 0.f;
			zcsofar = 0;
		}
		buf[writepos] = curval;

		//Print("prevval: %g, curval: %g, on: %i, drop: %i, outof: %i, mode: %i\n", prevval, curval, on, drop, outof, mode);

		// Now output the thing we read
		ZXP(out) = outval;
		prevval = curval;
	}

	// Store state
	unit->m_writepos = writepos;
	unit->m_readpos = readpos;
	unit->m_prevval = prevval;
	unit->m_zcsofar = zcsofar;
}

void Squiz_Dtor(Squiz* unit)
{
	RTFree(unit->mWorld, unit->m_buf);
}

////////////////////////////////////////////////////////////////////

PluginLoad(MCLDDistortion)
{
	ft = inTable;

	DefineSimpleUnit(InsideOut);
	DefineSimpleUnit(WaveLoss);
	DefineDtorUnit(Squiz);
}
