/*
 *  JoshPVUGens.cpp
 *  xSC3plugins
 *
 *  Created by Josh Parmenter on 9/27/05.
 *  Copyright 2005 __MyCompanyName__. All rights reserved.
 *
 */

//third party Phase Vocoder UGens

#include "FFT_UGens.h"
#include "SCComplex.h"
#include "SC_PlugIn.h"

// macros to put rgen state in registers
#define RGET \
	RGen& rgen = *unit->mParent->mRGen; \
	uint32 s1 = rgen.s1; \
	uint32 s2 = rgen.s2; \
	uint32 s3 = rgen.s3; 

#define RPUT \
	rgen.s1 = s1; \
	rgen.s2 = s2; \
	rgen.s3 = s3;

InterfaceTable *ft;	

/* PV work */

//struct PV_Unit : Unit
//{
//};

struct PV_NoiseSynthP : PV_Unit
{
    int m_numFrames, m_numLoops, m_remainingLoops, m_curframe, m_numframes, m_numbins, m_nextflag;
    float *m_phases;
    float *m_phasedifs;
    float *m_prevphasedifs;
};

struct PV_PartialSynthP : PV_Unit
{
    int m_numFrames, m_numLoops, m_remainingLoops, m_curframe, m_numframes, m_numbins, m_nextflag;
    float *m_phases;
    float *m_phasedifs;
    float *m_prevphasedifs;
};

struct PV_PartialSynthF : PV_Unit
{
    int m_numFrames, m_numLoops, m_remainingLoops, m_curframe, m_numframes, m_numbins, m_nextflag;
    float *m_phases;
    float *m_freqs;
    float *m_centerfreqs;
    };

struct PV_NoiseSynthF : PV_Unit
{
    int m_numFrames, m_numLoops, m_remainingLoops, m_curframe, m_numframes, m_numbins, m_nextflag;
    float *m_phases;
    float *m_freqs;
    float *m_centerfreqs;
    };
    
struct PV_MagMap : PV_Unit
{
    // contains a buffer with mag rescaling
    float m_fmagbufnum;
    SndBuf *m_magbuf;
    };
    
struct PV_MaxMagN : PV_Unit { };

struct PV_MinMagN : PV_Unit { };

struct PV_FreqBuffer : PV_Unit 
{
    SndBuf *m_databuf;
    float m_fdatabufnum;
    int m_numloops, m_numbins, m_firstflag;
    float *m_phases;
    float *m_centerfreqs;
    float *m_freqs;
};

struct PV_MagBuffer : PV_Unit 
{ 
    SndBuf *m_databuf;
    float m_fdatabufnum;
    int m_numloops;

};

struct PV_OddBin : PV_Unit {};

struct PV_EvenBin : PV_Unit {};

struct PV_Invert : PV_Unit {};

const int MAXDELAYBUFS = 512;

struct PV_BinDelay : PV_Unit 
{
    SCComplexBuf *m_databuf[MAXDELAYBUFS];
    SndBuf *m_deltimes; // the buffer holding the delay ammounts
    SndBuf *m_fb; // the biffer holding the fedback values
    float m_deltimesbufnum, m_fbbufnum;
    float m_srbins;
    int m_numFrames, m_curFrame, m_elapsedFrames;
    int dataFlag[MAXDELAYBUFS];
};

struct PV_Freeze : Unit
{
	int m_numbins;
	float *m_mags, m_dc, m_nyq;
	float *m_prevPhases, *m_difPhases;
};


extern "C"
{
	#include "fftlib.h"

	void load(InterfaceTable *inTable);

	void PV_NoiseSynthP_Ctor(PV_NoiseSynthP *unit);
	void PV_NoiseSynthP_Dtor(PV_NoiseSynthP *unit);
	void PV_NoiseSynthP_first(PV_NoiseSynthP* unit, int inNumSamples);
	void PV_NoiseSynthP_next_z(PV_NoiseSynthP* unit, int inNumSamples);
	void PV_NoiseSynthP_next(PV_NoiseSynthP* unit, int inNumSamples);
	
	void PV_PartialSynthP_Ctor(PV_PartialSynthP *unit);
	void PV_PartialSynthP_Dtor(PV_PartialSynthP *unit);
	void PV_PartialSynthP_first(PV_PartialSynthP* unit, int inNumSamples);
	void PV_PartialSynthP_next_z(PV_PartialSynthP* unit, int inNumSamples);
	void PV_PartialSynthP_next(PV_PartialSynthP* unit, int inNumSamples);
	
	void PV_PartialSynthF_Ctor(PV_PartialSynthF *unit);
	void PV_PartialSynthF_Dtor(PV_PartialSynthF *unit);
	void PV_PartialSynthF_first(PV_PartialSynthF* unit, int inNumSamples);
	void PV_PartialSynthF_next_z(PV_PartialSynthF* unit, int inNumSamples);
	void PV_PartialSynthF_next(PV_PartialSynthF* unit, int inNumSamples);
    
	void PV_NoiseSynthF_Ctor(PV_NoiseSynthF *unit);
	void PV_NoiseSynthF_Dtor(PV_NoiseSynthF *unit);
	void PV_NoiseSynthF_first(PV_NoiseSynthF* unit, int inNumSamples);
	void PV_NoiseSynthF_next_z(PV_NoiseSynthF* unit, int inNumSamples);
	void PV_NoiseSynthF_next(PV_NoiseSynthF* unit, int inNumSamples);
	
	void PV_MagMap_Ctor(PV_MagMap *unit);
	void PV_MagMap_next(PV_MagMap* unit, int inNumSamples);

	void PV_MaxMagN_Ctor(PV_MaxMagN *unit);
	void PV_MaxMagN_next(PV_MaxMagN* unit, int inNumSamples);

	void PV_MinMagN_Ctor(PV_MinMagN *unit);
	void PV_MinMagN_next(PV_MinMagN* unit, int inNumSamples);

	void PV_FreqBuffer_Ctor(PV_FreqBuffer *unit);
	void PV_FreqBuffer_Dtor(PV_FreqBuffer *unit);
	void PV_FreqBuffer_first(PV_FreqBuffer* unit, int inNumSamples);
	void PV_FreqBuffer_next(PV_FreqBuffer* unit, int inNumSamples);

	void PV_MagBuffer_Ctor(PV_MagBuffer *unit);
	void PV_MagBuffer_first(PV_MagBuffer* unit, int inNumSamples);
	void PV_MagBuffer_next(PV_MagBuffer* unit, int inNumSamples);
	    
	void PV_OddBin_Ctor(PV_OddBin *unit);
	void PV_OddBin_next(PV_OddBin* unit, int inNumSamples);
	
	void PV_EvenBin_Ctor(PV_EvenBin *unit);
	void PV_EvenBin_next(PV_EvenBin* unit, int inNumSamples);
	
	void PV_Invert_Ctor(PV_Invert *unit);
	void PV_Invert_next(PV_Invert* unit, int inNumSamples);
	
	void PV_BinDelay_Ctor(PV_BinDelay *unit);
	void PV_BinDelay_Dtor(PV_BinDelay *unit);
	void PV_BinDelay_first(PV_BinDelay *unit, int inNumSamples);
	void PV_BinDelay_empty(PV_BinDelay *unit, int inNumSamples);
	void PV_BinDelay_next(PV_BinDelay *unit, int inNumSamples);
	
	void PV_Freeze_Ctor(PV_Freeze *unit);
	void PV_Freeze_next(PV_Freeze *unit, int inNumSamples);
	void PV_Freeze_Dtor(PV_Freeze *unit);

}

