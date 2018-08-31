#include "SC_PlugIn.h"
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

#include "BetaBlockerUGens.h"




// InterfaceTable contains pointers to functions in the host (server).
static InterfaceTable *ft;

struct DetaBlockerBuf : public Unit
{
	DetaBlockerBuf(void) : bblocker()
		{}

	~DetaBlockerBuf(void)
		{}

	machine bblocker;

	
	float m_fbufnum;
	SndBuf *m_buf;
};


// declare unit generator functions

void DetaBlockerBuf_Ctor(DetaBlockerBuf *unit);
void DetaBlockerBuf_Dtor(DetaBlockerBuf *unit);
void DetaBlockerBuf_next(DetaBlockerBuf *unit, int inNumSamples);

//////////////// DetaBlockerBuf //////////////////////////////////////////////


void DetaBlockerBuf_Ctor(DetaBlockerBuf *unit)
{
	SETCALC(DetaBlockerBuf_next);

	new(unit) DetaBlockerBuf();
	unit->bblocker.init_thread(0);

	unit->m_fbufnum = -1e9f;

	DetaBlockerBuf_next(unit, 0);
	OUT0(0) = 0.f;
}

void DetaBlockerBuf_Dtor(DetaBlockerBuf *unit)
{
	unit->~DetaBlockerBuf();
	
}


void DetaBlockerBuf_next(DetaBlockerBuf *unit, int inNumSamples)
{

	DETA_GET_BUF
	DETA_CHECK_BUF
	
	machine &m = unit->bblocker;

	
	if (inNumSamples) {

		float z;
		
		// cast bufData (float()) to u8 and put it into machine's heap. For now, I assume the buffer to be 256 elements (HEAP_SIZE)
		// buffer data should be 0 .. 255
		
		for(size_t i = 0; i < HEAP_SIZE; i++)
		{
			m.m_heap[i] = (u8) bufData[i];
		}
		
		m.run();
		
		for(size_t i = 0; i < HEAP_SIZE; i++)
		{
			bufData[i] = (float) m.m_heap[i];
		}

		z = ((float) m.get_thread().top() / 127.f) - 1.f;

		// write the output
		OUT0(0) = z;
	} else {

		// release thread
		m.get_thread().set_active(false);

		float x = DEMANDINPUT_A(1, 1);

		if (!sc_isnan(x)) {
			u8 startpos = (u8) x;

			// get new thread with start value as written in scnd input;
			m.init_thread(startpos);
			
		}

	}
}


/////////// BBlockerBuf ///////////////////

struct BBlockerBuf : public Unit
{
	BBlockerBuf(void) : bblocker()
		{}

	~BBlockerBuf(void)
		{}

	machine bblocker;

	
	float  m_fbufnum;
	double m_phase;
	float  m_freqMul;
	float  m_lastFreq;
	SndBuf *m_buf;
};

void BBlockerBuf_Ctor(BBlockerBuf *unit);
void BBlockerBuf_Dtor(BBlockerBuf *unit);
void BBlockerBuf_next_i(BBlockerBuf *unit, int inNumSamples);
void BBlockerBuf_next_a(BBlockerBuf *unit, int inNumSamples);
void BBlockerBuf_next_k(BBlockerBuf *unit, int inNumSamples);


void BBlockerBuf_Ctor(BBlockerBuf *unit)
{
	new(unit) BBlockerBuf();
	unit->bblocker.init_thread(0);

	unit->m_fbufnum = -1e9f;
	unit->m_phase = 1.f;
	unit->m_freqMul = unit->mRate->mSampleDur;
	unit->m_lastFreq = ZIN0(1);
	
	// TODO: set initial start adress

	// SETCALC(BBlockerBuf_next_a);
	if(INRATE(1) == calc_ScalarRate) { // freq is not a scalar
// printf("freq is SCALAR     : %d\n", INRATE(1));
		SETCALC(BBlockerBuf_next_i);
		BBlockerBuf_next_i(unit, 1);
	} else if (INRATE(1) == calc_FullRate) {
// printf("freq is FULLRATE   : %d\n", INRATE(1));
		SETCALC(BBlockerBuf_next_a);
		BBlockerBuf_next_a(unit, 1);
	} else {
// printf("freq is CONTROLRATE: %d\n", INRATE(1));
		SETCALC(BBlockerBuf_next_k);
		BBlockerBuf_next_k(unit, 1);
	}

//	OUT0(0) = 0.f;
}

void BBlockerBuf_Dtor(BBlockerBuf *unit)
{
	unit->~BBlockerBuf();
	
}

