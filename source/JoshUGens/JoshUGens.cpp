/*
 *  JoshUGens.cpp
 *  xSC3plugins
 *
 *  Created by Josh Parmenter on 2/4/05.
 *  Copyright 2005 __MyCompanyName__. All rights reserved.
 *
 */
/*
	SuperCollider real time audio synthesis system
    Copyright (c) 2002 James McCartney. All rights reserved.
	http://www.audiosynth.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/


#include "SC_PlugIn.h"
#include <stdint.h>

#define ATS_BAND_WIDTH { 100.0, 100.0, 100.0, 100.0, 110.0, 120.0, 140.0, 150.0, 160.0, 190.0, 210.0, 240.0, 280.0, 320.0, 380.0, 450.0, 550.0, 700.0, 900.0, 1100.0, 1300.0, 1800.0, 2500.0, 3500.0, 4500.0}
#define ATS_CENTER_FREQ {50.0, 150.0, 250.0, 350.0, 455.0, 570.0, 700.0, 845.0, 1000.0, 1175.0, 1375.0, 1600.0, 1860.0, 2160.0, 2510.0, 2925.0, 3425.0, 4050.0, 4850.0, 5850.0, 7050.0, 8600.0, 10750.0, 13750.0, 17750.0}
#define ATS_OFFSET1 {0, 2, 3, 2, 3}
#define ATS_OFFSET2 {0, 1, 1, 26, 26}
#define P2F 6.283185f
#define PI 3.1415926535898f


// band edges from atsa
//{ 0.0, 100.0, 200.0, 300.0, 400.0, 510.0, 630.0, 770.0, 920.0,  1080.0, 1270.0, 1480.0, 1720.0, 2000.0, 2320.0, 2700.0, 3150.0, 3700.0, 4400.0, 5300.0, 6400.0, 7700.0, 9500.0, 12000.0, 15500.0, 20000.0}

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


const int kMaxGrains = 64;


inline float GRAIN_IN_AT(Unit* unit, int index, int offset)
{
    if (INRATE(index) == calc_FullRate) return IN(index)[offset];
    if (INRATE(index) == calc_DemandRate) return DEMANDINPUT_A(index, offset + 1);
    return IN0(index);
}

inline double sc_gloop(double in, double hi)
{
    // avoid the divide if possible
    if (in >= hi) {
	in -= hi;
	if (in < hi) return in;
    } else if (in < 0.) {
	in += hi;
	if (in >= 0.) return in;
    } else return in;

    return in - hi * floor(in/hi);
}

#define GRAIN_BUF \
    SndBuf *buf; \
    if (bufnum >= world->mNumSndBufs) { \
	int localBufNum = bufnum - world->mNumSndBufs; \
	Graph *parent = unit->mParent; \
	if(localBufNum <= parent->localBufNum) { \
	    buf = parent->mLocalSndBufs + localBufNum; \
	    } else { \
	    bufnum = 0; \
	    buf = world->mSndBufs + bufnum; \
	    } \
	} else { \
	if (bufnum < 0) { bufnum = 0; } \
	    buf = world->mSndBufs + bufnum; \
	    } \
	\
	float *bufData __attribute__((__unused__)) = buf->data; \
	uint32 bufChannels __attribute__((__unused__)) = buf->channels; \
	uint32 bufSamples __attribute__((__unused__)) = buf->samples; \
	uint32 bufFrames = buf->frames; \
	int guardFrame __attribute__((__unused__)) = bufFrames - 2; \

#define CHECK_BUF \
    if (!bufData) { \
    unit->mDone = true; \
    ClearUnitOutputs(unit, inNumSamples); \
    return; \
    }

#define GET_GRAIN_WIN \
    window = unit->mWorld->mSndBufs + (int)winType; \
    windowData = window->data; \
    windowSamples = window->samples; \
    windowFrames = window->frames; \
    windowGuardFrame = windowFrames - 1; \

#define SETUP_OUT \
    uint32 numOutputs = unit->mNumOutputs; \
    if (numOutputs > bufChannels) { \
	unit->mDone = true; \
	ClearUnitOutputs(unit, inNumSamples); \
	return; \
	} \
    float *out[16]; \
    for (uint32 i=0; i<numOutputs; ++i) out[i] = ZOUT(i);

#define CALC_NEXT_GRAIN_AMP \
    if(grain->winType < 0.){ \
	y0 = b1 * y1 - y2; \
	y2 = y1; \
	y1 = y0; \
	amp = y1 * y1; \
	} else { \
	winPos += winInc; \
	int iWinPos = (int)winPos; \
	double winFrac = winPos - (double)iWinPos; \
	float* winTable1 = windowData + iWinPos; \
	float* winTable2 = winTable1 + 1; \
	if (!windowData) break; \
	if (winPos > windowGuardFrame) { \
	    winTable2 -= windowSamples; \
	} \
    amp = lininterp(winFrac, winTable1[0], winTable2[0]); \
    } \

#define GET_GRAIN_AMP_PARAMS \
    if(grain->winType < 0.){ \
	b1 = grain->b1; \
	y1 = grain->y1; \
	y2 = grain->y2; \
	amp = grain->curamp; \
	} else { \
	window = unit->mWorld->mSndBufs + (int)grain->winType; \
	windowData = window->data; \
	windowSamples = window->samples; \
	windowFrames = window->frames; \
	windowGuardFrame = windowFrames - 1; \
	if (!windowData) break; \
	winPos = grain->winPos; \
	winInc = grain->winInc; \
	amp = grain->curamp; \
	} \

#define GET_GRAIN_INIT_AMP \
    if(grain->winType < 0.){ \
	w = pi / counter; \
	b1 = grain->b1 = 2. * cos(w); \
	y1 = sin(w); \
	y2 = 0.; \
	amp = y1 * y1; \
	} else { \
	amp = windowData[0]; \
	winPos = grain->winPos = 0.f; \
	winInc = grain->winInc = (double)windowSamples / counter; \
	} \


#define SAVE_GRAIN_AMP_PARAMS \
    grain->y1 = y1; \
    grain->y2 = y2; \
    grain->winPos = winPos; \
    grain->winInc = winInc; \
    grain->curamp = amp; \
    grain->counter -= nsmps; \


const double sqrt3 = sqrt(3.);
const double sqrt3div6 = sqrt(3.) * 0.1666666667;
const double sqrt3div2 = sqrt(3.) * 0.5;
const double rsqrt6 = 1. / sqrt(6.);
const double sqrt6div3 = sqrt(6.) * 0.3333333333;

static InterfaceTable *ft;


struct LPCVals : public Unit
{
	SndBuf *m_buf;
	float m_fbufnum, m_framestart;
};

struct LPCSynth : public Unit
{
	SndBuf *m_buf;
	float m_fbufnum;
	int m_valindex, m_first;
	float m_framestart;
	float *m_storevals; // this will handle up to 100 poles
	float *m_polevals;
};


struct AtsSynth : public Unit
{
	int32 *m_phase;
	float *m_lastfreq;
	float *m_lastamp;
	int32 m_phaseoffset, m_lomask;
	int m_numPartials, m_partialStart, m_partialSkip, phaseinit, m_totalPartials;
	float m_fbufnum, m_freqMul, m_freqAdd;
	double m_cpstoinc, m_radtoinc;
	int *m_partials;
	SndBuf *m_buf;
};

// I know this looks like I am still being lazy... BUT - these really do share a struct.
struct PVSynth : AtsSynth
{
};

struct PVInfo : public Unit
{
	float m_lastfreq, m_lastamp;
	float m_fbufnum;
	int m_partial, m_phaseinit;
	SndBuf *m_buf;
};

struct AtsNoiSynth : public Unit
{
	int32 *m_phase;
	float *m_lastfreq;
	float *m_lastamp;
	float *m_lastnoise;
	float *m_tempNoise;
	int32  m_lomask;
	float m_rScale, m_freqMul, m_freqAdd;;
	int m_numPartials, m_partialStart, m_partialSkip, m_totalPartials;
	int m_numBands, m_bandStart, m_bandSkip, m_totalBands;
	float m_sinePct, m_noisePct;
	float m_fbufnum;
	double m_cpstoinc, m_radtoinc;
	int32 m_sinphase[25];
	double m_sincpstoinc, m_sinradtoinc;
	float m_atsBandFreq[25];
	int32 m_atsCenterFreq[25];
	float m_level[25];
	float m_slope[25];
	int32 m_counter[25];
	SndBuf *m_buf;
	int *m_partials;
	int *m_bands;
	int phaseinit;
};

struct AtsPartial : public Unit
{
    int32 m_phase;
    float m_lastfreq;
    float m_lastamp;
    int32 m_phaseoffset, m_lomask;
    int m_numPartials, m_partialStart, m_partialSkip, phaseinit, m_totalPartials;
    float m_fbufnum, m_freqMul, m_freqAdd;
    double m_cpstoinc, m_radtoinc;
    int m_partials;
    SndBuf *m_buf;
};

struct AtsBand : public Unit
{
    int32  m_lomask;
    float m_rScale;
    int m_numBands, m_bandStart, m_bandSkip, m_totalBands;
    float m_fbufnum;
    double m_cpstoinc, m_radtoinc;
    int32 m_sinphase;
    double m_sincpstoinc, m_sinradtoinc;
    float m_atsBandFreq;
    int32 m_atsCenterFreq;
    float m_level;
    float m_slope;
    int32 m_counter;
    SndBuf *m_buf;
    float m_lastnoise;
    int m_bands;
    int phaseinit;
};

struct AtsFreq : public Unit
{
	int m_init, m_partial;
	float m_fbufnum, m_lastfreq;
	SndBuf *m_buf;
};

struct AtsAmp : public Unit
{
	int m_init, m_partial;
	float m_fbufnum, m_lastamp;
	SndBuf *m_buf;
};

struct AtsParInfo : public Unit
{
	int m_init, m_partial;
	float m_fbufnum, m_lastfreq, m_lastamp;
	SndBuf *m_buf;
};

struct AtsNoise : public Unit
{
	int m_init, m_band;
	float m_fbufnum, m_lastnoise;
	SndBuf *m_buf;
};

struct SinTone : public Unit
{
	double m_phase;
};


struct Maxamp : public Unit
{
	float m_maxamp, m_numSamps, m_remainingSamps, m_time, m_totalMaxamp, m_totalMaxampTime;
};


struct AudioMSG : public Unit
{
	float m_index;
};

struct Balance : public Unit
{
	float m_scaler;
	float m_hp, m_stor;
	float m_coef1, m_coef2, m_prevq, m_prevr, m_preva;
};

struct MoogVCF : public Unit
{
	float m_fco;
	float m_res;
	float m_xnm1, m_y1nm1, m_y2nm1, m_y3nm1;
	float m_y1n, m_y2n, m_y3n, m_y4n;
	float m_kp, m_pp1d2, m_k;
};

struct PosRatio : public Unit
{

	int maxsamples, posvals, numvals, testposvals, testnumvals;
	float period, lastperiod, ratio, testratio, outval, lastsample;
};

struct DelayUnit : public Unit
{
	float *m_dlybuf;

	float m_dsamp, m_fdelaylen;
	float m_delaytime, m_maxdelaytime;
	long m_iwrphase, m_idelaylen, m_mask;
	long m_numoutput;
};

struct FeedbackDelay : public DelayUnit
{
	float m_feedbk, m_decaytime;
};

struct CombLP : public FeedbackDelay
{	float m_lastsamp, m_prevtrig, m_coef;
	long m_inputsamps;
};

struct WarpWinGrain
{
    double phase, rate;
    double winPos, winInc, b1, y1, y2, curamp; // tells the grain where to look in the winBuf for an amp value
    int counter, bufnum, interp;
    float winType;
};

struct WarpZ : public Unit
{
    float m_fbufnum;
    SndBuf* m_buf;
    int mNumActive[16];
    int mNextGrain[16];
    WarpWinGrain mGrains[16][kMaxGrains];
};

/* New ATS UGens */

// Use these structs adapted from SinOsc and LFNoise1

typedef struct
{
    SndBuf *m_buf;
    float m_fbufnum;
    int32 m_phase;
    float m_phasein;
} SinOscA;

typedef struct
{
    float m_Level, m_Slope;
    int m_Counter;
} LFNoise1A;

typedef struct
{
    SinOscA m_partial;
    float m_lastamp, m_lastfreq;
    int m_partialNum;
} ATSPartial;

typedef struct
{
    SinOscA m_center;
    float m_lastamp;
    LFNoise1A m_band;
    int m_bandNum;
    double phaseinc;
} ATSBand;

struct ATSBase : public Unit
{
    double m_cpstoinc, m_radtoinc;
    int32 m_tableSize;
    int32 m_lomask;
    SndBuf* m_atsFile;
    float m_fbufnum, m_lastpointer;
    /* header data available to all UGens */
    float m_ampMax, m_freqMax, m_sndDur;
    int m_sr, m_frameSize, m_winSize, m_numPartials, m_numFrames, m_atsType;
    int m_offset1, m_offset2, m_increment;
    float m_init;
};

struct ATSPartialSynth : public ATSBase
{
    ATSPartial m_partial;
};

struct ATSSynth : public ATSBase
{
    ATSPartial* m_partials;
};

struct ATSBandSynth : public ATSBase
{
    ATSBand m_band;
};

struct ATSNoiSynth : public ATSBase
{
    ATSPartial* m_partials;
    ATSBand* m_bands;
};

struct ATSFreq : public ATSBase {};
struct ATSAmp : public ATSBase {};
struct ATSNoise : public ATSBase {};
struct ATSNumPartials : public ATSBase {};
struct ATSNumFrames : public ATSBase {};
struct ATSSndDur : public ATSBase {};

static const int DELAY_LINES_COMB = 5;
static const int DELAY_LINES_ALLPASS = 6;
static const int MAX_PRE_DELAY_MS = 1000;

typedef struct {
    float f, k, p, scale, r, x;
    float y1, y2, y3, y4, oldx, oldy1, oldy2, oldy3;
    float fCutoff, fActualCutoff;

    float sampleRateFactor;
} Filter;

typedef struct {
    float gain, minDamp;
    float* buffer;
    int readPtr1, readPtr2, writePtr;
    float z1;
    int bufferLengthDelay;
    float filterStore;
} CombFilter;

typedef struct {
    float delay, gain;
    float* buffer;
    int bufferLength, writePtr, readPtr1, readPtr2;
    float z1;
} AllPassFilter;

typedef struct {
    int counter;
    float actualValue;
    float actualValueFiltered;
    float filterFactor;
    float filterFactorInversePlusOne;
    int randomPeriod;
} NoiseGenerator;


struct TALReverb : public Unit
{
    float lastGain, lastRoomSize, lastPreDelay, lastLowCut, lastDamp, lastHighCut, lastStereoWidth;

    float reflectionGains[DELAY_LINES_COMB + DELAY_LINES_ALLPASS];
    float reflectionDelays[DELAY_LINES_COMB + DELAY_LINES_ALLPASS];

    CombFilter *combFiltersPreDelay;

    CombFilter *combFiltersL[DELAY_LINES_COMB];
    CombFilter *combFiltersR[DELAY_LINES_COMB];
    NoiseGenerator *noiseGeneratorDampL[DELAY_LINES_COMB];
    NoiseGenerator *noiseGeneratorDampR[DELAY_LINES_COMB];
    NoiseGenerator *noiseGeneratorDelayL[DELAY_LINES_COMB];
    NoiseGenerator *noiseGeneratorDelayR[DELAY_LINES_COMB];
    AllPassFilter *allPassFiltersL[DELAY_LINES_ALLPASS];
    AllPassFilter *allPassFiltersR[DELAY_LINES_ALLPASS];

    AllPassFilter *preAllPassFilter;

    Filter lpFilter;
    Filter hpFilter;

    bool first;
};

/* Delay UGens with different interps - from regular source, used for InterpDelays*/

void DelayUnit_AllocDelayLine(DelayUnit *unit)
{
    long delaybufsize = (long)ceil(unit->m_maxdelaytime * SAMPLERATE + 1.f);
    delaybufsize = delaybufsize + BUFLENGTH;
    delaybufsize = NEXTPOWEROFTWO(delaybufsize);  // round up to next power of two
    unit->m_fdelaylen = unit->m_idelaylen = delaybufsize;

    RTFree(unit->mWorld, unit->m_dlybuf);
    unit->m_dlybuf = (float*)RTAlloc(unit->mWorld, delaybufsize * sizeof(float));
    unit->m_mask = delaybufsize - 1;
}

float CalcDelay(DelayUnit *unit, float delaytime);
float CalcDelay(DelayUnit *unit, float delaytime)
{
    float next_dsamp = delaytime * SAMPLERATE;
    return sc_clip(next_dsamp, 1.f, unit->m_fdelaylen);
}

void DelayUnit_Reset(DelayUnit *unit)
{
    unit->m_maxdelaytime = ZIN0(1);
    unit->m_delaytime = ZIN0(2);
    unit->m_dlybuf = 0;

    DelayUnit_AllocDelayLine(unit);

    unit->m_dsamp = CalcDelay(unit, unit->m_delaytime);

    unit->m_numoutput = 0;
    unit->m_iwrphase = 0;
}


void DelayUnit_Dtor(DelayUnit *unit)
{
    RTFree(unit->mWorld, unit->m_dlybuf);
}

struct CubicDelay : public DelayUnit
{
};

struct PMLPF : public Unit
{
    float m_y1, m_y2, m_a0, m_a1, m_a2, m_b1, m_b2;
    float m_fc;
};

struct PMHPF : public Unit
{
    float m_y1, m_y2, m_a0, m_a1, m_a2, m_b1, m_b2;
    float m_fc;
};

struct NFC : public Unit
{
    float m_y1, m_a0, m_a1, m_b1;
    float m_distance;
};

struct Xover2 : public Unit
{
    float m_c0, m_c1, m_z0, m_z1, m_z2, m_fc;
};
/*
struct HermiteDelay : public DelayUnit
{
    float m_tension, m_bias;
};
*/

extern "C"
{
    void load(InterfaceTable *inTable);

    void Maxamp_next(Maxamp *unit, int inNumSamples);
    void Maxamp_Ctor(Maxamp* unit);

    // being a little lazy for the time being... treat PVSynth as
    // a type 1 ATS file ... HAH! Not anymore! Laziness be gone!

    void PVSynth_next(PVSynth *unit, int inNumSamples);
    void PVSynth_Ctor(PVSynth* unit);
    void PVSynth_Dtor(PVSynth* unit);

    void PVInfo_next(PVInfo *unit, int inNumSamples);
    void PVInfo_Ctor(PVInfo* unit);

    void AtsSynth_next(AtsSynth *unit, int inNumSamples);
    void AtsSynth_Ctor(AtsSynth* unit);
    void AtsSynth_Dtor(AtsSynth* unit);

    void AtsNoiSynth_next(AtsNoiSynth *unit, int inNumSamples);
    void AtsNoiSynth_Ctor(AtsNoiSynth* unit);
    void AtsNoiSynth_Dtor(AtsNoiSynth* unit);

    void AtsPartial_next(AtsPartial *unit, int inNumSamples);
    void AtsPartial_Ctor(AtsPartial* unit);

    void AtsBand_next(AtsBand *unit, int inNumSamples);
    void AtsBand_Ctor(AtsBand* unit);

    void AtsFreq_next(AtsFreq *unit, int inNumSamples);
    void AtsFreq_Ctor(AtsFreq* unit);

    void AtsAmp_next(AtsAmp *unit, int inNumSamples);
    void AtsAmp_Ctor(AtsAmp* unit);

    void AtsNoise_next(AtsNoise *unit, int inNumSamples);
    void AtsNoise_Ctor(AtsNoise* unit);

    void AtsParInfo_next(AtsParInfo *unit, int inNumSamples);
    void AtsParInfo_Ctor(AtsParInfo* unit);

    void SinTone_next(SinTone *unit, int inNumSamples);
    void SinTone_Ctor(SinTone* unit);

    void LPCVals_next_k(LPCVals *unit,  int inNumSamples);
    void LPCVals_next_a(LPCVals *unit,  int inNumSamples);
    void LPCVals_Ctor(LPCVals* unit);

    void LPCSynth_next_k(LPCSynth *unit, int inNumSamples);
    void LPCSynth_next_a(LPCSynth *unit, int inNumSamples);
    void LPCSynth_Ctor(LPCSynth* unit);
    void LPCSynth_Dtor(LPCSynth* unit);

    void AudioMSG_Ctor(AudioMSG* unit);
    void AudioMSG_next_a(AudioMSG *unit, int inNumSamples);
    void AudioMSG_next_k(AudioMSG *unit, int inNumSamples);

    void Balance_Ctor(Balance* unit);
    void Balance_next_a(Balance* unit, int inNumSamples);
    void Balance_next_k(Balance* unit, int inNumSamples);

    void MoogVCF_Ctor(MoogVCF* unit);
    void MoogVCF_next_ii(MoogVCF *unit, int inNumSamples);
    void MoogVCF_next_ki(MoogVCF *unit, int inNumSamples);
    void MoogVCF_next_kk(MoogVCF *unit, int inNumSamples);
    void MoogVCF_next_ka(MoogVCF *unit, int inNumSamples);
    void MoogVCF_next_ak(MoogVCF *unit, int inNumSamples);
    void MoogVCF_next_aa(MoogVCF *unit, int inNumSamples);

    void PosRatio_Ctor(PosRatio* unit);
    void PosRatio_next(PosRatio *unit, int inNumSamples);

    void CombLP_Ctor(CombLP* unit);
    void CombLP_next_aa(CombLP *unit, int inNumSamples);
    void CombLP_next_aa_z(CombLP *unit, int inNumSamples);
    void CombLP_next_kk(CombLP *unit, int inNumSamples);
    void CombLP_next_kk_z(CombLP *unit, int inNumSamples);
    void CombLP_next_ka(CombLP *unit, int inNumSamples);
    void CombLP_next_ka_z(CombLP *unit, int inNumSamples);
    void CombLP_next_ak(CombLP *unit, int inNumSamples);
    void CombLP_next_ak_z(CombLP *unit, int inNumSamples);

    void WarpZ_next(WarpZ *unit, int inNumSamples);
    void WarpZ_Ctor(WarpZ* unit);

    /* New ATS */
    void ATSSynth_next(ATSSynth *unit, int inNumSamples);
    void ATSSynth_Ctor(ATSSynth* unit);
    void ATSSynth_Dtor(ATSSynth* unit);

    void TALReverb_next(TALReverb *unit, int inNumSamples);
    void TALReverb_Ctor(TALReverb* unit);
    void TALReverb_Dtor(TALReverb* unit);

    void CubicDelay_Ctor(CubicDelay *unit);
    void CubicDelay_next_a(CubicDelay *unit, int inNumSamples);
    void CubicDelay_next_k(CubicDelay *unit, int inNumSamples);

    void Xover2_Ctor(Xover2 *unit);
    void Xover2_next(Xover2 *unit, int inNumSamples);

    void PMLPF_Ctor(PMLPF *unit);
    void PMLPF_next(PMLPF *unit, int inNumSamples);

    void PMHPF_Ctor(PMHPF *unit);
    void PMHPF_next(PMHPF *unit, int inNumSamples);

    void NFC_Ctor(NFC *unit);
    void NFC_next(NFC *unit, int inNumSamples);

    void Xover2_Ctor(Xover2 *unit);
    void Xover2_next(Xover2 *unit, int inNumSamples);
/*
    void HermiteDelay_Ctor(HermiteDelay *unit);
    void HermiteDelay_next(HermiteDelay *unit, int inNumSamples);
    void HermiteDelay_next_z(HermiteDelay *unit, int inNumSamples);
  */

}


/////////////////  AudioMSG ////////////////////////////////////////////////////////

void AudioMSG_Ctor(AudioMSG* unit)
{
    if (INRATE(1) == calc_FullRate) {
	SETCALC(AudioMSG_next_a);
	AudioMSG_next_a(unit, 1);
	   } else {
	SETCALC(AudioMSG_next_k);
	unit->m_index = IN0(1);
	AudioMSG_next_k(unit, 1);
	 };
}

void AudioMSG_next_a(AudioMSG *unit, int inNumSamples)
{
    float* in = IN(0);
    float* index = IN(1);
    float* out = OUT(0);

    for (int i = 0; i < inNumSamples; ++i)
	{
	    out[i] = sin((in[i] * pi2) + (index[i] * sin(in[i] * twopi)));
	}
}

void AudioMSG_next_k(AudioMSG *unit, int inNumSamples)
{
    float* in = IN(0);
    float endindex = IN0(1);
    float* out = OUT(0);

    float index = unit->m_index;

    float indexinc = CALCSLOPE(endindex, index);

    for (int i = 0; i < inNumSamples; ++i) {
	    out[i] = sin((in[i] * pi2) + (index * sin(in[i] * twopi)));
	    index += indexinc;
	}

    unit->m_index = endindex;

}


////////////////////////////////////////////////////////////////////////////////////////////////////////
// LPC Ugens built by Richard Karpen, Don Craig and Josh Parmenter
////////////////////////////////////////////////////////////////////////////////////////////////////////