SCPolarBuf* ToPolarApx(SndBuf *buf)
{
	if (buf->coord == coord_Complex) {
		SCComplexBuf* p = (SCComplexBuf*)buf->data;
		int numbins = buf->samples - 2 >> 1;
		for (int i=0; i<numbins; ++i) {
			p->bin[i].ToPolarApxInPlace();
		}
		buf->coord = coord_Polar;
	}
	return (SCPolarBuf*)buf->data;
}

SCComplexBuf* ToComplexApx(SndBuf *buf)
{
	if (buf->coord == coord_Polar) {
		SCPolarBuf* p = (SCPolarBuf*)buf->data;
		int numbins = buf->samples - 2 >> 1;
		for (int i=0; i<numbins; ++i) {
			p->bin[i].ToComplexApxInPlace();
		}
		buf->coord = coord_Complex;
	}
	return (SCComplexBuf*)buf->data;
}
	 
#define CALC_FREQS \
	for (int i = 0; i < numbins; i++){ \
	    float phasedif = p->bin[i].phase - phases[i]; /* get the phase differece */ \
	    while (phasedif > pi) /* unwrap the phase */ \
		phasedif -= twopi; \
	    while (phasedif < pi) \
		phasedif += twopi; \
	    /* calculate the freq */ \
	    freq = (sr / twopi) * (unit->m_centerfreqs[i] + (phasedif / (float)numbins)); \
	    freqs[i + (numbins * unit->m_curframe)] = freq; /* save the freqs */ \
	    /* store the current phases to the buffer */ \
	    phases[i] = p->bin[i].phase; \
	} \
	unit->m_curframe = (unit->m_curframe + 1) % unit->m_numFrames;

// for operation on one buffer
#define PV_GET_BUF \
	float fbufnum = ZIN0(0); \
	if (fbufnum < 0.f) { ZOUT0(0) = -1.f; return; } \
	ZOUT0(0) = fbufnum; \
	uint32 ibufnum = (uint32)fbufnum; \
	World *world = unit->mWorld; \
	if (ibufnum >= world->mNumSndBufs) ibufnum = 0; \
	SndBuf *buf = world->mSndBufs + ibufnum; \
	int numbins = buf->samples - 2 >> 1;
	
void PV_PartialSynthF_next(PV_PartialSynthF *unit, int inNumSamples)
{
	PV_GET_BUF
	
	/* convert to polar */
	
	SCPolarBuf *p = ToPolarApx(buf);
	float *phases = unit->m_phases; 
 	float *freqs = unit->m_freqs; 
	
	float thresh = ZIN0(1); /* a freq threshold */
	int numFrames = unit->m_numFrames;
	float freqcheck = 0.f;
	float sr = (float)unit->mWorld->mSampleRate; /* we need the audio rate... calc it here */
	float freq = 0.f;
	/* check if there is new data */
	CALC_FREQS
	
	/* check if the freqs for each bin are stable... if they aren't, zero out the mag */
	
	for (int i=0; i<numbins; ++i) {
		/* reset freqcheck */
		freqcheck = 0.f;
		/* get the average freqs for this bin */
		for (int j = 0; j < numFrames; ++j) {
		    freqcheck += freqs[i + (numbins * j)];
		}
		float freqavg = freqcheck / (float)numFrames;

		freq = freqs[i + (numbins * unit->m_curframe)];
		/* if the current phase - phaseavg of the last frames is greater then the threshold, 0 it out */
		if (fabsf(freq - freqavg) > thresh) p->bin[i].mag = 0.;
//		if (i == 10) Print("avg: %3.3f, freq: %3.3f, fabsf: %3.3f\n", BUFRATE, SAMPLERATE, (float)unit->mWorld->mBufLength);
		freq = 0.f;
	}

}

void PV_PartialSynthF_Ctor(PV_PartialSynthF *unit)
{
	SETCALC(PV_PartialSynthF_first);
	ZOUT0(0) = ZIN0(0);
	unit->m_numFrames = (int)IN0(2);
	unit->m_phases = 0;
	unit->m_freqs = 0;
	unit->m_curframe = 0;
	unit->m_nextflag = 0;

}

void PV_PartialSynthF_Dtor(PV_PartialSynthF *unit)
{
	RTFree(unit->mWorld, unit->m_phases);
	RTFree(unit->mWorld, unit->m_freqs);
	RTFree(unit->mWorld, unit->m_centerfreqs);
}

void PV_PartialSynthF_first(PV_PartialSynthF *unit, int inNumSamples)
{   
	PV_GET_BUF

	SCPolarBuf *p = ToPolarApx(buf);
	
	int numFrames = (int)unit->m_numFrames;
	float initflag = IN0(3);
	/* create buffers to store data in */
	if (!unit->m_phases) {
		unit->m_phases = (float*)RTAlloc(unit->mWorld, numbins * sizeof(float));
		unit->m_freqs = (float*)RTAlloc(unit->mWorld, numbins * numFrames * sizeof(float));
		unit->m_centerfreqs = (float*)RTAlloc(unit->mWorld, numbins * sizeof(float));
		unit->m_numbins = numbins;
	} else if (numbins != unit->m_numbins) return;
	
	/* initialize the phase data with phase info from the current frame */
	
	for(int i = 0; i < numbins; i++){ 
		unit->m_phases[i] = p->bin[i].phase; 
	    }	
	/* initialize the freqs data to 0.f */
	
	for(int i = 0; i < (numbins * numFrames); i++){
	    unit->m_freqs[i] = 0.f;
	}
	
	/* build the centerfreqs array */
	
	for(int i = 0; i < numbins; i++){
	    unit->m_centerfreqs[i] = i * (twopi / ((float)numbins * 2.));
	}
	
	// if initflag is 0, zero out all bins!
	if (initflag == 0.f) {
		for (int i=0; i<numbins; ++i) {
			p->bin[i].mag = 0.;
		}
	}
		
	/* call the next_z function */
	
	SETCALC(PV_PartialSynthF_next_z);
}

