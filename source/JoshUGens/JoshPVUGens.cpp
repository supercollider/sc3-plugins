/*
 *  JoshPVUGens.cpp
 *  xSC3plugins
 *
 *  Created by Josh Parmenter on 9/27/05.
 *  Copyright 2005 __MyCompanyName__. All rights reserved.
 *
 */

//third party Phase Vocoder UGens

#include "SC_fftlib.h"
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


struct PV_NoiseSynthP : PV_Unit
{
    int m_numFrames, m_numLoops, m_remainingLoops, m_curframe, m_numframes, m_numbins, m_nextflag;
    float *m_phases;
    float *m_phasedifs;
    float *m_prevphasedifs;
    SndBuf *m_buf;
};

struct PV_PartialSynthP : PV_Unit
{
    int m_numFrames, m_numLoops, m_remainingLoops, m_curframe, m_numframes, m_numbins, m_nextflag;
    float *m_phases;
    float *m_phasedifs;
    float *m_prevphasedifs;
    SndBuf *m_buf;
};

struct PV_PartialSynthF : PV_Unit
{
    int m_numFrames, m_numLoops, m_remainingLoops, m_curframe, m_numframes, m_numbins, m_nextflag;
    float *m_phases;
    float *m_freqs;
    float *m_centerfreqs;
    SndBuf *m_buf;
    };

struct PV_NoiseSynthF : PV_Unit
{
    int m_numFrames, m_numLoops, m_remainingLoops, m_curframe, m_numframes, m_numbins, m_nextflag;
    float *m_phases;
    float *m_freqs;
    float *m_centerfreqs;
    SndBuf *m_buf;
    };

struct PV_MagMap : PV_Unit
{
    // contains a buffer with mag rescaling
    float m_fmagbufnum;
    SndBuf *m_magbuf;
    SndBuf *m_buf;
    };

struct PV_MaxMagN : PV_Unit
{
    SndBuf *m_buf;
};

struct PV_MinMagN : PV_Unit
{
    SndBuf *m_buf;
};

struct PV_FreqBuffer : PV_Unit
{
    SndBuf *m_databuf;
    float m_fdatabufnum;
    int m_numloops, m_numbins, m_firstflag;
    float *m_phases;
    float *m_centerfreqs;
    float *m_freqs;
    SndBuf *m_buf;
};

struct PV_MagBuffer : PV_Unit
{
    SndBuf *m_databuf;
    float m_fdatabufnum;
    int m_numloops;
    SndBuf *m_buf;
};

struct BinData : PV_Unit
{
    int m_bin, m_firstflag;
    float m_lastPhase, m_lastPhasedif, m_centerfreq, m_curfreq, m_curmag, m_srOverTwopi, m_rNumbins, m_overlaps, m_rNumPeriodsPerValue;
    float m_freqinc, m_maginc;
    int elapsedSamps;
    SndBuf *m_buf;
};


struct PV_OddBin : PV_Unit
{
    SndBuf *m_buf;
};

struct PV_EvenBin : PV_Unit
{
    SndBuf *m_buf;
};

struct PV_BinFilter : PV_Unit
{
    SndBuf *m_buf;
};

struct PV_Invert : PV_Unit
{
    SndBuf *m_buf;
};


const int MAXDELAYBUFS = 512;

struct PV_BinDelay : PV_Unit
{
	SCComplexBuf *m_databuf[MAXDELAYBUFS];
	SndBuf *m_deltimes; // the buffer holding the delay ammounts
	SndBuf *m_fb; // the biffer holding the fedback values
	float m_deltimesbufnum, m_fbbufnum;
	float m_srbins, m_hop;
	int m_numFrames, m_curFrame, m_elapsedFrames;
	int dataFlag[MAXDELAYBUFS];
	SndBuf *m_buf;
};

struct PV_Freeze : PV_Unit
{
	int m_numbins;
	float *m_mags, m_dc, m_nyq;
	float *m_prevPhases, *m_difPhases;
	SndBuf *m_buf;
};

// write values to a buffer
struct PV_RecordBuf : PV_Unit
{
	float m_fdatabufnum;
	SndBuf *m_databuf;
	int m_frame, m_numAvailFrames;
	bool first;
	SndBuf *m_buf;
};

struct PV_PlayBuf : PV_Unit
{
	float m_fdatabufnum, m_frame;
	SndBuf *m_databuf;
	int m_numAvailFrames, m_numPeriods, m_periodsRemain;
	float *m_prevDatabuf;
	bool first;
	SndBuf *m_buf;
    };

struct PV_BinPlayBuf : PV_Unit
{
	float m_fdatabufnum, m_frame;
	SndBuf *m_databuf;
	int m_numAvailFrames, m_numPeriods, m_periodsRemain;
	float *m_prevDatabuf;
	bool first;
	float *m_bins;
        SndBuf *m_buf;
    };

struct PV_BufRd : PV_Unit
{
	float m_fdatabufnum;
	SndBuf *m_databuf;
	int m_numAvailFrames, m_numPeriods, m_periodsRemain;
	float *m_prevDatabuf;
	bool first;
        SndBuf *m_buf;
    };

struct PV_BinBufRd : PV_Unit
{
	float m_fdatabufnum;
	SndBuf *m_databuf;
	int m_numAvailFrames, m_numPeriods, m_periodsRemain;
	float *m_prevDatabuf;
	float *m_bins;
	bool first;
        SndBuf *m_buf;
    };

struct PV_SpectralMap : PV_Unit
{
    int m_numbins;
    float *m_mags;
    SndBuf *m_buf;
};

struct PV_SpectralEnhance : PV_Unit
{
    SndBuf *m_buf;
    float *m_phases;
    bool first;
};

struct PV_PitchShift : PV_Unit
{
    float *m_phases;
    SndBuf *m_buf;
    bool first;
};