void LPCVals_Ctor(LPCVals *unit)
{
	if (INRATE(1) == calc_FullRate) {
	    SETCALC(LPCVals_next_a);
	    } else {
	    SETCALC(LPCVals_next_k);
	    }
	ClearUnitOutputs(unit, 1);
	unit->m_fbufnum = -1e9f;
}

void LPCVals_next_k(LPCVals *unit, int inNumSamples)
{
	float fbufnum  = ZIN0(0);

	if (fbufnum != unit->m_fbufnum) {
		uint32 bufnum = (int)fbufnum;
		World *world = unit->mWorld;
		if (bufnum >= world->mNumSndBufs) bufnum = 0;
		unit->m_fbufnum = fbufnum;
		unit->m_buf = world->mSndBufs + bufnum;
		}

	SndBuf *buf = unit->m_buf;
	float *bufData __attribute__((__unused__)) = buf->data;
    if(!bufData) {
	    Print("WTF?\n");
	return;
    }
	float* fileFrames = bufData + 1;
	int numframes = (int)fileFrames[0];
	int rmsoStart = 3 + numframes;
	int nrmerrStart = 3 + (numframes * 2);
	int pchcpsStart = 3 + (numframes * 3);

	float *cpsout = OUT(0);
	float *rmsoout = OUT(1);
	float *errout = OUT(2);

	int intframes = ((int)numframes - 1);
	int frameround1, frameround2;
	float frame = unit->m_framestart * intframes;
	float frameend = IN0(1);

	float cps1, cps2, rmso1, rmso2, err1, err2, framepct;

	float frameinc = CALCSLOPE(frameend * intframes, frame);

	if (!bufData) {
                unit->mDone = true;
		ClearUnitOutputs(unit, inNumSamples);
		return;
		};

	for (int i = 0; i < inNumSamples; ++i){

	    frameround1 = (int)frame;
	    frameround2 = frameround1 + 1;
	    framepct = frame-(float)frameround1;
	    if (frameround2 > numframes) frameround2 = frameround1;

	    cps1 = *(bufData + (pchcpsStart + frameround1));
	    cps2 = *(bufData + (pchcpsStart + frameround2));
	    cpsout[i] = cps1 + ((cps2 - cps1) * framepct);

	    rmso1 = *(bufData + (rmsoStart + frameround1));
	    rmso2 = *(bufData + (rmsoStart + frameround2));
	    rmsoout[i] = rmso1 + ((rmso2 - rmso1) * framepct);

	    err1 = *(bufData + (nrmerrStart + frameround1));
	    err2 = *(bufData + (nrmerrStart + frameround2));
	    errout[i] = err1 + ((err2 - err1) * framepct);

	    frame += frameinc;
	    }

	    unit->m_framestart = frameend;
}


void LPCVals_next_a(LPCVals *unit, int inNumSamples)
{
	float fbufnum  = ZIN0(0);

	if (fbufnum != unit->m_fbufnum) {
		uint32 bufnum = (int)fbufnum;
		World *world = unit->mWorld;
		if (bufnum >= world->mNumSndBufs) bufnum = 0;
		unit->m_fbufnum = fbufnum;
		unit->m_buf = world->mSndBufs + bufnum;
		}

	SndBuf *buf = unit->m_buf;
	float *bufData __attribute__((__unused__)) = buf->data;
	float* fileFrames = bufData + 1;
	int numframes = (int)fileFrames[0];
	int rmsoStart = 3 + numframes;
	int nrmerrStart = 3 + (numframes * 2);
	int pchcpsStart = 3 + (numframes * 3);

	float *cpsout = OUT(0);
	float *rmsoout = OUT(1);
	float *errout = OUT(2);

	int intframes = ((int)numframes - 1);
	int frameround1, frameround2;
	float frame; //= unit->m_framestart * intframes;
	float *framein = IN(1);

	float cps1, cps2, rmso1, rmso2, err1, err2, framepct;


	if (!bufData) {
                unit->mDone = true;
		ClearUnitOutputs(unit, inNumSamples);
		return;
		};

	for (int i = 0; i < inNumSamples; ++i){
	    frame = framein[i] * (float)intframes;
	    frameround1 = (int)frame;
	    frameround2 = frameround1 + 1;
	    framepct = frame-(float)frameround1;
	    if (frameround2 > numframes) frameround2 = frameround1;

	    cps1 = *(bufData + (pchcpsStart + frameround1));
	    cps2 = *(bufData + (pchcpsStart + frameround2));
	    cpsout[i] = cps1 + ((cps2 - cps1) * framepct);

	    rmso1 = *(bufData + (rmsoStart + frameround1));
	    rmso2 = *(bufData + (rmsoStart + frameround2));
	    rmsoout[i] = rmso1 + ((rmso2 - rmso1) * framepct);

	    err1 = *(bufData + (nrmerrStart + frameround1));
	    err2 = *(bufData + (nrmerrStart + frameround2));
	    errout[i] = err1 + ((err2 - err1) * framepct);

	    }

}

void LPCSynth_Ctor(LPCSynth *unit)
{
	if (INRATE(2) == calc_FullRate) {
	    SETCALC(LPCSynth_next_a);
	    } else {
	    SETCALC(LPCSynth_next_k);
	    }

	ClearUnitOutputs(unit, 1);
	unit->m_fbufnum = -1e9f;
	unit->m_valindex = 0;
	unit->m_framestart = IN0(2);
	unit->m_first = 1;

}

void LPCSynth_next_k(LPCSynth *unit, int inNumSamples)
{
    float fbufnum  = ZIN0(0);

	if (fbufnum != unit->m_fbufnum) {
		uint32 bufnum = (int)fbufnum;
		World *world = unit->mWorld;
		if (bufnum >= world->mNumSndBufs) bufnum = 0;
		unit->m_fbufnum = fbufnum;
		unit->m_buf = world->mSndBufs + bufnum;
	}

	SndBuf *buf = unit->m_buf;
	float *bufData __attribute__((__unused__)) = buf->data;
	float start;
	float *out = OUT(0);
	float *signal = IN(1);
	float* numPoles = bufData;
	float* fileFrames = bufData + 1;
	int numframes = (int)fileFrames[0];
	int coefsStart = 3 + (numframes * 4);
	int npoles = (int)numPoles[0];
    if(unit->m_first > 0){
	// init the past sample's values to 0.0
	unit->m_storevals = (float*)RTAlloc(unit->mWorld, npoles * 2 * sizeof(float));
	unit->m_polevals = (float*)RTAlloc(unit->mWorld, npoles * sizeof(float));
	for (int i = 0; i < npoles * 2; ++i){
	    unit->m_storevals[i] = 0.0f;
	}
	for(int i = 0; i < npoles;++i){
	    unit->m_polevals[i] = 0.0f;
	}
	unit->m_first = 0;
    }


	float *polevals = unit->m_polevals;
	int valindex = unit->m_valindex;
	int intframes = ((int)numframes - 1);
	float frame = unit->m_framestart * intframes;
	float frameend = IN0(2);
	float frameendscale = frameend * intframes;
	float coef1, coef2;
	float framepct, frameinc;
	int frameround1, frameround2;

	frameinc = CALCSLOPE(frameendscale, frame);

	if (!bufData) {
                unit->mDone = true;
		ClearUnitOutputs(unit, inNumSamples);
		return;
		};

	//the all-pole filter
	for (int i = 0; i < inNumSamples; ++i){
		// get values for interpolating the current frames coefs
		frameround1 = (int)frame;
		frameround2 = frameround1 + 1;
		framepct = frame-(float)frameround1;
		if (frameround2 > numframes) frameround2 = frameround1;

	    //fill polevals with interpolated coefs
	    for (int j = 0; j < (int)npoles; j++){
		coef1 = *(bufData + (coefsStart + ((int)numframes * j) + frameround1));
		coef2 = *(bufData + (coefsStart + ((int)numframes * j) + frameround2));
		polevals[j] = (coef1 + ((coef2 - coef1) * framepct)); // lin interp for a pole val between frames
	    }
	    start = signal[i];
	    frame += frameinc;
	    for (int j = valindex, n=0; n < npoles; n++, j++)
		start +=  polevals[n] * unit->m_storevals[j];
		out[i] = unit->m_storevals[valindex] = unit->m_storevals[valindex + npoles] = zapgremlins(start);
		valindex = (valindex + 1) % npoles;
	    }

	unit->m_framestart = frameend;
	unit->m_valindex = valindex;
}

void LPCSynth_next_a(LPCSynth *unit, int inNumSamples)
{
	float fbufnum  = ZIN0(0);

	if (fbufnum != unit->m_fbufnum) {
		uint32 bufnum = (int)fbufnum;
		World *world = unit->mWorld;
		if (bufnum >= world->mNumSndBufs) bufnum = 0;
		unit->m_fbufnum = fbufnum;
		unit->m_buf = world->mSndBufs + bufnum;
	}

	SndBuf *buf = unit->m_buf;
	float *bufData __attribute__((__unused__)) = buf->data;
	float start;
	float *out = OUT(0);
	float *signal = IN(1);
	float* numPoles = bufData;
	float* fileFrames = bufData + 1;
	int numframes = (int)fileFrames[0];
	int coefsStart = 3 + (numframes * 4);
	int npoles = (int)numPoles[0];

    if(unit->m_first > 0){
	// init the past sample's values to 0.0
	unit->m_storevals = (float*)RTAlloc(unit->mWorld, npoles * 2 * sizeof(float));
	unit->m_polevals = (float*)RTAlloc(unit->mWorld, npoles * sizeof(float));
	for (int i = 0; i < npoles * 2; ++i){
	    unit->m_storevals[i] = 0.0f;
	}
	for(int i = 0; i < npoles;++i){
	    unit->m_polevals[i] = 0.0f;
	}

	unit->m_first = 0;
    }

	float *polevals = unit->m_polevals;
	int valindex = unit->m_valindex;
	int intframes = ((int)numframes - 1);
	float *framein = IN(2);
	float coef1, coef2;
	float framepct, frame;
	int frameround1, frameround2;

	if (!bufData) {
                unit->mDone = true;
		ClearUnitOutputs(unit, inNumSamples);
		delete [] polevals;
		return;
		};

//the all-pole filter
	for (int i = 0; i < inNumSamples; ++i){
		// get values for interpolating the current frames coefs
		frame = framein[i] * intframes;
		frameround1 = (int)frame;
		frameround2 = frameround1 + 1;
		framepct = frame-(float)frameround1;
		if (frameround2 > numframes) frameround2 = frameround1;

	    //fill polevals with interpolated coefs
	    for (int j = 0; j < (int)npoles; j++){
		coef1 = *(bufData + (coefsStart + ((int)numframes * j) + frameround1));
		coef2 = *(bufData + (coefsStart + ((int)numframes * j) + frameround2));
		polevals[j] = (coef1 + ((coef2 - coef1) * framepct)); // lin interp for a pole val between frames
	    }
	    start = signal[i];

	    for (int j = valindex, n=0; n < npoles; n++, j++)
		start +=  polevals[n] * unit->m_storevals[j];
		out[i] = unit->m_storevals[valindex] = unit->m_storevals[valindex + npoles] = zapgremlins(start);
		valindex = (valindex + 1) % npoles;
	    }

	unit->m_valindex = valindex;
}

void LPCSynth_Dtor(LPCSynth* unit)
{
	RTFree(unit->mWorld, unit->m_storevals);
	RTFree(unit->mWorld, unit->m_polevals);
}
///////////////////////////// Maxamp //////////////////////////////////////

void Maxamp_Ctor(Maxamp* unit)
{
	SETCALC(Maxamp_next);
	unit->m_numSamps = unit->m_remainingSamps = ZIN0(1);
	unit->m_maxamp = 0.0;
	unit->m_time = 0.0;
	unit->m_totalMaxampTime = 0.0;
	unit->m_totalMaxamp = 0.0;
	Maxamp_next(unit, 1);
}

void Maxamp_next(Maxamp *unit, int inNumSamples)
{
	float* in = IN(0);
	float* out = OUT(0);
	int numSamps = (int)unit->m_numSamps;
	int remainingSamps = (int)unit->m_remainingSamps;
	float currenttime = unit->m_time;
	float totalMaxamp = unit->m_totalMaxamp;
	float maxamp = unit->m_maxamp;
	float totalMaxampTime = unit->m_totalMaxampTime;
	float curamp, lasttime;

	for (int j = 0; j < inNumSamples; ++j){
	    curamp = fabsf(in[j]);
	    if (curamp > maxamp) maxamp = unit->m_maxamp = curamp;
	    if (curamp > totalMaxamp){
		totalMaxamp = unit->m_totalMaxamp = curamp;
		totalMaxampTime = unit->m_totalMaxampTime = currenttime + ((numSamps - remainingSamps) * SAMPLEDUR);
		}
	    if (remainingSamps == 0){
		lasttime = currenttime;
		currenttime = unit->m_time = currenttime + (SAMPLEDUR * numSamps);
		Print("Maxamp from %g to %g: %g, Overall Maxamp at %g: %g\n", lasttime, currenttime, maxamp, totalMaxampTime, totalMaxamp);
		unit->m_remainingSamps = remainingSamps = numSamps;
		unit->m_maxamp = maxamp = 0.0;
		}
		else{
		    unit->m_remainingSamps = --remainingSamps;
		    }
	    out[j] = in[j];
	}
}

//////////////////////////////// Ats and PV UGens ///////////////////////////////


#define ADD_SIN_PARTIALS \
	newamp = lininterp(framePct, amp1, amp2); \
	newfreq = ((lininterp(framePct, freq1, freq2) * freqMul) + freqAdd); \
	amp = unit->m_lastamp[i]; \
	freq = unit->m_lastfreq[i]; \
	ampslope = CALCSLOPE(newamp, amp); \
	freqslope = CALCSLOPE(newfreq, freq); \
	phase = unit->m_phase[i]; \
	for (int j = 0; j < inNumSamples; ++j){ \
	    ifreq = (int32)(unit->m_cpstoinc * freq); \
	    out[j] += (lookupi1(table0, table1, phase, lomask) * amp); \
	    phase += ifreq; \
	    freq += freqslope; \
	    amp += ampslope; \
	    freqMul += freqMulSlope; \
	    freqAdd += freqAddSlope; \
	}; \
	unit->m_lastfreq[i] = freq; \
	unit->m_lastamp[i] = amp; \
	unit->m_phase[i] = phase; \

#define GET_ATS_BUF \
	if (fbufnum != unit->m_fbufnum) { \
		uint32 bufnum = (int)fbufnum; \
		World *world = unit->mWorld; \
		if (bufnum >= world->mNumSndBufs) bufnum = 0; \
		unit->m_fbufnum = fbufnum; \
		unit->m_buf = world->mSndBufs + bufnum; \
	    } \
	SndBuf *buf = unit->m_buf; \
	float *bufData __attribute__((__unused__)) = buf->data; \
	if (!bufData) { \
                unit->mDone = true; \
		return; \
		}; \
	int fileType = (int)bufData[9]; \
	int fileNumPartials = (int)bufData[4]; \
	int fileNumFrames = (int)bufData[5]; \
	float* atsData = bufData + 11; \

#define CALC_FRAME_OFFSETS \
	int iFrame1 = (int)frame; \
	int iFrame2 = iFrame1 + 1; \
	float framePct = frame - (float)iFrame1; \
	if(iFrame2 >= fileNumFrames){ \
	    iFrame2 = iFrame1; \
	    framePct = 0.; \
	    } \


void AtsSynth_Ctor(AtsSynth* unit)
{
	SETCALC(AtsSynth_next);
	int tableSizeSin = ft->mSineSize;
	unit->m_numPartials = (int)IN0(1);
	unit->m_partialStart = (int)IN0(2);
	unit->m_partialSkip = (int)IN0(3);
	unit->m_lomask = (tableSizeSin - 1) << 3;
	unit->m_radtoinc = tableSizeSin * (rtwopi * 65536.);
	unit->m_cpstoinc = tableSizeSin * SAMPLEDUR * 65536.;
	unit->m_freqMul = IN0(5);
	unit->m_freqAdd = IN0(6);
	unit->phaseinit = 1;
	unit->m_fbufnum = -1e9f;
	ClearUnitOutputs(unit, 1);
}

void AtsSynth_Dtor(AtsSynth* unit)
{
	RTFree(unit->mWorld, unit->m_phase);
	RTFree(unit->mWorld, unit->m_lastamp);
	RTFree(unit->mWorld, unit->m_lastfreq);
	RTFree(unit->mWorld, unit->m_partials);
}


void AtsSynth_next(AtsSynth *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);
	// get the buffer that stores the ATS data
	float fbufnum = IN0(0);

	GET_ATS_BUF

	// calculate offsets based on file type
	int offset1 = 2;
	int offset2 = 1;
	if((fileType == 2) || (fileType == 4)) offset1 = 3;
	if(fileType >2) offset2 = 26;

	// set-up the output pointer
	float *out = OUT(0);

	// get the requested frames and other inputs
	float frame = sc_mod(IN0(4), 1.0f) * (float)fileNumFrames; // frame input should be between 0 and 1
	float nextFreqMul = IN0(5);
	float nextFreqAdd = IN0(6);

	float freqMul = unit->m_freqMul;
	float freqAdd = unit->m_freqAdd;
	float freqMulSlope = CALCSLOPE(nextFreqMul, freqMul);
	float freqAddSlope = CALCSLOPE(nextFreqAdd, freqAdd);

	// to get current data, we will need to interpolate values between frames - figure out the bounding frames
	CALC_FRAME_OFFSETS

	int block = ((fileNumPartials * offset1) + offset2);
	int block1 = iFrame1 * block;
	int block2 = iFrame2 * block;

	int thisPartial, dataPos1, dataPos2, partialOffset;
	float amp1, amp2, freq1, freq2, amp, freq, newamp, newfreq, ampslope, freqslope;
	int32 ifreq, phase;

	// if this is the first time through the next function, allocate the phase pointer, and zero it out, fill the initial amp and freq data
	if(unit->phaseinit > 0){

	    unit->m_totalPartials = unit->m_numPartials;
	    for (int j = 0; j < unit->m_numPartials; ++j){
		int aPartial = unit->m_partialStart + (unit->m_partialSkip * j);
		if (aPartial > fileNumPartials)
		    --unit->m_totalPartials;
	    }

	    unit->m_phase = (int32*)RTAlloc(unit->mWorld, unit->m_totalPartials * sizeof(int32));
	    unit->m_lastamp = (float*)RTAlloc(unit->mWorld, unit->m_totalPartials * sizeof(float));
	    unit->m_lastfreq = (float*)RTAlloc(unit->mWorld, unit->m_totalPartials * sizeof(float));
	    unit->m_partials = (int*)RTAlloc(unit->mWorld, unit->m_totalPartials * sizeof(int));

	    for (int j = 0; j < unit->m_totalPartials; ++j){
		int aPartial = unit->m_partialStart + (unit->m_partialSkip * j);
		unit->m_partials[j] = aPartial;
	    }

	    for (int i = 0; i < unit->m_totalPartials; i++){
		thisPartial = unit->m_partials[i];
		unit->m_phase[i] = 0.;
		partialOffset = (thisPartial * offset1);
		dataPos1 = block1 + partialOffset;
		dataPos2 = block2 + partialOffset;
		freq1 = atsData[dataPos1 + 1];
		freq2 = atsData[dataPos2 + 1];
		unit->m_lastfreq[i] = (lininterp(framePct, freq1, freq2) * freqMul) + freqAdd;
		amp1 = atsData[dataPos1];
		amp2 = atsData[dataPos2];
		unit->m_lastamp[i] = lininterp(framePct, amp1, amp2);
		}
	    unit->phaseinit = -1;
	}

	int *partials = unit->m_partials;

	// the sine table
	float *table0 = ft->mSineWavetable;
	float *table1 = table0 + 1;
	int32 lomask = unit->m_lomask;

	// this loop calulates the partial sample values for the each of the sins, stores them in an array
	for (int i = 0; i< unit->m_totalPartials; i++){
	    thisPartial = partials[i];
	    partialOffset = (thisPartial * offset1);
	    dataPos1 = block1 + partialOffset;
	    dataPos2 = block2 + partialOffset;
	    amp1 = atsData[dataPos1];
	    amp2 = atsData[dataPos2];
	    freq1 = atsData[dataPos1 + 1];
	    freq2 = atsData[dataPos2 + 1];
	    freqMul = unit->m_freqMul; //grab the original value from the struct
	    freqAdd = unit->m_freqAdd;

	    ADD_SIN_PARTIALS

	    }
	unit->m_freqMul = freqMul;
	unit->m_freqAdd = freqAdd;
}

///////////// New AtsNoiSynth

void AtsNoiSynth_Ctor(AtsNoiSynth* unit)
{
	SETCALC(AtsNoiSynth_next);
	int tableSizeSin = ft->mSineSize;
	unit->m_numPartials = (int)IN0(1);
	unit->m_partialStart = (int)IN0(2);
	unit->m_partialSkip = (int)IN0(3);
	unit->m_sinePct = IN0(5);
	unit->m_noisePct = IN0(6);
	unit->m_freqMul = IN0(7);
	unit->m_freqAdd = IN0(8);
	unit->m_numBands = (int)IN0(9);
	unit->m_bandStart = (int)IN0(10);
	unit->m_bandSkip = (int)IN0(11);
	unit->m_lomask = (tableSizeSin - 1) << 3;
	unit->m_radtoinc = tableSizeSin * (rtwopi * 65536.);
	unit->m_cpstoinc = tableSizeSin * SAMPLEDUR * 65536.;

	float atsBandFreq[25] = ATS_BAND_WIDTH;
	float atsCenterFreq[25] = ATS_CENTER_FREQ;
	RGET
	for (int i = 0; i < 25; i++){
	    unit->m_atsBandFreq[i] = atsBandFreq[i];
	    unit->m_atsCenterFreq[i] = (int32)(unit->m_cpstoinc * atsCenterFreq[i]);
	    unit->m_counter[i] = 0;
	    unit->m_level[i] = unit->mParent->mRGen->frand2();
	    unit->m_slope[i] = 0.f;
	    }
	RPUT
	unit->phaseinit = 1;
	unit->m_fbufnum = -1e9f;
	ClearUnitOutputs(unit, 1);
}

void AtsNoiSynth_Dtor(AtsNoiSynth* unit)
{
	RTFree(unit->mWorld, unit->m_phase);
	RTFree(unit->mWorld, unit->m_lastamp);
	RTFree(unit->mWorld, unit->m_lastfreq);
	RTFree(unit->mWorld, unit->m_lastnoise);
	RTFree(unit->mWorld, unit->m_partials);
	RTFree(unit->mWorld, unit->m_bands);
	RTFree(unit->mWorld, unit->m_tempNoise);
}