/* next_z will run until numFrames worth of freqs data has been stored */
void PV_PartialSynthF_next_z(PV_PartialSynthF *unit, int inNumSamples)
{
	PV_GET_BUF
	/* dec unit->m_remaining loops */
	SCPolarBuf *p = ToPolarApx(buf);
	float *phases = unit->m_phases; 
 	float *freqs = unit->m_freqs; 
	float sr = (float)unit->mWorld->mSampleRate; /* we need the audio rate... calc it here */
	float freq = 0.f;
	float initflag = IN0(3);
	
	/* check to see if there is another frames worth of phase data to collect... do so if there is and set
	unit->m_remainingLoops to the number of loops until new data is available ++ curframe */
	
	CALC_FREQS

	// if initflag is 0, zero out all bins!
	if (initflag == 0.f) {
		for (int i=0; i<numbins; ++i) {
			p->bin[i].mag = 0.;
		}
	}
	
	/* if we have enough data to start modifying the buffer, then change to the next function */
//	if (unit->m_curframe == (unit->m_numFrames - 1)) {
	if (unit->m_curframe == 1) unit->m_nextflag = 1;
	if ((unit->m_curframe == 0) && (unit->m_nextflag == 1)) {
	    /* reset m_curframe */
//	    unit->m_curframe = 0;
	    SETCALC(PV_PartialSynthF_next);
	}
}

void PV_NoiseSynthF_next(PV_NoiseSynthF *unit, int inNumSamples)
{
	PV_GET_BUF
	/* convert to polar */
	
	SCPolarBuf *p = ToPolarApx(buf);
	float *phases = unit->m_phases; 
 	float *freqs = unit->m_freqs; 
	unit->m_remainingLoops -= 1;
	
	float thresh = ZIN0(1); /* This will make the percentage passed in work with phase data */
	int numFrames = unit->m_numFrames;
	float freqcheck = 0.f;
	float sr = SAMPLERATE * (float)unit->mWorld->mBufLength; /* we need the audio rate... calc it here */
	float freq = 0.f;
	/* check if there is new data */
	CALC_FREQS
	
	/* check if the freqs for each bin are stable... if they aren't, zero out the mag */
	
	for (int i=0; i<numbins; ++i) {
		/* reset freqcheck */
		freqcheck = 0.f;
		/* get the average freqs for this bin */
		for (int j = 0; j < numFrames; ++j) {
		    freqcheck += freqs[i + (numbins * j)];
		}
		float freqavg = freqcheck / (float)numFrames;

		freq = freqs[i + (numbins * unit->m_curframe)];
		/* if the current phase - phaseavg of the last frames is greater then the threshold, 0 it out */
		if (fabsf(freq - freqavg) < thresh) p->bin[i].mag = 0.;
		freq = 0.f;
	}

}

void PV_NoiseSynthF_Ctor(PV_NoiseSynthF *unit)
{
	SETCALC(PV_NoiseSynthF_first);
	ZOUT0(0) = ZIN0(0);
	unit->m_numFrames = (int)IN0(2);
	unit->m_phases = 0;
	unit->m_freqs = 0;
	unit->m_curframe = 0;
	unit->m_nextflag = 0;

}

void PV_NoiseSynthF_Dtor(PV_NoiseSynthF *unit)
{
	RTFree(unit->mWorld, unit->m_phases);
	RTFree(unit->mWorld, unit->m_freqs);
	RTFree(unit->mWorld, unit->m_centerfreqs);
}

void PV_NoiseSynthF_first(PV_NoiseSynthF *unit, int inNumSamples)
{   
	PV_GET_BUF

	SCPolarBuf *p = ToPolarApx(buf);
	
	int numFrames = (int)unit->m_numFrames;
	
	/* create buffers to store data in */
	if (!unit->m_phases) {
		unit->m_phases = (float*)RTAlloc(unit->mWorld, numbins * sizeof(float));
		unit->m_freqs = (float*)RTAlloc(unit->mWorld, numbins * numFrames * sizeof(float));
		unit->m_centerfreqs = (float*)RTAlloc(unit->mWorld, numbins * sizeof(float));
		unit->m_numbins = numbins;
	} else if (numbins != unit->m_numbins) return;
	
	/* initialize the phase data with phase info from the current frame */
	
	for(int i = 0; i < numbins; i++){ 
		unit->m_phases[i] = p->bin[i].phase; 
	    }	
	/* initialize the freqs data to 0.f */
	
	for(int i = 0; i < (numbins * numFrames); i++){
	    unit->m_freqs[i] = 0.f;
	}
	
	/* build the centerfreqs array */
	
	for(int i = 0; i < numbins; i++){
	    unit->m_centerfreqs[i] = i * (twopi / ((float)numbins * 2.));
	}

	float initflag = IN0(3);

	// if initflag is 0, zero out all bins!
	if (initflag == 0.f) {
		for (int i=0; i<numbins; ++i) {
			p->bin[i].mag = 0.;
		}
	}	
	/* call the next_z function */
	
	SETCALC(PV_NoiseSynthF_next_z);
}

/* next_z will run until numFrames worth of freqs data has been stored */
void PV_NoiseSynthF_next_z(PV_NoiseSynthF *unit, int inNumSamples)
{
	PV_GET_BUF
	/* dec unit->m_remaining loops */
	
	SCPolarBuf *p = ToPolarApx(buf);
	float *phases = unit->m_phases; 
 	float *freqs = unit->m_freqs; 
	float sr = SAMPLERATE * BUFRATE; /* we need the audio rate... calc it here */
	float freq = 0.f;
	
	/* check to see if there is another frames worth of phase data to collect... do so if there is and set
	unit->m_remainingLoops to the number of loops until new data is available ++ curframe */
	
	CALC_FREQS

	float initflag = IN0(3);

	// if initflag is 0, zero out all bins!
	if (initflag == 0.f) {
		for (int i=0; i<numbins; ++i) {
			p->bin[i].mag = 0.;
		}
	}	
		
	/* if we have enought data to start modifying the buffer, then change to the next function */
	if (unit->m_curframe == 1) unit->m_nextflag = 1;
	if ((unit->m_curframe == 0) && (unit->m_nextflag == 1)) {
	    /* reset m_curframe */
	    SETCALC(PV_NoiseSynthF_next);
	}
}