static inline bool bbcheckBuffer(Unit * unit, const float * bufData, uint32 bufChannels, uint32 expectedChannels, int inNumSamples) {
	if (!bufData){
		goto handle_failure;
	}
	if (expectedChannels > bufChannels) {
		if(unit->mWorld->mVerbosity > -1 && !unit->mDone){
			Print("Buffer UGen channel mismatch: expected %i, yet buffer has %i channels\n",
				  expectedChannels, bufChannels);
		}
		goto handle_failure;
	}
	return true;

handle_failure:
	unit->mDone = true;
	ClearUnitOutputs(unit, inNumSamples);
	return false;
}



void BBlockerBuf_next_i(BBlockerBuf *unit, int inNumSamples) {
	// buffer access
	GET_BUF
	uint32 numInputChannels = 1; // input should be one channel.
	if (!bbcheckBuffer(unit, bufData, bufChannels, numInputChannels, inNumSamples))
		return;

	// get info from unit
	machine &m = unit->bblocker;
	// thread  t  = m.get_thread();

	float *pCOut       = ZOUT(0);

	float *stackOut0  = ZOUT(1);
	float *stackOut1  = ZOUT(2);
	float *stackOut2  = ZOUT(3);
	float *stackOut3  = ZOUT(4);
	float *stackOut4  = ZOUT(5);
	float *stackOut5  = ZOUT(6);
	float *stackOut6  = ZOUT(7);
	float *stackOut7  = ZOUT(8);

	float  freq    = ZIN0(1);
	double phase   = unit->m_phase;
	float  freqmul = unit->m_freqMul;

	// get heap from buffer
	/* 		Cast bufData (float()) to u8 and put it into machine's heap. 
			For now, I assume the buffer to be HEAP_SIZE elements (i.e. 256). 
			Buffer items should range between 0 and 255.	*/
	for(size_t i = 0; i < HEAP_SIZE; i++) {
		m.m_heap[i] = (u8) bufData[i];
	}

	// compute samples
	LOOP1(inNumSamples,
		if (phase >= 1.f) {
			//printf("running: %f (%e * %f)\n", phase, ZXP(freq), freqmul);
			phase -= 1.f;
			m.run();
		}
		phase += freq * freqmul;

//		printf("1: %d  %d\n", m.get_thread().m_pc, m.get_thread().at(0));

		// out must be written last for in place operation
		ZXP(pCOut)     = ((float) m.get_thread().m_pc  / 127.f) - 1.f;
		ZXP(stackOut0) = ((float) m.get_thread().at(0) / 127.f) - 1.f;
		ZXP(stackOut1) = ((float) m.get_thread().at(1) / 127.f) - 1.f;
		ZXP(stackOut2) = ((float) m.get_thread().at(2) / 127.f) - 1.f;
		ZXP(stackOut3) = ((float) m.get_thread().at(3) / 127.f) - 1.f;
		ZXP(stackOut4) = ((float) m.get_thread().at(4) / 127.f) - 1.f;
		ZXP(stackOut5) = ((float) m.get_thread().at(5) / 127.f) - 1.f;
		ZXP(stackOut6) = ((float) m.get_thread().at(6) / 127.f) - 1.f;
		ZXP(stackOut7) = ((float) m.get_thread().at(7) / 127.f) - 1.f;
	)

	// write back to unit, resp. buffer
	for(size_t i = 0; i < HEAP_SIZE; i++) {
		bufData[i] = (float) m.m_heap[i];
	}
	unit->m_phase = phase;
} // end BBlockerBuf_next_i

void BBlockerBuf_next_a(BBlockerBuf *unit, int inNumSamples) {
	// buffer access
	GET_BUF
	uint32 numInputChannels = 1; // input should be one channel.
	if (!bbcheckBuffer(unit, bufData, bufChannels, numInputChannels, inNumSamples))
		return;

	// get info from unit
	machine &m = unit->bblocker;
	// thread  t  = m.get_thread();

	float *pCOut       = ZOUT(0);

	float *stackOut0  = ZOUT(1);
	float *stackOut1  = ZOUT(2);
	float *stackOut2  = ZOUT(3);
	float *stackOut3  = ZOUT(4);
	float *stackOut4  = ZOUT(5);
	float *stackOut5  = ZOUT(6);
	float *stackOut6  = ZOUT(7);
	float *stackOut7  = ZOUT(8);

	float *freq    = ZIN(1);
	double phase   = unit->m_phase;
	float freqmul  = unit->m_freqMul;

	// get heap from buffer
	/* 		Cast bufData (float()) to u8 and put it into machine's heap. 
			For now, I assume the buffer to be HEAP_SIZE elements (i.e. 256). 
			Buffer items should range between 0 and 255.	*/
	for(size_t i = 0; i < HEAP_SIZE; i++) {
		m.m_heap[i] = (u8) bufData[i];
	}

	// compute samples
	LOOP1(inNumSamples,
		if (phase >= 1.f) {
			phase -= 1.f;
			m.run();
		}
		
		phase += ZXP(freq) * freqmul;

//		printf("1: %d  %d\n", m.get_thread().m_pc, m.get_thread().at(0));

		// out must be written last for in place operation
		ZXP(pCOut)     = ((float) m.get_thread().m_pc  / 127.f) - 1.f;
		ZXP(stackOut0) = ((float) m.get_thread().at(0) / 127.f) - 1.f;
		ZXP(stackOut1) = ((float) m.get_thread().at(1) / 127.f) - 1.f;
		ZXP(stackOut2) = ((float) m.get_thread().at(2) / 127.f) - 1.f;
		ZXP(stackOut3) = ((float) m.get_thread().at(3) / 127.f) - 1.f;
		ZXP(stackOut4) = ((float) m.get_thread().at(4) / 127.f) - 1.f;
		ZXP(stackOut5) = ((float) m.get_thread().at(5) / 127.f) - 1.f;
		ZXP(stackOut6) = ((float) m.get_thread().at(6) / 127.f) - 1.f;
		ZXP(stackOut7) = ((float) m.get_thread().at(7) / 127.f) - 1.f;
	)

	// write back to unit, resp. buffer
	for(size_t i = 0; i < HEAP_SIZE; i++) {
		bufData[i] = (float) m.m_heap[i];
	}
	unit->m_phase = phase;
	
} // end BBlockerBuf_next_a