void AtsNoiSynth_next(AtsNoiSynth *unit, int inNumSamples)
{
	int thisPartial, dataPos1, dataPos2, partialOffset;
	float rScale, amp1, amp2, freq1, freq2, noise1, noise2, amp, freq, noise, newamp, newfreq, ampslope, freqslope, noiseslope;
	int32 ifreq, phase;

	ClearUnitOutputs(unit, inNumSamples);
	// get the buffer that stores the ATS data
	float fbufnum = IN0(0);

	GET_ATS_BUF

	// calculate offsets based on file type
	int offset1 = 2;
	int offset2 = 1;
	if((fileType == 2) || (fileType == 4)) offset1 = 3;
	if(fileType >2) offset2 = 26;

	// set-up the output pointer
	float *out = OUT(0);

	// get the requested frames and other inputs
	float frame = sc_mod(IN0(4), 1.0f) * (float)fileNumFrames; // frame should be between 0 and 1

	// to get current data, we will need to interpolate values between frames - figure out the bounding frames
	CALC_FRAME_OFFSETS

	int block = ((fileNumPartials * offset1) + offset2);
	int block1 = iFrame1 * block;
	int block2 = iFrame2 * block;

	float nextSinePct = IN0(5);
	float nextNoisePct = IN0(6);

	float sinePct = unit->m_sinePct;
	float noisePct = unit->m_noisePct;
	float sinePctSlope = CALCSLOPE(nextSinePct, sinePct);
	float noisePctSlope = CALCSLOPE(nextNoisePct, noisePct);

	float nextFreqMul = IN0(7);
	float nextFreqAdd = IN0(8);

	float freqMul = unit->m_freqMul;
	float freqAdd = unit->m_freqAdd;
	float freqMulSlope = CALCSLOPE(nextFreqMul, freqMul);
	float freqAddSlope = CALCSLOPE(nextFreqAdd, freqAdd);

	// if this is the first time through the 'next' function, allocate the pointers, and zero out if needed, fill the initial data
	if(unit->phaseinit > 0){
	    unit->m_totalPartials = unit->m_numPartials;
	    for (int j = 0; j < unit->m_numPartials; ++j){
		int aPartial = unit->m_partialStart + (unit->m_partialSkip * j);
		if (aPartial > fileNumPartials)
		    --unit->m_totalPartials;
	    }

	    unit->m_totalBands = unit->m_numBands;
	    for (int i = 0; i < unit->m_numBands; ++i){
		int aBand = unit->m_bandStart + (unit->m_bandSkip * i);
		if (aBand > 25)
		    --unit->m_totalBands;
	    }

	    unit->m_phase = (int32*)RTAlloc(unit->mWorld, unit->m_totalPartials * sizeof(int32));
	    unit->m_lastamp = (float*)RTAlloc(unit->mWorld, unit->m_totalPartials * sizeof(float));
	    unit->m_lastfreq = (float*)RTAlloc(unit->mWorld, unit->m_totalPartials * sizeof(float));
	    unit->m_lastnoise = (float*)RTAlloc(unit->mWorld, unit->m_totalBands * sizeof(float));
	    unit->m_partials = (int*)RTAlloc(unit->mWorld, unit->m_totalPartials * sizeof(int));
	    unit->m_bands = (int*)RTAlloc(unit->mWorld, unit->m_totalBands * sizeof(int));
	    unit->m_tempNoise = (float*)RTAlloc(unit->mWorld, inNumSamples * sizeof(float));

	    for (int j = 0; j < unit->m_totalPartials; ++j){
		int aPartial = unit->m_partialStart + (unit->m_partialSkip * j);
		unit->m_partials[j] = aPartial;
	    }

	    for (int i = 0; i < unit->m_totalBands; ++i){
		int aBand = unit->m_bandStart + (unit->m_bandSkip * i);
		unit->m_bands[i] = aBand;
	    }

	    unit->m_rScale = rScale = 1 / (bufData[3] * 0.33166610955984);
	    for (int i = 0; i < unit->m_totalPartials; i++){
		thisPartial = unit->m_partials[i];
		unit->m_phase[i] = 0.;
		partialOffset = (thisPartial * offset1);
		dataPos1 = block1 + partialOffset;
		dataPos2 = block2 + partialOffset;
		freq1 = atsData[dataPos1 + 1];
		freq2 = atsData[dataPos2 + 1];
		unit->m_lastfreq[i] = (lininterp(framePct, freq1, freq2) * freqMul) + freqAdd;
		amp1 = atsData[dataPos1];
		amp2 = atsData[dataPos2];
		unit->m_lastamp[i] = lininterp(framePct, amp1, amp2);
		}
	    for (int i = 0; i < unit->m_totalBands; i++) {
		int bandnum = unit->m_bands[i];
		int dataOffset = (fileNumPartials * offset1) + bandnum;
		noise1 = atsData[block1 + dataOffset]; // 1 + 10 slots of header
		noise2 = atsData[block2 + dataOffset];
		unit->m_lastnoise[i] = sqrt(lininterp(framePct, noise1, noise2) * rScale);
		}
	    unit->phaseinit = -1;
	}

	int *partials = unit->m_partials;
	int totalPartials = unit->m_totalPartials;
	int *bands = unit->m_bands;
	int totalBands = unit->m_totalBands;

	// the sine table
	float *table0 = ft->mSineWavetable;
	float *table1 = table0 + 1;
	int32 lomask = unit->m_lomask;

	// this loop calulates the partial sample values for the each of the sins, stores them in an array
	for (int i = 0; i< totalPartials; ++i){
	    thisPartial = partials[i];
	    partialOffset = (thisPartial * offset1);
	    dataPos1 = block1 + partialOffset;
	    dataPos2 = block2 + partialOffset;
	    amp1 = atsData[dataPos1];
	    amp2 = atsData[dataPos2];
	    freq1 = atsData[dataPos1 + 1];
	    freq2 = atsData[dataPos2 + 1];
	    freqMul = unit->m_freqMul; //grab the original value from the struct
	    freqAdd = unit->m_freqAdd;

	    ADD_SIN_PARTIALS

	    }
	unit->m_freqMul = freqMul;
	unit->m_freqAdd = freqAdd;

	rScale = unit->m_rScale;

	RGET

	float *tempNoise = unit->m_tempNoise;
	//calculate the noise
	if(fileType > 2) {
	    for (int i = 0; i < inNumSamples; i++) tempNoise[i] = 0.f;
	    for (int i = 0; i < totalBands; i++){
		int bandnum = bands[i];
		float noiseStart = unit->m_lastnoise[i];
		int dataOffset = (fileNumPartials * offset1) + bandnum;
		noise1 = atsData[block1 + dataOffset];
		noise2 = atsData[block2 + dataOffset];
		unit->m_lastnoise[i] = noise = sqrt(lininterp(framePct, noise1, noise2) * rScale);
		noiseslope = CALCSLOPE(noise, noiseStart);

		int32 sinphase = unit->m_sinphase[i];

		float noisefreq = unit->m_atsBandFreq[bandnum];
		int32 freq = unit->m_atsCenterFreq[bandnum];

		float level = unit->m_level[i];
		float slope = unit->m_slope[i];
		int32 counter = unit->m_counter[i];
		float nextlevel = 0.;
		for (int j = 0; j < inNumSamples; j++){
		    if (counter<=0) {
			counter = (int32)(unit->mRate->mSampleRate / sc_max(noisefreq, .001f));
			counter = sc_max(1, counter);
			nextlevel = frand2(s1, s2, s3);
			slope = (nextlevel - level) / counter;
		    } else --counter;
		    tempNoise[j] += lookupi1(table0, table1, sinphase, lomask) * level * noiseStart;
		    noiseStart += noiseslope;
		    level += slope;
		    sinphase += freq;
		    }
		unit->m_level[i] = level;
		unit->m_slope[i] = slope;
		unit->m_counter[i] = counter;
		unit->m_sinphase[i] = sinphase;
		}
	    } else {
	    Print("This ATS file doesn't appear to have noise data. Use AtsSynth\n");
	    }

	RPUT

	// NOW - scale by the sinPct and noisePct (saves TONS of multiplies and adds this way)

	for (int i = 0; i < inNumSamples; i++){
	    out[i] = (out[i] * sinePct) + (tempNoise[i] * noisePct);
	    sinePct += sinePctSlope;
	    noisePct += noisePctSlope;
	    }
	unit->m_sinePct = sinePct;
	unit->m_noisePct = noisePct;
}


 /////////////AtsPartial

void AtsPartial_Ctor(AtsPartial* unit)
{
    SETCALC(AtsPartial_next);
    int tableSizeSin = ft->mSineSize;
    unit->m_numPartials = (int)IN0(1);
    unit->m_lomask = (tableSizeSin - 1) << 3;
    unit->m_radtoinc = tableSizeSin * (rtwopi * 65536.);
    unit->m_cpstoinc = tableSizeSin * SAMPLEDUR * 65536.;
    unit->m_freqMul = IN0(3);
    unit->m_freqAdd = IN0(4);
    unit->phaseinit = 1;
    unit->m_fbufnum = -1e9f;
    ClearUnitOutputs(unit, 1);
}


void AtsPartial_next(AtsPartial *unit, int inNumSamples)
{
    ClearUnitOutputs(unit, inNumSamples);
    // get the buffer that stores the ATS data
    float fbufnum = IN0(0);

    GET_ATS_BUF

    // calculate offsets based on file type
    int offset1 = 2;
    int offset2 = 1;
    if((fileType == 2) || (fileType == 4)) offset1 = 3;
    if(fileType >2) offset2 = 26;

    // set-up the output pointer
    float *out = OUT(0);

    // get the requested frames and other inputs
    float frame = sc_mod(IN0(2), 1.0f) * (float)fileNumFrames; // frame input should be between 0 and 1
    float nextFreqMul = IN0(3);
    float nextFreqAdd = IN0(4);

    float freqMul = unit->m_freqMul;
    float freqAdd = unit->m_freqAdd;
    float freqMulSlope = CALCSLOPE(nextFreqMul, freqMul);
    float freqAddSlope = CALCSLOPE(nextFreqAdd, freqAdd);

    // to get current data, we will need to interpolate values between frames - figure out the bounding frames
    CALC_FRAME_OFFSETS

    int block = ((fileNumPartials * offset1) + offset2);
    int block1 = iFrame1 * block;
    int block2 = iFrame2 * block;

    int thisPartial, dataPos1, dataPos2, partialOffset;
    float amp1, amp2, freq1, freq2, amp, freq, newamp, newfreq, ampslope, freqslope;
    int32 ifreq, phase;

    // if this is the first time through the next function, allocate the phase pointer, and zero it out, fill the initial amp and freq data
    if(unit->phaseinit > 0){
	unit->m_partials = (int)IN0(1);

	thisPartial = unit->m_partials;
	unit->m_phase = 0;
	partialOffset = (thisPartial * offset1);
	dataPos1 = block1 + partialOffset;
	dataPos2 = block2 + partialOffset;
	freq1 = atsData[dataPos1 + 1];
	freq2 = atsData[dataPos2 + 1];
	unit->m_lastfreq = (lininterp(framePct, freq1, freq2) * freqMul) + freqAdd;
	amp1 = atsData[dataPos1];
	amp2 = atsData[dataPos2];
	unit->m_lastamp = lininterp(framePct, amp1, amp2);

	unit->phaseinit = -1;
    }

    int partials = unit->m_partials;

    // the sine table
    float *table0 = ft->mSineWavetable;
    float *table1 = table0 + 1;
    int32 lomask = unit->m_lomask;

    // this loop calulates the partial sample values for the each of the sins, stores them in an array
    thisPartial = partials;
    partialOffset = (thisPartial * offset1);
    dataPos1 = block1 + partialOffset;
    dataPos2 = block2 + partialOffset;
    amp1 = atsData[dataPos1];
    amp2 = atsData[dataPos2];
    freq1 = atsData[dataPos1 + 1];
    freq2 = atsData[dataPos2 + 1];
    freqMul = unit->m_freqMul; //grab the original value from the struct
    freqAdd = unit->m_freqAdd;

    newamp = lininterp(framePct, amp1, amp2); \
    newfreq = ((lininterp(framePct, freq1, freq2) * freqMul) + freqAdd);
    amp = unit->m_lastamp;
    freq = unit->m_lastfreq;
    ampslope = CALCSLOPE(newamp, amp);
    freqslope = CALCSLOPE(newfreq, freq);
    phase = unit->m_phase;
    for (int j = 0; j < inNumSamples; ++j){
	ifreq = (int32)(unit->m_cpstoinc * freq);
	out[j] += (lookupi1(table0, table1, phase, lomask) * amp);
	phase += ifreq;
	freq += freqslope;
	amp += ampslope;
	freqMul += freqMulSlope;
	freqAdd += freqAddSlope;
    };
    unit->m_lastfreq = freq;
    unit->m_lastamp = amp;
    unit->m_phase = phase;

    unit->m_freqMul = freqMul;
    unit->m_freqAdd = freqAdd;
}

/* AtsBand */

void AtsBand_Ctor(AtsBand* unit)
{
    SETCALC(AtsBand_next);
    int tableSizeSin = ft->mSineSize;
    int band = (int)IN0(1);
    unit->m_lomask = (tableSizeSin - 1) << 3;
    unit->m_radtoinc = tableSizeSin * (rtwopi * 65536.);
    unit->m_cpstoinc = tableSizeSin * SAMPLEDUR * 65536.;

    float atsBandFreq[25] = ATS_BAND_WIDTH;
    float atsCenterFreq[25] = ATS_CENTER_FREQ;

    RGET

    unit->m_atsBandFreq = atsBandFreq[band];
    unit->m_atsCenterFreq = (int32)(unit->m_cpstoinc * atsCenterFreq[band]);
    unit->m_counter = 0;
    unit->m_level = unit->mParent->mRGen->frand2();
    unit->m_slope = 0.f;

    RPUT

    unit->phaseinit = 1;
    unit->m_fbufnum = -1e9f;
    ClearUnitOutputs(unit, 1);
}

void AtsBand_next(AtsBand *unit, int inNumSamples)
{
    int dataPos1, dataPos2, partialOffset;
    float rScale, noise1, noise2, amp, freq, noise, newamp, newfreq, ampslope, freqslope, noiseslope;
    int32 ifreq, phase;

    ClearUnitOutputs(unit, inNumSamples);
    // get the buffer that stores the ATS data
    float fbufnum = IN0(0);

    GET_ATS_BUF

    // calculate offsets based on file type
    int offset1 = 2;
    int offset2 = 1;
    if((fileType == 2) || (fileType == 4)) offset1 = 3;
    if(fileType >2) offset2 = 26;

    // set-up the output pointer
    float *out = OUT(0);

    // get the requested frames and other inputs
    float frame = sc_mod(IN0(2), 1.0f) * (float)fileNumFrames; // frame should be between 0 and 1

    // to get current data, we will need to interpolate values between frames - figure out the bounding frames
    CALC_FRAME_OFFSETS

    int block = ((fileNumPartials * offset1) + offset2);
    int block1 = iFrame1 * block;
    int block2 = iFrame2 * block;

    // if this is the first time through the 'next' function, allocate the pointers, and zero out if needed, fill the initial data
    if(unit->phaseinit > 0){

	unit->m_bands = (int)IN0(1);

	unit->m_rScale = rScale = 1 / (bufData[3] * 0.33166610955984);
	int bandnum = unit->m_bands;
	int dataOffset = (fileNumPartials * offset1) + bandnum;
	noise1 = atsData[block1 + dataOffset]; // 1 + 10 slots of header
	noise2 = atsData[block2 + dataOffset];
	unit->m_lastnoise = sqrt(lininterp(framePct, noise1, noise2) * rScale);
	unit->m_sinphase = 0;
	unit->phaseinit = -1;
    }

    int bands = unit->m_bands;
    int totalBands = 1; //unit->m_totalBands;

    // the sine table
    float *table0 = ft->mSineWavetable;
    float *table1 = table0 + 1;
    int32 lomask = unit->m_lomask;

    rScale = unit->m_rScale;

    RGET

    //calculate the noise
    if(fileType > 2) {
	    int bandnum = bands;
	    float noiseStart = unit->m_lastnoise;
	    int dataOffset = (fileNumPartials * offset1) + bandnum;
	    noise1 = atsData[block1 + dataOffset];
	    noise2 = atsData[block2 + dataOffset];
	    unit->m_lastnoise = noise = sqrt(lininterp(framePct, noise1, noise2) * rScale);
	    noiseslope = CALCSLOPE(noise, noiseStart);

	    int32 sinphase = unit->m_sinphase;

	    float noisefreq = unit->m_atsBandFreq;
	    int32 freq = unit->m_atsCenterFreq;

	    float level = unit->m_level;
	    float slope = unit->m_slope;
	    int32 counter = unit->m_counter;
	    float nextlevel = 0.;
	    for (int j = 0; j < inNumSamples; j++){
		if (counter<=0) {
		    counter = (int32)(unit->mRate->mSampleRate / sc_max(noisefreq, .001f));
		    counter = sc_max(1, counter);
		    nextlevel = frand2(s1, s2, s3);
		    slope = (nextlevel - level) / counter;
		} else --counter;
		out[j] += lookupi1(table0, table1, sinphase, lomask) * level * noiseStart;
		noiseStart += noiseslope;
		level += slope;
		sinphase += freq;
	    }
	    unit->m_level = level;
	    unit->m_slope = slope;
	    unit->m_counter = counter;
	    unit->m_sinphase = sinphase;
    } else {
	Print("This ATS file doesn't appear to have noise data. Use AtsSynth\n");
    }

    RPUT

}

void AtsFreq_Ctor(AtsFreq* unit)
{
	SETCALC(AtsFreq_next);
	unit->m_fbufnum = -1e9f;
	unit->m_init = -1;
	unit->m_partial = IN0(1);
	AtsFreq_next(unit, 1);
}


void AtsFreq_next(AtsFreq *unit, int inNumSamples)
{
	// get the buffer that stores the ATS data
	float fbufnum = IN0(0);

	GET_ATS_BUF

	// calculate offsets based on file type
	int offset1 = 2;
	int offset2 = 1;
	if((fileType == 2) || (fileType == 4)) offset1 = 3;
	if(fileType >2) offset2 = 26;

	// set-up the output pointer
	float *out = OUT(0);

	// get the requested frames and other inputs
	float frame = sc_mod(IN0(2), 1.0f) * (float)fileNumFrames; // frame input should be between 0 and 1

	// to get current data, we will need to interpolate values between frames - figure out the bounding frames
	CALC_FRAME_OFFSETS

	int block = ((fileNumPartials * offset1) + offset2);
	int block1 = iFrame1 * block;
	int block2 = iFrame2 * block;

	int dataPos1, dataPos2, partialOffset;
	float freq1, freq2, freq, freqslope, nextFreq;

	if(unit->m_init < 0){
	    partialOffset = (unit->m_partial * offset1);
	    dataPos1 = block1 + partialOffset;
	    dataPos2 = block2 + partialOffset;
	    freq1 = atsData[dataPos1 + 1];
	    freq2 = atsData[dataPos2 + 1];
	    unit->m_lastfreq = lininterp(framePct, freq1, freq2);
	    unit->m_init = 1;
	    }

	// this loop calulates the partial sample values for the each of the sins, stores them in an array

	partialOffset = (unit->m_partial * offset1);
	dataPos1 = block1 + partialOffset;
	dataPos2 = block2 + partialOffset;
	freq1 = atsData[dataPos1 + 1];
	freq2 = atsData[dataPos2 + 1];
	freq = unit->m_lastfreq;
	nextFreq = lininterp(framePct, freq1, freq2);
	freqslope = CALCSLOPE(nextFreq, freq);
	for(int i = 0; i < inNumSamples; i++){
	    out[i] = freq;
	    freq += freqslope;
	    }
	unit->m_lastfreq = freq;

}

void AtsAmp_Ctor(AtsAmp* unit)
{
	SETCALC(AtsAmp_next);
	unit->m_fbufnum = -1e9f;
	unit->m_init = -1;
	unit->m_partial = IN0(1);
	AtsAmp_next(unit, 1);
}


void AtsAmp_next(AtsAmp *unit, int inNumSamples)
{
	// get the buffer that stores the ATS data
	float fbufnum = IN0(0);

	GET_ATS_BUF

	// calculate offsets based on file type
	int offset1 = 2;
	int offset2 = 1;
	if((fileType == 2) || (fileType == 4)) offset1 = 3;
	if(fileType >2) offset2 = 26;

	// set-up the output pointer
	float *out = OUT(0);

	// get the requested frames and other inputs
	float frame = sc_mod(IN0(2), 1.0f) * (float)fileNumFrames; // frame input should be between 0 and 1

	// to get current data, we will need to interpolate values between frames - figure out the bounding frames
	CALC_FRAME_OFFSETS

	int block = ((fileNumPartials * offset1) + offset2);
	int block1 = iFrame1 * block;
	int block2 = iFrame2 * block;

	int dataPos1, dataPos2, partialOffset;
	float amp1, amp2, amp, ampslope, nextAmp;

	if(unit->m_init < 0){
	    partialOffset = (unit->m_partial * offset1);
	    dataPos1 = block1 + partialOffset;
	    dataPos2 = block2 + partialOffset;
	    amp1 = atsData[dataPos1];
	    amp2 = atsData[dataPos2];
	    unit->m_lastamp = lininterp(framePct, amp1, amp2);
	    unit->m_init = 1;
	    }

	// this loop calulates the partial sample values for the each of the sins, stores them in an array

	partialOffset = (unit->m_partial * offset1);
	dataPos1 = block1 + partialOffset;
	dataPos2 = block2 + partialOffset;
	amp1 = atsData[dataPos1];
	amp2 = atsData[dataPos2];
	amp = unit->m_lastamp;
	nextAmp = lininterp(framePct, amp1, amp2);
	ampslope = CALCSLOPE(nextAmp, amp);
	for(int i = 0; i < inNumSamples; i++){
	    out[i] = amp;
	    amp += ampslope;
	    }
	unit->m_lastamp = amp;

}


void AtsParInfo_Ctor(AtsParInfo* unit)
{
	SETCALC(AtsParInfo_next);
	unit->m_fbufnum = -1e9f;
	unit->m_init = -1;
	unit->m_partial = IN0(1);
	AtsParInfo_next(unit, 1);
}


void AtsParInfo_next(AtsParInfo *unit, int inNumSamples)
{
	// get the buffer that stores the ATS data
	float fbufnum = IN0(0);

	GET_ATS_BUF

	// calculate offsets based on file type
	int offset1 = 2;
	int offset2 = 1;
	if((fileType == 2) || (fileType == 4)) offset1 = 3;
	if(fileType >2) offset2 = 26;

	// set-up the output pointer
	float *ampOut = OUT(0);
	float *freqOut = OUT(1);

	// get the requested frames and other inputs
	float frame = sc_mod(IN0(2), 1.0f) * (float)fileNumFrames; // frame input should be between 0 and 1

	// to get current data, we will need to interpolate values between frames - figure out the bounding frames
	CALC_FRAME_OFFSETS

	int block = ((fileNumPartials * offset1) + offset2);
	int block1 = iFrame1 * block;
	int block2 = iFrame2 * block;

	int dataPos1, dataPos2, partialOffset;
	float amp1, amp2, amp, ampslope, nextAmp;
	float freq1, freq2, freq, freqslope, nextFreq;

	if(unit->m_init < 0){
	    partialOffset = (unit->m_partial * offset1);
	    dataPos1 = block1 + partialOffset;
	    dataPos2 = block2 + partialOffset;
	    amp1 = atsData[dataPos1];
	    amp2 = atsData[dataPos2];
	    unit->m_lastamp = lininterp(framePct, amp1, amp2);
	    freq1 = atsData[dataPos1 + 1];
	    freq2 = atsData[dataPos2 + 1];
	    unit->m_lastfreq = lininterp(framePct, freq1, freq2);
	    unit->m_init = 1;
	    }

	// this loop calulates the partial sample values for the each of the sins, stores them in an array

	partialOffset = (unit->m_partial * offset1);
	dataPos1 = block1 + partialOffset;
	dataPos2 = block2 + partialOffset;
	amp1 = atsData[dataPos1];
	amp2 = atsData[dataPos2];
	amp = unit->m_lastamp;
	nextAmp = lininterp(framePct, amp1, amp2);
	ampslope = CALCSLOPE(nextAmp, amp);
	freq1 = atsData[dataPos1 + 1];
	freq2 = atsData[dataPos2 + 1];
	freq = unit->m_lastfreq;
	nextFreq = lininterp(framePct, freq1, freq2);
	freqslope = CALCSLOPE(nextFreq, freq);

	for(int i = 0; i < inNumSamples; i++){
	    ampOut[i] = amp;
	    freqOut[i] = freq;
	    amp += ampslope;
	    freq += freqslope;
	    }
	unit->m_lastamp = amp;
	unit->m_lastfreq = freq;

}

void AtsNoise_Ctor(AtsNoise* unit)
{
	SETCALC(AtsAmp_next);
	unit->m_fbufnum = -1e9f;
	unit->m_init = -1;
	unit->m_band = IN0(1);
	AtsNoise_next(unit, 1);
}


void AtsNoise_next(AtsNoise *unit, int inNumSamples)
{
	// get the buffer that stores the ATS data
	float fbufnum = IN0(0);

	GET_ATS_BUF

	// calculate offsets based on file type
	int offset1 = 2;
	int offset2 = 1;
	if((fileType == 2) || (fileType == 4)) offset1 = 3;
	if(fileType >2) offset2 = 26;

	// set-up the output pointer
	float *out = OUT(0);

	// get the requested frames and other inputs
	float frame = sc_mod(IN0(2), 1.0f) * (float)fileNumFrames; // frame input should be between 0 and 1

	// to get current data, we will need to interpolate values between frames - figure out the bounding frames
	CALC_FRAME_OFFSETS

	int block = ((fileNumPartials * offset1) + offset2);
	int block1 = iFrame1 * block;
	int block2 = iFrame2 * block;

	int dataPos1, dataPos2, dataOffset;
	float noise1, noise2, noise, noiseslope, nextNoise;

	if(unit->m_init < 0){
	    dataOffset = (fileNumPartials * offset1) + unit->m_band;
	    dataPos1 = block1 + dataOffset;
	    dataPos2 = block2 + dataOffset;
	    noise1 = atsData[dataPos1];
	    noise2 = atsData[dataPos2];
	    unit->m_lastnoise = lininterp(framePct, noise1, noise2);
	    unit->m_init = 1;
	    }

	// this loop calulates the partial sample values for the each of the sins, stores them in an array

	dataOffset = (fileNumPartials * offset1) + unit->m_band;
	dataPos1 = block1 + dataOffset;
	dataPos2 = block2 + dataOffset;
	noise1 = atsData[dataPos1];
	noise2 = atsData[dataPos2];
	noise = unit->m_lastnoise;
	nextNoise = lininterp(framePct, noise1, noise2);
	noiseslope = CALCSLOPE(nextNoise, noise);
	for(int i = 0; i < inNumSamples; i++){
	    out[i] = noise;
	    noise += noiseslope;
	    }
	unit->m_lastnoise = noise;

}

void PVSynth_Ctor(PVSynth* unit)
{
	SETCALC(PVSynth_next);
	int tableSizeSin = ft->mSineSize;
	unit->m_numPartials = (int)IN0(1);
	unit->m_partialStart = (int)IN0(2);
	unit->m_partialSkip = (int)IN0(3);
	unit->m_lomask = (tableSizeSin - 1) << 3;
	unit->m_radtoinc = tableSizeSin * (rtwopi * 65536.);
	unit->m_cpstoinc = tableSizeSin * SAMPLEDUR * 65536.;
	unit->phaseinit = 1;
	unit->m_fbufnum = -1e9f;
	unit->m_freqMul = IN0(5);
	unit->m_freqAdd = IN0(6);
	ClearUnitOutputs(unit, 1);
	PVSynth_next(unit, 1);
}