/*
take the average phase difs for the past frames
compare it to the difference in phase between the current and prev frame
(avg - fabs(current-last))
if < thresh, resynth
*/

#define CALC_PHASEDIF \
	int skip = (numbins * unit->m_curframe); \
	    for (int i = 0; i < numbins; i++){ \
		float prevphase = phases[i]; \
		float phase = p->bin[i].phase; \
		while (phase > pi) /* unwrap the phase */ \
		    phase -= twopi; \
		while (phase < pi) \
		    phase += twopi; \
		float phasedif = phase - prevphase; /* get the phase differece */ \
		while (phasedif > pi) /* unwrap the phase */ \
		    phasedif -= twopi; \
		while (phasedif < pi) \
		    phasedif += twopi; \
		phasedifs[i + skip] = phasedif; /* save the phasedif */ \
		/* store the current phases to the buffer */ \
		phases[i] = p->bin[i].phase; \
	    unit->m_curframe = (unit->m_curframe + 1) % unit->m_numFrames; \
	} 

	    
void PV_PartialSynthP_next(PV_PartialSynthP *unit, int inNumSamples)
{
	PV_GET_BUF
	
	/* convert to polar */
	
	SCPolarBuf *p = ToPolarApx(buf);
	float *phases = unit->m_phases; 
 	float *phasedifs = unit->m_phasedifs; 
	
	float thresh = ZIN0(1); /* This expects values between 0 and 2pi */
	int numFrames = unit->m_numFrames;
	float phasecheck = 0.f;
	
	/* check if there is new data */
	CALC_PHASEDIF
	
	/* check if the phasedifs for each bin are stable... if they aren't, zero out the mag */
	
	for (int i=0; i<numbins; ++i) {
		/* get the average phasedif for this bin */
		for (int j = 0; j < numFrames; ++j) {
		    phasecheck += phasedifs[i + (numbins * j)];
		}
		float phaseavg = phasecheck / numFrames;

		/* if the current phase - phaseavg of the last frames is greater then the threshold, 0 it out */
		if (fabsf(phaseavg - phasedifs[i + skip]) > thresh) p->bin[i].mag = 0.;
		/* reset phasecheck */
		phasecheck = 0.f;
	}
}

void PV_PartialSynthP_Ctor(PV_PartialSynthP *unit)
{
	SETCALC(PV_PartialSynthP_first);
	ZOUT0(0) = ZIN0(0);
	unit->m_numFrames = (int)IN0(2);
	unit->m_phases = 0;
	unit->m_phasedifs = 0;
	unit->m_curframe = 0;
	unit->m_nextflag = 0;
}

void PV_PartialSynthP_Dtor(PV_PartialSynthP *unit)
{
	RTFree(unit->mWorld, unit->m_phases);
	RTFree(unit->mWorld, unit->m_phasedifs);
}

void PV_PartialSynthP_first(PV_PartialSynthP *unit, int inNumSamples)
{   
	PV_GET_BUF

	SCPolarBuf *p = ToPolarApx(buf);
	
	int numFrames = (int)unit->m_numFrames;
	
	/* create buffers to store data in */
	if (!unit->m_phases) {
		unit->m_phases = (float*)RTAlloc(unit->mWorld, numbins * sizeof(float));
		unit->m_phasedifs = (float*)RTAlloc(unit->mWorld, numbins * numFrames * sizeof(float));
		unit->m_numbins = numbins;
	} else if (numbins != unit->m_numbins) return;
	
	/* initialize the phase data with phase info from the current frame */
	
	for(int i = 0; i < numbins; i++){ 
		unit->m_phases[i] = p->bin[i].phase; 
	    }	
	/* initialize the phasedif data to 0.f */
	
	for(int i = 0; i < (numbins * numFrames); i++){
	    unit->m_phasedifs[i] = 0.f;
	}

	float initflag = IN0(3);

	// if initflag is 0, zero out all bins!
	if (initflag == 0.f) {
		for (int i=0; i<numbins; ++i) {
			p->bin[i].mag = 0.;
		}
	}	
		
	/* call the next_z function */
	
	SETCALC(PV_PartialSynthP_next_z);
}

/* next_z will run until numFrames worth of phasedif data has been stored */
void PV_PartialSynthP_next_z(PV_PartialSynthP *unit, int inNumSamples)
{
	PV_GET_BUF
	/* dec unit->m_remaining loops */
	
	SCPolarBuf *p = ToPolarApx(buf);
	float *phases = unit->m_phases; 
 	float *phasedifs = unit->m_phasedifs;
	
	/* check to see if there is another frames worth of phase data to collect... do so if there is and set
	unit->m_remainingLoops to the number of loops until new data is available ++ curframe */
	
	CALC_PHASEDIF

	float initflag = IN0(3);

	// if initflag is 0, zero out all bins!
	if (initflag == 0.f) {
		for (int i=0; i<numbins; ++i) {
			p->bin[i].mag = 0.;
		}
	}	
		
	/* if we have enought data to start modifying the buffer, then change to the next function */
	if (unit->m_curframe == 1) unit->m_nextflag = 1;
	if ((unit->m_curframe == 0) && (unit->m_nextflag == 1)) {
	    /* reset m_curframe */
//	    unit->m_curframe = 0;
	    SETCALC(PV_PartialSynthP_next);
	}
}

void PV_NoiseSynthP_next(PV_NoiseSynthP *unit, int inNumSamples)
{
	PV_GET_BUF
	
	/* convert to polar */
	
	SCPolarBuf *p = ToPolarApx(buf);
	float *phases = unit->m_phases; 
 	float *phasedifs = unit->m_phasedifs; 
	
	float thresh = ZIN0(1); /* This expects values between 0 and 2pi */
	int numFrames = unit->m_numFrames;
	float phasecheck = 0.f;
	
	/* check if there is new data */
	CALC_PHASEDIF
	
	/* check if the phasedifs for each bin are stable... if they aren't, zero out the mag */
	
	for (int i=0; i<numbins; ++i) {
		/* get the average phasedif for this bin */
		for (int j = 0; j < numFrames; ++j) {
		    phasecheck += phasedifs[i + (numbins * j)];
		}
		float phaseavg = phasecheck / numFrames;
		
		//float phase = p->bin[i].phase;
		/* if the current phase - phaseavg of the last frames is greater then the threshold, 0 it out */
		if (fabsf(phaseavg - phasedifs[i + skip]) < thresh) p->bin[i].mag = 0.;
		/* reset phasecheck */
		phasecheck = 0.f;
	}
}