extern "C"
{
	#include "SC_fftlib.h"

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

	void BinData_Ctor(BinData *unit);
	void BinData_next(BinData *unit, int inNumSamples);

	void PV_OddBin_Ctor(PV_OddBin *unit);
	void PV_OddBin_next(PV_OddBin* unit, int inNumSamples);

	void PV_EvenBin_Ctor(PV_EvenBin *unit);
	void PV_EvenBin_next(PV_EvenBin* unit, int inNumSamples);

	void PV_BinFilter_Ctor(PV_BinFilter *unit);
	void PV_BinFilter_next(PV_BinFilter* unit, int inNumSamples);

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

	void PV_RecordBuf_Ctor(PV_RecordBuf *unit);
	void PV_RecordBuf_next(PV_RecordBuf *unit, int inNumSamples);

	void PV_PlayBuf_Ctor(PV_PlayBuf *unit);
	void PV_PlayBuf_Dtor(PV_PlayBuf *unit);
	void PV_PlayBuf_next(PV_PlayBuf *unit, int inNumSamples);
	void PV_PlayBuf_first(PV_PlayBuf *unit, int inNumSamples);

	void PV_BinPlayBuf_Ctor(PV_BinPlayBuf *unit);
	void PV_BinPlayBuf_Dtor(PV_BinPlayBuf *unit);
	void PV_BinPlayBuf_next(PV_BinPlayBuf *unit, int inNumSamples);
	void PV_BinPlayBuf_first(PV_BinPlayBuf *unit, int inNumSamples);

	void PV_BufRd_Ctor(PV_BufRd *unit);
	void PV_BufRd_Dtor(PV_BufRd *unit);
	void PV_BufRd_next(PV_BufRd *unit, int inNumSamples);
	void PV_BufRd_first(PV_BufRd *unit, int inNumSamples);

	void PV_BinBufRd_Ctor(PV_BinBufRd *unit);
	void PV_BinBufRd_Dtor(PV_BinBufRd *unit);
	void PV_BinBufRd_next(PV_BinBufRd *unit, int inNumSamples);
	void PV_BinBufRd_first(PV_BinBufRd *unit, int inNumSamples);

    void PV_SpectralMap_Ctor(PV_SpectralMap *unit);
    void PV_SpectralMap_Dtor(PV_SpectralMap *unit);
    void PV_SpectralMap_next(PV_SpectralMap *unit, int inNumSamples);

    void PV_SpectralEnhance_Ctor(PV_SpectralEnhance *unit);
    void PV_SpectralEnhance_next(PV_SpectralEnhance *unit, int inNumSamples);

    void PV_PitchShift_Ctor(PV_PitchShift *unit);
    void PV_PitchShift_Dtor(PV_PitchShift *unit);
    void PV_PitchShift_next(PV_PitchShift *unit, int inNumSamples);

}

#define CALC_FREQS \
	for (int i = 0; i < numbins; i++){ \
	    float phasedif = p->bin[i].phase - phases[i]; /* get the phase differece */ \
	    while (phasedif > pi) /* wrap the phase */ \
		phasedif -= twopi; \
	    while (phasedif < -pi) \
		phasedif += twopi; \
	    /* calculate the freq */ \
	    freq = (sr / twopi) * (unit->m_centerfreqs[i] + (phasedif / (float)numbins)); \
	    freqs[i + (numbins * unit->m_curframe)] = freq; /* save the freqs */ \
	    /* store the current phases to the buffer */ \
	    phases[i] = p->bin[i].phase; \
	} \
	unit->m_curframe = (unit->m_curframe + 1) % unit->m_numFrames;

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
		while (phase > pi) /* wrap the phase */ \
		    phase -= twopi; \
		while (phase < -pi) \
		    phase += twopi; \
		float phasedif = phase - prevphase; /* get the phase differece */ \
		while (phasedif > pi) /* wrap the phase */ \
		    phasedif -= twopi; \
		while (phasedif < -pi) \
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
	    OUT0(0) = -1.f; //ClearUnitOutputs(unit, inNumSamples);
		return;
	}
	float *magbufData __attribute__((__unused__)) = magbuf->data;
	if (!magbufData) {
	    OUT0(0) = -1.f; //ClearUnitOutputs(unit, inNumSamples);
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
	    OUT0(0) = -1.f; //ClearUnitOutputs(unit, inNumSamples);
		return;
	}
	float *databufData __attribute__((__unused__)) = databuf->data;


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
			while (phasedif < -pi)
			    phasedif += twopi;
			/* calculate the freq */
			float freq = (sr / twopi) * (unit->m_centerfreqs[i] + (phasedif / (float)numbins));
			float* table0 = databufData + i;
			table0[0] = freq; /* save the freq */
			/* store the current phases to the buffer */
			unit->m_phases[i] = p->bin[i].phase;
			}

		}
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
	    OUT0(0) = -1.f; //ClearUnitOutputs(unit, inNumSamples);
		return;
	}
	float *databufData __attribute__((__unused__)) = databuf->data;
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
	    OUT0(0) = -1.f; //ClearUnitOutputs(unit, inNumSamples);
		return;
	}
	float *databufData __attribute__((__unused__)) = databuf->data;


	for(int i = 0; i < numbins; i++){
	    databufData[i] = p->bin[i].mag;
	    }
}

void BinData_Ctor(BinData *unit)
{
    unit->m_bin = (int)IN0(1);
    unit->m_overlaps = IN0(2);
    unit->m_curfreq = 0.;
    unit->m_curmag = 0.;
    unit->m_freqinc = 0.;
    unit->m_maginc = 0.;
    unit->m_firstflag = -1;
    unit->elapsedSamps = 0;
    unit->m_lastPhasedif = 0.;
    float sr = (float)unit->mWorld->mSampleRate; /* we need the audio rate... calc it here */
    unit->m_srOverTwopi = sr / twopi;
    SETCALC(BinData_next);
    OUT0(0) = -1.f; //ClearUnitOutputs(unit, inNumSamples);
}