void PVSynth_Dtor(PVSynth* unit)
{
	RTFree(unit->mWorld, unit->m_phase);
	RTFree(unit->mWorld, unit->m_lastamp);
	RTFree(unit->mWorld, unit->m_lastfreq);
	RTFree(unit->mWorld, unit->m_partials);
}


void PVSynth_next(PVSynth *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);
	// get the buffer that stores the PV data
	float fbufnum = IN0(0);

	if (fbufnum != unit->m_fbufnum) {
		uint32 bufnum = (int)fbufnum;
		World *world = unit->mWorld;
		if (bufnum >= world->mNumSndBufs) bufnum = 0;
		unit->m_fbufnum = fbufnum;
		unit->m_buf = world->mSndBufs + bufnum;
	    }
	SndBuf *buf = unit->m_buf;
	float *bufData __attribute__((__unused__)) = buf->data;
	if (!bufData) {
                unit->mDone = true;
		return;
		};
	float frameBSize = bufData[8];
	int nBins = (int)(bufData[6] * 0.5) + 1; //(int)frameBSize >> 3;
	float fileSize = bufData[2];
	int nFrames = (int)(fileSize / frameBSize) - 1;
	float* pvData = bufData + 13; //headerOffset;

	// set-up the output pointer
	float *out = OUT(0);
	// get the requested frames and other inputs

	float frame = sc_mod(IN0(4), 1.0f) * (float)nFrames; // frame input should be between 0 and 1
	float nextFreqMul = IN0(5);
	float nextFreqAdd = IN0(6);

	float freqMul = unit->m_freqMul;
	float freqAdd = unit->m_freqAdd;
	float freqMulSlope = CALCSLOPE(nextFreqMul, freqMul);
	float freqAddSlope = CALCSLOPE(nextFreqAdd, freqAdd);

	// to get current data, we will need to interpolate values between frames - figure out the bounding frames
	int iFrame1 = (int)frame;
	int iFrame2 = iFrame1 + 1;
	float framePct = frame - (float)iFrame1;
	if(iFrame2 >= nFrames){
	    iFrame2 = iFrame1;
	    framePct = 0.;
	    }
	int block = (nBins * 2);
	int block1 = iFrame1 * block;
	int block2 = iFrame2 * block;

	int thisPartial, dataPos1, dataPos2, partialOffset;
	float amp1, amp2, freq1, freq2, amp, freq, newamp, newfreq, ampslope, freqslope;
	int32 ifreq, phase;

	// if this is the first time through the next function, allocate the phase pointer, and zero it out, fill the initial amp and freq data
	if(unit->phaseinit > 0){

	    unit->m_totalPartials = unit->m_numPartials;
	    for (int j = 0; j < unit->m_numPartials; ++j){
		int aPartial = unit->m_partialStart + (unit->m_partialSkip * j);
		if (aPartial > nBins)
		    --unit->m_totalPartials;
	    }

	    unit->m_phase = (int32*)RTAlloc(unit->mWorld, unit->m_totalPartials * sizeof(int32));
	    unit->m_lastamp = (float*)RTAlloc(unit->mWorld, unit->m_totalPartials * sizeof(float));
	    unit->m_lastfreq = (float*)RTAlloc(unit->mWorld, unit->m_totalPartials * sizeof(float));
	    unit->m_partials = (int*)RTAlloc(unit->mWorld, unit->m_totalPartials * sizeof(int));

	    for (int j = 0; j < unit->m_totalPartials; ++j){
		int aPartial = unit->m_partialStart + (unit->m_partialSkip * j);
		unit->m_partials[j] = aPartial;
	    }

	    for (int i = 0; i < unit->m_totalPartials; i++){
		thisPartial = unit->m_partials[i];
		partialOffset = thisPartial * 2;
		unit->m_phase[i] = 0.;
		dataPos1 = block1 + thisPartial;
		dataPos2 = block2 + thisPartial;
		freq1 = pvData[block1 + partialOffset + 1];
		freq2 = pvData[block2 + partialOffset + 1];
		unit->m_lastfreq[i] = (lininterp(framePct, freq1, freq2) * freqMul) + freqAdd;
		amp1 = pvData[block1 + partialOffset];
		amp2 = pvData[block2 + partialOffset];
		unit->m_lastamp[i] = lininterp(framePct, amp1, amp2);
		}
	    unit->phaseinit = -1;
	}

	int *partials = unit->m_partials;

	// the sine table
	float *table0 = ft->mSineWavetable;
	float *table1 = table0 + 1;
	int32 lomask = unit->m_lomask;

	// this loop calulates the partial sample values for the each of the sins, stores them in an array
	for (int i = 0; i< unit->m_totalPartials; i++){
	    thisPartial = partials[i];
	    partialOffset = thisPartial * 2;
	    dataPos1 = block1 + thisPartial;
	    dataPos2 = block2 + thisPartial;

	    amp1 = pvData[block1 + partialOffset];
	    amp2 = pvData[block2 + partialOffset];
	    freq1 = pvData[block1 + partialOffset + 1];
	    freq2 = pvData[block2 + partialOffset + 1];
	    freqMul = unit->m_freqMul; //grab the original value from the struct
	    freqAdd = unit->m_freqAdd;

	    ADD_SIN_PARTIALS

	    }
	unit->m_freqMul = freqMul;
	unit->m_freqAdd = freqAdd;
}

///////////// PVInfo

void PVInfo_Ctor(PVInfo* unit)
{
	SETCALC(PVInfo_next);
	unit->m_partial = (int)IN0(1);
	unit->m_phaseinit = 1;
	unit->m_fbufnum = -1e9f;
	ClearUnitOutputs(unit, 1);
	PVInfo_next(unit, 1);
}


void PVInfo_next(PVInfo *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);
	// get the buffer that stores the PV data
	float fbufnum = IN0(0);

	if (fbufnum != unit->m_fbufnum) {
		uint32 bufnum = (int)fbufnum;
		World *world = unit->mWorld;
		if (bufnum >= world->mNumSndBufs) bufnum = 0;
		unit->m_fbufnum = fbufnum;
		unit->m_buf = world->mSndBufs + bufnum;
	    }
	SndBuf *buf = unit->m_buf;
	float *bufData __attribute__((__unused__)) = buf->data;
	if (!bufData) {
                unit->mDone = true;
		return;
		};
	float frameBSize = bufData[8];
	int nBins = (int)(bufData[6] * 0.5) + 1; //(int)frameBSize >> 3;
	float fileSize = bufData[2];
	int nFrames = (int)(fileSize / frameBSize) - 1;
	float* pvData = bufData + 13; //headerOffset;

	// set-up the output pointer
	float *ampOut = OUT(0);
	float *freqOut = OUT(1);
	// get the requested frames and other inputs

	float frame = sc_mod(IN0(2), 1.0f) * (float)nFrames; // frame input should be between 0 and 1

	// to get current data, we will need to interpolate values between frames - figure out the bounding frames
	int iFrame1 = (int)frame;
	int iFrame2 = iFrame1 + 1;
	float framePct = frame - (float)iFrame1;
	if(iFrame2 >= nFrames){
	    iFrame2 = iFrame1;
	    framePct = 0.;
	    }
	int block = (nBins * 2);
	int block1 = iFrame1 * block;
	int block2 = iFrame2 * block;

	int thisPartial, dataPos1, dataPos2, partialOffset;
	float amp1, amp2, freq1, freq2, amp, freq, newamp, newfreq, ampslope, freqslope;

	// if this is the first time through the next function, allocate the phase pointer, and zero it out, fill the initial amp and freq data
	if(unit->m_phaseinit > 0){

	    thisPartial = unit->m_partial;
	    partialOffset = thisPartial * 2;
	    dataPos1 = block1 + thisPartial;
	    dataPos2 = block2 + thisPartial;
	    freq1 = pvData[block1 + partialOffset + 1];
	    freq2 = pvData[block2 + partialOffset + 1];
	    unit->m_lastfreq = lininterp(framePct, freq1, freq2);
	    amp1 = pvData[block1 + partialOffset];
	    amp2 = pvData[block2 + partialOffset];
	    unit->m_lastamp = lininterp(framePct, amp1, amp2);

	    unit->m_phaseinit = -1;
	}

	// this loop calulates the partial sample values for the each of the sins, stores them in an array
	thisPartial = unit->m_partial;
	partialOffset = thisPartial * 2;
	dataPos1 = block1 + thisPartial;
	dataPos2 = block2 + thisPartial;

	amp1 = pvData[block1 + partialOffset];
	amp2 = pvData[block2 + partialOffset];
	newamp = lininterp(framePct, amp1, amp2);
	amp = unit->m_lastamp;
	freq1 = pvData[block1 + partialOffset + 1];
	freq2 = pvData[block2 + partialOffset + 1];
	newfreq = lininterp(framePct, freq1, freq2);
	freq = unit->m_lastfreq;

	ampslope = CALCSLOPE(newamp, amp);
	freqslope = CALCSLOPE(newfreq, freq);

	for(int i = 0; i < inNumSamples; i++)
	    {
		ampOut[i] = amp;
		freqOut[i] = freq;
		amp += ampslope;
		freq += freqslope;
	    }
	unit->m_lastamp = newamp;
	unit->m_lastfreq = newfreq;
}



/////////////////////////////////////////////////////////////////

void SinTone_Ctor(SinTone* unit)
{
    SETCALC(SinTone_next);
    unit->m_phase = IN0(1);
    SinTone_next(unit, 1);

}

void SinTone_next(SinTone *unit, int inNumSamples)
{
	float freq = IN0(0);
	double phase = unit->m_phase;
	float *valout = OUT(0);
	float incr = (freq / SAMPLERATE) * 6.2831853071796;

	for (int i = 0; i< inNumSamples; ++i)
	    {
	    valout[i] = sin(phase);
	    phase += incr;
	    };

	unit->m_phase = phase;
}


void Balance_Ctor(Balance* unit)
{
	unit->m_scaler = 0.f;
	if (INRATE(1) == calc_FullRate) {
	    SETCALC(Balance_next_a);
	    } else {
	    SETCALC(Balance_next_k);
	    }
	unit->m_hp = IN0(2);
	unit->m_stor = IN0(3);
	float b = 2.0 - cos(unit->m_hp * (twopi/SAMPLERATE));
	unit->m_coef2 = b - sqrt(b*b - 1.0);
	unit->m_coef1 = 1.0 - unit->m_coef2;
	unit->m_prevq = unit->m_prevr = unit->m_preva =  0.f;
	ClearUnitOutputs(unit, 1);
}

void Balance_next_a(Balance* unit, int inNumSamples)
{
    float* insig = IN(0);
    float* testsig = IN(1);
    float* out = OUT(0);
    float q = unit->m_prevq;
    float coef1 = unit->m_coef1;
    float coef2 = unit->m_coef2;
    float r = unit->m_prevr;
    float preva = unit->m_preva; // the scaler
    float a;
    for(int i = 0; i < inNumSamples; i++){
	float as = powf(insig[i], 2);
	float cs = powf(testsig[i], 2);
	q = coef1 * as + coef2 * q;
	r = coef1 * cs + coef2 * r;
	}
    unit->m_prevq = q;
    unit->m_prevr = r;
    if (q != 0.0)
	a = sqrt(r/q);
    else
	a = sqrt(r);
    float inc = CALCSLOPE(a, preva);
    for(int i = 0; i < inNumSamples; i++){
	out[i] = insig[i] * preva;
	preva += inc;
	}
    unit->m_preva = preva;
}

void Balance_next_k(Balance* unit, int inNumSamples)
{
    float* insig = IN(0);
    float testsig = IN0(1);
    float* out = OUT(0);
    float q = unit->m_prevq;
    float coef1 = unit->m_coef1;
    float coef2 = unit->m_coef2;
    float r = unit->m_prevr;
    float preva = unit->m_preva; // the scaler
    float a;
    for(int i = 0; i < inNumSamples; i++){
	float as = powf(insig[i], 2);
	q = coef1 * as + coef2 * q;
	}
    r = testsig;
    unit->m_prevq = q;
    unit->m_prevr = r;
    if (q != 0.0)
	a = sqrt(r/q);
    else
	a = sqrt(r);
    float inc = CALCSLOPE(a, preva);
    for(int i = 0; i < inNumSamples; i++){
	out[i] = insig[i] * preva;
	preva += inc;
	}
    unit->m_preva = preva;
}

/*

moogvcf is a digital emulation of the Moog diode ladder filter configuration. This emulation is based loosely
on the paper "Analyzing the Moog VCF with Considerations for Digital Implemnetation" by Stilson and Smith (CCRMA).
This version was originally coded in Csound by Josep Comajuncosas. Some modifications and conversion to C were
done by Hans Mikelson
SC version by Joshua Parmenter
Note: This filter requires that the input signal be normalized to one.
MoogVCF.ar(in, fco, res, mul, add); fco = filter cut-off, res = resonance (0 -> 1)

*/

// fastexp taken from:

/*=====================================================================*
 *                   Copyright (C) 2011 Paul Mineiro                   *
 * All rights reserved.                                                *
 *                                                                     *
 * Redistribution and use in source and binary forms, with             *
 * or without modification, are permitted provided that the            *
 * following conditions are met:                                       *
 *                                                                     *
 *     * Redistributions of source code must retain the                *
 *     above copyright notice, this list of conditions and             *
 *     the following disclaimer.                                       *
 *                                                                     *
 *     * Redistributions in binary form must reproduce the             *
 *     above copyright notice, this list of conditions and             *
 *     the following disclaimer in the documentation and/or            *
 *     other materials provided with the distribution.                 *
 *                                                                     *
 *     * Neither the name of Paul Mineiro nor the names                *
 *     of other contributors may be used to endorse or promote         *
 *     products derived from this software without specific            *
 *     prior written permission.                                       *
 *                                                                     *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND              *
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,         *
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES               *
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE             *
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER               *
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,                 *
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES            *
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE           *
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR                *
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF          *
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT           *
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY              *
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE             *
 * POSSIBILITY OF SUCH DAMAGE.                                         *
 *                                                                     *
 * Contact: Paul Mineiro <paul@mineiro.com>                            *
 *=====================================================================*/

static inline float
fastpow2 (float p)
{
	float offset = (p < 0) ? 1.0f : 0.0f;
	float clipp = (p < -126) ? -126.0f : p;
	int w = clipp;
	float z = clipp - w + offset;
	union { uint32_t i; float f; } v;
	v.i = (1 << 23) * (clipp + 121.2740575f + 27.7280233f / (4.84252568f - z) - 1.49012907f * z);

	return v.f;
}

static inline float
fastexp (float p)
{
	return fastpow2 (1.442695040f * p);
}


static inline void MoogVCF_calc_parameters(float fco, float res, float & kp, float & pp1d2, float & k)
{
	fco = std::min(fco, 1.f);
	kp = (3.6f * fco) - ((1.6f * fco) * fco) - 1.0f;  /* Emperical tuning     */
	pp1d2 = (kp + 1.0f) * 0.5f;              /* Timesaver                  */
	float scale = fastexp((1.0f - pp1d2 ) * 1.386249f); /* Scaling factor  */
	k = res * scale;
}

inline float MoogVCF_blowup_prevention(float f)
{
	const float oneOverSix = 1/6.0f;
	const float sqrt2 = std::sqrt(2.0);
	f = sc_clip(f, -sqrt2, sqrt2);
	return f - (f*f) * (f * oneOverSix);
}

void MoogVCF_Ctor(MoogVCF* unit)
{
	unit->m_fco = (IN0(1) * 2) * SAMPLEDUR;
	unit->m_res = IN0(2);
	unit->m_xnm1 = unit->m_y1nm1 = unit->m_y2nm1 = unit->m_y3nm1 = 0.0f;
	unit->m_y1n = unit->m_y2n = unit->m_y3n = unit->m_y4n = 0.0f;

	switch (INRATE(1)) {
	case calc_FullRate:
		switch (INRATE(2)) {
		case calc_FullRate:
			SETCALC(MoogVCF_next_aa); // a-rate fco and res
			break;
		default:
			SETCALC(MoogVCF_next_ak); // a-rate fco, k-rate res
			break;
		}
		break;

	case calc_BufRate:
		switch (INRATE(2)) {
			case calc_FullRate:
				SETCALC(MoogVCF_next_ka); // k-rate fco and res
				break;
			case calc_BufRate:
				SETCALC(MoogVCF_next_kk); // k-rate fco, k-rate res
				break;
			default:
				SETCALC(MoogVCF_next_ki); // k-rate fco, i-rate res
				break;
		}
		break;

	case calc_ScalarRate:
		switch (INRATE(2)) {
			case calc_ScalarRate:
				SETCALC(MoogVCF_next_ii); // i-rate fco and res
				MoogVCF_calc_parameters(unit->m_fco, unit->m_res, unit->m_kp, unit->m_pp1d2, unit->m_k);
				break;
			default:
				SETCALC(MoogVCF_next_kk);
				break;
		}
		break;
	}

	ClearUnitOutputs(unit, 1);
}

void MoogVCF_next_ii(MoogVCF *unit, int inNumSamples){
	float* in = IN(0);
	float* out = OUT(0);

	float xnm1 = unit->m_xnm1;
	float y1nm1 = unit->m_y1nm1;
	float y2nm1 = unit->m_y2nm1;
	float y3nm1 = unit->m_y3nm1;
	float y1n = unit->m_y1n;
	float y2n = unit->m_y2n;
	float y3n = unit->m_y3n;
	float y4n = unit->m_y4n;

	float kp = unit->m_kp, pp1d2 = unit->m_pp1d2, k = unit->m_k;

	for (int i = 0; i < inNumSamples; i++) {
		float xn = in[i]; // make this similar to the CSound stuff for now... easier translation
		xn = xn - (k * y4n); /* Inverted feed back for corner peaking */

		/* Four cascaded onepole filters (bilinear transform) */
		y1n   = (xn  * pp1d2) + (xnm1  * pp1d2) - (kp * y1n);
		y2n   = (y1n * pp1d2) + (y1nm1 * pp1d2) - (kp * y2n);
		y3n   = (y2n * pp1d2) + (y2nm1 * pp1d2) - (kp * y3n);
		y4n   = (y3n * pp1d2) + (y3nm1 * pp1d2) - (kp * y4n);
		y4n   = MoogVCF_blowup_prevention(y4n);
		xnm1  = xn;       /* Update Xn-1  */
		y1nm1 = y1n;      /* Update Y1n-1 */
		y2nm1 = y2n;      /* Update Y2n-1 */
		y3nm1 = y3n;      /* Update Y3n-1 */
		out[i] = y4n;
	}

	unit->m_xnm1 = zapgremlins(xnm1);
	unit->m_y1nm1 = zapgremlins(y1nm1);
	unit->m_y2nm1 = zapgremlins(y2nm1);
	unit->m_y3nm1 = zapgremlins(y3nm1);
	unit->m_y1n = zapgremlins(y1n);
	unit->m_y2n = zapgremlins(y2n);
	unit->m_y3n = zapgremlins(y3n);
	unit->m_y4n = zapgremlins(y4n);
}

void MoogVCF_next_ki(MoogVCF *unit, int inNumSamples){
	float* in = IN(0);
	float* out = OUT(0);
	float nextfco = IN0(1);
	float fco = unit->m_fco; // already normalized
	float res = unit->m_res;
	float fcon = (nextfco * 2.0) * SAMPLEDUR;   // filt freq, normalized to 0 to Nyquist

	float fcoslope = CALCSLOPE(fcon, fco);

	float xnm1 = unit->m_xnm1;
	float y1nm1 = unit->m_y1nm1;
	float y2nm1 = unit->m_y2nm1;
	float y3nm1 = unit->m_y3nm1;
	float y1n = unit->m_y1n;
	float y2n = unit->m_y2n;
	float y3n = unit->m_y3n;
	float y4n = unit->m_y4n;

	if (fcoslope) {
		for (int i = 0; i < inNumSamples; i++) {
			float kp, pp1d2, xn, k;
			MoogVCF_calc_parameters(fco, res, kp, pp1d2, k);
			xn = in[i]; // make this similar to the CSound stuff for now... easier translation
			xn = xn - (k * y4n); /* Inverted feed back for corner peaking */

			/* Four cascaded onepole filters (bilinear transform) */
			y1n   = (xn  * pp1d2) + (xnm1  * pp1d2) - (kp * y1n);
			y2n   = (y1n * pp1d2) + (y1nm1 * pp1d2) - (kp * y2n);
			y3n   = (y2n * pp1d2) + (y2nm1 * pp1d2) - (kp * y3n);
			y4n   = (y3n * pp1d2) + (y3nm1 * pp1d2) - (kp * y4n);
			y4n   = MoogVCF_blowup_prevention(y4n);
			xnm1  = xn;       /* Update Xn-1  */
			y1nm1 = y1n;      /* Update Y1n-1 */
			y2nm1 = y2n;      /* Update Y2n-1 */
			y3nm1 = y3n;      /* Update Y3n-1 */
			out[i] = y4n;
			fco += fcoslope;
		}
	} else {
		float kp, pp1d2, xn, k;
		MoogVCF_calc_parameters(fco, res, kp, pp1d2, k);
		for (int i = 0; i < inNumSamples; i++) {
			xn = in[i]; // make this similar to the CSound stuff for now... easier translation
			xn = xn - (k * y4n); /* Inverted feed back for corner peaking */

			/* Four cascaded onepole filters (bilinear transform) */
			y1n   = (xn  * pp1d2) + (xnm1  * pp1d2) - (kp * y1n);
			y2n   = (y1n * pp1d2) + (y1nm1 * pp1d2) - (kp * y2n);
			y3n   = (y2n * pp1d2) + (y2nm1 * pp1d2) - (kp * y3n);
			y4n   = (y3n * pp1d2) + (y3nm1 * pp1d2) - (kp * y4n);
			y4n   = MoogVCF_blowup_prevention(y4n);
			xnm1  = xn;       /* Update Xn-1  */
			y1nm1 = y1n;      /* Update Y1n-1 */
			y2nm1 = y2n;      /* Update Y2n-1 */
			y3nm1 = y3n;      /* Update Y3n-1 */
			out[i] = y4n;
		}
	}

	unit->m_fco = fcon; // store the normalized frequency
	unit->m_xnm1 = zapgremlins(xnm1);
	unit->m_y1nm1 = zapgremlins(y1nm1);
	unit->m_y2nm1 = zapgremlins(y2nm1);
	unit->m_y3nm1 = zapgremlins(y3nm1);
	unit->m_y1n = zapgremlins(y1n);
	unit->m_y2n = zapgremlins(y2n);
	unit->m_y3n = zapgremlins(y3n);
	unit->m_y4n = zapgremlins(y4n);
}


void MoogVCF_next_kk(MoogVCF *unit, int inNumSamples){
	float* in = IN(0);
	float* out = OUT(0);
	float nextfco = IN0(1);
	float nextres = IN0(2);
	float fco = unit->m_fco; // already normalized
	float res = unit->m_res;
	float fcon = (nextfco * 2.0) * SAMPLEDUR;   // filt freq, normalized to 0 to Nyquist

	float fcoslope = CALCSLOPE(fcon, fco);
	float resslope = CALCSLOPE(nextres, res);

	float xnm1 = unit->m_xnm1;
	float y1nm1 = unit->m_y1nm1;
	float y2nm1 = unit->m_y2nm1;
	float y3nm1 = unit->m_y3nm1;
	float y1n = unit->m_y1n;
	float y2n = unit->m_y2n;
	float y3n = unit->m_y3n;
	float y4n = unit->m_y4n;

	for (int i = 0; i < inNumSamples; i++) {
		float kp, pp1d2, xn, k;
		MoogVCF_calc_parameters(fco, res, kp, pp1d2, k);
		xn = in[i]; // make this similar to the CSound stuff for now... easier translation
		xn = xn - (k * y4n); /* Inverted feed back for corner peaking */

		/* Four cascaded onepole filters (bilinear transform) */
		y1n   = (xn  * pp1d2) + (xnm1  * pp1d2) - (kp * y1n);
		y2n   = (y1n * pp1d2) + (y1nm1 * pp1d2) - (kp * y2n);
		y3n   = (y2n * pp1d2) + (y2nm1 * pp1d2) - (kp * y3n);
		y4n   = (y3n * pp1d2) + (y3nm1 * pp1d2) - (kp * y4n);
		y4n   = MoogVCF_blowup_prevention(y4n);
		xnm1  = xn;       /* Update Xn-1  */
		y1nm1 = y1n;      /* Update Y1n-1 */
		y2nm1 = y2n;      /* Update Y2n-1 */
		y3nm1 = y3n;      /* Update Y3n-1 */
		out[i] = y4n;
		fco += fcoslope;
		res += resslope;
	}

	unit->m_fco = fcon; // store the normalized frequency
	unit->m_res = nextres;
	unit->m_xnm1 = zapgremlins(xnm1);
	unit->m_y1nm1 = zapgremlins(y1nm1);
	unit->m_y2nm1 = zapgremlins(y2nm1);
	unit->m_y3nm1 = zapgremlins(y3nm1);
	unit->m_y1n = zapgremlins(y1n);
	unit->m_y2n = zapgremlins(y2n);
	unit->m_y3n = zapgremlins(y3n);
	unit->m_y4n = zapgremlins(y4n);
}