void PV_NoiseSynthP_Ctor(PV_NoiseSynthP *unit)
{
	SETCALC(PV_NoiseSynthP_first);
	ZOUT0(0) = ZIN0(0);
	unit->m_numFrames = (int)IN0(2);
	unit->m_phases = 0;
	unit->m_phasedifs = 0;
	unit->m_curframe = 0;
	unit->m_nextflag = 0;

}

void PV_NoiseSynthP_Dtor(PV_NoiseSynthP *unit)
{
	RTFree(unit->mWorld, unit->m_phases);
	RTFree(unit->mWorld, unit->m_phasedifs);
}

void PV_NoiseSynthP_first(PV_NoiseSynthP *unit, int inNumSamples)
{   
	PV_GET_BUF

	SCPolarBuf *p = ToPolarApx(buf);
	
	int numFrames = (int)unit->m_numFrames;
	
	/* create buffers to store data in */
	if (!unit->m_phases) {
		unit->m_phases = (float*)RTAlloc(unit->mWorld, numbins * sizeof(float));
		unit->m_phasedifs = (float*)RTAlloc(unit->mWorld, numbins * numFrames * sizeof(float));
		unit->m_numbins = numbins;
	} else if (numbins != unit->m_numbins) return;
	
	/* initialize the phase data with phase info from the current frame */
	
	for(int i = 0; i < numbins; i++){ 
		unit->m_phases[i] = p->bin[i].phase; 
	    }	
	/* initialize the phasedif data to 0.f */
	
	for(int i = 0; i < (numbins * numFrames); i++){
	    unit->m_phasedifs[i] = 0.f;
	}

	float initflag = IN0(3);

	// if initflag is 0, zero out all bins!
	if (initflag == 0.f) {
		for (int i=0; i<numbins; ++i) {
			p->bin[i].mag = 0.;
		}
	}	
		
	/* call the next_z function */
	
	SETCALC(PV_NoiseSynthP_next_z);
}

/* next_z will run until numFrames worth of phasedif data has been stored */
void PV_NoiseSynthP_next_z(PV_NoiseSynthP *unit, int inNumSamples)
{
	PV_GET_BUF
	/* dec unit->m_remaining loops */
	
	SCPolarBuf *p = ToPolarApx(buf);
	float *phases = unit->m_phases; 
 	float *phasedifs = unit->m_phasedifs; 
	
	/* check to see if there is another frames worth of phase data to collect... do so if there is and set
	unit->m_remainingLoops to the number of loops until new data is available ++ curframe */
	
	CALC_PHASEDIF

	float initflag = IN0(3);

	// if initflag is 0, zero out all bins!
	if (initflag == 0.f) {
		for (int i=0; i<numbins; ++i) {
			p->bin[i].mag = 0.;
		}
	}	
		
	/* if we have enought data to start modifying the buffer, then change to the next function */
	if (unit->m_curframe == 1) unit->m_nextflag = 1;
	if ((unit->m_curframe == 0) && (unit->m_nextflag == 1)) {
	    /* reset m_curframe */
//	    unit->m_curframe = 0;
	    SETCALC(PV_NoiseSynthP_next);
	}
}


		
void PV_MagMap_Ctor(PV_MagMap *unit)
{
	SETCALC(PV_MagMap_next);
	ZOUT0(0) = ZIN0(0);
	unit->m_fmagbufnum = -1e9f;
}


void PV_MagMap_next(PV_MagMap* unit, int inNumSamples)
{
	PV_GET_BUF

	SCPolarBuf *p = ToPolarApx(buf);
	
	// get table
	float fmagbufnum = ZIN0(1); 
	if (fmagbufnum != unit->m_fmagbufnum) { 
		uint32 magbufnum = (uint32)fmagbufnum; 
		World *world = unit->mWorld; 
		if (magbufnum >= world->mNumSndBufs) magbufnum = 0; 
		unit->m_magbuf = world->mSndBufs + magbufnum; 
	} 
	SndBuf *magbuf = unit->m_magbuf; 
	if(!magbuf) { 
		ClearUnitOutputs(unit, inNumSamples); 
		return; 
	} 
	float *magbufData __attribute__((__unused__)) = magbuf->data; 
	if (!magbufData) { 
		ClearUnitOutputs(unit, inNumSamples); 
		return; 
	} 
	int tableSize = magbuf->samples;
	float *table = magbufData;
	int32 maxindex = tableSize - 1;
	
	// find the max magnitude in this frame 
	float maxmag = 0.f; // initialize to 0.f

	for (int i=0; i<numbins; ++i) {
		float mag = p->bin[i].mag;
		if (mag > maxmag) maxmag = mag;
	} 

	// make sure there is magnitude!
	if (maxmag != 0.f) {
	    for (int i = 0; i < numbins; ++ i) {
		// normalize magnitudes to 1.
		float mag = p->bin[i].mag / maxmag;
		// access appropriate section of magmap buffer. scale mag from 0 to 1 to 0 tablesize
		float point = mag * (float)(tableSize - 1);
		int32 index = (int32)point;
		index = sc_clip(index, 0, maxindex);
		int32 index2 = (int32)(point + 1);
		index2 = sc_clip(index2, 0, maxindex);
		float pct = point - (float)index;
		float newmag = lininterp(pct, table[index], table[index2]);
		// rescale magnitude and output
		p->bin[i].mag = newmag * maxmag;
		};
	};

}


/* a function for sorting floats */
int isfloatgreater(const void *a, const void *b);
int isfloatgreater (const void *a, const void *b)
{
  const float *fa = (const float *) a;
  const float *fb = (const float *) b;

  return (*fa > *fb) - (*fa < *fb);
}


int isfloatless(const void *a, const void *b);
int isfloatless (const void *a, const void *b)
{
  const float *fa = (const float *) a;
  const float *fb = (const float *) b;

  return (*fa < *fb) - (*fa > *fb);
}

void PV_MaxMagN_Ctor(PV_MaxMagN *unit)
{
	SETCALC(PV_MaxMagN_next);
	ZOUT0(0) = ZIN0(0);
//	unit->m_fmagbuf = -1e9f;
}