void BinData_next(BinData *unit, int inNumSamples)
{
	float fbufnum = IN0(0);
	float *freqOut = OUT(0);
	float *magOut = OUT(1);
	if(fbufnum >= 0.f){
//	    Print("%3,3f\n", (float)unit->elapsedSamps);
//	    unit->elapsedSamps = 0;
	    uint32 ibufnum = (uint32)fbufnum;
	    World *world = unit->mWorld;
	    if (ibufnum >= world->mNumSndBufs) ibufnum = 0;
	    SndBuf *buf = world->mSndBufs + ibufnum;
	    float fnumbins = (float)(buf->samples - 2 >> 1);
	    int bin = unit->m_bin;
	    SCPolarBuf *p = ToPolarApx(buf);
	    float sr = (float)unit->mWorld->mSampleRate; /* we need the audio rate... calc it here */

	    // These calculations only have to be done the first time FFT data comes in.
	    // Sorry - for now, can't change FFT buffer
	    if(unit->m_firstflag < 0){
		float rInNumSamples = 1. / (float)inNumSamples;
		unit->m_firstflag = 1;
		// initilaize the curfreq slot to the centerfreq
		unit->m_curfreq = unit->m_centerfreq = bin * (sr / (fnumbins * 2.));
		unit->m_rNumbins = 1. / fnumbins;
		unit->m_lastPhase = 0.;
		// to determine how big the increment between output values between FFT frames, a number of
		// things need to be taken into account - is BinData running at k-rate or a-rate?
		// how many overlaps in the FFT? what is the FFT size? This figures that out, and stores
		// it to the struct so we NEVER HAVE TO DO THIS NASTY DIVISION AGAIN!!!
		unit->m_rNumPeriodsPerValue = (1. / (((float)buf->samples * unit->m_overlaps) /
		    ((float)unit->mWorld->mBufLength))) * rInNumSamples;
		}

	    /* look for change in phase, wrap between 0 and two pi, calculate the freq */
	    float curphase = p->bin[bin].phase;
	    float phasedif = curphase - unit->m_lastPhase; //lastPhasedif - curPhasedif;
	    while (phasedif > pi)
		phasedif -=twopi;
	    while (phasedif < -pi)
		phasedif += twopi;
//	    float nextfreq = unit->m_centerfreq + (unit->m_srOverTwopi * (phasedif / fnumbins));
/*
freq = (SR / frameSize) * (bin(i) + ((phase(i) * overlapFactor) / twopi))

(44100 / 512) * (256 + ((0.5pi * 0.5) / 2pi))
*/
	    float nextfreq = (sr / (fnumbins * 2)) * (bin + ((phasedif * (1 / unit->m_overlaps)) / twopi));
	    //(  unit->m_curfreq + (unit->m_srOverTwopi * (phasedif / fnumbins));
	    unit->m_lastPhase = curphase;

	    // find the amount to change freq and mag over each sample OR control period (depending on if BinData is ar or kr
	    unit->m_maginc = (p->bin[bin].mag - unit->m_curmag) * unit->m_rNumPeriodsPerValue;
	    unit->m_freqinc = (nextfreq - unit->m_curfreq) * unit->m_rNumPeriodsPerValue;

// 	    Print("Next values! %3,3f, %3,3f, %3,3f\n", unit->m_curmag, p->bin[bin].mag, unit->m_maginc);

	    } //else {
//	    unit->elapsedSamps += inNumSamples;
//	    }

	// check if there is even data that is ready yet. If not, clear the outputs
	if(unit->m_firstflag < 0) {
	    OUT0(0) = -1.f; //ClearUnitOutputs(unit, inNumSamples);
	    } else {
	    // otherwise, output the current freq and mag. If a-rate, these values will be inc'd
	    // sample by sample. If not, they will increase every k-rate
	    for(int i = 0; i < inNumSamples; i++){
		freqOut[i] = unit->m_curfreq;
		magOut[i] = unit->m_curmag;
		unit->m_curfreq += unit->m_freqinc;
		unit->m_curmag += unit->m_maginc;
		}
	    }
//
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

void PV_BinFilter_Ctor(PV_BinFilter *unit)
{
    ZOUT0(0) = ZIN0(0);
    SETCALC(PV_BinFilter_next);
}

void PV_BinFilter_next(PV_BinFilter* unit, int inNumSamples)
{
    PV_GET_BUF
    SCPolarBuf *p = ToPolarApx(buf);
    float start = IN0(1);
    float end = IN0(2);
    int istart = (int)start;
    if (istart < 0) istart = 0;
    int iend = (int)end;
    if (iend >= numbins) iend = numbins;

    for(int i = 0; i < istart; i++){
	p->bin[i].mag = 0.f;
    }
    for(int i = iend + 1; i < numbins; i++){
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
    float hop = unit->m_hop = 0.5 / IN0(4);

    SETUP_DELS_FB
//    Print("%3,3f, %3,3f\n", (float)(sizeof(float)), (float)sizeof(SCPolar));
    // how many frames is that?
    float srbins = unit->m_srbins = ((float)unit->mWorld->mSampleRate / numbins);
    int numFrames = unit->m_numFrames = (int)(srbins * maxdelay * hop) + 1;
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
    float hop = unit->m_hop;
    PV_GET_BUF
    SCComplexBuf *p = ToComplexApx(buf);
    int curFrame = unit->m_curFrame - 1;
    int numFrames = unit->m_numFrames;
    if(curFrame < 0) curFrame += numFrames;
    unit->m_curFrame = curFrame;
    float srbins = unit->m_srbins * hop;

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
    float srbins = unit->m_srbins * unit->m_hop;

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
			while (prevPhases[i] > pi) /* wrap the phase */
			    prevPhases[i] -= twopi;
			while (prevPhases[i] < -pi)
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

// PV_RecordBuf
void PV_RecordBuf_Ctor(PV_RecordBuf* unit)
{
	unit->m_fdatabufnum = -1e9f;
	SETCALC(PV_RecordBuf_next);
	OUT0(0) = IN0(0);
	unit->m_frame = (int)IN0(2);
	unit->first = true;
}

void PV_RecordBuf_next(PV_RecordBuf* unit, int inNumSamples)
{
	int itwo, frameadd, numAvailFrames;

	PV_GET_BUF

	float run = IN0(3);
	float loop = IN0(4);
	// get the buffer to store data in
	float fdatabufnum = IN0(1);
	if (fdatabufnum != unit->m_fdatabufnum) {
		unit->m_fdatabufnum = fdatabufnum;
		uint32 databufnum = (uint32)fdatabufnum;
		World *world = unit->mWorld;
		if (databufnum >= world->mNumSndBufs) databufnum = 0;
		unit->m_databuf = world->mSndBufs + databufnum;
		unit->m_numAvailFrames = (int)(unit->m_databuf->frames / buf->samples);
	}

	SndBuf *databuf = unit->m_databuf;
	if(!databuf) {
	    OUT0(0) = -1.f; //ClearUnitOutputs(unit, inNumSamples);
		return;
	}
	float *databufData __attribute__((__unused__)) = databuf->data;
	numAvailFrames = unit->m_numAvailFrames;

	if(unit->first){
	    databufData[0] = buf->samples;
	    databufData[1] = IN0(5); // hop
	    databufData[2] = IN0(6); // wintype
	    unit->first = false;
	    }

	SCPolarBuf *p = ToPolarApx(buf);
	if((loop > 0.f) && (unit->m_frame >= numAvailFrames)) unit->m_frame -=numAvailFrames;

	if((unit->m_frame < numAvailFrames) && (run > 0.f)){
	    frameadd = (unit->m_frame * buf->samples) + 3;
	    databufData[frameadd] = p->dc;
	    databufData[frameadd + 1] = p->nyq;
	    for(int i = 1, j = 0; i <= numbins; i++, j++){
		itwo = i * 2;
		databufData[frameadd + itwo] = p->bin[j].phase;
		databufData[frameadd + (itwo + 1)]= p->bin[j].mag;
		}

	    unit->m_frame++;
	    }
}

float wrapphase(float phase){
    while (phase > twopi)
	phase -= twopi;
    while (phase < 0.)
	phase += twopi;
    return phase;
    }

#define FILL_FFT \
	    p->dc = lininterp(framepct, databufData[iframeloc], databufData[iframep1loc]); \
	    p->nyq = lininterp(framepct, databufData[iframeloc + 1], databufData[iframep1loc + 1]); \
	    bool phasedecision = (unit->first || (((frame - rate) < 0) && bloop) || (((frame + rate) > numAvailFrames) && bloop)); \
	    for(int i = 1, j = 0; i <= numbins; i++, j++){ \
		itwo = i * 2; \
		phase = databufData[iframeloc + itwo]; \
		phasem1 = databufData[iframem1loc + itwo]; \
		phasep1 = databufData[iframep1loc + itwo]; \
		mag = databufData[iframeloc + itwo + 1]; \
		magp1 = databufData[iframep1loc + itwo + 1]; \
		while (phase < phasem1) \
		    phase += twopi; \
		while (phasep1 < phase) \
		    phasep1 += twopi; \
		phase1 = lininterp(framepct, phasem1, phase); \
		phase2 = lininterp(framepct, phase, phasep1); \
		phasedif = phase2 - phase1; \
		if(phasedecision) \
		    pd[j] = phasedif; \
		    else \
		    pd[j] += phasedif; \
		\
		pd[j] = wrapphase(pd[j]); \
		\
		p->bin[j].phase = pd[j]; \
		maginterp = lininterp(framepct, mag, magp1); \
		p->bin[j].mag = maginterp; \
	    } \



//if(test) Print("Stop 4 %f\n", pd[j]); \


#define FILL_FFT_BINS \
	    p->dc = lininterp(framepct, databufData[iframeloc], databufData[iframep1loc]); \
	    p->nyq = lininterp(framepct, databufData[iframeloc + 1], databufData[iframep1loc + 1]); \
	    bool phasedecision = (unit->first || (((frame - rate) < 0) && bloop) || (((frame + rate) > numAvailFrames) && bloop)); \
	    for(int i = 1, j = 0; j < numbins; i++, j++){ \
		if(bins[j] > 0.0){ \
		    itwo = i * 2; \
		    phase = databufData[iframeloc + itwo]; \
		    phasem1 = databufData[iframem1loc + itwo]; \
		    phasep1 = databufData[iframep1loc + itwo]; \
		    mag = databufData[iframeloc + itwo + 1]; \
		    magp1 = databufData[iframep1loc + itwo + 1]; \
		    while (phase < phasem1) \
			phase += twopi; \
		    while (phasep1 < phase) \
			phasep1 += twopi; \
		    phase1 = lininterp(framepct, phasem1, phase); \
		    phase2 = lininterp(framepct, phase, phasep1); \
		    phasedif = phase2 - phase1; \
		    if(phasedecision) \
			pd[j] = phasedif; \
			else \
			pd[j] += phasedif; \
		    \
		    pd[j] = wrapphase(pd[j]); \
		    p->bin[j].phase = pd[j]; \
		    maginterp = lininterp(framepct, mag, magp1); \
		    p->bin[j].mag = maginterp; \
		    } else { \
		    if(clear)p->bin[j].mag = 0.f; \
		    }\
		}



// for operation on one buffer
#define PV_GET_BUF_INIT \
	float fbufnum = ZIN0(0); \
	if (unit->m_periodsRemain > 0) { ZOUT0(0) = -1.f; unit->m_periodsRemain--; return; } \
	unit->m_periodsRemain = unit->m_numPeriods; \
	ZOUT0(0) = fbufnum; \
	uint32 bufnum = (uint32)fbufnum; \
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
	SndBuf *buf = unit->m_buf; \
	int numbins = buf->samples - 2 >> 1;


#define PV_DATA_INIT_READ \
    SCPolarBuf *p = ToPolarApx(buf); \
    float *pd; \
    \
    frameskip = buf->samples; \
    \
    float fdatabufnum = IN0(1); \
    if (fdatabufnum != unit->m_fdatabufnum) { \
	unit->m_fdatabufnum = fdatabufnum; \
	uint32 databufnum = (uint32)fdatabufnum; \
	World *world = unit->mWorld; \
	if (databufnum >= world->mNumSndBufs) databufnum = 0; \
	    unit->m_databuf = world->mSndBufs + databufnum; \
	    unit->m_numAvailFrames = (int)(unit->m_databuf->frames / frameskip); \
	    } \
    \
    SndBuf *databuf = unit->m_databuf; \
    if(!databuf) { \
	OUT0(0) = -1.f; \
	return; \
    } \
    float *databufData __attribute__((__unused__)) = databuf->data; \
    \
    int numSamples = unit->mWorld->mFullRate.mBufLength; \
    \
    if(unit->first){ \
	unit->m_numPeriods = (int)((databufData[0] * databufData[1]) / numSamples) - 1; \
	unit->m_periodsRemain = unit->m_numPeriods; \
	pd = unit->m_prevDatabuf = (float*)RTAlloc(unit->mWorld, numbins * sizeof(float)); \
	for(int z = 0; z < numbins; z++) pd[z] = 0.0f; \
	unit->first = false; \
    } else { \
	pd = unit->m_prevDatabuf; \
    } \

#define COND_FFT_BUF \
    World *world = unit->mWorld; \
    \
    uint32 ibufnum = (uint32)IN0(0); \
    SndBuf *buf; \
    \
    if (ibufnum >= world->mNumSndBufs) { \
	int localBufNum = ibufnum - world->mNumSndBufs; \
	Graph *parent = unit->mParent; \
	if(localBufNum <= parent->localBufNum) { \
	    buf = parent->mLocalSndBufs + localBufNum; \
	} else { \
	    buf = world->mSndBufs; \
	} \
    } else { \
	buf = world->mSndBufs + ibufnum; \
    } \
    \
    buf->coord = coord_Polar; \


// PV_PlayBuf

void PV_PlayBuf_Ctor(PV_PlayBuf* unit)
{
	unit->m_fdatabufnum = -1e9f;
	SETCALC(PV_PlayBuf_first);
	OUT0(0) = IN0(0);
	unit->first = true;
	unit->m_frame = (int)IN0(3);

	COND_FFT_BUF
}

void PV_PlayBuf_first(PV_PlayBuf* unit, int inNumSamples)
{
	int itwo, iframe, iframem1, iframep1, frameskip, iframeloc, iframem1loc, iframep1loc, numAvailFrames;
	float framepct;
	float phase, phasem1, phasep1, mag, magp1, phase1, phase2, phasedif, maginterp;
	bool test;

    PV_GET_BUF

    PV_DATA_INIT_READ

    test = true;
    if(unit->m_periodsRemain <= 0){
	unit->m_periodsRemain = unit->m_numPeriods;
	float rate = IN0(2);
	float loop = IN0(4);
	bool bloop = loop > 0.;

	numAvailFrames = unit->m_numAvailFrames;
	if(bloop && (unit->m_frame >= numAvailFrames)) unit->m_frame -=numAvailFrames;

	float frame = unit->m_frame;

	iframe = (int)frame;

	iframem1 = iframe - 1;
	iframep1 = iframe + 1;
	if(iframem1 < 0) iframem1 = iframe;
	if(iframep1 >= numAvailFrames) iframep1 = iframe;

	framepct = frame - (float)iframe;

	iframeloc = iframe * frameskip + 3;
	iframem1loc = iframem1 * frameskip + 3;
	iframep1loc = iframep1 * frameskip + 3;
	if((databufData[0] != buf->samples)){
	    Print("WARNING: There is a mismatch between the PV databuffer you are using and this instance of PV_PlayBuf\n");
	    Print("FFT size of databuf: %5,3f\n", (float)databufData[0]);
	    Print("FFT size of current process: %5,3f\n", (float) buf->samples);
	    }
	if(frame < numAvailFrames){

	    FILL_FFT

	}

	unit->m_prevDatabuf = pd;
	unit->m_frame = frame + rate;
	unit->first = false;

	OUT0(0) = fbufnum;
	SETCALC(PV_PlayBuf_next);
    } else {
	unit->m_periodsRemain -= 1;
	OUT0(0) = -1.f;
    }
}

void PV_PlayBuf_next(PV_PlayBuf* unit, int inNumSamples)
{
	int itwo, iframeloc, iframem1loc, iframep1loc, numAvailFrames, iframe, iframem1, iframep1, frameskip;
	float phasedif, framepct, phase, phasem1, phasep1, phase1, phase2, mag, magp1, maginterp;
    bool test;
    test = false;


	PV_GET_BUF_INIT

	float rate = IN0(2);
	float loop = IN0(4);
	bool bloop = loop > 0.;
	float *pd;

	frameskip = buf->samples;

	// get the buffer to store data in
	float fdatabufnum = IN0(1);

	if (fdatabufnum != unit->m_fdatabufnum) {
		unit->m_fdatabufnum = fdatabufnum;
		uint32 databufnum = (uint32)fdatabufnum;
		World *world = unit->mWorld;
		if (databufnum >= world->mNumSndBufs) databufnum = 0;
		unit->m_databuf = world->mSndBufs + databufnum;
		unit->m_numAvailFrames = (int)(unit->m_databuf->frames / frameskip);
	}

	SndBuf *databuf = unit->m_databuf;
	if(!databuf) {
	    OUT0(0) = -1.f; //ClearUnitOutputs(unit, inNumSamples);
		return;
	}
	float *databufData __attribute__((__unused__)) = databuf->data;

	numAvailFrames = unit->m_numAvailFrames;
	if(bloop && (unit->m_frame >= numAvailFrames)) unit->m_frame -=numAvailFrames;
	if(bloop && (unit->m_frame < 0.f)) unit->m_frame +=numAvailFrames;

	float frame = unit->m_frame;

	iframe = (int)frame;
	iframem1 = iframe - 1;
	iframep1 = iframe + 1;
	if(iframem1 < 0) iframem1 = iframe;
	if(iframep1 >= numAvailFrames) iframep1 = iframe;

	framepct = frame - (float)iframe;

	iframeloc = iframe * frameskip + 3;
	iframem1loc = iframem1 * frameskip + 3;
	iframep1loc = iframep1 * frameskip + 3;

	SCPolarBuf *p = ToPolarApx(buf);

	pd = unit->m_prevDatabuf;

	if(unit->m_frame < numAvailFrames){

	    FILL_FFT

	    unit->m_prevDatabuf = pd;
	    unit->m_frame = frame + rate;
	    }
}

void PV_PlayBuf_Dtor(PV_PlayBuf* unit)
{
    RTFree(unit->mWorld, unit->m_prevDatabuf);
}

// PV_BinPlayBuf

void PV_BinPlayBuf_Ctor(PV_BinPlayBuf* unit)
{
	unit->m_fdatabufnum = -1e9f;
	SETCALC(PV_BinPlayBuf_first);
	OUT0(0) = IN0(0);
	unit->m_frame = IN0(3);
	COND_FFT_BUF
}


void PV_BinPlayBuf_first(PV_BinPlayBuf* unit, int inNumSamples)
{
    int itwo, iframe, iframem1, iframep1, frameskip, iframeloc, iframem1loc, iframep1loc, numAvailFrames;
    float framepct;
    float phase, phasem1, phasep1, mag, magp1, phase1, phase2, phasedif, maginterp;

    PV_GET_BUF

    float rate = IN0(2);
    float loop = IN0(4);
    bool bloop = loop > 0.;

    int binStart = (int)IN0(5);
    int binSkip = (int)IN0(6);
    int numBins = (int)IN0(7);
    bool clear = (IN0(8) > 0.0);


    if((binSkip > 1) && ((numBins * binSkip) > numbins))
    {
	numBins = (int)(numbins / binSkip);
    } else {
	if(numBins > numbins) numBins = numbins;
    }

    float bins[numbins];

    for(int i = 0; i < numbins; i++) bins[i] = 0.f;
    for(int i = 0; i < numBins; i++) bins[binStart + (binSkip * i)] = 1.f;

    float *pd;

    frameskip = buf->samples;

    // get the buffer to store data in
    float fdatabufnum = IN0(1);
    if (fdatabufnum != unit->m_fdatabufnum) {
	unit->m_fdatabufnum = fdatabufnum;
	uint32 databufnum = (uint32)fdatabufnum;
	World *world = unit->mWorld;
	if (databufnum >= world->mNumSndBufs) databufnum = 0;
	unit->m_databuf = world->mSndBufs + databufnum;
	unit->m_numAvailFrames = (int)(unit->m_databuf->frames / frameskip);
    }

    SndBuf *databuf = unit->m_databuf;

    if(!databuf) {
	ClearUnitOutputs(unit, inNumSamples);
	return;
    }
    float *databufData __attribute__((__unused__)) = databuf->data;

    numAvailFrames = unit->m_numAvailFrames;
    if(bloop && (unit->m_frame >= numAvailFrames)) unit->m_frame -=numAvailFrames;

    float frame = unit->m_frame;

    iframe = (int)frame;

    iframem1 = iframe - 1;
    iframep1 = iframe + 1;
    if(iframem1 < 0) iframem1 = iframe;
    if(iframep1 >= numAvailFrames) iframep1 = iframe;

    framepct = frame - (float)iframe;

    iframeloc = iframe * frameskip + 3;
    iframem1loc = iframem1 * frameskip + 3;
    iframep1loc = iframep1 * frameskip + 3;

    SCPolarBuf *p = ToPolarApx(buf);

    // this buffer will hold the previous output's frame data
    pd = unit->m_prevDatabuf = (float*)RTAlloc(unit->mWorld, numbins * sizeof(float));

    if(databufData[0] != buf->samples){
	Print("WARNING: There is a mismatch between the PV databuffer you are using and this instance of PV_BinPlayBuf\n");
	Print("FFT size of databuf: %5,3f\n", (float)databufData[0]);
	Print("FFT size of current process: %5,3f\n", (float) buf->samples);
    }
    if(frame < numAvailFrames){

	FILL_FFT_BINS

	}

    unit->m_prevDatabuf = pd;
    unit->m_frame = frame + rate;
    unit->first = false;

    SETCALC(PV_BinPlayBuf_next);
}

void PV_BinPlayBuf_next(PV_BinPlayBuf* unit, int inNumSamples)
{
    int itwo, iframeloc, iframem1loc, iframep1loc, numAvailFrames, iframe, iframem1, iframep1, frameskip;
    float phasedif, framepct, phase, phasem1, phasep1, phase1, phase2, mag, magp1, maginterp;

    PV_GET_BUF

    float rate = IN0(2);
    float loop = IN0(4);
    bool bloop = loop > 0.;
    float *pd;

    int binStart = (int)IN0(5);
    int binSkip = (int)IN0(6);
    int numBins = (int)IN0(7);
    bool clear = (IN0(8) > 0.0);

    if((binSkip > 1) && ((numBins * binSkip) > numbins))
    {
	numBins = (int)(numbins / binSkip);
    } else {
	if(numBins > numbins) numBins = numbins;
    }

    float bins[numbins];

    for(int i = 0; i < numbins; i++) bins[i] = 0.f;
    for(int i = 0; i < numBins; i++) bins[binStart + (binSkip * i)] = 1.f;

    frameskip = buf->samples;

    // get the buffer to store data in
    float fdatabufnum = IN0(1);

    if (fdatabufnum != unit->m_fdatabufnum) {
	unit->m_fdatabufnum = fdatabufnum;
	uint32 databufnum = (uint32)fdatabufnum;
	World *world = unit->mWorld;
	if (databufnum >= world->mNumSndBufs) databufnum = 0;
	unit->m_databuf = world->mSndBufs + databufnum;
	unit->m_numAvailFrames = (int)(unit->m_databuf->frames / frameskip);
    }

    SndBuf *databuf = unit->m_databuf;
    if(!databuf) {
	ClearUnitOutputs(unit, inNumSamples);
	return;
    }
    float *databufData __attribute__((__unused__)) = databuf->data;

    numAvailFrames = unit->m_numAvailFrames;
    if(bloop && (unit->m_frame >= numAvailFrames)) unit->m_frame -=numAvailFrames;
    if(bloop && (unit->m_frame < 0.f)) unit->m_frame +=numAvailFrames;

    float frame = unit->m_frame;

    iframe = (int)frame;
    iframem1 = iframe - 1;
    iframep1 = iframe + 1;
    if(iframem1 < 0) iframem1 = iframe;
    if(iframep1 >= numAvailFrames) iframep1 = iframe;

    framepct = frame - (float)iframe;

    iframeloc = iframe * frameskip + 3;
    iframem1loc = iframem1 * frameskip + 3;
    iframep1loc = iframep1 * frameskip + 3;

    SCPolarBuf *p = ToPolarApx(buf);

    pd = unit->m_prevDatabuf;

    if(unit->m_frame < numAvailFrames){

	FILL_FFT_BINS

	}

    unit->m_prevDatabuf = pd;
    unit->m_frame = frame + rate;

}


void PV_BinPlayBuf_Dtor(PV_BinPlayBuf* unit)
{
    RTFree(unit->mWorld, unit->m_prevDatabuf);
}

// PV_BufRd

void PV_BufRd_Ctor(PV_BufRd* unit)
{
	unit->m_fdatabufnum = -1e9f;
	SETCALC(PV_BufRd_first);
	OUT0(0) = IN0(0);
	unit->first = true;

	COND_FFT_BUF
}

void PV_BufRd_first(PV_BufRd* unit, int inNumSamples)
{
    int itwo, iframe, iframem1, iframep1, frameskip, iframeloc, iframem1loc, iframep1loc, numAvailFrames;
    float framepct;
    float phase, phasem1, phasep1, mag, magp1, phase1, phase2, phasedif, maginterp;
    bool test;
    test = true;

    PV_GET_BUF

    PV_DATA_INIT_READ

    if(unit->m_periodsRemain <= 0){
	unit->m_periodsRemain = unit->m_numPeriods;
	float point = IN0(2);
	bool bloop = true; //loop > 0.;
	float rate = 1.; // this helps with logic in FFT fill


	numAvailFrames = unit->m_numAvailFrames;

	while(point > 1.0) point -= 1.0;
	while(point < 0.0) point += 1.0;

	float frame = point * numAvailFrames;

	iframe = (int)frame;
	framepct = frame - (float)iframe;

	iframem1 = iframe - 1;
	iframep1 = iframe + 1;
	if(iframem1 < 0) iframem1 = iframe;
	if(iframep1 >= numAvailFrames) iframep1 = iframe;

	iframeloc = iframe * frameskip + 3;
	iframem1loc = iframem1 * frameskip + 3;
	iframep1loc = iframep1 * frameskip + 3;

	if(databufData[0] != buf->samples){
	    Print("WARNING: There is a mismatch between the PV databuffer you are using and this instance of PV_BufRd\n");
	    Print("FFT size of databuf: %5,3f\n", (float)databufData[0]);
	    Print("FFT size of current process: %5,3f\n", (float)buf->samples);
	    }
	if(frame < numAvailFrames){

	    FILL_FFT

	    unit->m_prevDatabuf = pd;
	    }

	SETCALC(PV_BufRd_next);

    } else {
	unit->m_periodsRemain -= 1;
	OUT0(0) = -1.f;
    }
}

void PV_BufRd_next(PV_BufRd* unit, int inNumSamples)
{
	int itwo, iframeloc, iframem1loc, iframep1loc, numAvailFrames, iframe, iframem1, iframep1, frameskip;
	float phasedif, framepct, phase, phasem1, phasep1, phase1, phase2, mag, magp1, maginterp;

    bool test;
    test = false;

	PV_GET_BUF_INIT

	float point = IN0(2);
	bool bloop = true;
	float *pd;
	float rate = 1.;

	frameskip = buf->samples;

	float fdatabufnum = IN0(1);

	if (fdatabufnum != unit->m_fdatabufnum) {
		unit->m_fdatabufnum = fdatabufnum;
		uint32 databufnum = (uint32)fdatabufnum;
		World *world = unit->mWorld;
		if (databufnum >= world->mNumSndBufs) databufnum = 0;
		unit->m_databuf = world->mSndBufs + databufnum;
		unit->m_numAvailFrames = (int)(unit->m_databuf->frames / frameskip);
	}

	SndBuf *databuf = unit->m_databuf;
	if(!databuf) {
	    OUT0(0) = -1.f;
		return;
	}
	float *databufData __attribute__((__unused__)) = databuf->data;

	numAvailFrames = unit->m_numAvailFrames;

	while(point > 1.0) point -= 1.0;
	while(point < 0.0) point += 1.0;

	float frame = point * numAvailFrames;

	iframe = (int)frame;
	framepct = frame - (float)iframe;

	iframem1 = iframe - 1;
	iframep1 = iframe + 1;
	if(iframem1 < 0) iframem1 = iframe;
	if(iframep1 >= numAvailFrames) iframep1 = iframe;

	iframeloc = iframe * frameskip + 3;
	iframem1loc = iframem1 * frameskip + 3;
	iframep1loc = iframep1 * frameskip + 3;

	SCPolarBuf *p = ToPolarApx(buf);

	pd = unit->m_prevDatabuf;

	if(frame < numAvailFrames){

	    FILL_FFT

	    unit->m_prevDatabuf = pd;
	    }
}

void PV_BufRd_Dtor(PV_BufRd* unit)
{
    RTFree(unit->mWorld, unit->m_prevDatabuf);
}


// PV_BinBufRd

void PV_BinBufRd_Ctor(PV_BinBufRd* unit)
{
	unit->m_fdatabufnum = -1e9f;
	SETCALC(PV_BinBufRd_first);
	OUT0(0) = IN0(0);
	unit->first = true;
}

void PV_BinBufRd_first(PV_BinBufRd* unit, int inNumSamples)
{
    int itwo, iframe, iframem1, iframep1, frameskip, iframeloc, iframem1loc, iframep1loc, numAvailFrames;
    float framepct;
    float phase, phasem1, phasep1, mag, magp1, phase1, phase2, phasedif, maginterp;

    PV_GET_BUF

    float point = IN0(2);
    bool bloop = true; //loop > 0.;
    float rate = 1.; // this helps with logic in FFT fill

    int binStart = (int)IN0(3);
    int binSkip = (int)IN0(4);
    int numBins = (int)IN0(5);
    bool clear = (IN0(6) > 0.0);

    if((binSkip > 1) && ((numBins * binSkip) > numbins))
    {
	numBins = (int)(numbins / binSkip);
    } else {
	if(numBins > numbins) numBins = numbins;
    }

    float bins[numbins];

    for(int i = 0; i < numbins; i++) bins[i] = 0.f;
    for(int i = 0; i < numBins; i++) bins[binStart + (binSkip * i)] = 1.f;

    float *pd;

    frameskip = buf->samples;

    // get the buffer to store data in
    float fdatabufnum = IN0(1);
    if (fdatabufnum != unit->m_fdatabufnum) {
	unit->m_fdatabufnum = fdatabufnum;
	uint32 databufnum = (uint32)fdatabufnum;
	World *world = unit->mWorld;
	if (databufnum >= world->mNumSndBufs) databufnum = 0;
	unit->m_databuf = world->mSndBufs + databufnum;
	unit->m_numAvailFrames = (int)(unit->m_databuf->frames / frameskip);
    }

    SndBuf *databuf = unit->m_databuf;

    if(!databuf) {
	ClearUnitOutputs(unit, inNumSamples);
	return;
    }
    float *databufData __attribute__((__unused__)) = databuf->data;

    numAvailFrames = unit->m_numAvailFrames;

    while(point > 1.0) point -= 1.0;
    while(point < 0.0) point += 1.0;

    float frame = point * numAvailFrames;

    iframe = (int)frame;
    framepct = frame - (float)iframe;

    iframem1 = iframe - 1;
    iframep1 = iframe + 1;
    if(iframem1 < 0) iframem1 = iframe;
    if(iframep1 >= numAvailFrames) iframep1 = iframe;

    iframeloc = iframe * frameskip + 3;
    iframem1loc = iframem1 * frameskip + 3;
    iframep1loc = iframep1 * frameskip + 3;

    SCPolarBuf *p = ToPolarApx(buf);

    // this buffer will hold the previous output's frame data
    pd = unit->m_prevDatabuf = (float*)RTAlloc(unit->mWorld, numbins * sizeof(float));

    if(databufData[0] != buf->samples){
	Print("WARNING: There is a mismatch between the PV databuffer you are using and this instance of PV_BufRd\n");
	Print("FFT size of databuf: %5,3f\n", (float)databufData[0]);
	Print("FFT size of current process: %5,3f\n", (float) buf->samples);
    }
    if(frame < numAvailFrames){
	FILL_FFT_BINS
    }
    unit->m_prevDatabuf = pd;
    unit->first = false;

    SETCALC(PV_BinBufRd_next);
}

void PV_BinBufRd_next(PV_BinBufRd* unit, int inNumSamples)
{
    int itwo, iframeloc, iframem1loc, iframep1loc, numAvailFrames, iframe, iframem1, iframep1, frameskip;
    float phasedif, framepct, phase, phasem1, phasep1, phase1, phase2, mag, magp1, maginterp;

    PV_GET_BUF

    float point = IN0(2);
    bool bloop = true; //loop > 0.;
    float *pd;
    float rate = 1.; // this helps with logic in FFT fill

    int binStart = (int)IN0(3);
    int binSkip = (int)IN0(4);
    int numBins = (int)IN0(5);
    bool clear = (IN0(6) > 0.0);

    if((binSkip > 1) && ((numBins * binSkip) > numbins))
    {
	numBins = (int)(numbins / binSkip);
    } else {
	if(numBins > numbins) numBins = numbins;
    }

    float bins[numbins];

    for(int i = 0; i < numbins; i++) bins[i] = 0.f;
    for(int i = 0; i < numBins; i++) bins[binStart + (binSkip * i)] = 1.f;

    frameskip = buf->samples;

    // get the buffer to store data in
    float fdatabufnum = IN0(1);

    if (fdatabufnum != unit->m_fdatabufnum) {
	unit->m_fdatabufnum = fdatabufnum;
	uint32 databufnum = (uint32)fdatabufnum;
	World *world = unit->mWorld;
	if (databufnum >= world->mNumSndBufs) databufnum = 0;
	unit->m_databuf = world->mSndBufs + databufnum;
	unit->m_numAvailFrames = (int)(unit->m_databuf->frames / frameskip);
    }

    SndBuf *databuf = unit->m_databuf;
    if(!databuf) {
	ClearUnitOutputs(unit, inNumSamples);
	return;
    }
    float *databufData __attribute__((__unused__)) = databuf->data;

    numAvailFrames = unit->m_numAvailFrames;

    while(point > 1.0) point -= 1.0;
    while(point < 0.0) point += 1.0;

    float frame = point * numAvailFrames;

    iframe = (int)frame;
    framepct = frame - (float)iframe;

    iframem1 = iframe - 1;
    iframep1 = iframe + 1;
    if(iframem1 < 0) iframem1 = iframe;
    if(iframep1 >= numAvailFrames) iframep1 = iframe;

    iframeloc = iframe * frameskip + 3;
    iframem1loc = iframem1 * frameskip + 3;
    iframep1loc = iframep1 * frameskip + 3;

    SCPolarBuf *p = ToPolarApx(buf);

    pd = unit->m_prevDatabuf;

    if(frame < numAvailFrames){

	FILL_FFT_BINS

    }

    unit->m_prevDatabuf = pd;

}

void PV_BinBufRd_Dtor(PV_BinBufRd* unit)
{
    RTFree(unit->mWorld, unit->m_prevDatabuf);
}

/* PV_SpectralMap */
void PV_SpectralMap_next(PV_SpectralMap *unit, int inNumSamples)
{
    float maxMag = 0.0f;
    float rMaxMag, scaler, rScaler;
    bool rejectFlag = false;
    scaler = rScaler = 1.0;

    PV_GET_BUF2

    SCPolarBuf *p = ToPolarApx(buf1);
    SCPolarBuf *s = ToPolarApx(buf2);

    if (!unit->m_mags) {
	unit->m_mags = (float*)RTAlloc(unit->mWorld, numbins * sizeof(float));
	Clear(numbins, unit->m_mags);
	unit->m_numbins = numbins;
    } else if (numbins != unit->m_numbins) return;

    float freeze = ZIN0(3);
    float floor = ZIN0(2);
    float mode = ZIN0(4); // 1 acts as bandpass, -1 as bandreject, 0 hs no effect
    float norm = ZIN0(5);

    float *mags = unit->m_mags;

    if (freeze <= 0.f) {
	if(norm <= 0.f) {
	    for (int i=0; i<numbins; ++i) {
		mags[i] = s->bin[i].mag;
		if(maxMag < mags[i]) maxMag = mags[i];
	    }
	    // make sure there isn't a divide by 0.0;
	    if(maxMag > 0.00000001){
		rMaxMag = 1.0 / maxMag;
	    } else {
		rMaxMag = 0.0;
	    }
	    for(int i = 0; i < numbins; ++i){
		unit->m_mags[i] = mags[i] *= rMaxMag;
	    }
	} else {
	    float wintype = ZIN0(6);
	    if(wintype >= 1){
		scaler = numbins * 0.5;
	    } else {
		if(wintype >= 0.0){
		    scaler = numbins * 0.6369426751592;
		} else {
		    scaler = numbins;
		}
	    }
	    rScaler = 1.0 / scaler;
	    for (int i=0; i<numbins; ++i) {
		unit->m_mags[i] = mags[i] = s->bin[i].mag * rScaler;
	    }
	}
    }
    if(mode > 0.0){
	rejectFlag = false;
	if(mode > 1.0){
	    mode = 1.0;
	}
    } else {
	rejectFlag = true;
	if(mode < -1.0){
	    mode = -1.0;
	}
    }
    float amode = fabs(mode);
    float onemamode = 1.0 - amode;
    if(rejectFlag){
	if(mode == -1.0){
	    for (int i=0; i<numbins; ++i) {
		if(mags[i] > floor){
		    p->bin[i].mag *= 1.0 - mags[i];
		} else {
		    p->bin[i].mag *= onemamode;
		}
	    }
	} else {
	for (int i=0; i<numbins; ++i) {
	    if(mags[i] > floor){
		p->bin[i].mag *= lininterp(amode, 1.0, 1.0 - mags[i]);
	    } else {
		p->bin[i].mag *= onemamode;
	    }
	}
	}
    } else {
	if(mode == 1.0){
	    for (int i=0; i<numbins; ++i) {
		if(mags[i] > floor){
		    p->bin[i].mag *= mags[i];
		} else {
		    p->bin[i].mag *= onemamode;
		}
	    }
	} else {
	for (int i=0; i<numbins; ++i) {
	    if(mags[i] > floor){
		p->bin[i].mag *= lininterp(amode, 1.0, mags[i]);
	    } else {
		p->bin[i].mag *= onemamode;
	    }
	}
	}
    }
}


void PV_SpectralMap_Ctor(PV_SpectralMap* unit)
{
    SETCALC(PV_SpectralMap_next);
    ZOUT0(0) = ZIN0(0);
    unit->m_mags = 0;
}

void PV_SpectralMap_Dtor(PV_SpectralMap* unit)
{
    RTFree(unit->mWorld, unit->m_mags);
}

/* PV_SpectralEnhance */

void PV_SpectralEnhance_Ctor(PV_SpectralEnhance *unit)
{
    SETCALC(PV_SpectralEnhance_next);
    ZOUT0(0) = ZIN0(0);
    unit->first = true;
}

void PV_SpectralEnhance_next(PV_SpectralEnhance *unit, int inNumSamples)
{
    PV_GET_BUF

    SCPolarBuf *p = ToPolarApx(buf);

    float numPartials = IN0(1);
    float ratio = IN0(2);
    float strength = IN0(3);
    float curMag;
    int startBin = (int)((float)numbins / ratio);
    int i = 1;
    int thisPar;

    for(int j = startBin; j > 0; j--){
	i = 1;
	curMag = p->bin[j].mag; // the mag of this bin - to be added to bins above
	while (((thisPar = (int)roundf((float)(i * j) * ratio)) < numbins) && (i < numPartials)){
	    p->bin[thisPar].mag += (curMag * (strength / (float)i));
	    i++;
	}
	startBin = startBin - 1;
    }

}

void PV_PitchShift_Ctor(PV_PitchShift *unit)
{
    OUT0(0) = IN0(0);
    SETCALC(PV_PitchShift_next);
    unit->first = true;

}

void PV_PitchShift_Dtor(PV_PitchShift *unit)
{
    RTFree(unit->mWorld, unit->m_phases);

}

void PV_PitchShift_next(PV_PitchShift *unit, int inNumSamples)
{
    PV_GET_BUF

    if(unit->first){
	unit->first = false;
	unit->m_phases = (float*)RTAlloc(unit->mWorld, numbins * sizeof(float));
    }

    SCPolarBuf *p = ToPolarApx(buf);
    float ratio = IN0(1);
    float rratio = 1. / ratio;
    if(ratio == 1) return;
    if(ratio > 1){
	//work top down
	int target, last;
	last = 0;
	for(int i = numbins; i > 0; i--)
	{
	    target = (int)roundf((float)i * rratio);
	    if((target > 0) && (last != target))
	    {
		float phasedif, tmp;
		phasedif = unit->m_phases[target] - p->bin[target].phase;
		last = target;
		tmp = p->bin[i].phase;
		p->bin[i].phase = unit->m_phases[i] + (phasedif * ratio);// p->bin[target].phase;
		p->bin[i].mag = p->bin[target].mag;
		unit->m_phases[i] = tmp;

	    } else {
		unit->m_phases[i] = p->bin[i].phase;
		p->bin[i].mag = 0.;
	    }
	}
    } else {
	// work bottom up
	int target;
	for(int i = 0; i > numbins; i++)
	{
	    target = (int)roundf((float)i * rratio);
	    if(target < (numbins - 1))
	       {
		   p->bin[i].phase = p->bin[target].phase;
		   p->bin[i].mag = p->bin[target].mag;
	       }
	}
    }

}

void init_SCComplex(InterfaceTable *inTable);

#define DefinePVUnit(name) \
	(*ft->fDefineUnit)(#name, sizeof(PV_Unit), (UnitCtorFunc)&name##_Ctor, 0, 0);

PluginLoad(JoshPV)
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
	DefinePVUnit(PV_BinFilter);
	DefinePVUnit(PV_Invert);
	DefineDtorUnit(PV_BinDelay);
	DefineDtorUnit(PV_Freeze);
	DefinePVUnit(PV_RecordBuf);
	DefineDtorUnit(PV_PlayBuf);
	DefineDtorUnit(PV_BinPlayBuf);
	DefineDtorUnit(PV_BufRd);
	DefineDtorUnit(PV_BinBufRd);
	DefineDtorUnit(PV_SpectralMap);
	DefinePVUnit(PV_SpectralEnhance);
	DefineSimpleCantAliasUnit(BinData);
	DefineDtorUnit(PV_PitchShift);
	}