void MoogVCF_next_ka(MoogVCF *unit, int inNumSamples){
	float* in = IN(0);
	float* out = OUT(0);
	float nextfco = IN0(1);
	float* res = IN(2);
	float fco = unit->m_fco; // already normalized
	float fcon = (nextfco * 2.0) * SAMPLEDUR;   // filt freq, normalized to 0 to Nyquist

	float fcoslope = CALCSLOPE(fcon, fco);

	float xnm1 = unit->m_xnm1;
	float y1nm1 = unit->m_y1nm1;
	float y2nm1 = unit->m_y2nm1;
	float y3nm1 = unit->m_y3nm1;
	float y1n = unit->m_y1n;
	float y2n = unit->m_y2n;
	float y3n = unit->m_y3n;
	float y4n = unit->m_y4n;

	for (int i = 0; i < inNumSamples; i++) {
		float kp, pp1d2, xn, k;
		MoogVCF_calc_parameters(fco, res[i], kp, pp1d2, k);
		xn = in[i]; // make this similar to the CSound stuff for now... easier translation
		xn = xn - (k * y4n); /* Inverted feed back for corner peaking */

		/* Four cascaded onepole filters (bilinear transform) */
		y1n   = (xn  * pp1d2) + (xnm1  * pp1d2) - (kp * y1n);
		y2n   = (y1n * pp1d2) + (y1nm1 * pp1d2) - (kp * y2n);
		y3n   = (y2n * pp1d2) + (y2nm1 * pp1d2) - (kp * y3n);
		y4n   = (y3n * pp1d2) + (y3nm1 * pp1d2) - (kp * y4n);
		y4n   = MoogVCF_blowup_prevention(y4n);
		xnm1  = xn;       /* Update Xn-1  */
		y1nm1 = y1n;      /* Update Y1n-1 */
		y2nm1 = y2n;      /* Update Y2n-1 */
		y3nm1 = y3n;      /* Update Y3n-1 */
		out[i] = y4n;
		fco += fcoslope;
	}

	unit->m_fco = fcon; // store the normalized frequency
	unit->m_xnm1 = zapgremlins(xnm1);
	unit->m_y1nm1 = zapgremlins(y1nm1);
	unit->m_y2nm1 = zapgremlins(y2nm1);
	unit->m_y3nm1 = zapgremlins(y3nm1);
	unit->m_y1n = zapgremlins(y1n);
	unit->m_y2n = zapgremlins(y2n);
	unit->m_y3n = zapgremlins(y3n);
	unit->m_y4n = zapgremlins(y4n);
}

void MoogVCF_next_ak(MoogVCF *unit, int inNumSamples){
	float* in = IN(0);
	float* out = OUT(0);
	float* fco = IN(1);
	float nextres = IN0(2);
	float res = unit->m_res;
	float fcon = 2.0 * SAMPLEDUR;   // multiplier for filt freq, normalized to 0 to Nyquist

	float resslope = CALCSLOPE(nextres, res);

	float xnm1 = unit->m_xnm1;
	float y1nm1 = unit->m_y1nm1;
	float y2nm1 = unit->m_y2nm1;
	float y3nm1 = unit->m_y3nm1;
	float y1n = unit->m_y1n;
	float y2n = unit->m_y2n;
	float y3n = unit->m_y3n;
	float y4n = unit->m_y4n;

	for (int i = 0; i < inNumSamples; i++) {
		float thisfco = fco[i] * fcon;
		float kp, pp1d2, xn, k;
		MoogVCF_calc_parameters(thisfco, res, kp, pp1d2, k);
		xn = in[i]; // make this similar to the CSound stuff for now... easier translation
		xn = xn - (k * y4n); /* Inverted feed back for corner peaking */

		/* Four cascaded onepole filters (bilinear transform) */
		y1n   = (xn  * pp1d2) + (xnm1  * pp1d2) - (kp * y1n);
		y2n   = (y1n * pp1d2) + (y1nm1 * pp1d2) - (kp * y2n);
		y3n   = (y2n * pp1d2) + (y2nm1 * pp1d2) - (kp * y3n);
		y4n   = (y3n * pp1d2) + (y3nm1 * pp1d2) - (kp * y4n);
		y4n   = MoogVCF_blowup_prevention(y4n);
		xnm1  = xn;       /* Update Xn-1  */
		y1nm1 = y1n;      /* Update Y1n-1 */
		y2nm1 = y2n;      /* Update Y2n-1 */
		y3nm1 = y3n;      /* Update Y3n-1 */
		out[i] = y4n;
		res += resslope;
	}

	unit->m_res = nextres;
	unit->m_xnm1 = zapgremlins(xnm1);
	unit->m_y1nm1 = zapgremlins(y1nm1);
	unit->m_y2nm1 = zapgremlins(y2nm1);
	unit->m_y3nm1 = zapgremlins(y3nm1);
	unit->m_y1n = zapgremlins(y1n);
	unit->m_y2n = zapgremlins(y2n);
	unit->m_y3n = zapgremlins(y3n);
	unit->m_y4n = zapgremlins(y4n);
}

void MoogVCF_next_aa(MoogVCF *unit, int inNumSamples){
	float* in = IN(0);
	float* out = OUT(0);
	float* fco = IN(1);
	float* res = IN(2);
	float fcon = 2.0 * SAMPLEDUR;   // multiplier for filt freq, normalized to 0 to Nyquist

	float xnm1 = unit->m_xnm1;
	float y1nm1 = unit->m_y1nm1;
	float y2nm1 = unit->m_y2nm1;
	float y3nm1 = unit->m_y3nm1;
	float y1n = unit->m_y1n;
	float y2n = unit->m_y2n;
	float y3n = unit->m_y3n;
	float y4n = unit->m_y4n;

	for (int i = 0; i < inNumSamples; i++) {
		float thisfco = fco[i] * fcon;
		float kp, pp1d2, xn, k;
		MoogVCF_calc_parameters(thisfco, res[i], kp, pp1d2, k);

		xn = in[i]; // make this similar to the CSound stuff for now... easier translation
		xn = xn - (k * y4n); /* Inverted feed back for corner peaking */

		/* Four cascaded onepole filters (bilinear transform) */
		y1n   = (xn  * pp1d2) + (xnm1  * pp1d2) - (kp * y1n);
		y2n   = (y1n * pp1d2) + (y1nm1 * pp1d2) - (kp * y2n);
		y3n   = (y2n * pp1d2) + (y2nm1 * pp1d2) - (kp * y3n);
		y4n   = (y3n * pp1d2) + (y3nm1 * pp1d2) - (kp * y4n);
		y4n   = MoogVCF_blowup_prevention(y4n);

		xnm1  = xn;       /* Update Xn-1  */
		y1nm1 = y1n;      /* Update Y1n-1 */
		y2nm1 = y2n;      /* Update Y2n-1 */
		y3nm1 = y3n;      /* Update Y3n-1 */
		out[i] = y4n;
	}

	unit->m_xnm1 = zapgremlins(xnm1);
	unit->m_y1nm1 = zapgremlins(y1nm1);
	unit->m_y2nm1 = zapgremlins(y2nm1);
	unit->m_y3nm1 = zapgremlins(y3nm1);
	unit->m_y1n = zapgremlins(y1n);
	unit->m_y2n = zapgremlins(y2n);
	unit->m_y3n = zapgremlins(y3n);
	unit->m_y4n = zapgremlins(y4n);
}



void PosRatio_Ctor(PosRatio* unit){
    float period = unit->period = IN0(1);
	unit->maxsamples = (int)(SAMPLERATE / period);
    float firstsample = unit->lastsample = IN0(0);
    SETCALC(PosRatio_next);
    if(firstsample > 0.f) unit->posvals = 1; else unit->posvals = 0;
	ClearUnitOutputs(unit, 1);
}



void PosRatio_next(PosRatio *unit, int inNumSamples){
    float *in = IN(0);
    float *out = OUT(0);
	float period = IN0(1); // period can only change AFTER the previous period is complete
	float thresh = IN0(2);
	float lastperiod = unit->lastperiod;
	int maxsamples = unit->maxsamples;
	 int posvals = unit->posvals;
	int numvals = unit->numvals;
	int testposvals = unit->testposvals;
	int testnumvals = unit->testnumvals;
	float ratio = unit->ratio;
	float testratio = unit->testratio;
	float outval = unit->outval;
	float lastsample = unit->lastsample;
	float cursample;
	int i;


	// loop for inNumSamples
	// check for zero crossings...  and for a given period of pos values.
	// if there is a zero crossing, calc new ratio and IF it is within
	// given threshold, output that value
	// Forget interpolation for now between ratios

	for(i = 0; i < inNumSamples; i++){
		cursample = in[i];
		if(cursample > 0.) {posvals++; testposvals++;}
		numvals++;
		testnumvals++;
		// adjust the test values... if new ones are needed, also adjust period and maxsamples if needed
		if(testnumvals == maxsamples){
			testratio = testposvals / testnumvals;
			testposvals = testnumvals = 0;
			if(lastperiod != period){unit->maxsamples = maxsamples = (int)(SAMPLERATE/period); unit->lastperiod = period;}
			}
		// check for a zero crossing OR if maxvals has been reached
		if(((lastsample <= 0.) && (cursample > 0.f)) || (numvals == maxsamples)){
			ratio = (float)posvals / (float)numvals;
			posvals = numvals = 0; // zero out the bookkeeping
			if((testratio * (1. + thresh) > ratio) || ((testratio * (1. - thresh)) < ratio))
				outval = ratio;
				else
				outval = testratio;
			}

		lastsample = cursample;
		out[i] = outval;
	}
		unit->lastsample = lastsample;
		unit->posvals = posvals;
		unit->numvals = numvals;
		unit->testposvals = testposvals;
		unit->testnumvals = testnumvals;
		unit->ratio = ratio;
		unit->testratio = testratio;
		unit->outval = outval;
	}


inline float CalcFeedback(float delaytime, float decaytime)
{
	if (delaytime == 0.f) {
		return 0.f;
	} else if (decaytime > 0.f) {
		return exp(log001 * delaytime / decaytime);
	} else if (decaytime < 0.f) {
		return -exp(log001 * delaytime / -decaytime);
	} else {
		return 0.f;
	}
}

void FeedbackDelay_Reset(FeedbackDelay *unit)
{
	unit->m_decaytime = ZIN0(4);

	DelayUnit_Reset(unit);

	unit->m_feedbk = CalcFeedback(unit->m_delaytime, unit->m_decaytime);
}

/* CombLP... gated input to delay line */

void CombLP_Ctor(CombLP *unit)
{
	FeedbackDelay_Reset(unit);
	if (INRATE(1) == calc_FullRate) {
	    if(INRATE(5) == calc_FullRate){
		SETCALC(CombLP_next_aa_z);
		} else {
		SETCALC(CombLP_next_ak_z); //ak
		}
	    } else {
	    if(INRATE(5) == calc_FullRate){
		SETCALC(CombLP_next_ka_z); //ka
		} else {
		SETCALC(CombLP_next_kk_z); //kk
		}
	    }
	OUT0(0) = unit->m_lastsamp = 0.f;
	unit->m_prevtrig = 0.f;
	unit->m_coef = IN0(5);
}

void CombLP_next_aa(CombLP *unit, int inNumSamples)
{
	float *out = OUT(0);
	float *in = IN(0);
	float *gate = IN(1);
	float delaytime = IN0(3);
	float decaytime = IN0(4);
	float *coef = IN(5);
	float lastsamp = unit->m_lastsamp;

	float *dlybuf = unit->m_dlybuf;
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	float feedbk = unit->m_feedbk;
	long mask = unit->m_mask;
	float thisin;

	if (delaytime == unit->m_delaytime && decaytime == unit->m_decaytime) {
		long idsamp = (long)dsamp;
		float frac = dsamp - idsamp;
	for(int i = 0; i < inNumSamples; i++){
			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;
			thisin = in[i] * gate[i];
			float d0 = dlybuf[irdphase0 & mask];
			float d1 = dlybuf[irdphase1 & mask];
			float d2 = dlybuf[irdphase2 & mask];
			float d3 = dlybuf[irdphase3 & mask];
			float value = cubicinterp(frac, d0, d1, d2, d3);
			float thiscoef = coef[i];
			float onepole = ((1.f - std::fabs(thiscoef)) * value) + (thiscoef * lastsamp);
			dlybuf[iwrphase & mask] = thisin + feedbk * onepole;
			out[i] = lastsamp = onepole;
			iwrphase++;
		};
	} else {

		float next_dsamp = CalcDelay(unit, delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);

		float next_feedbk = CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);

	    for(int i = 0; i < inNumSamples; i++){
			dsamp += dsamp_slope;
			long idsamp = (long)dsamp;
			float frac = dsamp - idsamp;
			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;
			thisin = in[i] * gate[i];
			float d0 = dlybuf[irdphase0 & mask];
			float d1 = dlybuf[irdphase1 & mask];
			float d2 = dlybuf[irdphase2 & mask];
			float d3 = dlybuf[irdphase3 & mask];
			float value = cubicinterp(frac, d0, d1, d2, d3);
			float thiscoef = coef[i];
			float onepole = ((1.f - std::fabs(thiscoef)) * value) + (thiscoef * lastsamp);
			dlybuf[iwrphase & mask] = thisin + feedbk * onepole;
			out[i] = lastsamp = onepole;
			feedbk += feedbk_slope;
			iwrphase++;
		};
		unit->m_feedbk = feedbk;
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
		unit->m_decaytime = decaytime;
	}

	unit->m_lastsamp = zapgremlins(lastsamp);
	unit->m_iwrphase = iwrphase;

}


void CombLP_next_aa_z(CombLP *unit, int inNumSamples)
{
	float *out = OUT(0);
	float *in = IN(0);
	float *gate = IN(1);
	float delaytime = IN0(3);
	float decaytime = IN0(4);
	float *coef = IN(5);
	float lastsamp = unit->m_lastsamp;

	float *dlybuf = unit->m_dlybuf;
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	float feedbk = unit->m_feedbk;
	long mask = unit->m_mask;
	float d0, d1, d2, d3;
	float thisin;

	if (delaytime == unit->m_delaytime && decaytime == unit->m_decaytime) {
		long idsamp = (long)dsamp;
		float frac = dsamp - idsamp;
		for(int i = 0; i < inNumSamples; i++) {
			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;
			thisin = in[i] * gate[i];
			if (irdphase0 < 0) {
				dlybuf[iwrphase & mask] = thisin;
				out[i] = 0.f;
			} else {
				if (irdphase1 < 0) {
					d1 = d2 = d3 = 0.f;
					d0 = dlybuf[irdphase0 & mask];
				} else if (irdphase2 < 0) {
					d1 = d2 = d3 = 0.f;
					d0 = dlybuf[irdphase0 & mask];
					d1 = dlybuf[irdphase1 & mask];
				} else if (irdphase3 < 0) {
					d3 = 0.f;
					d0 = dlybuf[irdphase0 & mask];
					d1 = dlybuf[irdphase1 & mask];
					d2 = dlybuf[irdphase2 & mask];
				} else {
					d0 = dlybuf[irdphase0 & mask];
					d1 = dlybuf[irdphase1 & mask];
					d2 = dlybuf[irdphase2 & mask];
					d3 = dlybuf[irdphase3 & mask];
				}
				float value = cubicinterp(frac, d0, d1, d2, d3);
			float thiscoef = coef[i];
			float onepole = ((1.f - std::fabs(thiscoef)) * value) + (thiscoef * lastsamp);
			dlybuf[iwrphase & mask] = thisin + feedbk * onepole;
			out[i] = lastsamp = onepole;
			}
			iwrphase++;
		};
	} else {

		float next_dsamp = CalcDelay(unit, delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);

		float next_feedbk = CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);

		for(int i = 0; i < inNumSamples; i++) {
			dsamp += dsamp_slope;
			long idsamp = (long)dsamp;
			float frac = dsamp - idsamp;
			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;
			thisin = in[i] * gate[i];
			if (irdphase0 < 0) {
				dlybuf[iwrphase & mask] = thisin;
				out[i] = 0.f;
			    } else {
				if (irdphase1 < 0) {
					d1 = d2 = d3 = 0.f;
					d0 = dlybuf[irdphase0 & mask];
				} else if (irdphase2 < 0) {
					d1 = d2 = d3 = 0.f;
					d0 = dlybuf[irdphase0 & mask];
					d1 = dlybuf[irdphase1 & mask];
				} else if (irdphase3 < 0) {
					d3 = 0.f;
					d0 = dlybuf[irdphase0 & mask];
					d1 = dlybuf[irdphase1 & mask];
					d2 = dlybuf[irdphase2 & mask];
				} else {
					d0 = dlybuf[irdphase0 & mask];
					d1 = dlybuf[irdphase1 & mask];
					d2 = dlybuf[irdphase2 & mask];
					d3 = dlybuf[irdphase3 & mask];
				}
				float value = cubicinterp(frac, d0, d1, d2, d3);
			float thiscoef = coef[i];
			float onepole = ((1.f - std::fabs(thiscoef)) * value) + (thiscoef * lastsamp);
			dlybuf[iwrphase & mask] = thisin + feedbk * onepole;
			out[i] = lastsamp = onepole;
			}
			feedbk += feedbk_slope;
			iwrphase++;
		};
		unit->m_feedbk = feedbk;
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
		unit->m_decaytime = decaytime;
	}

	unit->m_lastsamp = zapgremlins(lastsamp);
	unit->m_iwrphase = iwrphase;

	unit->m_numoutput += inNumSamples;
	if (unit->m_numoutput >= unit->m_idelaylen) {
		SETCALC(CombLP_next_aa);
	}
}

void CombLP_next_kk(CombLP *unit, int inNumSamples)
{
	float *out = OUT(0);
	float *in = IN(0);
	float gate = IN0(1);
	float delaytime = IN0(3);
	float decaytime = IN0(4);
	float coef = IN0(5);
	float lastsamp = unit->m_lastsamp;

	float *dlybuf = unit->m_dlybuf;
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	float feedbk = unit->m_feedbk;
	long mask = unit->m_mask;
	float thisin;

	if (delaytime == unit->m_delaytime && decaytime == unit->m_decaytime && coef == unit->m_coef) {
		long idsamp = (long)dsamp;
		float frac = dsamp - idsamp;

	    for(int i = 0; i < inNumSamples; i++){
			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;
			thisin = in[i] * gate;
			float d0 = dlybuf[irdphase0 & mask];
			float d1 = dlybuf[irdphase1 & mask];
			float d2 = dlybuf[irdphase2 & mask];
			float d3 = dlybuf[irdphase3 & mask];
			float value = cubicinterp(frac, d0, d1, d2, d3);
			float onepole = ((1.f - std::fabs(coef)) * value) + (coef * lastsamp);
			dlybuf[iwrphase & mask] = thisin + (feedbk * onepole);
			out[i] = lastsamp = onepole; //value;
			iwrphase++;
		};
	} else {

		float next_dsamp = CalcDelay(unit, delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);

		float next_feedbk = CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);

		float curcoef = unit->m_coef;
		float coef_slope = CALCSLOPE(coef, curcoef);

	    for(int i = 0; i < inNumSamples; i++){
			dsamp += dsamp_slope;
			long idsamp = (long)dsamp;
			float frac = dsamp - idsamp;
			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;
			thisin = in[i] * gate;

			float d0 = dlybuf[irdphase0 & mask];
			float d1 = dlybuf[irdphase1 & mask];
			float d2 = dlybuf[irdphase2 & mask];
			float d3 = dlybuf[irdphase3 & mask];
			float value = cubicinterp(frac, d0, d1, d2, d3);
			float onepole = ((1.f - std::fabs(curcoef)) * value) + (curcoef * lastsamp);
			dlybuf[iwrphase & mask] = thisin + (feedbk * onepole);
			out[i] = lastsamp = onepole; //value;
			feedbk += feedbk_slope;
			curcoef += coef_slope;
			iwrphase++;
		};
		unit->m_feedbk = feedbk;
		unit->m_coef = coef;
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
		unit->m_decaytime = decaytime;
	}

	unit->m_lastsamp = zapgremlins(lastsamp);
	unit->m_iwrphase = iwrphase;

}


void CombLP_next_kk_z(CombLP *unit, int inNumSamples)
{
	float *out = OUT(0);
	float *in = IN(0);
	float gate = IN0(1);
	float delaytime = IN0(3);
	float decaytime = IN0(4);
	float coef = IN0(5);
	float lastsamp = unit->m_lastsamp;

	float *dlybuf = unit->m_dlybuf;
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	float feedbk = unit->m_feedbk;
	long mask = unit->m_mask;
	float d0, d1, d2, d3;
	float thisin;

	if (delaytime == unit->m_delaytime && decaytime == unit->m_decaytime && coef == unit->m_coef) {
		long idsamp = (long)dsamp;
		float frac = dsamp - idsamp;
		for(int i = 0; i < inNumSamples; i++){

			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;
			thisin = in[i] * gate;
			if (irdphase0 < 0) {
				dlybuf[iwrphase & mask] = thisin;
				out[i] = 0.f;
			} else {
				if (irdphase1 < 0) {
					d1 = d2 = d3 = 0.f;
					d0 = dlybuf[irdphase0 & mask];
				} else if (irdphase2 < 0) {
					d1 = d2 = d3 = 0.f;
					d0 = dlybuf[irdphase0 & mask];
					d1 = dlybuf[irdphase1 & mask];
				} else if (irdphase3 < 0) {
					d3 = 0.f;
					d0 = dlybuf[irdphase0 & mask];
					d1 = dlybuf[irdphase1 & mask];
					d2 = dlybuf[irdphase2 & mask];
				} else {
					d0 = dlybuf[irdphase0 & mask];
					d1 = dlybuf[irdphase1 & mask];
					d2 = dlybuf[irdphase2 & mask];
					d3 = dlybuf[irdphase3 & mask];
				}
				float value = cubicinterp(frac, d0, d1, d2, d3);
			float onepole = ((1.f - std::fabs(coef)) * value) + (coef * lastsamp);
			dlybuf[iwrphase & mask] = thisin + (feedbk * onepole);
			out[i] = lastsamp = onepole; //value;
			}
			iwrphase++;
		};
	} else {

		float next_dsamp = CalcDelay(unit, delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);

		float next_feedbk = CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);

		float curcoef = unit->m_coef;
		float coef_slope = CALCSLOPE(coef, curcoef);

		for(int i = 0; i < inNumSamples; i++) {
			dsamp += dsamp_slope;
			long idsamp = (long)dsamp;
			float frac = dsamp - idsamp;
			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;
			thisin = in[i] * gate;
			if (irdphase0 < 0) {
				dlybuf[iwrphase & mask] = thisin;
				out[i] = 0.f;
			    } else {
				if (irdphase1 < 0) {
					d1 = d2 = d3 = 0.f;
					d0 = dlybuf[irdphase0 & mask];
				} else if (irdphase2 < 0) {
					d1 = d2 = d3 = 0.f;
					d0 = dlybuf[irdphase0 & mask];
					d1 = dlybuf[irdphase1 & mask];
				} else if (irdphase3 < 0) {
					d3 = 0.f;
					d0 = dlybuf[irdphase0 & mask];
					d1 = dlybuf[irdphase1 & mask];
					d2 = dlybuf[irdphase2 & mask];
				} else {
					d0 = dlybuf[irdphase0 & mask];
					d1 = dlybuf[irdphase1 & mask];
					d2 = dlybuf[irdphase2 & mask];
					d3 = dlybuf[irdphase3 & mask];
				}
			float value = cubicinterp(frac, d0, d1, d2, d3);
			float onepole = ((1.f - std::fabs(curcoef)) * value) + (curcoef * lastsamp);
			dlybuf[iwrphase & mask] = thisin + (feedbk * onepole);
			out[i] = lastsamp = onepole; //value;
			}
			feedbk += feedbk_slope;
			curcoef += coef_slope;
			iwrphase++;
		};
		unit->m_feedbk = feedbk;
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
		unit->m_decaytime = decaytime;
		unit->m_coef = coef;
	}

	unit->m_lastsamp = zapgremlins(lastsamp);
	unit->m_iwrphase = iwrphase;

	unit->m_numoutput += inNumSamples;
	if (unit->m_numoutput >= unit->m_idelaylen) {
		SETCALC(CombLP_next_kk);
	}
}

