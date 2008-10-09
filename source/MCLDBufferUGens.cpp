#include "SC_PlugIn.h"

static InterfaceTable *ft;

struct Logger : public Unit
{
	float m_prevtrig, m_prevreset;
	unsigned int m_writepos;
	
	// Also the Buffer stuff used by BufWr
	float m_fbufnum;
	SndBuf *m_buf;
	
	bool m_maypost, m_notfull;
};

struct ListTrig : public Unit
{
	float m_prevreset;
	unsigned int m_bufpos;
	double m_timepos, m_timeincrement;
	
	float m_fbufnum;
	SndBuf *m_buf;
};

struct ListTrig2 : public Unit
{
	float m_prevreset;
	unsigned int m_bufpos;
	double m_timepos, m_timeincrement;
	
	float m_fbufnum;
	SndBuf *m_buf;
};

struct GaussClass : public Unit
{
	int m_numdims, m_numclasses, m_numnumsperclass;
	
	float *m_indata; 
	float *m_centred; // data after mean-removal
	
	float m_result, m_fbufnum;
	SndBuf *m_buf;
};


extern "C"
{
	void load(InterfaceTable *inTable);
	
	void Logger_Ctor(Logger* unit);
	void Logger_next(Logger *unit, int inNumSamples);
	
	void ListTrig_Ctor(ListTrig* unit);
	void ListTrig_next(ListTrig *unit, int inNumSamples);
	
	void ListTrig2_Ctor(ListTrig2* unit);
	void ListTrig2_next(ListTrig2 *unit, int inNumSamples);

	void GaussClass_Ctor(GaussClass* unit);
	void GaussClass_next(GaussClass *unit, int inNumSamples);
	void GaussClass_Dtor(GaussClass* unit);

};

//////////////////////////////////////////////////////////////////

#define GET_BUF_ALTERED \
       float fbufnum  = ZIN0(0); \
       bool justInitialised = false; \
       if (fbufnum != unit->m_fbufnum) { \
               uint32 bufnum = (int)fbufnum; \
               World *world = unit->mWorld; \
				if (bufnum >= world->mNumSndBufs) { \
					int localBufNum = bufnum - world->mNumSndBufs; \
					Graph *parent = unit->mParent; \
					if(localBufNum <= parent->localBufNum) { \
						unit->m_buf = parent->mLocalSndBufs + localBufNum; \
					} else { \
						bufnum = 0; \
						unit->m_buf = world->mSndBufs + bufnum; \
					} \
				} else { \
					unit->m_buf = world->mSndBufs + bufnum; \
				} \
				unit->m_fbufnum = fbufnum; \
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
	float *in[64]; \
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
	
	unit->m_maypost = (unit->mWorld->mVerbosity >= 0);
	
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
	
	// Stuff a la BufWr - NB I have modified GET_BUF slightly
	GET_BUF_ALTERED
	CHECK_BUF
	SETUP_IN(3)
	
	float* table0 = bufData + writepos;
	
	// First, handle reset
	if(justInitialised || (reset > 0.f && prevreset <= 0.f)){
		writepos = 0;
		unit->m_notfull = true;
		memset(bufData, 0, bufChannels * bufFrames * sizeof(float));
	}
	
	// Now check for trigger
	if(unit->m_notfull && trig > 0.f && prevtrig <= 0.f){
		if(writepos == bufChannels * bufFrames){
			unit->m_notfull = false;
			if(unit->m_maypost){
				Print("Logger.kr warning: Buffer full, dropped values: first channel %g\n", *in[0]);
			}
		}else{
			for(uint32 i=0; i<numInputs; ++i){
				table0[i] = *++(in[i]);
			}
			writepos += numInputs;
		}
	}
	
	// Store state
	unit->m_prevtrig = trig;
	unit->m_prevreset = reset;
	unit->m_writepos = writepos;
	
	ZOUT0(0) = unit->m_notfull ? 1.f : 0.f;
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

////////////////////////////////////////////////////////////////////

/** ListTrig2 by nescivi
Does the same as ListTrig but instead of absolute times the buffer contains intervals
*/

void ListTrig2_Ctor(ListTrig2* unit)
{
	SETCALC(ListTrig2_next);
	
	unit->m_fbufnum = -1e9f;
	
	unit->m_prevreset = 0.f;
	unit->m_bufpos = 0;
	unit->m_timepos = 0.0;
	unit->m_timeincrement = (double)BUFDUR;
	
	//Print("ListTrig: time increment set to %g, i.e. freq of %g/s", unit->m_timeincrement, 1.0/unit->m_timeincrement);
	
	ClearUnitOutputs(unit, 1);
}

void ListTrig2_next(ListTrig2 *unit, int inNumSamples)
{
	float reset = ZIN0(1);
	unsigned int numframes = (unsigned int)ZIN0(2);
	
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
		timepos = 0.0;
	}
	
	if(bufpos<numframes){
		float* table0 = bufData + bufpos;
		
		if(table0[0] <= (float)timepos){
			out = 1.f;
			// reset timepos to zero
			timepos = 0.f;
			// Also increment buffer read position until we're either at the end of the buffer, or we've found a "future" value
			if( bufpos<numframes ){
				bufpos++;
			}
		}
	}
	
	// Store state
	unit->m_prevreset = reset;
	unit->m_bufpos = bufpos;
	unit->m_timepos = timepos + timeinc; // Shift time on to what it will be on the next go
	
	ZOUT0(0) = out;
}