void PV_MaxMagN_next(PV_MaxMagN* unit, int inNumSamples) 
{
	PV_GET_BUF

	SCPolarBuf *p = ToPolarApx(buf);
	
	float magarray[numbins];
		
	for (int i = 0; i < numbins; ++i){  
	    magarray[i] = 0.f;
	    magarray[i] = p->bin[i].mag;
	    };

	float numpars = IN0(1);
	    
	qsort(magarray, numbins, sizeof (float), isfloatless);
	float minmag = magarray[(int)numpars];
	for (int i = 0; i < numbins; ++i){
	    if (p->bin[i].mag <= minmag) p->bin[i].mag = 0.f;
	    };	
}

void PV_MinMagN_Ctor(PV_MinMagN *unit)
{
	SETCALC(PV_MinMagN_next);
	ZOUT0(0) = ZIN0(0);
//	unit->m_fmagbuf = -1e9f;
}

void PV_MinMagN_next(PV_MinMagN* unit, int inNumSamples) 
{
	PV_GET_BUF

	SCPolarBuf *p = ToPolarApx(buf);
	
	float magarray[numbins];
	
	for (int i = 0; i < numbins; ++i){  
	    magarray[i] = 0.f;
	    magarray[i] = p->bin[i].mag;
	    };

	float numpars = IN0(1);
	    
	qsort(magarray, numbins, sizeof (float), isfloatgreater);
	
	float maxmag = magarray[(int)numpars];
	
	for (int i = 0; i < numbins; ++i){
	    if (p->bin[i].mag >= maxmag) p->bin[i].mag = 0.f;
	    };
}
	
void PV_FreqBuffer_Ctor(PV_FreqBuffer *unit)
{
	ZOUT0(0) = ZIN0(0);
	unit->m_fdatabufnum = -1e9f;
	SETCALC(PV_FreqBuffer_next);
	unit->m_numloops = 0;
	unit->m_firstflag = 0;
	PV_FreqBuffer_next(unit, 1);

}

void PV_FreqBuffer_Dtor(PV_FreqBuffer *unit)
{
	RTFree(unit->mWorld, unit->m_phases);
	RTFree(unit->mWorld, unit->m_centerfreqs);
}

void PV_FreqBuffer_next(PV_FreqBuffer *unit, int inNumSamples)
{
	float sr = (float)unit->mWorld->mSampleRate; 
	int numloops = unit->m_numloops;
	PV_GET_BUF
	
	SCPolarBuf *p = ToPolarApx(buf);
	
	/* get the buffer to store data in */
	float fdatabufnum = IN0(1); 
	if (fdatabufnum != unit->m_fdatabufnum) {
		unit->m_fdatabufnum = fdatabufnum;
		uint32 databufnum = (uint32)fdatabufnum; 
		World *world = unit->mWorld; 
		if (databufnum >= world->mNumSndBufs) databufnum = 0; 
		unit->m_databuf = world->mSndBufs + databufnum; 
	} 
	SndBuf *databuf = unit->m_databuf; 
	if(!databuf) { 
		ClearUnitOutputs(unit, inNumSamples);
		return; 
	} 
	float *databufData __attribute__((__unused__)) = databuf->data; 
	
	if(unit->m_numloops == 0){
		if(unit->m_firstflag == 0){
		    /* create buffers to store data in */
		    unit->m_phases = (float*)RTAlloc(unit->mWorld, numbins * sizeof(float));
		    unit->m_centerfreqs = (float*)RTAlloc(unit->mWorld, numbins * sizeof(float));

		    /* initialize the phase data with phase info to 0.f and calc the centerfreqs */
		    for(int i = 0; i < numbins; i++){
			unit->m_phases[i] = 0.f; 
			float freq = unit->m_centerfreqs[i] = i * (twopi / ((float)numbins * 2.));
			float* table0 = databufData + i;
			table0[0] = freq; /* save the freq */
		    }
		    unit->m_firstflag = 1;
		    unit->m_numloops = (int)(numbins / (sr / BUFRATE)); 
		    } else {
		    for (int i = 0; i < numbins; i++){ 
			float phasedif = p->bin[i].phase - unit->m_phases[i]; 
			while (phasedif > pi) /* unwrap the phase */  
			    phasedif -= twopi; 
			while (phasedif < pi) 
			    phasedif += twopi; 
			/* calculate the freq */ 
			float freq = (sr / twopi) * (unit->m_centerfreqs[i] + (phasedif / (float)numbins)); 
			float* table0 = databufData + i; 
			table0[0] = freq; /* save the freq */
			/* store the current phases to the buffer */  
			unit->m_phases[i] = p->bin[i].phase; 
			} 
		    unit->m_numloops = numloops =  (int)(numbins / (sr / BUFRATE));
		}
	    } else (unit->m_numloops -= 1);
}

void PV_MagBuffer_Ctor(PV_MagBuffer *unit)
{
	ZOUT0(0) = ZIN0(0);
	unit->m_fdatabufnum = -1e9f;
	SETCALC(PV_MagBuffer_first);
	unit->m_numloops = 0;
//	PV_MagBuffer_first(unit, 1);
}

void PV_MagBuffer_first(PV_MagBuffer *unit, int inNumSamples)
{
	PV_GET_BUF

	// get the buffer to store data in 
	float fdatabufnum = IN0(1); 
	if (fdatabufnum != unit->m_fdatabufnum) { 
		unit->m_fdatabufnum = fdatabufnum;
		uint32 databufnum = (uint32)fdatabufnum; 
		World *world = unit->mWorld; 
		if (databufnum >= world->mNumSndBufs) databufnum = 0; 
		unit->m_databuf = world->mSndBufs + databufnum; 
	} 
	SndBuf *databuf = unit->m_databuf; 
	if(!databuf) { 
		ClearUnitOutputs(unit, inNumSamples); 
		return; 
	} 
	float *databufData __attribute__((__unused__)) = databuf->data; 
	float sr = (float)unit->mWorld->mSampleRate; 
	unit->m_numloops = (int)(numbins / (sr / BUFRATE)) - 1;
	for(int i = 0; i < numbins; i++){
		    databufData[i] = 0.f; // zero out the initial data in the buffer (until the FFT buffer is filled)
		}	
	SETCALC(PV_MagBuffer_next);
}