void CombLP_next_ak(CombLP *unit, int inNumSamples)
{
	float *out = OUT(0);
	float *in = IN(0);
	float *gate = IN(1);
	float delaytime = IN0(3);
	float decaytime = IN0(4);
	float coef = IN0(5);
	float lastsamp = unit->m_lastsamp;

	float *dlybuf = unit->m_dlybuf;
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	float feedbk = unit->m_feedbk;
	long mask = unit->m_mask;
	float thisin;

	if (delaytime == unit->m_delaytime && decaytime == unit->m_decaytime) {
		long idsamp = (long)dsamp;
		float frac = dsamp - idsamp;
	for(int i = 0; i < inNumSamples; i++){
			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;
			thisin = in[i] * gate[i];
			float d0 = dlybuf[irdphase0 & mask];
			float d1 = dlybuf[irdphase1 & mask];
			float d2 = dlybuf[irdphase2 & mask];
			float d3 = dlybuf[irdphase3 & mask];
			float value = cubicinterp(frac, d0, d1, d2, d3);
			float onepole = ((1.f - std::fabs(coef)) * value) + (coef * lastsamp);
			dlybuf[iwrphase & mask] = thisin + feedbk * onepole;
			out[i] = lastsamp = onepole;
			iwrphase++;
		};
	} else {

		float next_dsamp = CalcDelay(unit, delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);

		float next_feedbk = CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);

		float curcoef = unit->m_coef;
		float coef_slope = CALCSLOPE(coef, curcoef);

	    for(int i = 0; i < inNumSamples; i++){
			dsamp += dsamp_slope;
			long idsamp = (long)dsamp;
			float frac = dsamp - idsamp;
			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;
			thisin = in[i] * gate[i];
			float d0 = dlybuf[irdphase0 & mask];
			float d1 = dlybuf[irdphase1 & mask];
			float d2 = dlybuf[irdphase2 & mask];
			float d3 = dlybuf[irdphase3 & mask];
			float value = cubicinterp(frac, d0, d1, d2, d3);
			float onepole = ((1.f - std::fabs(curcoef)) * value) + (curcoef * lastsamp);
			dlybuf[iwrphase & mask] = thisin + feedbk * onepole;
			out[i] = lastsamp = onepole;
			feedbk += feedbk_slope;
			curcoef += coef_slope;
			iwrphase++;
		};
		unit->m_feedbk = feedbk;
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
		unit->m_decaytime = decaytime;
		unit->m_coef = coef;
	}

	unit->m_lastsamp = zapgremlins(lastsamp);
	unit->m_iwrphase = iwrphase;

}


void CombLP_next_ak_z(CombLP *unit, int inNumSamples)
{
	float *out = OUT(0);
	float *in = IN(0);
	float *gate = IN(1);
	float delaytime = IN0(3);
	float decaytime = IN0(4);
	float coef = IN0(5);
	float lastsamp = unit->m_lastsamp;

	float *dlybuf = unit->m_dlybuf;
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	float feedbk = unit->m_feedbk;
	long mask = unit->m_mask;
	float d0, d1, d2, d3;
	float thisin;

	if (delaytime == unit->m_delaytime && decaytime == unit->m_decaytime && coef == unit->m_coef) {
		long idsamp = (long)dsamp;
		float frac = dsamp - idsamp;
		for(int i = 0; i < inNumSamples; i++){
			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;
			thisin = in[i] * gate[i];
			if (irdphase0 < 0) {
				dlybuf[iwrphase & mask] = thisin;
				out[i] = 0.f;
			} else {
				if (irdphase1 < 0) {
					d1 = d2 = d3 = 0.f;
					d0 = dlybuf[irdphase0 & mask];
				} else if (irdphase2 < 0) {
					d1 = d2 = d3 = 0.f;
					d0 = dlybuf[irdphase0 & mask];
					d1 = dlybuf[irdphase1 & mask];
				} else if (irdphase3 < 0) {
					d3 = 0.f;
					d0 = dlybuf[irdphase0 & mask];
					d1 = dlybuf[irdphase1 & mask];
					d2 = dlybuf[irdphase2 & mask];
				} else {
					d0 = dlybuf[irdphase0 & mask];
					d1 = dlybuf[irdphase1 & mask];
					d2 = dlybuf[irdphase2 & mask];
					d3 = dlybuf[irdphase3 & mask];
				}
				float value = cubicinterp(frac, d0, d1, d2, d3);
			float onepole = ((1.f - std::fabs(coef)) * value) + (coef * lastsamp);
			dlybuf[iwrphase & mask] = thisin + feedbk * onepole;
			out[i] = lastsamp = onepole;
			}
			iwrphase++;
		};
	} else {

		float next_dsamp = CalcDelay(unit, delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);

		float next_feedbk = CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);

		float curcoef = unit->m_coef;
		float coef_slope = CALCSLOPE(coef, curcoef);

		for(int i = 0; i < inNumSamples; i++) {
			dsamp += dsamp_slope;
			long idsamp = (long)dsamp;
			float frac = dsamp - idsamp;
			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;
			thisin = in[i] * gate[i];
			if (irdphase0 < 0) {
				dlybuf[iwrphase & mask] = thisin;
				out[i] = 0.f;
			    } else {
				if (irdphase1 < 0) {
					d1 = d2 = d3 = 0.f;
					d0 = dlybuf[irdphase0 & mask];
				} else if (irdphase2 < 0) {
					d1 = d2 = d3 = 0.f;
					d0 = dlybuf[irdphase0 & mask];
					d1 = dlybuf[irdphase1 & mask];
				} else if (irdphase3 < 0) {
					d3 = 0.f;
					d0 = dlybuf[irdphase0 & mask];
					d1 = dlybuf[irdphase1 & mask];
					d2 = dlybuf[irdphase2 & mask];
				} else {
					d0 = dlybuf[irdphase0 & mask];
					d1 = dlybuf[irdphase1 & mask];
					d2 = dlybuf[irdphase2 & mask];
					d3 = dlybuf[irdphase3 & mask];
				}
				float value = cubicinterp(frac, d0, d1, d2, d3);
			float onepole = ((1.f - std::fabs(curcoef)) * value) + (curcoef * lastsamp);
			dlybuf[iwrphase & mask] = thisin + feedbk * onepole;
			out[i] = lastsamp = onepole;
			}
			feedbk += feedbk_slope;
			curcoef +=coef_slope;
			iwrphase++;
		};
		unit->m_feedbk = feedbk;
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
		unit->m_decaytime = decaytime;
		unit->m_coef = coef;
	}

	unit->m_lastsamp = zapgremlins(lastsamp);
	unit->m_iwrphase = iwrphase;

	unit->m_numoutput += inNumSamples;
	if (unit->m_numoutput >= unit->m_idelaylen) {
		SETCALC(CombLP_next_ak);
	}
}


void CombLP_next_ka(CombLP *unit, int inNumSamples)
{
	float *out = OUT(0);
	float *in = IN(0);
	float gate = IN0(1);
	float delaytime = IN0(3);
	float decaytime = IN0(4);
	float *coef = IN(5);
	float lastsamp = unit->m_lastsamp;

	float *dlybuf = unit->m_dlybuf;
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	float feedbk = unit->m_feedbk;
	long mask = unit->m_mask;
	float thisin;


	if (delaytime == unit->m_delaytime && decaytime == unit->m_decaytime) {
		long idsamp = (long)dsamp;
		float frac = dsamp - idsamp;
	for(int i = 0; i < inNumSamples; i++){
			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;
			thisin = in[i] * gate;
			float d0 = dlybuf[irdphase0 & mask];
			float d1 = dlybuf[irdphase1 & mask];
			float d2 = dlybuf[irdphase2 & mask];
			float d3 = dlybuf[irdphase3 & mask];
			float value = cubicinterp(frac, d0, d1, d2, d3);
			float thiscoef = coef[i];
			float onepole = ((1.f - std::fabs(thiscoef)) * value) + (thiscoef * lastsamp);
			dlybuf[iwrphase & mask] = thisin + feedbk * onepole;
			out[i] = lastsamp = onepole;
			iwrphase++;
		};
	} else {

		float next_dsamp = CalcDelay(unit, delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);

		float next_feedbk = CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);

	    for(int i = 0; i < inNumSamples; i++){
			dsamp += dsamp_slope;
			long idsamp = (long)dsamp;
			float frac = dsamp - idsamp;
			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;
			thisin = in[i] * gate;
			float d0 = dlybuf[irdphase0 & mask];
			float d1 = dlybuf[irdphase1 & mask];
			float d2 = dlybuf[irdphase2 & mask];
			float d3 = dlybuf[irdphase3 & mask];
			float value = cubicinterp(frac, d0, d1, d2, d3);
			float thiscoef = coef[i];
			float onepole = ((1.f - std::fabs(thiscoef)) * value) + (thiscoef * lastsamp);
			dlybuf[iwrphase & mask] = thisin + feedbk * onepole;
			out[i] = lastsamp = onepole;
			feedbk += feedbk_slope;
			iwrphase++;
		};
		unit->m_feedbk = feedbk;
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
		unit->m_decaytime = decaytime;
	}

	unit->m_lastsamp = zapgremlins(lastsamp);
	unit->m_iwrphase = iwrphase;

}


void CombLP_next_ka_z(CombLP *unit, int inNumSamples)
{
	float *out = OUT(0);
	float *in = IN(0);
	float gate = IN0(1);
	float delaytime = IN0(3);
	float decaytime = IN0(4);
	float *coef = IN(5);
	float lastsamp = unit->m_lastsamp;

	float *dlybuf = unit->m_dlybuf;
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	float feedbk = unit->m_feedbk;
	long mask = unit->m_mask;
	float d0, d1, d2, d3;
	float thisin;

	if (delaytime == unit->m_delaytime && decaytime == unit->m_decaytime) {
		long idsamp = (long)dsamp;
		float frac = dsamp - idsamp;
		for(int i = 0; i < inNumSamples; i++){
			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;
			thisin = in[i] * gate;
			if (irdphase0 < 0) {
				dlybuf[iwrphase & mask] = thisin;
				out[i] = 0.f;
			} else {
				if (irdphase1 < 0) {
					d1 = d2 = d3 = 0.f;
					d0 = dlybuf[irdphase0 & mask];
				} else if (irdphase2 < 0) {
					d1 = d2 = d3 = 0.f;
					d0 = dlybuf[irdphase0 & mask];
					d1 = dlybuf[irdphase1 & mask];
				} else if (irdphase3 < 0) {
					d3 = 0.f;
					d0 = dlybuf[irdphase0 & mask];
					d1 = dlybuf[irdphase1 & mask];
					d2 = dlybuf[irdphase2 & mask];
				} else {
					d0 = dlybuf[irdphase0 & mask];
					d1 = dlybuf[irdphase1 & mask];
					d2 = dlybuf[irdphase2 & mask];
					d3 = dlybuf[irdphase3 & mask];
				}
				float value = cubicinterp(frac, d0, d1, d2, d3);
			float thiscoef = coef[i];
			float onepole = ((1.f - std::fabs(thiscoef)) * value) + (thiscoef * lastsamp);
			dlybuf[iwrphase & mask] = thisin + feedbk * onepole;
			out[i] = lastsamp = onepole;
			}
			iwrphase++;
		};
	} else {

		float next_dsamp = CalcDelay(unit, delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);

		float next_feedbk = CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);

		for(int i = 0; i < inNumSamples; i++) {
			dsamp += dsamp_slope;
			long idsamp = (long)dsamp;
			float frac = dsamp - idsamp;
			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;
			thisin = in[i] * gate;
			if (irdphase0 < 0) {
				dlybuf[iwrphase & mask] = thisin;
				out[i] = 0.f;
			    } else {
				if (irdphase1 < 0) {
					d1 = d2 = d3 = 0.f;
					d0 = dlybuf[irdphase0 & mask];
				} else if (irdphase2 < 0) {
					d1 = d2 = d3 = 0.f;
					d0 = dlybuf[irdphase0 & mask];
					d1 = dlybuf[irdphase1 & mask];
				} else if (irdphase3 < 0) {
					d3 = 0.f;
					d0 = dlybuf[irdphase0 & mask];
					d1 = dlybuf[irdphase1 & mask];
					d2 = dlybuf[irdphase2 & mask];
				} else {
					d0 = dlybuf[irdphase0 & mask];
					d1 = dlybuf[irdphase1 & mask];
					d2 = dlybuf[irdphase2 & mask];
					d3 = dlybuf[irdphase3 & mask];
				}
				float value = cubicinterp(frac, d0, d1, d2, d3);
			float thiscoef = coef[i];
			float onepole = ((1.f - std::fabs(thiscoef)) * value) + (thiscoef * lastsamp);
			dlybuf[iwrphase & mask] = thisin + feedbk * onepole;
			out[i] = lastsamp = onepole;
			}
			feedbk += feedbk_slope;
			iwrphase++;
		};
		unit->m_feedbk = feedbk;
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
		unit->m_decaytime = decaytime;
	}

	unit->m_lastsamp = zapgremlins(lastsamp);
	unit->m_iwrphase = iwrphase;

	unit->m_numoutput += inNumSamples;
	if (unit->m_numoutput >= unit->m_idelaylen) {
		SETCALC(CombLP_next_ka);
	}
}

struct PanX : public Unit
{
    float m_chanamp[16];
};

extern "C"
{
    void PanX_next(PanX *unit, int inNumSamples);
    void PanX_Ctor(PanX* unit);
}


void PanX_Ctor(PanX *unit)
{
    int numOutputs = unit->mNumOutputs;
    for (int i=0; i<numOutputs; ++i) {
	unit->m_chanamp[i] = 0;
	ZOUT0(i) = 0.f;
    }
    SETCALC(PanX_next);
}

void PanX_next(PanX *unit, int inNumSamples)
{
    float pos = ZIN0(1); // from zero to 1 * numChans - 1
    float level = ZIN0(2);
    float width = ZIN0(3);
    float width2 = width * 0.5;

    int numOutputs = unit->mNumOutputs;
    pos = pos * ((float)numOutputs - 1.0); // + width;
    float rwidth = 1.f / width;

    float *zin0 = ZIN(0);
    float lower = pos - width2;
    float upper = pos + width2;
    for (int i=0; i<numOutputs; ++i) {
	float *out = ZOUT(i);
	float fi = (float)i;
	float nextchanamp;
	if ((fi <= lower) || (fi >= upper)) {
	    nextchanamp = 0.f;
	} else {
	    float chanpos = (fi - lower) * rwidth;
	    nextchanamp  = level * ft->mSine[(long)(4096.f * chanpos)];
	}
	float chanamp = unit->m_chanamp[i];

	if (nextchanamp == chanamp) {
	    if (nextchanamp == 0.f) {
		ZClear(inNumSamples, out);
	    } else {
		float *in = zin0;
		LOOP(inNumSamples,
		     ZXP(out) = ZXP(in) * chanamp;
		     )
	    }
	} else {
	    float chanampslope  = CALCSLOPE(nextchanamp, chanamp);
	    float *in = zin0;
	    LOOP(inNumSamples,
		 ZXP(out) = ZXP(in) * chanamp;
		 chanamp += chanampslope;
		 )
	    unit->m_chanamp[i] = nextchanamp;
	}
    }
}



#define BUF_GRAIN_LOOP_BODY_4_N \
    phase = sc_gloop(phase, loopMax); \
    int32 iphase = (int32)phase; \
    float* table1 = bufData + iphase * bufChannels; \
    float* table0 = table1 - bufChannels; \
    float* table2 = table1 + bufChannels; \
    float* table3 = table2 + bufChannels; \
    if (iphase == 0) { \
        table0 += bufSamples; \
        } else if (iphase >= guardFrame) { \
            if (iphase == guardFrame) { \
		table3 -= bufSamples; \
	    } else { \
	    table2 -= bufSamples; \
	    table3 -= bufSamples; \
	} \
    } \
    float fracphase = phase - (double)iphase; \
    float a = table0[n]; \
    float b = table1[n]; \
    float c = table2[n]; \
    float d = table3[n]; \
    float outval = amp * cubicinterp(fracphase, a, b, c, d); \
    ZXP(out1) += outval; \

#define BUF_GRAIN_LOOP_BODY_2_N \
    phase = sc_gloop(phase, loopMax); \
    int32 iphase = (int32)phase; \
    float* table1 = bufData + iphase * bufChannels; \
    float* table2 = table1 + bufChannels; \
    if (iphase > guardFrame) { \
	table2 -= bufSamples; \
    } \
    float fracphase = phase - (double)iphase; \
    float b = table1[n]; \
    float c = table2[n]; \
    float outval = amp * (b + fracphase * (c - b)); \
    ZXP(out1) += outval; \

    // amp needs to be calculated by looking up values in window

#define BUF_GRAIN_LOOP_BODY_1_N \
    phase = sc_gloop(phase, loopMax); \
    int32 iphase = (int32)phase; \
    float outval = amp * bufData[iphase + n]; \
    ZXP(out1) += outval; \

void WarpZ_next(WarpZ *unit, int inNumSamples)
{
    ClearUnitOutputs(unit, inNumSamples);

    GET_BUF
    SETUP_OUT
    CHECK_BUF

    World *world = unit->mWorld;
    uint32 numBufs = world->mNumSndBufs;
    double amp = 0.;
    double winPos, winInc, w, b1, y1, y2, y0;
    winPos = winInc = w = b1 = y1 = y2 = y0 = 0.;
    SndBuf *window;
    float *windowData __attribute__((__unused__));
    uint32 windowSamples __attribute__((__unused__)) = 0;
    uint32 windowFrames __attribute__((__unused__)) = 0;
    int windowGuardFrame = 0;

    for (uint32 n=0; n < numOutputs; n++){
	int nextGrain = unit->mNextGrain[n];
	for (int i=0; i < unit->mNumActive[n]; ) {
	    WarpWinGrain *grain = unit->mGrains[n] + i;

	    double loopMax = (double)bufFrames;

	    double rate = grain->rate;
	    double phase = grain->phase;
	    GET_GRAIN_AMP_PARAMS
	    float *out1 = out[n];
	    int nsmps = sc_min(grain->counter, inNumSamples);
	    if (grain->interp >= 4) {
		for (int j=0; j<nsmps; ++j) {
		    BUF_GRAIN_LOOP_BODY_4_N
		    CALC_NEXT_GRAIN_AMP
		    phase += rate;
		}
	    } else if (grain->interp >= 2) {
		for (int j=0; j<nsmps; ++j) {
		    BUF_GRAIN_LOOP_BODY_2_N
		    CALC_NEXT_GRAIN_AMP
		    phase += rate;
		}
	    } else {
		for (int j=0; j<nsmps; ++j) {
		    BUF_GRAIN_LOOP_BODY_1_N
		    CALC_NEXT_GRAIN_AMP
		    phase += rate;
		}
	    }

	    grain->phase = phase;
	    SAVE_GRAIN_AMP_PARAMS
	    if (grain->counter <= 0) {
		// remove grain
		*grain = unit->mGrains[n][--unit->mNumActive[n]];
	    } else ++i;
	}

	for (int i=0; i<inNumSamples; ++i) {
	    --nextGrain;
	    if (nextGrain == 0) {
		// start a grain
		if (unit->mNumActive[n]+1 >= kMaxGrains) break;

		float bufSampleRate = buf->samplerate;
		float bufRateScale = bufSampleRate * SAMPLEDUR;
		double loopMax = (double)bufFrames;

		WarpWinGrain *grain = unit->mGrains[n] + unit->mNumActive[n]++;

		RGET

		float overlaps = GRAIN_IN_AT(unit, 5, i);
		float counter = GRAIN_IN_AT(unit, 3, i) * SAMPLERATE;
		double winrandamt = frand2(s1, s2, s3) * (double)GRAIN_IN_AT(unit, 6, i);
		counter = sc_max(4., floor(counter + (counter * winrandamt)));
		grain->counter = (int)counter;

		nextGrain = (int)(counter / overlaps);

		unit->mNextGrain[n] = nextGrain;

		float rate = grain->rate = GRAIN_IN_AT(unit, 2, i) * bufRateScale;
		float phase = GRAIN_IN_AT(unit, 1, i) * (float)bufFrames;

		float zeroSearch, zeroStart;
		zeroSearch = GRAIN_IN_AT(unit, 8, i);
		zeroStart = GRAIN_IN_AT(unit, 9, i);
		// if we want the contents of the current grain to happen near a zero crossing...
		if(zeroSearch > 0.0) {
		    bool findzero = true;
		    int maxcount = (int)(bufSampleRate * zeroSearch);
		    float *a;
		    float *aTmp;
		    int iTmp = 0;
		    int iLoopMax = (int)loopMax;
		    int iPhase = (int)phase;
		    float a0, a1;
		    while(findzero && (iTmp < maxcount) && ((iPhase + bufChannels) < iLoopMax)){
			a = bufData + iPhase;
			a0 = a[0];
			a1 = a[bufChannels];
			if ((a0 <= 0.0) && (a1 > 0.0)) {
			    float frac = a0 / ((a0 * -1.0) + a1);
			    phase += ((float)iTmp - frac);
			    iPhase = (int32)phase;
			    findzero = false;
			} else {
			    iTmp++;
			    iPhase++;
			}}
		    if(zeroStart > 0.0){
			// now - adjust nextGrain to be near a zero crossing in the current grain
			findzero = true;
			int maxDev = (int)((float)nextGrain * zeroStart);

			int ppos = iPhase + (nextGrain * bufChannels);
			int loopTest = ppos + bufChannels;
			iTmp = 0;
			a = bufData + ppos;
			while (findzero && (iTmp < maxDev) && (loopTest < iLoopMax)){
			    aTmp = a + iTmp;
			    a0 = aTmp[0];
			    a1 = aTmp[bufChannels];
			    // check ahead for a zero crossing in the CURRENT grain...
			    if((a0 <= 0.0) && (a1 > 0.0)){
				nextGrain = unit->mNextGrain[n] += iTmp;
				findzero = false;
			    } else {
				// and behind ...
				aTmp = a - iTmp;
				a0 = aTmp[0];
				a1 = aTmp[bufChannels];
				if((a0 <= 0.0) && (a1 > 0.0)){
				    nextGrain = unit->mNextGrain[n] -= iTmp;
				    findzero = false;
				} else {
				    loopTest += bufChannels;
				    iTmp++;
				}
			    }
			}
		    }
		}

		grain->interp = (int)GRAIN_IN_AT(unit, 7, i);
		float winType = grain->winType = (int)GRAIN_IN_AT(unit, 4, i); // the buffer that holds the grain shape
		GET_GRAIN_WIN
		if((windowData) || (winType < 0.)) {
		    GET_GRAIN_INIT_AMP

		    float *out1 = out[n] + i;

		    int nsmps = sc_min(grain->counter, inNumSamples - i);
		    if (grain->interp >= 4) {
			for (int j=0; j<nsmps; ++j) {
			    BUF_GRAIN_LOOP_BODY_4_N
			    CALC_NEXT_GRAIN_AMP
			    phase += rate;
			}
		    } else if (grain->interp >= 2) {
			for (int j=0; j<nsmps; ++j) {
			    BUF_GRAIN_LOOP_BODY_2_N
			    CALC_NEXT_GRAIN_AMP
			    phase += rate;
			}
		    } else {
			for (int j=0; j<nsmps; ++j) {
			    BUF_GRAIN_LOOP_BODY_1_N
			    CALC_NEXT_GRAIN_AMP
			    phase += rate;
			}
		    }

		    grain->phase = phase;
		    SAVE_GRAIN_AMP_PARAMS
		    // store random values
		    RPUT
		    // end change
		    if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[n][--unit->mNumActive[n]];
		    }
		}
	    }
	}

	unit->mNextGrain[n] = nextGrain;
    }
}

void WarpZ_Ctor(WarpZ *unit)
{
    SETCALC(WarpZ_next);

    for(int i = 0; i < 16; i++){
	unit->mNumActive[i] = 0;
	unit->mNextGrain[i] = 1;
    }

    ClearUnitOutputs(unit, 1);
    unit->m_fbufnum = -1e9f;

}


// Metronome
inline float IN_AT(Unit* unit, int index, int offset)
{
    if (INRATE(index) == calc_FullRate) return IN(index)[offset];
    if (INRATE(index) == calc_DemandRate) return DEMANDINPUT(index);
    return ZIN0(index);
}

struct Metro : public Unit
{
    int m_nextPulse;
};

extern "C"
{
    void Metro_next(Metro *unit, int inNumSamples);
    void Metro_Ctor(Metro* unit);
}


void Metro_Ctor(Metro *unit)
{
    SETCALC(Metro_next);
    unit->m_nextPulse = 0;
    Metro_next(unit, 1);
}

void Metro_next(Metro *unit, int inNumSamples)
{
    float *out = OUT(0);
    int nextPulse = unit->m_nextPulse;
    if(nextPulse > inNumSamples){
	unit->m_nextPulse -= inNumSamples; ClearUnitOutputs(unit, inNumSamples); return;
    } else {
	for(int i = 0; i < inNumSamples; i++){
	    if(nextPulse <= 0){
		out[i] = 1.;
		float bpm = IN_AT(unit, 0, i);
		float numBeats = IN_AT(unit, 1, i);
		if(bpm > 0.0){
		    nextPulse = (int)sc_max(roundf((60.0 / bpm) * numBeats * SAMPLERATE), 2.f);
		} else {
		    unit->mDone = true;
		    return;
		}
	    }
	    nextPulse--;
	}
	unit->m_nextPulse = nextPulse;
    }
}

// TTendency
struct TTendency : public Unit
{
    float m_outVal, m_lastTrig;
};

