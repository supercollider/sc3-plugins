#include "SC_PlugIn.h"

static InterfaceTable *ft;

struct Logger : public Unit
{
	float m_prevtrig, m_prevreset;
	unsigned int m_writepos;
	
	// Also the Buffer stuff used by BufWr
	float m_fbufnum;
	SndBuf *m_buf;
};

struct ListTrig : public Unit
{
	float m_prevreset;
	unsigned int m_bufpos;
	double m_timepos, m_timeincrement;
	
	float m_fbufnum;
	SndBuf *m_buf;
};

extern "C"
{
	void load(InterfaceTable *inTable);
	
	void Logger_Ctor(Logger* unit);
	void Logger_next(Logger *unit, int inNumSamples);
	
	void ListTrig_Ctor(ListTrig* unit);
	void ListTrig_next(ListTrig *unit, int inNumSamples);
};

//////////////////////////////////////////////////////////////////

#define GET_BUF \
	float fbufnum  = ZIN0(0); \
	bool justInitialised = false; \
	if (fbufnum != unit->m_fbufnum) { \
		uint32 bufnum = (int)fbufnum; \
		World *world = unit->mWorld; \
		if (bufnum >= world->mNumSndBufs) bufnum = 0; \
		unit->m_fbufnum = fbufnum; \
		unit->m_buf = world->mSndBufs + bufnum; \
		justInitialised = true; \
	} \
	SndBuf *buf = unit->m_buf; \
	float *bufData __attribute__((__unused__)) = buf->data; \
	uint32 bufChannels __attribute__((__unused__)) = buf->channels; \
	uint32 bufSamples __attribute__((__unused__)) = buf->samples; \
	uint32 bufFrames = buf->frames; \
	int mask __attribute__((__unused__)) = buf->mask; \
	int guardFrame __attribute__((__unused__)) = bufFrames - 2; 

#define CHECK_BUF \
	if (!bufData) { \
                unit->mDone = true; \
		ClearUnitOutputs(unit, inNumSamples); \
		return; \
	}

#define SETUP_IN(offset) \
	uint32 numInputs = unit->mNumInputs - (uint32)offset; \
	if (numInputs != bufChannels) { \
                unit->mDone = true; \
		ClearUnitOutputs(unit, inNumSamples); \
		return; \
	} \
	float *in[16]; \
	for (uint32 i=0; i<numInputs; ++i) in[i] = ZIN(i+offset); 

//////////////////////////////////////////////////////////////////

void Logger_Ctor(Logger* unit)
{
	SETCALC(Logger_next);
	
	// a la BufWr
	unit->m_fbufnum = -1e9f;
	
	unit->m_prevtrig = 0.f;
	unit->m_prevreset = 0.f;
	unit->m_writepos = 0;
	
	//Logger_next(unit, 1);
	ClearUnitOutputs(unit, 1);
}

void Logger_next(Logger *unit, int inNumSamples)
{
	float trig = ZIN0(1);
	float reset = ZIN0(2);
	
	float prevtrig = unit->m_prevtrig;
	float prevreset = unit->m_prevreset;
	unsigned int writepos = unit->m_writepos; // The write position (takes account of num channels)
	
	float out = 0.f;
	
	// Stuff a la BufWr - NB I have modified GET_BUF slightly
	GET_BUF
	CHECK_BUF
	SETUP_IN(3)
	
	float* table0 = bufData + writepos;
	
	// First, handle reset
	if(justInitialised || (reset > 0.f && prevreset <= 0.f)){
		writepos = 0;
		memset(bufData, 0, bufChannels * bufFrames * sizeof(float));
	}
	
	// Now check for trigger
	if(trig > 0.f && prevtrig <= 0.f){
		if(writepos == bufChannels * bufFrames){
			Print("Logger.kr warning: Buffer full, dropped values: first channel %g\n", *in[0]);
		}else{
			for(uint32 i=0; i<numInputs; ++i){
				table0[i] = *++(in[i]);
			}
			writepos += numInputs;
			out = 1.0f;
		}
	}
	
	// Store state
	unit->m_prevtrig = trig;
	unit->m_prevreset = reset;
	unit->m_writepos = writepos;
	
	ZOUT0(0) = out;
}

////////////////////////////////////////////////////////////////////

void ListTrig_Ctor(ListTrig* unit)
{
	SETCALC(ListTrig_next);
	
	unit->m_fbufnum = -1e9f;
	
	unit->m_prevreset = 0.f;
	unit->m_bufpos = 0;
	unit->m_timepos = 0.0 - (double)ZIN0(2);
	unit->m_timeincrement = (double)BUFDUR;
	
	//Print("ListTrig: time increment set to %g, i.e. freq of %g/s", unit->m_timeincrement, 1.0/unit->m_timeincrement);
	
	ClearUnitOutputs(unit, 1);
}

void ListTrig_next(ListTrig *unit, int inNumSamples)
{
	float reset = ZIN0(1);
	unsigned int numframes = (unsigned int)ZIN0(3);
	
	float prevreset = unit->m_prevreset;
	unsigned int bufpos = unit->m_bufpos; // The readback position
	double timepos = unit->m_timepos;
	double timeinc = unit->m_timeincrement;
	
	float out = 0.f;
	
	// Stuff a la BufWr - NB I have modified GET_BUF slightly
	GET_BUF
	CHECK_BUF
	
	// First, handle reset
	if(reset > 0.f && prevreset <= 0.f){
		bufpos = 0;
		timepos = 0.0 - (double)ZIN0(2);
	}
	
	if(bufpos<numframes){
		float* table0 = bufData + bufpos;
		
		if(table0[0] <= (float)timepos){
			out = 1.f;
			
			// Also increment buffer read position until we're either at the end of the buffer, or we've found a "future" value
			while((bufpos<numframes) && (table0[0] <= (float)timepos)){
				bufpos++;
				table0 = bufData + bufpos;
			}
		}
	}
	
	// Store state
	unit->m_prevreset = reset;
	unit->m_bufpos = bufpos;
	unit->m_timepos = timepos + timeinc; // Shift time on to what it will be on the next go
	
	ZOUT0(0) = out;
}

//////////////////////////////////////////////////////////////////

void load(InterfaceTable *inTable)
{
	ft = inTable;

	DefineSimpleUnit(Logger);
	DefineSimpleUnit(ListTrig);
}