void PV_MagBuffer_next(PV_MagBuffer *unit, int inNumSamples)
{
	PV_GET_BUF
	SCPolarBuf *p = ToPolarApx(buf);

	/* get the buffer to store data in */
	float fdatabufnum = IN0(1); 
	if (fdatabufnum != unit->m_fdatabufnum) { 
		unit->m_fdatabufnum = fdatabufnum;
		uint32 databufnum = (uint32)fdatabufnum; 
		World *world = unit->mWorld; 
		if (databufnum >= world->mNumSndBufs) databufnum = 0; 
		unit->m_databuf = world->mSndBufs + databufnum; 
	} 
	SndBuf *databuf = unit->m_databuf; 
	if(!databuf) { 
		ClearUnitOutputs(unit, inNumSamples); 
		return; 
	} 
	float *databufData __attribute__((__unused__)) = databuf->data; 
	
	if(unit->m_numloops <= 0){
	    for(int i = 0; i < numbins; i++){
		databufData[i] = p->bin[i].mag;
		}
	    float sr = (float)unit->mWorld->mSampleRate; 
	    unit->m_numloops = (int)(numbins / (sr / BUFRATE));
	    } else {
	    unit->m_numloops -= 1;
	    }
}

void PV_OddBin_Ctor(PV_OddBin *unit)
{
	ZOUT0(0) = ZIN0(0);
	SETCALC(PV_OddBin_next);
}

void PV_OddBin_next(PV_OddBin* unit, int inNumSamples)
{
	PV_GET_BUF
	SCPolarBuf *p = ToPolarApx(buf);

	for(int i = 0; i < numbins; i+=2){
	    p->bin[i].mag = 0.f;
	    }
}

void PV_EvenBin_Ctor(PV_EvenBin *unit)
{
	ZOUT0(0) = ZIN0(0);
	SETCALC(PV_EvenBin_next);
}

void PV_EvenBin_next(PV_EvenBin* unit, int inNumSamples)
{
	PV_GET_BUF
	SCPolarBuf *p = ToPolarApx(buf);

	for(int i = 1; i < numbins; i+=2){
	    p->bin[i].mag = 0.f;
	    }
}


void PV_Invert_Ctor(PV_Invert *unit)
{
	ZOUT0(0) = ZIN0(0);
	SETCALC(PV_Invert_next);
}

void PV_Invert_next(PV_Invert* unit, int inNumSamples)
{
	PV_GET_BUF
	RGET
	SCPolarBuf *p = ToPolarApx(buf);
	float mymag = 0.f;
	
	for(int i = 1; i < numbins; i++){
	    mymag = p->bin[i].mag;
	    if(mymag > 0.00001) // if the magnitude is greater then -96dB
		p->bin[i].mag = log(p->bin[i].mag) * -1; // invert it
		else
		p->bin[i].mag = 11.052408446371 + (frand(s1, s2, s3) * 2.763102111593); // avoid infs... invert ran num between -96 and -120dB
	    }
	RPUT
}

/*
While all the code below is mine, this UGen owes much conceptually to the Native Instruments SpektralDelay (which I haven't actually heard... but I imagine it sounds a bit like ...) some SynthDefs that William 'Pete' Moss <petemoss at petemoss dot org> and I coded to try and emulate said SpektralDelay (not using FFT) and a final kick to implement this after playing with Jesse Chappell's FreqTweak < jesse at essej dot net >. 
*/

void PV_BinDelay_Ctor(PV_BinDelay* unit)
{
    OUT0(0) = IN0(0);
    unit->m_deltimesbufnum = -1e9f;
    unit->m_fbbufnum = -1e9f;
    unit->m_elapsedFrames = 0;
    SETCALC(PV_BinDelay_first);
}

void PV_BinDelay_Dtor(PV_BinDelay* unit)
{
    for(int i = 0; i < unit->m_numFrames; i++){
	RTFree(unit->mWorld, unit->m_databuf[i]);
	}
}

#define SETUP_DELS_FB \
    float fdeltimesbufnum = IN0(2); \
    if(fdeltimesbufnum != unit->m_deltimesbufnum){ \
	uint32 deltimesbufnum = (uint32)fdeltimesbufnum; \
	World *world = unit->mWorld; \
	if (deltimesbufnum >= world->mNumSndBufs) deltimesbufnum = 0; \
	unit->m_deltimes = world->mSndBufs + deltimesbufnum; \
	} \
    SndBuf *deltimebuf = unit->m_deltimes; \
    float *delbufData __attribute__((__unused__)) = deltimebuf->data; \
    float *deltimes = delbufData; \
    float ffbbufnum = IN0(3); \
    if(ffbbufnum != unit->m_fbbufnum){ \
	uint32 fbbufnum = (uint32)ffbbufnum; \
	World *world = unit->mWorld; \
	if (fbbufnum >= world->mNumSndBufs) fbbufnum = 0; \
	unit->m_fb = world->mSndBufs + fbbufnum; \
	} \
    SndBuf *fbbuf = unit->m_fb; \
    float *fbbufData __attribute__((__unused__)) = fbbuf->data; \
    float *fb = fbbufData; \
        

void PV_BinDelay_first(PV_BinDelay* unit, int inNumSamples)
{
    int delframe;
    // get the current buffer
    PV_GET_BUF
    
    SCComplexBuf *p = ToComplexApx(buf);

    // figure out the maxdelay in seconds
    float maxdelay = IN0(1);

    SETUP_DELS_FB
    
    // how many frames is that?
    float srbins = unit->m_srbins = ((float)unit->mWorld->mSampleRate / numbins);
    int numFrames = unit->m_numFrames = (int)(srbins * maxdelay) + 1;
    // allocate the big buffer for the delay
    for(int i = 0; i < numFrames; i++){
	unit->m_databuf[i] = (SCComplexBuf*)RTAlloc(unit->mWorld, buf->samples * sizeof(float));
	}
    int curFrame = unit->m_curFrame = numFrames - 1;
    SCComplexBuf *delFrame = unit->m_databuf[curFrame];
    // write the current values into the delay frame
    memcpy(delFrame->bin, p->bin, numbins * sizeof(SCComplex));

    unit->m_databuf[curFrame] = delFrame;
    
    // loop through the delayed frame, and write those values to the current buffer
    for(int i = 0; i < numbins; i++){
	delframe = curFrame + (int)roundf(deltimes[i] * srbins);
	if(delframe >= numFrames) {
	    p->bin[i].real = 0.f;
	    p->bin[i].imag = 0.f;
	    } else {
	    p->bin[i] = unit->m_databuf[delframe]->bin[i];
	    SCPolar thisone = unit->m_databuf[delframe]->bin[i].ToPolar();	
	    thisone.mag *= fb[i];	
	    unit->m_databuf[delframe]->bin[i] = thisone.ToComplex();
	    unit->m_databuf[curFrame]->bin[i] += unit->m_databuf[delframe]->bin[i];
	    }
	}

    unit->m_elapsedFrames++;    
    
    SETCALC(PV_BinDelay_empty);
}