extern "C"
{
    void TTendency_next_k(TTendency *unit, int inNumSamples);
    void TTendency_next_a(TTendency *unit, int inNumSamples);
    void TTendency_Ctor(TTendency *unit);
}

void TTendency_Ctor(TTendency *unit)
{
    unit->m_outVal = 0.f;
    unit->m_lastTrig = 0.f;
    if(INRATE(0) == calc_FullRate)
	SETCALC(TTendency_next_a);
    else
	SETCALC(TTendency_next_k);
    TTendency_next_k(unit, 1);
}
/*
dist types:
0 = uniform
1 = lowpass
2 = highpass
3 = mean
4 = beta
5 = cauchy
6 = gauss
7 = poisson
8 = expRand
9 = exponential
10 = gamma
11 = laplace
12 = alaplace
13 = hcos
14 = logistic
15 = arcsin
*/

inline void getUniformVal(TTendency *unit, float parX, float parY)
{
    RGen& rgen = *unit->mParent->mRGen;
    unit->m_outVal = (rgen.frand() * (parY - parX)) + parX;
}

inline void getLowpassVal(TTendency *unit, float parX, float parY)
{
    float val1, val2;
    RGen& rgen = *unit->mParent->mRGen;
    val1 = rgen.frand();
    val2 = rgen.frand();
    if(val2 < val1) val1 = val2;
    unit->m_outVal = (val1 * (parY - parX)) + parX;
}

inline void getHighpassVal(TTendency *unit, float parX, float parY)
{
    float val1, val2;
    RGen& rgen = *unit->mParent->mRGen;
    val1 = rgen.frand();
    val2 = rgen.frand();
    if(val2 > val1) val1 = val2;
    unit->m_outVal = (val1 * (parY - parX)) + parX;
}

inline void getMeanVal(TTendency *unit, float parX, float parY)
{
    float val1, val2;
    RGen& rgen = *unit->mParent->mRGen;
    val1 = rgen.frand();
    val2 = rgen.frand();
    val1 = (val1 + val2) * 0.5;
    unit->m_outVal = (val1 * (parY - parX)) + parX;
}

inline void getBetaVal(TTendency *unit, float parX, float parY, float parA, float parB)
{
    float sum, rprob1, rprob2, temp, i;
    RGen& rgen = *unit->mParent->mRGen;
    sum = 2.0;
    i = 0;
    rprob1 = 1.0 / parA;
    rprob2 = 1.0 / parB;
    while((sum > 1.0) && (i < 10))
    {
	temp = powf(rgen.frand(), rprob1);
	sum = temp + powf(rgen.frand(), rprob2);
	i++;
    }
    if(sum < 0.000000001f) sum = 0.000000001f;
    unit->m_outVal = (float)((temp / sum) * (parX - parY)) + parY;
}

inline void getCauchyVal(TTendency *unit, float parX, float parY, float parA)
{
    float u;
    RGen& rgen = *unit->mParent->mRGen;
    u = rgen.frand();
    if(parA > 0.f) u = u * 0.5;
    u = u * PI;
    unit->m_outVal = (parX * tanf(u)) + parY;
}

inline void getGaussVal(TTendency *unit, float parX, float parY)
{
    float val1, val2;
    RGen& rgen = *unit->mParent->mRGen;
    val1 = rgen.frand();
    val2 = rgen.frand();
    unit->m_outVal = (sqrtf(-2.0 * logf(1.0 - val1)) * parX) + parY;
}

inline void getPoissonVal(TTendency *unit, float parX)
{
    float val1, tmp;
    int count;
    RGen& rgen = *unit->mParent->mRGen;
    count = -1;
    val1 = rgen.frand();
    tmp = expf(-parX);
    while(val1 > tmp)
    {
	val1 = val1 * rgen.frand();
	count++;
    }
    unit->m_outVal = count;
}

inline void getExpRandVal(TTendency *unit, float parX, float parY)
{
    float ratio;
    RGen& rgen = *unit->mParent->mRGen;
    ratio = parY / parX;
    unit->m_outVal = powf(ratio, rgen.frand()) * parX;
}

inline void getExponentialVal(TTendency *unit, float parX, float parY)
{
    float xs;
    RGen& rgen = *unit->mParent->mRGen;
    xs = logf(rgen.frand());
    unit->m_outVal = (-xs/parX) + parY;
}

inline void getGammaVal(TTendency *unit, float parX, float parY)
{
    float sum;
    int i;
    RGen& rgen = *unit->mParent->mRGen;
    sum = 1.0;
    for(i = 1; i < (int)parX; i++)
    {
	sum *= (1 - rgen.frand());
    }
    unit->m_outVal = (logf(sum) * -1) + parY;
}

inline void getLaplaceVal(TTendency *unit, float parX)
{
    float u, val;
    RGen& rgen = *unit->mParent->mRGen;
    u = rgen.frand() * 2.0;
    if(u > 1.0)
    {
	u = 2.0 - u;
	val = -parX * logf(u);
    } else {
	val = parX * logf(u);
    }
    unit->m_outVal = val + parX;
}

inline void getAlaplaceVal(TTendency *unit, float parX)
{
    float u, val;
    RGen& rgen = *unit->mParent->mRGen;
    u = rgen.frand() * 2.0;
    if(u > 1.0)
    {
	u = 2.0 - u;
	val = -parX * expf(u);
    } else {
	val = parX * expf(u);
    }
    unit->m_outVal = val + parX;
}

inline void getHcosVal(TTendency *unit, float parX, float parY)
{
    float u, val;
    RGen& rgen = *unit->mParent->mRGen;
    u = rgen.frand();
    val = logf(tanf(0.5 * PI * u));
    unit->m_outVal = (parX * val) + parY;
}

inline void getLogisticVal(TTendency *unit, float parX, float parY)
{
    float u, val;
    RGen& rgen = *unit->mParent->mRGen;
    u = rgen.frand();
    val = logf((1.0 / u) - 1.0);
    unit->m_outVal = (-parX * val) + parY;
}

inline void getArcsinVal(TTendency *unit, float parX, float parY)
{
    float u, val;
    RGen& rgen = *unit->mParent->mRGen;
    u = rgen.frand();
    val = (1.0 -sinf(PI * (u - 0.5))) * 0.5;
    unit->m_outVal = (parX * val) + parY;
}

void TTendency_next_a(TTendency *unit, int inNumSamples)
{
    float dist, parX, parY, parA, parB;
    int iDist;
    float *out = OUT(0);
    float *trigIn = IN(0);
    for(int i = 0; i < inNumSamples; i++)
    {
	if(trigIn[i] > 0.f)
	{
	    if(unit->m_lastTrig <= 0.f)
	    {
		unit->m_lastTrig = trigIn[i];
		// there is a trigger
		dist = IN_AT(unit, 1, i);
		parX = IN_AT(unit, 2, i);
		parY = IN_AT(unit, 3, i);
		parA = IN_AT(unit, 4, i);
		parB = IN_AT(unit, 5, i);
		iDist = (int)floorf(dist);
		switch (iDist)
		{
		    case 0:
			getUniformVal(unit, parX, parY); break;
		    case 1:
			getLowpassVal(unit, parX, parY); break;
		    case 2:
			getHighpassVal(unit, parX, parY); break;
		    case 3:
			getMeanVal(unit, parX, parY); break;
		    case 4:
			getBetaVal(unit, parX, parY, parA, parB); break;
		    case 5:
			getCauchyVal(unit, parX, parY, parA); break;
		    case 6:
			getGaussVal(unit, parX, parY); break;
		    case 7:
			getPoissonVal(unit, parX); break;
		    case 8:
			getExpRandVal(unit, parX, parY); break;
		    case 9:
			getExponentialVal(unit, parX, parY); break;
		    case 10:
			getGammaVal(unit, parX, parY); break;
		    case 11:
			getLaplaceVal(unit, parX); break;
		    case 12:
			getAlaplaceVal(unit, parX); break;
		    case 13:
			getHcosVal(unit, parX, parY); break;
		    case 14:
			getLogisticVal(unit, parX, parY); break;
		    case 15:
			getArcsinVal(unit, parX, parY); break;
		    default:
			getUniformVal(unit, parX, parY); break;
		}
	    }
	} else {
	    if(unit->m_lastTrig > 0.f) unit->m_lastTrig = 0.f;
	}
    out[i] = unit->m_outVal;
    }
}

void TTendency_next_k(TTendency *unit, int inNumSamples)
{
    float dist, parX, parY, parA, parB;
    int iDist;
    float *out = OUT(0);
    float trigIn = IN0(0);
    for(int i = 0; i < inNumSamples; i++)
    {
	if(trigIn > 0.f)
	{
	    if(unit->m_lastTrig <= 0.f)
	    {
		unit->m_lastTrig = trigIn;
		// there is a trigger
		dist = IN0(1);
		parX = IN0(2);
		parY = IN0(3);
		parA = IN0(4);
		parB = IN0(5);
		iDist = (int)floorf(dist);
		switch (iDist)
		{
		    case 0:
			getUniformVal(unit, parX, parY); break;
		    case 1:
			getLowpassVal(unit, parX, parY); break;
		    case 2:
			getHighpassVal(unit, parX, parY); break;
		    case 3:
			getMeanVal(unit, parX, parY); break;
		    case 4:
			getBetaVal(unit, parX, parY, parA, parB); break;
		    case 5:
			getCauchyVal(unit, parX, parY, parA); break;
		    case 6:
			getGaussVal(unit, parX, parY); break;
		    case 7:
			getPoissonVal(unit, parX); break;
		    case 8:
			getExpRandVal(unit, parX, parY); break;
		    case 9:
			getExponentialVal(unit, parX, parY); break;
		    case 10:
			getGammaVal(unit, parX, parY); break;
		    case 11:
			getLaplaceVal(unit, parX); break;
		    case 12:
			getAlaplaceVal(unit, parX); break;
		    case 13:
			getHcosVal(unit, parX, parY); break;
		    case 14:
			getLogisticVal(unit, parX, parY); break;
		    case 15:
			getArcsinVal(unit, parX, parY); break;
		    default:
			getUniformVal(unit, parX, parY); break;
		}
	    }
	} else {
	    if(unit->m_lastTrig > 0.f) unit->m_lastTrig = 0.f;
	}
	out[i] = unit->m_outVal;
    }
}

/* new ATS UGens */

#define POPULATE_ATS_DATA \
    float fbufnum = IN0(0); \
    if (fbufnum != unit->m_fbufnum) { \
    uint32 bufnum = (int)fbufnum; \
    World *world = unit->mWorld; \
    if (bufnum >= world->mNumSndBufs) bufnum = 0; \
	unit->m_fbufnum = fbufnum; \
	unit->m_atsFile = world->mSndBufs + bufnum; \
	} \
    SndBuf *atsFile = unit->m_atsFile; \
    float *atsFileData __attribute__((__unused__)) = atsFile->data; \
    if (!atsFileData) { \
	unit->mDone = true; \
	return; \
    }; \
    unit->m_sr = (int)atsFileData[1]; \
    unit->m_frameSize = (int)atsFileData[2]; \
    unit->m_winSize = (int)atsFileData[3]; \
    unit->m_numPartials = (int)atsFileData[4]; \
    unit->m_numFrames = (int)atsFileData[5]; \
    unit->m_ampMax = atsFileData[6]; \
    unit->m_freqMax = atsFileData[7]; \
    unit->m_sndDur = atsFileData[8]; \
    unit->m_atsType = atsFileData[9]; \
    float* atsData = atsFileData + 11; \
    if(( unit->m_atsType == 1 ) || (unit->m_atsType == 3)) { \
	unit->m_offset1 = 2; \
	} else { \
	unit->m_offset1 = 3; \
	} \
    if( unit->m_atsType < 3 ){ \
	unit->m_offset2 = 1; \
	} else { \
	unit->m_offset2 = 26; \
    } \
    unit->m_increment = unit->m_numPartials * unit->m_offset1 * unit->m_offset2; \
    unit->m_init = 1.;





void ATSSynth_Ctor(ATSSynth* unit)
{
    SETCALC(ATSSynth_next);
    unit->m_fbufnum = -1e9f;
    unit->m_init = -1.f;
    ClearUnitOutputs(unit, 1);
}

void ATSSynth_Dtor(ATSSynth* unit)
{
    RTFree(unit->mWorld, unit->m_partials);
}

// ATSSynth.ar(buffer, pointer, numPartials, partialStart, partialSkip, freqMul, freqAdd)

void ATSSynth_next(ATSSynth* unit, int inNumSamples)
{

    if(unit->m_init < 0.0){

        POPULATE_ATS_DATA


	unit->m_lastpointer = IN0(1);
        int32 tableSize = unit->m_tableSize = ft->mSineSize;
        int32 lomask = unit->m_lomask = (tableSize - 1) << 3;
        float fTableSize = (float)tableSize;
        double radtoinc = unit->m_radtoinc = fTableSize * rtwopi * 65536.;
        double cpstoinc = unit->m_cpstoinc = fTableSize * SAMPLEDUR * 65536.;

        int numPartialsRequested = (int)IN0(2);
        int partialStart = (int)IN0(3);
        int partialSkip = (int)IN0(4);

        int totalPartials = numPartialsRequested;
        for(int i = 0; i < numPartialsRequested; i++){
            if((partialStart + (partialSkip * i)) >= unit->m_numPartials)
                totalPartials--;
	}

        unit->m_partials = (ATSPartial*)RTAlloc(unit->mWorld, totalPartials * sizeof(ATSPartial));

        for(int i = 0; i < totalPartials; i++){
            unit->m_partials[i].m_partialNum = (partialStart + (partialSkip * i));
            unit->m_partials[i].m_lastamp = 0.0; // grab actual data
	    unit->m_partials[i].m_lastfreq = 0.0; // grab actual data

            unit->m_partials[i].m_partial.m_phase = 0;
            unit->m_partials[i].m_partial.m_phasein = 0;
        }
    }
}

/////////////// TALReverb //////////////////////
// void TALReverb_next(TALReverb *unit, int inNumSamples);
// void TALReverb_Ctor(TALReverb* unit);
// void TALReverb_Dtor(TALReverb* unit);

void TALReverb_Ctor(TALReverb* unit)
{
    unit->first = true;
    SETCALC(TALReverb_next);
    ClearUnitOutputs(unit, 1);
    unit->lastGain = IN0(1);
    unit->lastRoomSize = IN0(2);
    unit->lastPreDelay = IN0(3);
    unit->lastLowCut = IN0(4);
    unit->lastDamp = IN0(5);
    unit->lastHighCut = IN0(6);
    unit->lastStereoWidth = IN0(7);
}
/* functions from the TALReverb source - SC'ized */

bool isPrime(int value)
{
    bool answer = true;
    if (value == 0) value = 1;
    for (int i = 2; i <= sqrtf((float)value) ; i++)
    {
	if (value % i == 0)
	{
	    answer = false;
	    break;
	}
    }
    return answer;
};

void SetupComb(TALReverb *unit, CombFilter *comb, float delayTime, float minDamp)
{
    comb->bufferLengthDelay = (int)(delayTime * SAMPLERATE * 0.001);
    while(!isPrime(comb->bufferLengthDelay)) comb->bufferLengthDelay++;
    comb->buffer = (float*)RTAlloc(unit->mWorld, sizeof(float) * comb->bufferLengthDelay);
    for(int i = 0; i < comb->bufferLengthDelay; i++)
    {
	comb->buffer[i] = 0.f;
    }
    comb->writePtr = 0;
    comb->readPtr1 = 0;
    comb->z1 = comb->filterStore = 0.f;
    comb->minDamp = minDamp;
}

inline float CombProcess(CombFilter *comb, float input, float damp, float feedback, float delay)
{
    float offset = (comb->bufferLengthDelay - 2) * delay + 1.0;
    comb->readPtr1 = comb->writePtr - (int)floorf(offset);
    if(comb->readPtr1 < 0)
	comb->readPtr1 += (comb->bufferLengthDelay);

    float output = comb->buffer[comb->readPtr1];
    comb->filterStore = output * (1. - damp) + comb->filterStore * damp;
    comb->buffer[comb->writePtr] = input + (comb->filterStore * feedback);
    if(++comb->writePtr >= comb->bufferLengthDelay)
	comb->writePtr = 0;

    return output;
}

inline float CombProcessInterpolated(CombFilter *comb, float input, float damp, float feedback, float delay)
{
    float offset = (float)(comb->bufferLengthDelay - 2) * delay + 1.;
    comb->readPtr1 = comb->writePtr - (int)floorf(offset);
    if(comb->readPtr1 < 0) comb->readPtr1 += comb->bufferLengthDelay;
    comb->readPtr2 = comb->readPtr1 - 1;
    if(comb->readPtr2 < 0) comb->readPtr2 += comb->bufferLengthDelay;
    float frac = offset - (int)floorf(offset);
    float output = comb->buffer[comb->readPtr2] + comb->buffer[comb->readPtr1] * (1-frac) - (1-frac) * comb->z1;
    comb->z1 = output;
    damp = comb->minDamp * damp;
    comb->filterStore = output * (1. - damp) + comb->filterStore * damp;
    comb->buffer[comb->writePtr] = input + (comb->filterStore * feedback);
    if(++comb->writePtr >= comb->bufferLengthDelay) comb->writePtr = 0;
    return output;
}

inline float tickNoise(TALReverb *unit)
{
    RGET

    float nextlevel = frand2(s1, s2, s3);

    RPUT

    return nextlevel;
}

inline int getNextRandomPeriod(TALReverb *unit)
{
    return (int)(tickNoise(unit) * 22768.) + 22188;
}

inline float tickFilteredNoise(TALReverb *unit, NoiseGenerator *noise)
{
    if(noise->counter++ % noise->randomPeriod == 0)
    {
	noise->actualValue = tickNoise(unit);
	noise->randomPeriod = getNextRandomPeriod(unit);
    }
    float nextLevel = noise->actualValueFiltered = (noise->actualValueFiltered * noise->filterFactor + noise->actualValue) * noise->filterFactorInversePlusOne;
    return nextLevel;
}


inline float tickFilteredNoiseFast(TALReverb *unit)
{
    return tickNoise(unit);
}

void SetupNoise(TALReverb *unit, NoiseGenerator *noise)
{
    noise->counter = 0;
    noise->actualValue = noise->actualValueFiltered = 0.f;
    noise->filterFactor = 5000.;
    noise->filterFactorInversePlusOne = 1. / (noise->filterFactor + 1.);
    noise->randomPeriod = getNextRandomPeriod(unit);
}

void SetupAllPass(TALReverb *unit, AllPassFilter *allPass, float delayTime, float feedbackGain)
{
    allPass->gain = feedbackGain;
    allPass->delay = delayTime;
    allPass->bufferLength = (int)(delayTime * SAMPLERATE * 0.001);
    allPass->writePtr = allPass->readPtr1 = allPass->readPtr2 = 0;
    allPass->z1 = 0.;
    while(!isPrime(allPass->bufferLength)) allPass->bufferLength++;
    allPass->buffer = (float*)RTAlloc(unit->mWorld, sizeof(float) * allPass->bufferLength);
    for(int i = 0; i < allPass->bufferLength; i++)
    {
	allPass->buffer[i] = 0.f;
    }
}

inline float AllPassProcess(AllPassFilter *allPass, float input)
{
    float tmp = allPass->buffer[allPass->readPtr1];
    allPass->buffer[allPass->readPtr1] = allPass->gain * tmp + input;
    float output = tmp - allPass->gain * allPass->buffer[allPass->readPtr1];
    if(++allPass->readPtr1 >= allPass->bufferLength) allPass->readPtr1 = 0;
    return output;
}

inline float AllPassProcessInterpolated(AllPassFilter *allPass, float input, float delayLength, float diffuse)
{
    float offset = ((float)allPass->bufferLength - 2.) * delayLength + 1.;
    float fOffset = floorf(offset);
    allPass->readPtr1 = allPass->writePtr - (int)fOffset;
    if(allPass->readPtr1 < 0)
	allPass->readPtr1 += allPass->bufferLength;
    allPass->readPtr2 = allPass->readPtr1 - 1;
    if(allPass->readPtr2 < 0)
	allPass->readPtr2 += allPass->bufferLength;
    float frac = offset - fOffset;
    float tmp = allPass->buffer[allPass->readPtr2] + allPass->buffer[allPass->readPtr1] * (1.-frac) - (1.-frac) * allPass->z1;
    allPass->z1 = tmp;
    allPass->buffer[allPass->writePtr] = diffuse * allPass->gain * tmp + input;
    float output = tmp - diffuse * allPass->gain * allPass->buffer[allPass->writePtr];
    if(++allPass->writePtr >= allPass->bufferLength)
	allPass->writePtr = 0;
    return output;
}

void SetupFilter(TALReverb *unit, Filter *filter)
{
    filter->y1 = filter->y2 = filter->y3 = filter->y4 = filter->oldx = filter->oldy1 = filter->oldy2 = filter->oldy3 = 0.0f;
    filter->sampleRateFactor = 44100. / SAMPLERATE;
    if(filter->sampleRateFactor > 1.) filter->sampleRateFactor = 1.;
    filter->fActualCutoff = -1.;
}

inline void updateValues(Filter *filter, float fCutoff)
{
    filter->f = fCutoff;
    filter->k = 3.6 * filter->f - 1.6 * filter->f * filter->f - 1.;
    filter->p = (filter->k + 1.) * 0.5;
    filter->scale = expf((1. - filter->p) * 1.386249);
}

inline float FilterProcess(Filter *filter, float input, float fCutoff, float fRes, bool highPass)
{
    if(fCutoff != filter->fActualCutoff)
    {
	filter->fActualCutoff = fCutoff;
	fCutoff *= filter->sampleRateFactor;
	updateValues(filter, fCutoff * fCutoff);
    }
    if(input > 1.) input = 1.;
    if(input < -1.) input = -1.;
    filter->x = input;
    filter->y1 = (filter->x + filter->oldx) * filter->p - filter->k * filter->y1;
    filter->y2 = (filter->y1 + filter->oldy1) + filter->p - filter->k * filter->y2;
    filter->oldx = filter->x;
    filter->oldy1 = filter->y1;
    if(highPass) input = input - filter->y2;
    else input = filter->y2;
    return input;
}