void BBlockerBuf_next_k (BBlockerBuf *unit, int inNumSamples) {
	// buffer access
	GET_BUF
	uint32 numInputChannels = 1; // input should be one channel.
	if (!bbcheckBuffer(unit, bufData, bufChannels, numInputChannels, inNumSamples))
		return;

	// get info from unit
	machine &m = unit->bblocker;
	// thread  t  = m.get_thread();

	float *pCOut       = ZOUT(0);

	float *stackOut0  = ZOUT(1);
	float *stackOut1  = ZOUT(2);
	float *stackOut2  = ZOUT(3);
	float *stackOut3  = ZOUT(4);
	float *stackOut4  = ZOUT(5);
	float *stackOut5  = ZOUT(6);
	float *stackOut6  = ZOUT(7);
	float *stackOut7  = ZOUT(8);

	float freq     = ZIN0(1);
	float lastFreq = unit->m_lastFreq;
	float freqmul  = unit->m_freqMul;
	double phase   = unit->m_phase;

	// get heap from buffer
	/* 		Cast bufData (float()) to u8 and put it into machine's heap. 
			For now, I assume the buffer to be HEAP_SIZE elements (i.e. 256). 
			Buffer items should range between 0 and 255.	*/
	for(size_t i = 0; i < HEAP_SIZE; i++) {
		m.m_heap[i] = (u8) bufData[i];
	}

	// compute samples
	LOOP1(inNumSamples,
		if (phase >= 1.f) {
			//printf("running: %f (%e * %f)\n", phase, ZXP(freq), freqmul);
			phase -= 1.f;
			m.run();
		}
		float freqslope  = CALCSLOPE(freq, lastFreq);
		phase += (freq+freqslope) * freqmul;

//		printf("1: %d  %d\n", m.get_thread().m_pc, m.get_thread().at(0));

		// out must be written last for in place operation
		ZXP(pCOut)     = ((float) m.get_thread().m_pc  / 127.f) - 1.f;
		ZXP(stackOut0) = ((float) m.get_thread().at(0) / 127.f) - 1.f;
		ZXP(stackOut1) = ((float) m.get_thread().at(1) / 127.f) - 1.f;
		ZXP(stackOut2) = ((float) m.get_thread().at(2) / 127.f) - 1.f;
		ZXP(stackOut3) = ((float) m.get_thread().at(3) / 127.f) - 1.f;
		ZXP(stackOut4) = ((float) m.get_thread().at(4) / 127.f) - 1.f;
		ZXP(stackOut5) = ((float) m.get_thread().at(5) / 127.f) - 1.f;
		ZXP(stackOut6) = ((float) m.get_thread().at(6) / 127.f) - 1.f;
		ZXP(stackOut7) = ((float) m.get_thread().at(7) / 127.f) - 1.f;
	)

	// write back to unit, resp. buffer
	for(size_t i = 0; i < HEAP_SIZE; i++) {
		bufData[i] = (float) m.m_heap[i];
	}
	unit->m_phase = phase;
	
} // end BBlockerBuf_next_k



//////////////////////////////

// the entry point is called by the host when the plug-in is loaded
PluginLoad(Betablocker)
{
	// InterfaceTable *inTable implicitly given as argument to the load function
	ft = inTable; // store pointer to InterfaceTable
	DefineDtorUnit(DetaBlockerBuf);
	DefineDtorUnit(BBlockerBuf);
}