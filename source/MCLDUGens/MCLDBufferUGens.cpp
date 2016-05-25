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

struct BufMax : public Unit
{
	float m_fbufnum;
	SndBuf *m_buf;

	float m_bestval;
	float m_bestpos;
};

struct BufMin : BufMax {};

struct ArrayMax : public Unit {};
struct ArrayMin : ArrayMax {};

/*
const size_t MIDelay_numbins = 6;
struct MIDelay : public Unit
{
	uint32 *m_xbins, *m_ybins, *m_xybins;
	int m_mindly, m_maxdly;
	float m_bestval, m_bestpos;
	float *m_in1, *m_in2, *m_cutoffs1, *m_cutoffs2;
	size_t m_inbufsize;
};
*/


//////////////////////////////////////////////////////////////////

extern "C"
{
	void Logger_Ctor(Logger* unit);
	void Logger_next(Logger *unit, int inNumSamples);

	void ListTrig_Ctor(ListTrig* unit);
	void ListTrig_next(ListTrig *unit, int inNumSamples);

	void ListTrig2_Ctor(ListTrig2* unit);
	void ListTrig2_next(ListTrig2 *unit, int inNumSamples);

	void GaussClass_Ctor(GaussClass* unit);
	void GaussClass_next(GaussClass *unit, int inNumSamples);
	void GaussClass_Dtor(GaussClass* unit);

	void BufMax_Ctor(BufMax* unit);
	void BufMax_next(BufMax *unit, int inNumSamples);

	void BufMin_Ctor(BufMin* unit);
	void BufMin_next(BufMin *unit, int inNumSamples);

	void ArrayMax_Ctor(ArrayMax* unit);
	void ArrayMax_next(ArrayMax *unit, int inNumSamples);

	void ArrayMin_Ctor(ArrayMin* unit);
	void ArrayMin_next(ArrayMin *unit, int inNumSamples);

	//void MIDelay_Ctor(MIDelay* unit);
	//void MIDelay_next(MIDelay *unit, int inNumSamples);
	//void MIDelay_Dtor(MIDelay* unit);

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

////////////////////////////////////////////////////////////////////

void BufMax_Ctor(BufMax* unit)
{
	SETCALC(BufMax_next);
	unit->m_fbufnum = -1e9f;
	unit->m_bestval = 0.f;
	unit->m_bestpos = 0;
	BufMax_next(unit, 1);
}

void BufMax_next(BufMax *unit, int inNumSamples)
{
	bool gate = ZIN0(1) > 0.f;

	GET_BUF
	CHECK_BUF

//	Print("BufMax: fbufnum %g, gate %i\n", fbufnum, gate);

	float bestval = unit->m_bestval;
	uint32 bestpos = unit->m_bestpos;

	if(gate){
		bestval = -INFINITY;
		bestpos = 0;
		for(uint32 i=0; i<bufSamples; ++i){
			if(bestval < bufData[i]){
				bestval = bufData[i];
				bestpos = i;
			}
		}
		// Store result
		unit->m_bestval = bestval;
		unit->m_bestpos = bestpos;
	}

	ZOUT0(0) = bestval;
	ZOUT0(1) = bestpos;
}

////////////////////////////////////////////////////////////////////

void BufMin_Ctor(BufMin* unit)
{
	SETCALC(BufMin_next);
	unit->m_fbufnum = -1e9f;
	unit->m_bestval = 0.f;
	unit->m_bestpos = 0;
	BufMin_next(unit, 1);
}

void BufMin_next(BufMin *unit, int inNumSamples)
{
	bool gate = ZIN0(1) > 0.f;

	GET_BUF
	CHECK_BUF

	float bestval = unit->m_bestval;
	uint32 bestpos = unit->m_bestpos;

	if(gate){
		bestval = INFINITY;
		bestpos = 0;
		for(uint32 i=0; i<bufSamples; ++i){
			if(bestval > bufData[i]){
				bestval = bufData[i];
				bestpos = i;
			}
		}
		// Store result
		unit->m_bestval = bestval;
		unit->m_bestpos = bestpos;
	}

	ZOUT0(0) = bestval;
	ZOUT0(1) = bestpos;
}

////////////////////////////////////////////////////////////////////

void ArrayMax_Ctor(ArrayMax* unit)
{
	SETCALC(ArrayMax_next);
	ArrayMax_next(unit, 1);
}

void ArrayMax_next(ArrayMax *unit, int inNumSamples)
{
	float *out0 = ZOUT(0);
	float *out1 = ZOUT(1);
	uint16 numInputs = unit->mNumInputs;

	float val, bestval;
	uint16 bestpos;
	for(int j=0; j<inNumSamples; ++j){
		bestval = -INFINITY;
		bestpos = 0;
		for(uint16 i=0; i<numInputs; ++i){
			val = IN(i)[j];
			if(bestval < val){
				bestval = val;
				bestpos = i;
			}
		}
		ZXP(out0) = bestval;
		ZXP(out1) = (float)bestpos;
	}
}

//////////

void ArrayMin_Ctor(ArrayMin* unit)
{
	SETCALC(ArrayMin_next);
	ArrayMin_next(unit, 1);
}

void ArrayMin_next(ArrayMin *unit, int inNumSamples)
{
	float *out0 = ZOUT(0);
	float *out1 = ZOUT(1);
	uint16 numInputs = unit->mNumInputs;

	float val, bestval;
	uint16 bestpos;
	for(int j=0; j<inNumSamples; ++j){
		bestval = INFINITY;
		bestpos = 0;
		for(uint16 i=0; i<numInputs; ++i){
			val = IN(i)[j];
			if(bestval > val){
				bestval = val;
				bestpos = i;
			}
		}
		ZXP(out0) = bestval;
		ZXP(out1) = (float)bestpos;
	}
}

////////////////////////////////////////////////////////////////////
/*
void MIDelay_Ctor(MIDelay* unit)
{
	SETCALC(MIDelay_next);

	unit->m_bestval = 0.f;
	unit->m_bestpos = 0.f;
	// Decide (in samples) the lowest and highest delay, in samples (can be +ve or -ve)
	unit->m_mindly = 0;//(int)(ZIN0(2) * SAMPLERATE);
	unit->m_maxdly = (int)(ZIN0(2) * FULLRATE);
	// Allocate the buffers for incoming audio
	unit->m_inbufsize = sc_max(unit->m_maxdly, 0 - unit->m_mindly) * 2;
	unit->m_in1 = (float*)RTAlloc(unit->mWorld, unit->m_inbufsize * sizeof(float));
	unit->m_in2 = (float*)RTAlloc(unit->mWorld, unit->m_inbufsize * sizeof(float));
	// Allocate space for the bins
	unit->m_xbins  = (uint32*)RTAlloc(unit->mWorld, MIDelay_numbins * sizeof(uint32));
	unit->m_ybins  = (uint32*)RTAlloc(unit->mWorld, MIDelay_numbins * sizeof(uint32));
	unit->m_xybins = (uint32*)RTAlloc(unit->mWorld, MIDelay_numbins * MIDelay_numbins * sizeof(uint32));
	// and for the cutoffs
	unit->m_cutoffs1  = (float*)RTAlloc(unit->mWorld, MIDelay_numbins * sizeof(float));
	unit->m_cutoffs2  = (float*)RTAlloc(unit->mWorld, MIDelay_numbins * sizeof(float));

	ClearUnitOutputs(unit, 1);
}

void MIDelay_next(MIDelay *unit, int inNumSamples)
{
	bool gate = ZIN0(3) > 0.f;

	// Add new samples to the storage buffers
	// (NB here we assume the buffers are at least large enough to hold inNumSamples)
	float* in1 = unit->m_in1;
	float* in2 = unit->m_in2;
	size_t inbufsize = unit->m_inbufsize;
	// inNumSamples isn't right!
	inNumSamples = unit->mWorld->mFullRate.mBufLength;
	memmove(in1, in1 + inNumSamples, (inbufsize - inNumSamples) * sizeof(float));
	memmove(in2, in2 + inNumSamples, (inbufsize - inNumSamples) * sizeof(float));
	Copy(inNumSamples, in1 + (inbufsize - inNumSamples), IN(0));
	Copy(inNumSamples, in2 + (inbufsize - inNumSamples), IN(1));

	if(gate){
		// Iterate over the two buffers to find min and max
		float min1 = INFINITY, min2 = INFINITY, max1 = -INFINITY, max2=-INFINITY;
		for(size_t i=0; i<inbufsize; ++i){
			if(min1 > in1[i]) min1 = in1[i];
			if(min2 > in2[i]) min2 = in2[i];
			if(max1 < in1[i]) max1 = in1[i];
			if(max2 < in2[i]) max2 = in2[i];
		}
//		Print("min1 %g max1 %g min2 %g max2 %g\n", min1, max1, min2, max2);

		if(min1==max1){
		}else if(min2==max2){
		}else{

			// Establish the lists of boundaries
			float* cutoffs1 = unit->m_cutoffs1;
			float* cutoffs2 = unit->m_cutoffs2;
			for(size_t i=0; i<MIDelay_numbins; ++i){
				cutoffs1[i] = min1 + (max1 - min1) * (i+1) / MIDelay_numbins;
				cutoffs2[i] = min2 + (max2 - min2) * (i+1) / MIDelay_numbins;
//				Print("cutoffs1[%i] = %g, cutoffs2[%i] = %g\n", i, cutoffs1[i], i, cutoffs2[i]);
			}
//			Print("Expected total: %u\n", inbufsize);

			uint32* xbins  = unit->m_xbins;
			uint32* ybins  = unit->m_ybins;
			// First zero the 1D bins
			memset(xbins, 0, MIDelay_numbins);
			memset(ybins, 0, MIDelay_numbins);
			// Iterate over the joint buffers to accumulate the marginal bin subtotals
			for(size_t i=0; i<inbufsize; ++i){
				size_t x=0, y=0;
				while(in1[i] > cutoffs1[x]) ++x;
				while(in2[i] > cutoffs2[y]) ++y;
				++xbins[x];
				++ybins[y];
			}

			uint32* xybins = unit->m_xybins;
			int mindly = unit->m_mindly, maxdly = unit->m_maxdly;
			double bestmi=-INFINITY;
			int bestdelta=0;
			// Foreach shiftval:
			for(int delta = mindly; delta < maxdly; ++delta){
				// Zero the 2D bins
				memset(xybins, 0, MIDelay_numbins * MIDelay_numbins);
//		Print("xybins[0][0]: %u\n", *xybins);

				// Get pointers to the buffers, shunted so as to represent the time shift
				float *in1win, *in2win;
				size_t winsize;
				if(delta < 0){
					in1win = in1 - delta;
					in2win = in2;
					winsize = inbufsize + delta;
				}else{
					in1win = in1;
					in2win = in2 + delta;
					winsize = inbufsize - delta;
				}

				// Iterate along a suitable region of the buffers, incrementing the 2D bins
				uint32 x, y;
				for(size_t i=0; i<winsize; ++i){
					x=0, y=0;
					while(in1win[i] > cutoffs1[x]) ++x;
					while(in2win[i] > cutoffs2[y]) ++y;
//					if(delta==0){
//						printf("Item [%g, %g] goes in bin [%i, %i] (not smaller )\n", in1win[i], in2win[i], x, y);
//					}
//		Print("increment xybins[%u][%u] from value of %u\n", x, y, xybins[x * MIDelay_numbins + y]);
					// note, x-val (chan 1) is the big leap
					++xybins[x * MIDelay_numbins + y];
				}

				// Iterate the 2D bins, calculating the MI value
				double mi = 0.;
				for(size_t x=0; x < MIDelay_numbins; ++x){
					for(size_t y=0; y < MIDelay_numbins; ++y){
						// MI = sum of   p(x,y)   log [   p(x,y) / p(x)p(y) ]
						//    = sum of   n(x,y)/winsize log [ N N n(x,y) / n(x)n(y)winsize ]
//						if(delta==0){
//							printf("%u,", xybins[x * MIDelay_numbins + y]);
//						}
						if(xybins[x * MIDelay_numbins + y] != 0){
						//	double logthing = winsize * xybins[x * MIDelay_numbins + y] / (xbins[x] * ybins[y]);
						//	mi += (xybins[x * MIDelay_numbins + y] / winsize)
						//		* log(logthing);

							double logthing = inbufsize * (double)inbufsize * (double)xybins[x * MIDelay_numbins + y]
										/
									(xbins[x] * ybins[y] * (double)winsize);
							mi += ((double)xybins[x * MIDelay_numbins + y] / (double)winsize)
								* log(logthing);
						}
					}
//					if(delta==0){
//						printf("\n");
//					}
				}
				Print("MI[%i]\t  %g \n", delta, mi);
				// If the MI value is the highest so far, store it and the current shift
				if(bestmi < mi){
					bestmi = mi;
					bestdelta = delta;
				}
			} // end foreach shiftval
			Print("Best MI was %g at offset %i (offset range was [%i, %i])\n", bestmi, bestdelta, mindly, maxdly);
			//unit->m_bestval = bestmi;
			unit->m_bestpos = bestdelta / SAMPLERATE;

		} // end check for nonzero range
	}
	ZOUT0(0) = unit->m_bestpos;
}

void MIDelay_Dtor(MIDelay* unit)
{
	if(unit->m_xbins){
		RTFree(unit->mWorld, unit->m_xbins );
		RTFree(unit->mWorld, unit->m_ybins );
		RTFree(unit->mWorld, unit->m_xybins);
		RTFree(unit->mWorld, unit->m_in1);
		RTFree(unit->mWorld, unit->m_in2);
		RTFree(unit->mWorld, unit->m_cutoffs1);
		RTFree(unit->mWorld, unit->m_cutoffs2);
	}
}
*/

//////////////////////////////////////////////////////////////////

PluginLoad(MCLDBuffer)
{
	ft = inTable;

	DefineSimpleUnit(Logger);
	DefineSimpleUnit(ListTrig);
	DefineSimpleUnit(ListTrig2);
	DefineDtorUnit(GaussClass);
	DefineSimpleUnit(BufMax);
	DefineSimpleUnit(BufMin);
	DefineSimpleUnit(ArrayMax);
	DefineSimpleUnit(ArrayMin);
	//DefineDtorUnit(MIDelay);
}