void TALReverb_next(TALReverb *unit, int inNumSamples)
{

    float *in = IN(0);
    float *out0 = OUT(0);
    float *out1 = OUT(1);
    float nextGain = IN0(1);
    float nextRoomSize = IN0(2);
    float nextPreDelay = IN0(3);
    float nextLowCut = IN0(4);
    float nextDamp = IN0(5);
    float nextHighCut = IN0(6);
    float nextStereoWidth = IN0(7);
    float gain = unit->lastGain;
    float gainSlope = CALCSLOPE(nextGain, gain);
    float roomSize = unit->lastRoomSize;
    float roomSizeSlope = CALCSLOPE(nextRoomSize, roomSize);
    float preDelay = unit->lastPreDelay;
    float preDelaySlope = CALCSLOPE(nextPreDelay, preDelay);
    float lowCut = unit->lastLowCut;
    float lowCutSlope = CALCSLOPE(nextLowCut, lowCut);
    float damp = unit->lastDamp;
    float dampSlope = CALCSLOPE(nextDamp, damp);
    float highCut = unit->lastHighCut;
    float highCutSlope = CALCSLOPE(nextHighCut, highCut);
    float stereoWidth = unit->lastStereoWidth;
    float stereoWidthSlope = CALCSLOPE(nextStereoWidth, stereoWidth);

    if(unit->first) // set-up the delayline and struct instances
    {
	// also - for combs and allpasses, need to alloc and init buffer!!!!
	unit->first = false;
	float delayLength = 80.;
	float volumeScale = (float)((-3. * delayLength) / log10f(1.0f));
	int numlines = DELAY_LINES_COMB + DELAY_LINES_ALLPASS;
	for(int n = numlines - 1; n>= 0; n--)
	{
	    unit->reflectionDelays[numlines - 1 - n] = delayLength / powf(2.0, (float)n/numlines);
	    unit->reflectionGains[numlines - 1 - n] = powf(10.0, - (3. * unit->reflectionDelays[numlines - 1 - n]) / volumeScale);
	}

	unit->combFiltersPreDelay = (CombFilter*)RTAlloc(unit->mWorld, sizeof(CombFilter));
	SetupComb(unit, unit->combFiltersPreDelay, (float)MAX_PRE_DELAY_MS, 0.0f);
	float stereoSpreadValue = 0.005f;
	float stereoSpreadSign = 1.0f;

	for(int i = 0; i < DELAY_LINES_COMB; i++)
	{
	    float stereoSpreadFactor = 1. + stereoSpreadValue;
	    unit->combFiltersL[i] = (CombFilter*)RTAlloc(unit->mWorld, sizeof(CombFilter));
	    if(stereoSpreadSign > 0){
		SetupComb(unit, unit->combFiltersL[i], unit->reflectionDelays[i] * stereoSpreadValue, unit->reflectionGains[i]);
	    } else {
		SetupComb(unit, unit->combFiltersL[i], unit->reflectionDelays[i], unit->reflectionGains[i]);
	    }
	    unit->combFiltersR[i] = (CombFilter*)RTAlloc(unit->mWorld, sizeof(CombFilter));
	    if(stereoSpreadSign > 0){
		SetupComb(unit, unit->combFiltersR[i], unit->reflectionDelays[i], unit->reflectionGains[i]);
	    } else {
		SetupComb(unit, unit->combFiltersR[i], unit->reflectionDelays[i] * stereoSpreadValue, unit->reflectionGains[i]);
	    }

	    stereoSpreadSign *= -1.;

	    unit->noiseGeneratorDampL[i] = (NoiseGenerator*)RTAlloc(unit->mWorld, sizeof(NoiseGenerator));
	    SetupNoise(unit, unit->noiseGeneratorDampL[i]);
	    unit->noiseGeneratorDampR[i] = (NoiseGenerator*)RTAlloc(unit->mWorld, sizeof(NoiseGenerator));
	    SetupNoise(unit, unit->noiseGeneratorDampR[i]);
	    unit->noiseGeneratorDelayL[i] = (NoiseGenerator*)RTAlloc(unit->mWorld, sizeof(NoiseGenerator));
	    SetupNoise(unit, unit->noiseGeneratorDelayL[i]);
	    unit->noiseGeneratorDelayR[i] = (NoiseGenerator*)RTAlloc(unit->mWorld, sizeof(NoiseGenerator));
	    SetupNoise(unit, unit->noiseGeneratorDelayR[i]);
	}

	unit->preAllPassFilter = (AllPassFilter*)RTAlloc(unit->mWorld, sizeof(AllPassFilter));
	SetupAllPass(unit, unit->preAllPassFilter, 15., 0.68);
	for(int i = 0; i < DELAY_LINES_ALLPASS; i++)
	{
	    unit->allPassFiltersL[i] = (AllPassFilter*)RTAlloc(unit->mWorld, sizeof(AllPassFilter));
	    SetupAllPass(unit, unit->allPassFiltersL[i], unit->reflectionDelays[i + DELAY_LINES_COMB - 1] * 0.21, 0.68);
	    unit->allPassFiltersR[i] = (AllPassFilter*)RTAlloc(unit->mWorld, sizeof(AllPassFilter));
	    SetupAllPass(unit, unit->allPassFiltersR[i], unit->reflectionDelays[i + DELAY_LINES_COMB - 1] * 0.22, 0.68);
	}
	Filter lpFilter;
	SetupFilter(unit, &lpFilter);
	unit->lpFilter = lpFilter;
	Filter hpFilter;
	SetupFilter(unit, &hpFilter);
	unit->hpFilter = hpFilter;
    }


    for(int i = 0; i < inNumSamples; i++)
    {
	float thisRoomSize = 1. - roomSize;
	thisRoomSize *= thisRoomSize * thisRoomSize;
	thisRoomSize = 1. - thisRoomSize;

	float revL = in[i];
	float noise = tickNoise(unit);
	revL += (noise * 0.000000001);
//	/* Pre Delay */
	revL += CombProcess(unit->combFiltersPreDelay, revL, 0., 0., preDelay * preDelay);
//	/* Pre AllPass */
	revL += AllPassProcessInterpolated(unit->preAllPassFilter, revL, 0.01 + thisRoomSize * 0.99, 0.97 + noise * 0.03) * 0.2;
	revL *= 0.2;
	revL = FilterProcess(&unit->lpFilter, revL, highCut, 0., false);
	revL = FilterProcess(&unit->hpFilter, revL, 0.95 * lowCut + 0.05, 0., true);
	float revR = revL;
	float outL = 0.;
	float outR = 0.;
	float scaledDamp = 0.95 * damp * damp;
	float scaledRoomSize = thisRoomSize * 0.97;

	for(int j = 0; j < DELAY_LINES_COMB; j++)
	{
	    outL += CombProcessInterpolated(unit->combFiltersL[j],
					    revL,
					    scaledDamp + 0.05 * tickFilteredNoiseFast(unit),
					    scaledRoomSize,
					    0.6 * scaledRoomSize + 0.395 + 0.012 * tickFilteredNoise(unit, unit->noiseGeneratorDelayL[j]) * thisRoomSize);
	    outR += CombProcessInterpolated(unit->combFiltersR[j],
					    revR,
					    scaledDamp + 0.05 * tickFilteredNoiseFast(unit),
					    scaledRoomSize,
					    0.6 * scaledRoomSize + 0.398 + 0.012 * tickFilteredNoise(unit, unit->noiseGeneratorDelayR[j]) * thisRoomSize);
	}

	//out0[i] = outL; //revL;
	//out1[i] = outR; //revR;

	for(int j = 0; j < DELAY_LINES_ALLPASS; j++)
	{
	    outL = AllPassProcess(unit->allPassFiltersL[j], outL);
	    outR = AllPassProcess(unit->allPassFiltersR[j], outR);
	}

	float thisGain = gain * gain;
	float wet1 = thisGain * (stereoWidth * 0.5 + 0.5);
	float wet2 = thisGain * ((1.-stereoWidth) * 0.5);

	out0[i] = outL * wet1 + outR * wet2;
	out1[i] = outR * wet1 + outL * wet2;

	gain += gainSlope;
	roomSize += roomSizeSlope;
	preDelay += preDelaySlope;
	lowCut += lowCutSlope;
	damp += dampSlope;
	highCut += highCutSlope;
	stereoWidth += stereoWidthSlope;
    }

    unit->lastGain = gain;
    unit->lastRoomSize = roomSize;
    unit->lastPreDelay = preDelay;
    unit->lastLowCut = lowCut;
    unit->lastDamp = damp;
    unit->lastHighCut = highCut;
    unit->lastStereoWidth = stereoWidth;

}

void TALReverb_Dtor(TALReverb* unit)
{
    /* RELEASE INTERNAL BUFFERS FIRST!!!! */
    RTFree(unit->mWorld, unit->combFiltersPreDelay->buffer);
    RTFree(unit->mWorld, unit->combFiltersPreDelay);
    RTFree(unit->mWorld, unit->preAllPassFilter->buffer);
    RTFree(unit->mWorld, unit->preAllPassFilter);
    for(int i = 0; i < DELAY_LINES_COMB; i++)
    {
	RTFree(unit->mWorld, unit->combFiltersL[i]->buffer);
	RTFree(unit->mWorld, unit->combFiltersL[i]);
	RTFree(unit->mWorld, unit->combFiltersR[i]->buffer);
	RTFree(unit->mWorld, unit->combFiltersR[i]);
	RTFree(unit->mWorld, unit->noiseGeneratorDampL[i]);
	RTFree(unit->mWorld, unit->noiseGeneratorDampR[i]);
	RTFree(unit->mWorld, unit->noiseGeneratorDelayL[i]);
	RTFree(unit->mWorld, unit->noiseGeneratorDelayL[i]);
    }
    for(int i = 0; i < DELAY_LINES_ALLPASS; i++)
    {
	RTFree(unit->mWorld, unit->allPassFiltersL[i]->buffer);
	RTFree(unit->mWorld, unit->allPassFiltersL[i]);
	RTFree(unit->mWorld, unit->allPassFiltersR[i]->buffer);
	RTFree(unit->mWorld, unit->allPassFiltersR[i]);
    }
}


/* Interpolating Delays */

/* hacked up versions of DelayC to use other interp methods */
/* below functions are based on function at:
 http://local.wasp.uwa.edu.au/~pbourke/miscellaneous/interpolation/
*/

float CubicDelayInterp(float mu, float y0,float y1,float y2 ,float y3 )
{
    float a0,a1,a2,a3,mu2;

    mu2 = mu*mu;
    a0 = y3 - y2 - y0 + y1;
    a1 = y0 - y1 - a0;
    a2 = y2 - y0;
    a3 = y1;

    return(a0*mu*mu2+a1*mu2+a2*mu+a3);
}

void CubicDelay_Ctor(CubicDelay *unit)
{
    DelayUnit_Reset(unit);
    // zero out the buffer since I am too lazy to write multiple functions
    for(int i = 0; i < unit->m_idelaylen; i++) unit->m_dlybuf[i] = 0.0;
    if (INRATE(2) == calc_FullRate) {
	SETCALC(CubicDelay_next_a);
    } else {
	SETCALC(CubicDelay_next_k);
    }
    unit->m_iwrphase--;
    unit->m_iwrphase--;
    ClearUnitOutputs(unit, 1);
}

void CubicDelay_next_a(CubicDelay *unit, int inNumSamples)
{
    float *out = OUT(0);
    float *in = IN(0);
    float *delaytime = IN(2);

    float *dlybuf = unit->m_dlybuf;
    long iwrphase = unit->m_iwrphase;
    float dsamp;
    long idsamp, idsampm1, idsampp1, idsampp2, mask;
    idsamp = idsampm1 = idsampp1 = idsampp2 = 0;
    mask = unit->m_mask;
    float y0, y1, y2, y3, frac;
    for(int i = 0; i < inNumSamples; i++)
    {
	dlybuf[iwrphase & mask] = in[i];
	dsamp = CalcDelay(unit, delaytime[i]);
	idsamp = (long)std::floor(dsamp);
	frac = dsamp - (float)idsamp;
//	if(frac == 0.0) frac = 1.0;
	idsamp = iwrphase - idsamp;
	idsampm1 = idsamp-1;
	idsampp1 = idsamp+1;
	idsampp2 = idsamp+2;
	y0 = dlybuf[idsampm1 & mask];
	y1 = dlybuf[idsamp & mask];
	y2 = dlybuf[idsampp1 & mask];
	y3 = dlybuf[idsampp2 & mask];
	out[i] = CubicDelayInterp(frac, y0, y1, y2, y3);
	iwrphase++;
    }
    Print("%3,6f, %3,6f\n", dsamp, frac);
    unit->m_iwrphase = iwrphase;

}

void CubicDelay_next_k(CubicDelay *unit, int inNumSamples)
{
    float *out = OUT(0);
    float *in = IN(0);
    float delaytime = IN0(2);

    float *dlybuf = unit->m_dlybuf;
    long iwrphase = unit->m_iwrphase;
    float dsamp = unit->m_dsamp;

    float next_dsamp = CalcDelay(unit, delaytime);
    float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);
    long idsamp, idsampm1, idsampp1, idsampp2, mask;
    idsamp = idsampm1 = idsampp1 = idsampp2 = 0;
    mask = unit->m_mask;
    float y0, y1, y2, y3, frac;
    for(int i = 0; i < inNumSamples; i++)
    {
	dlybuf[iwrphase & mask] = in[i];
	idsamp = (long)dsamp;
	frac = dsamp - idsamp;
	idsamp = iwrphase - idsamp;
	idsampm1 = idsamp-1;
	idsampp1 = idsamp+1;
	idsampp2 = idsamp+2;
	y0 = dlybuf[idsampm1 & mask];
	y1 = dlybuf[idsamp & mask];
	y2 = dlybuf[idsampp1 & mask];
	y3 = dlybuf[idsampp2 & mask];
	out[i] = CubicDelayInterp(frac, y0, y1, y2, y3);
	iwrphase++;
	dsamp += dsamp_slope;
    }
    unit->m_dsamp = next_dsamp;
    unit->m_delaytime = delaytime;
    unit->m_iwrphase = iwrphase;

}

#define CALC_PMLPF_COEFS \
    float fs, k, ksquared, ktwice; \
    fs = SAMPLERATE; \
    k = tanf((PI * unit->m_fc) / fs); \
    ksquared = k * k; \
    ktwice = k * 2.; \
    unit->m_a0 = unit->m_a2 = ksquared / (ksquared + ktwice + 1.); \
    unit->m_a1 = 2. * unit->m_a0; \
    unit->m_b1 = -1. * ((2. * (ksquared - 1.)) / ( ksquared + ktwice + 1.)); \
    unit->m_b2 = -1. * ((ksquared - ktwice + 1.) / ( ksquared + ktwice + 1.)); \


void PMLPF_Ctor(PMLPF *unit)
{
    unit->m_y1 = 0.f;
    unit->m_y2 = 0.f;

    ZOUT0(0) = 0.f;
    unit->m_fc = ZIN0(1);
    CALC_PMLPF_COEFS
    SETCALC(PMLPF_next);
}

void PMLPF_next(PMLPF *unit, int inNumSamples)
{
    float *out = ZOUT(0);
    float *in = ZIN(0);
    float fc = ZIN0(1);
    float y0;
    float y1 = unit->m_y1;
    float y2 = unit->m_y2;

    float a0 = unit->m_a0;
    float a1 = unit->m_a1;
    float a2 = unit->m_a2;
    float b1 = unit->m_b1;
    float b2 = unit->m_b2;

    if(unit->m_fc == fc)
    {

	LOOP(unit->mRate->mFilterLoops,
	      y0 = ZXP(in) + b1 * y1 + b2 * y2;
	      ZXP(out) = a0 * y0 + a1 * y1 + a2 * y2;

	      y2 = ZXP(in) + b1 * y0 + b2 * y1;
	      ZXP(out) = a0 * y2 + a1 * y0 + a2 * y1;

	      y1 = ZXP(in) + b1 * y2 + b2 * y0;
	      ZXP(out) = a0 * y1 + a1 * y2 + a2 * y0;
	      );
	LOOP(unit->mRate->mFilterRemain,
	     y0 = ZXP(in) + b1 * y1 + b2 * y2;
	     ZXP(out) = a0 * y0 + a1 * y1 + a2 * y2;
	     y2 = y1;
	     y1 = y0;
	     );
    } else {
	unit->m_fc = fc;

	CALC_PMLPF_COEFS

	float new_a0 = unit->m_a0;
	float new_a1 = unit->m_a1;
	float new_a2 = unit->m_a2;
	float new_b1 = unit->m_b1;
	float new_b2 = unit->m_b2;

	float a0_slope = (new_a0 - a0) * unit->mRate->mFilterSlope;
	float a1_slope = (new_a1 - a1) * unit->mRate->mFilterSlope;
	float a2_slope = (new_a2 - a2) * unit->mRate->mFilterSlope;
	float b1_slope = (new_b1 - b1) * unit->mRate->mFilterSlope;
	float b2_slope = (new_b2 - b2) * unit->mRate->mFilterSlope;

	LOOP(unit->mRate->mFilterLoops,
	 y0 = ZXP(in) + b1 * y1 + b2 * y2;
	 ZXP(out) = a0 * y0 + a1 * y1 + a2 * y2;

	 y2 = ZXP(in) + b1 * y0 + b2 * y1;
	 ZXP(out) = a0 * y2 + a1 * y0 + a2 * y1;

	 y1 = ZXP(in) + b1 * y2 + b2 * y0;
	 ZXP(out) = a0 * y1 + a1 * y2 + a2 * y0;

	 a0 += a0_slope;
	 a1 += a1_slope;
	 a2 += a2_slope;
	 b1 += b1_slope;
	 b2 += b2_slope;
	 );
    LOOP(unit->mRate->mFilterRemain,
	 y0 = ZXP(in) + b1 * y1 + b2 * y2;
	 ZXP(out) = a0 * y0 + a1 * y1 + a2 * y2;
	 y2 = y1;
	 y1 = y0;
	 );
    }

    unit->m_y1 = zapgremlins(y1);
    unit->m_y2 = zapgremlins(y2);
}


#define CALC_PMHPF_COEFS \
float fs, k, ksquared, ktwice; \
fs = SAMPLERATE; \
k = tanf((PI * unit->m_fc) / fs); \
ksquared = k * k; \
ktwice = k * 2.; \
unit->m_a0 = unit->m_a2 = 1. / (ksquared + ktwice + 1.); \
unit->m_a1 = -2. * unit->m_a0; \
unit->m_b1 = -1. * ((2. * (ksquared - 1.)) / ( ksquared + ktwice + 1.)); \
unit->m_b2 = -1. * (((ksquared - ktwice) + 1.) / ( ksquared + ktwice + 1.)); \

void PMHPF_Ctor(PMHPF *unit)
{
    unit->m_y1 = 0.f;
    unit->m_y2 = 0.f;
    ZOUT0(0) = 0.f;
    unit->m_fc = ZIN0(1);
    CALC_PMHPF_COEFS
    SETCALC(PMHPF_next);
}

void PMHPF_next(PMHPF *unit, int inNumSamples)
{
    float *out = ZOUT(0);
    float *in = ZIN(0);
    float fc = ZIN0(1);
    float y0;
    float y1 = unit->m_y1;
    float y2 = unit->m_y2;

    float a0 = unit->m_a0;
    float a1 = unit->m_a1;
    float a2 = unit->m_a2;
    float b1 = unit->m_b1;
    float b2 = unit->m_b2;

    if(unit->m_fc == fc)
    {
	LOOP(unit->mRate->mFilterLoops,
	     y0 = ZXP(in) + b1 * y1 + b2 * y2;
	     ZXP(out) = (a0 * y0 + a1 * y1 + a2 * y2) * -1.;

	     y2 = ZXP(in) + b1 * y0 + b2 * y1;
	     ZXP(out) = (a0 * y2 + a1 * y0 + a2 * y1) * -1.;

	     y1 = ZXP(in) + b1 * y2 + b2 * y0;
	     ZXP(out) = (a0 * y1 + a1 * y2 + a2 * y0) * -1.;
	     );
	LOOP(unit->mRate->mFilterRemain,
	     y0 = ZXP(in) + b1 * y1 + b2 * y2;
	     ZXP(out) = (a0 * y0 + a1 * y1 + a2 * y2) * -1.;
	     y2 = y1;
	     y1 = y0;
	     );
    } else {
	unit->m_fc = fc;

	CALC_PMHPF_COEFS

	float new_a0 = unit->m_a0;
	float new_a1 = unit->m_a1;
	float new_a2 = unit->m_a2;
	float new_b1 = unit->m_b1;
	float new_b2 = unit->m_b2;

	float a0_slope = (new_a0 - a0) * unit->mRate->mFilterSlope;
	float a1_slope = (new_a1 - a1) * unit->mRate->mFilterSlope;
	float a2_slope = (new_a2 - a2) * unit->mRate->mFilterSlope;
	float b1_slope = (new_b1 - b1) * unit->mRate->mFilterSlope;
	float b2_slope = (new_b2 - b2) * unit->mRate->mFilterSlope;

	LOOP(unit->mRate->mFilterLoops,
	     y0 = ZXP(in) + b1 * y1 + b2 * y2;
	     ZXP(out) = (a0 * y0 + a1 * y1 + a2 * y2) * -1.;

	     y2 = ZXP(in) + b1 * y0 + b2 * y1;
	     ZXP(out) = (a0 * y2 + a1 * y0 + a2 * y1) * -1.;

	     y1 = ZXP(in) + b1 * y2 + b2 * y0;
	     ZXP(out) = (a0 * y1 + a1 * y2 + a2 * y0) * -1.;

	     a0 += a0_slope;
	     a1 += a1_slope;
	     a2 += a2_slope;
	     b1 += b1_slope;
	     b2 += b2_slope;
	     );
	LOOP(unit->mRate->mFilterRemain,
	     y0 = ZXP(in) + b1 * y1 + b2 * y2;
	     ZXP(out) = (a0 * y0 + a1 * y1 + a2 * y2) * -1.;
	     y2 = y1;
	     y1 = y0;
	     );
    }

    unit->m_y1 = zapgremlins(y1);
    unit->m_y2 = zapgremlins(y2);
}

#define CALC_NFC_COEFS \
    float c, fc, fs, k; \
    c = 340; \
    fc = c/((PI * 2.) * unit->m_distance); \
    fs = SAMPLERATE; \
    k = tanf((PI * fc) / fs); \
    unit->m_a0 = 1. / (k + 1.); \
    unit->m_a1 = -1. * unit->m_a0; \
    unit->m_b1 = (k - 1.) * unit->m_a0 * -1.; \


void NFC_Ctor(NFC *unit)
{
    unit->m_y1 = 0.f;
    OUT0(0) = 0.f;
    unit->m_distance = IN0(1);
    CALC_NFC_COEFS
    SETCALC(NFC_next);
}

void NFC_next(NFC *unit, int inNumSamples)
{
    float *out = ZOUT(0);
    float *in = ZIN(0);
    float distance = ZIN0(1);
    float a0 = unit->m_a0;
    float a1 = unit->m_a1;
    float b1 = unit->m_b1;
    float y1 = unit->m_y1;

    if(distance == unit->m_distance)
    {

	LOOP1(inNumSamples,
	      float y0 = ZXP(in) + b1 * y1;
	      ZXP(out) = a0 * y0 + a1 * y1;
	      y1 = y0;
	      );

	unit->m_y1 = zapgremlins(y1);
    } else {

	unit->m_distance = distance;
	CALC_NFC_COEFS

	float new_a0 = unit->m_a0;
	float new_a1 = unit->m_a1;
	float new_b1 = unit->m_b1;
	float a0_slope = CALCSLOPE(new_a0, a0);
	float a1_slope = CALCSLOPE(new_a1, a1);
	float b1_slope = CALCSLOPE(new_b1, b1);
	LOOP1(inNumSamples,
	      float y0 = ZXP(in) + b1 * y1;
	      ZXP(out) = a0 * y0 + a1 * y1;
	      y1 = y0;

	      a0 += a0_slope;
	      a1 += a1_slope;
	      b1 += b1_slope;
	      );
    }
    unit->m_y1 = zapgremlins(y1);
}

#define CALC_XOVER2_COEFS \
float fc = IN0(1) * SAMPLEDUR; \
float w, c, s; \
w = 2. * PI * fc; \
c = cosf(w); \
s = sinf(w); \
unit->m_c0 = (c < 1e-3f) ? (-0.5 * c) : ((s - 1.) / c); \
unit->m_c1 = 0.5 * (1. + unit->m_c0); \

void Xover2_Ctor(Xover2 *unit)
{
    CALC_XOVER2_COEFS
    unit->m_fc = fc;
    unit->m_z0 = unit->m_z1 = unit->m_z2 = 0.;
    SETCALC(Xover2_next);
    OUT0(0) = 0.;
    OUT0(1) = 0.;
}

void Xover2_next(Xover2 *unit, int inNumSamples)
{
    float *in = IN(0);
    float *lp = OUT(0);
    float *hp = OUT(1);

    float d, t, x0, x1, z0, z1, z2, c0, c1;
    z0 = unit->m_z0;
    z1 = unit->m_z1;
    z2 = unit->m_z2;
    c0 = unit->m_c0;
    c1 = unit->m_c1;
//    if(unit->m_fc == IN0(1))
//    {
	for(int i = 0; i < inNumSamples; i++)
	{
	    //hp[i] = lp[i]= in[i];

	    x1 = x0 = in[i];
	    d = c1 * (x1 - z1) + 1e-20f;
	    x1 = z1 + d;
	    z1 = x1 + d;
	    d = c1 * (x1 - z2) + 1e-20f;
	    x1 = z2 + d;
	    z2 = x1 + d;
	    lp[i] = x1;

	    t = x0 - c0 * z0;
	    x0 = z0 + c0 * t;
	    z0 = t + 1e-20f;
	    hp[i] = x0 - x1;
	}
//    }
    unit->m_z0 = z0;
    unit->m_z1 = z1;
    unit->m_z2 = z2;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

PluginLoad(Josh)
{
	ft = inTable;
	DefineDtorUnit(AtsSynth);
	DefineDtorUnit(AtsNoiSynth);
	DefineSimpleUnit(AtsPartial);
	DefineSimpleUnit(AtsBand);
	DefineSimpleUnit(AtsFreq);
	DefineSimpleUnit(AtsAmp);
	DefineSimpleUnit(AtsNoise);
	DefineSimpleCantAliasUnit(AtsParInfo);

	DefineSimpleUnit(SinTone);
	DefineSimpleCantAliasUnit(Maxamp);

	DefineDtorUnit(LPCSynth);
	DefineSimpleUnit(LPCVals);
	DefineSimpleUnit(AudioMSG);
	DefineDtorCantAliasUnit(PVSynth);
	DefineSimpleUnit(PVInfo);
	DefineSimpleCantAliasUnit(Balance);
	DefineSimpleUnit(MoogVCF);
	DefineSimpleUnit(PosRatio);

	#define DefineDelayUnit(name) \
	    (*ft->fDefineUnit)(#name, sizeof(name), (UnitCtorFunc)&name##_Ctor, \
	    (UnitDtorFunc)&DelayUnit_Dtor, 0);

	DefineDelayUnit(CombLP);
	DefineSimpleCantAliasUnit(PanX);
	DefineSimpleCantAliasUnit(WarpZ);
	DefineSimpleUnit(Metro);

	DefineDtorUnit(ATSSynth);
	DefineDtorCantAliasUnit(TALReverb);

	#define DefineDelayCantAliasUnit(name) \
	    (*ft->fDefineUnit)(#name, sizeof(name), (UnitCtorFunc)&name##_Ctor, \
	    (UnitDtorFunc)&DelayUnit_Dtor, kUnitDef_CantAliasInputsToOutputs);

	DefineDelayCantAliasUnit(CubicDelay);

	DefineSimpleUnit(Xover2);
	DefineSimpleUnit(PMLPF);
	DefineSimpleUnit(PMHPF);
	DefineSimpleUnit(NFC);
	DefineSimpleCantAliasUnit(Xover2);
	DefineSimpleCantAliasUnit(TTendency);
	//DefineDelayUnit(HermiteDelay);
}