void PV_BinDelay_empty(PV_BinDelay* unit, int inNumSamples)
{
    int delframe;
    PV_GET_BUF
    SCComplexBuf *p = ToComplexApx(buf);
    int curFrame = unit->m_curFrame - 1;
    int numFrames = unit->m_numFrames;
    if(curFrame < 0) curFrame += numFrames;
    unit->m_curFrame = curFrame;
    float srbins = unit->m_srbins;

    SETUP_DELS_FB

    SCComplexBuf *delFrame = unit->m_databuf[curFrame];

    memcpy(delFrame->bin, p->bin, numbins * sizeof(SCComplex));

    for(int i = 0; i < numbins; i++){
	delframe = curFrame + (int)roundf(deltimes[i] * srbins);
	if(delframe >= numFrames) {
	    p->bin[i].real = 0.f;
	    p->bin[i].imag = 0.f;
	    } else {
	    p->bin[i] = unit->m_databuf[delframe]->bin[i];
	    SCPolar thisone = unit->m_databuf[delframe]->bin[i].ToPolar();	
	    thisone.mag *= fb[i];	
	    unit->m_databuf[delframe]->bin[i] = thisone.ToComplex();
	    unit->m_databuf[curFrame]->bin[i] += unit->m_databuf[delframe]->bin[i];
	    }
	}
	
    unit->m_databuf[curFrame] = delFrame;
    
    unit->m_elapsedFrames++;
    
    if(unit->m_elapsedFrames == numFrames) {
	SETCALC(PV_BinDelay_next);
	}

}

void PV_BinDelay_next(PV_BinDelay* unit, int inNumSamples)
{
    int delframe;
    PV_GET_BUF
    SCComplexBuf *p = ToComplexApx(buf);
    int curFrame = unit->m_curFrame - 1;
    int numFrames = unit->m_numFrames;
    if(curFrame < 0) curFrame += numFrames;
    unit->m_curFrame = curFrame;
    float srbins = unit->m_srbins;

    SETUP_DELS_FB

    SCComplexBuf *delFrame = unit->m_databuf[curFrame];

    memcpy(delFrame->bin, p->bin, numbins * sizeof(SCComplex));

    for(int i = 0; i < numbins; i++){
	delframe = curFrame + (int)roundf(deltimes[i] * srbins);
	if(delframe >= numFrames) delframe -= numFrames;
	p->bin[i] = unit->m_databuf[delframe]->bin[i];
	SCPolar thisone = unit->m_databuf[delframe]->bin[i].ToPolar();	
	thisone.mag *= fb[i];	
	unit->m_databuf[delframe]->bin[i] = thisone.ToComplex();
	unit->m_databuf[curFrame]->bin[i] += unit->m_databuf[delframe]->bin[i];
	}
	
    unit->m_databuf[curFrame] = delFrame;

}


//////////////////////

void PV_Freeze_next(PV_Freeze *unit, int inNumSamples)
{
	float curphase;
	
	PV_GET_BUF
	
	if (!unit->m_mags) {
		unit->m_mags = (float*)RTAlloc(unit->mWorld, numbins * sizeof(float));
		unit->m_difPhases = (float*)RTAlloc(unit->mWorld, numbins * sizeof(float));
		unit->m_prevPhases = (float*)RTAlloc(unit->mWorld, numbins * sizeof(float));
		unit->m_numbins = numbins;
	} else if (numbins != unit->m_numbins) return;

	SCPolarBuf *p = ToPolarApx(buf);

	float freeze = ZIN0(1);
	float *mags = unit->m_mags;
	float *difPhases = unit->m_difPhases;
	float *prevPhases = unit->m_prevPhases;
	if (freeze > 0.f) {
		for (int i=0; i<numbins; ++i) {
			p->bin[i].mag = mags[i];
			prevPhases[i] += difPhases[i];
			while (prevPhases[i] > pi) /* unwrap the phase */ 
			    prevPhases[i] -= twopi; 
			while (prevPhases[i] < pi) 
			    prevPhases[i] += twopi; 
			p->bin[i].phase = prevPhases[i];
		}
		p->dc = unit->m_dc;
		p->nyq = unit->m_nyq;
	} else {
		for (int i=0; i<numbins; ++i) {
			mags[i] = p->bin[i].mag;
			// store the difference in phase between the current and last frame
			difPhases[i] = p->bin[i].phase - prevPhases[i];
			prevPhases[i] = p->bin[i].phase;
		}
		unit->m_dc = p->dc;
		unit->m_nyq = p->nyq;
	}
}


void PV_Freeze_Ctor(PV_Freeze* unit)
{
	SETCALC(PV_Freeze_next);
	ZOUT0(0) = ZIN0(0);
	unit->m_mags = 0;
}

void PV_Freeze_Dtor(PV_Freeze* unit)
{
	RTFree(unit->mWorld, unit->m_mags);
	RTFree(unit->mWorld, unit->m_difPhases);
	RTFree(unit->mWorld, unit->m_prevPhases);
}

void init_SCComplex(InterfaceTable *inTable);

#define DefinePVUnit(name) \
	(*ft->fDefineUnit)(#name, sizeof(PV_Unit), (UnitCtorFunc)&name##_Ctor, 0, 0);
	
void load(InterfaceTable *inTable)
    {
    	ft = inTable;
	init_SCComplex(inTable);
	
	DefineDtorUnit(PV_PartialSynthF);
	DefineDtorUnit(PV_NoiseSynthF);
	DefineDtorUnit(PV_PartialSynthP);
	DefineDtorUnit(PV_NoiseSynthP);
	DefinePVUnit(PV_MagMap);
	DefinePVUnit(PV_MaxMagN);
	DefinePVUnit(PV_MinMagN);
	DefineDtorUnit(PV_FreqBuffer);
	DefinePVUnit(PV_MagBuffer);
	DefinePVUnit(PV_OddBin);
	DefinePVUnit(PV_EvenBin);
	DefinePVUnit(PV_Invert);
	DefineDtorUnit(PV_BinDelay);
	DefineDtorUnit(PV_Freeze);
	}
	