////////////////////////////////////////////////////////////////////

void GaussClass_Ctor(GaussClass* unit)
{
	SETCALC(GaussClass_next);
	
	// The dimensionality is specified by the dimensionality of inputs, appended to params
	int numdims = unit->mNumInputs - 2;
	unit->m_numdims = numdims;
	unit->m_numclasses = 0; // This will be filled in when the buffer first arrives
	unit->m_numnumsperclass = numdims*numdims + numdims + 1;
	
	unit->m_indata  = (float*)RTAlloc(unit->mWorld, numdims * sizeof(float));
	unit->m_centred = (float*)RTAlloc(unit->mWorld, numdims * sizeof(float));
	
	unit->m_result = 0.f;
	unit->m_fbufnum = -1e9f;
	
	ClearUnitOutputs(unit, 1);
}

// Exponent for any Gaussian PDF. The (inverted) covariance matrix is in row-first order.
inline double GaussClass_exponent(const int numdims, const float *centred, const float *invcov);
inline double GaussClass_exponent(const int numdims, const float *centred, const float *invcov){
	int covpos = -1;
	double sum=0., partial;
	for(int i=0; i<numdims; ){
		partial=0.;
		for(int j=0; j<numdims; ){
			partial += centred[j++] * invcov[++covpos];
		}
		sum += partial * centred[i++];
	}
	return sum * -0.5;
}
void GaussClass_next(GaussClass *unit, int inNumSamples)
{
	if(ZIN0(1)>0.f){ // If gate>0
		
		int numdims = unit->m_numdims;
		int numnumsperclass  = unit->m_numnumsperclass;
		
		// Do the GET_BUF-like bit
		float fbufnum  = ZIN0(0);
		if (fbufnum != unit->m_fbufnum) {
			uint32 bufnum = (int)fbufnum;
			World *world = unit->mWorld;
			if (bufnum >= world->mNumSndBufs) bufnum = 0;
			unit->m_fbufnum = fbufnum;
			unit->m_buf = world->mSndBufs + bufnum;
			uint32 bufFrames = unit->m_buf->frames;
			
			if(unit->m_buf->channels != 1 && world->mVerbosity > -1){
				Print("GaussClass: warning, Buffer should be single-channel\n");
			}
			// Infer the number of classes:
			unit->m_numclasses = bufFrames / numnumsperclass;
		}
		SndBuf *buf = unit->m_buf;
		float *bufData = buf->data;
		
		CHECK_BUF
		
		int numclasses  = unit->m_numclasses;
		float *indata   = unit->m_indata;
		float *centred  = unit->m_centred;
		
		// Grab  the input data
		for(int i=0; i<numdims; ++i){
			indata[i] = ZIN0(i + 2);
		}
				
		// Locations of the (first) class's data, these will be incremented
		float *mean                 = bufData;
		float *invcov               = bufData + numdims;
		float *weightoversqrtdetcov = bufData + numnumsperclass - 1;
		
		// Iterate the classes, calculating the score
		int winningclass=0;
		double winningclassscore=0.;
		double curscore;
		for(int i=0; i<numclasses; ++i){
			// Centre the input data on the current gaussian
			for(int j=0; j<numdims; ++j){
				centred[j] = indata[j] - mean[j];
			}
			
			// Now calculate the score, see if we've won
			curscore = (*weightoversqrtdetcov)
					* exp(GaussClass_exponent(numdims, centred, invcov));
			if(curscore > winningclassscore){
				winningclassscore = curscore;
				winningclass      = i;
			}
			
			// Increment pointers for the next class
			mean                 += numnumsperclass;
			invcov               += numnumsperclass;
			weightoversqrtdetcov += numnumsperclass;
		}
		
		// Store the winner
		unit->m_result = (float)winningclass;
		
	} // end gate check
	
	ZOUT0(0) = unit->m_result;
}
void GaussClass_Dtor(GaussClass* unit)
{
	RTFree(unit->mWorld, unit->m_indata );
	RTFree(unit->mWorld, unit->m_centred);
}

//////////////////////////////////////////////////////////////////

void load(InterfaceTable *inTable)
{
	ft = inTable;

	DefineSimpleUnit(Logger);
	DefineSimpleUnit(ListTrig);
	DefineSimpleUnit(ListTrig2);
	DefineDtorUnit(GaussClass);
}
