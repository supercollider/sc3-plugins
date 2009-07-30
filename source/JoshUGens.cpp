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

#define ATS_BAND_WIDTH { 100.0, 100.0, 100.0, 100.0, 110.0, 120.0, 140.0, 150.0, 160.0, 190.0, 210.0, 240.0, 280.0, 320.0, 380.0, 450.0, 550.0, 700.0, 900.0, 1100.0, 1300.0, 1800.0, 2500.0, 3500.0, 4500.0}
#define ATS_CENTER_FREQ {50.0, 150.0, 250.0, 350.0, 455.0, 570.0, 700.0, 845.0, 1000.0, 1175.0, 1375.0, 1600.0, 1860.0, 2160.0, 2510.0, 2925.0, 3425.0, 4050.0, 4850.0, 5850.0, 7050.0, 8600.0, 10750.0, 13750.0, 17750.0} 
#define ATS_OFFSET1 {0, 2, 3, 2, 3}
#define ATS_OFFSET2 {0, 1, 1, 26, 26};

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

static float cubicinterp(float x, float y0, float y1, float y2, float y3)
{
    // 4-point, 3rd-order Hermite (x-form)
    float c0 = y1;
    float c1 = 0.5f * (y2 - y0);
    float c2 = y0 - 2.5f * y1 + 2.f * y2 - 0.5f * y3;
    float c3 = 0.5f * (y3 - y0) + 1.5f * (y1 - y2);
    
    return ((c3 * x + c2) * x + c1) * x + c0;
}
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

struct DelTapWr : public Unit 
{
	SndBuf *m_buf;
	float m_fbufnum;
	int32 m_phase;
};

struct DelTapRd : public Unit
{
	SndBuf *m_buf;
	float m_fbufnum, m_delTime;
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

    void DelTapWr_Ctor(DelTapWr* unit);
    void DelTapWr_next(DelTapWr *unit, int inNumSamples);
    void DelTapWr_first(DelTapWr *unit, int inNumSamples);
    
    void DelTapRd_Ctor(DelTapRd* unit);
    void DelTapRd_next1_a(DelTapRd *unit, int inNumSamples);
    void DelTapRd_next2_a(DelTapRd *unit, int inNumSamples);
    void DelTapRd_next4_a(DelTapRd *unit, int inNumSamples);
    void DelTapRd_next1_k(DelTapRd *unit, int inNumSamples);
    void DelTapRd_next2_k(DelTapRd *unit, int inNumSamples);
    void DelTapRd_next4_k(DelTapRd *unit, int inNumSamples);

    void WarpZ_next(WarpZ *unit, int inNumSamples);
    void WarpZ_Ctor(WarpZ* unit);
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

void MoogVCF_Ctor(MoogVCF* unit) {
    if (INRATE(1) == calc_FullRate) {
	if (INRATE(2) == calc_FullRate) {
		SETCALC(MoogVCF_next_aa); // a-rate fco and res
		} else {
		SETCALC(MoogVCF_next_ak); // a-rate fco, k-rate res
		}
	    } else {
	if (INRATE(2) == calc_FullRate) {
		SETCALC(MoogVCF_next_ka); // k-rate fco, a-rate res
		} else {
		SETCALC(MoogVCF_next_kk);
		} // k-rate fco and res
	    }
	unit->m_fco = (IN0(1) * 2) / SAMPLERATE;
	unit->m_res = IN0(2);
	unit->m_xnm1 = unit->m_y1nm1 = unit->m_y2nm1 = unit->m_y3nm1 = 0.0f;
	unit->m_y1n = unit->m_y2n = unit->m_y3n = unit->m_y4n = 0.0f;
	ClearUnitOutputs(unit, 1);
}

void MoogVCF_next_kk(MoogVCF *unit, int inNumSamples){
    float* in = IN(0);
    float* out = OUT(0);
    float nextfco = IN0(1);
    float nextres = IN0(2);
    float fco = unit->m_fco; // already normalized
    float res = unit->m_res;
    float fcon = (nextfco * 2.0) / SAMPLERATE;   // filt freq, normalized to 0 to Nyquist

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

    float kp, pp1d2, scale, xn, k;

    for (int i = 0; i < inNumSamples; i++) {
	kp = (3.6 * fco) - ((1.6 * fco) * fco) - 1.0;  /* Emperical tuning     */
	pp1d2 = (kp + 1.0) * 0.5;              /* Timesaver                  */
	scale = (float)exp((1.0 - (double)pp1d2 ) * 1.386249); /* Scaling factor  */
	k = res * scale;
	xn = in[i]; // make this similar to the CSound stuff for now... easier translation
	xn = xn - (k * y4n); /* Inverted feed back for corner peaking */
	
	/* Four cascaded onepole filters (bilinear transform) */
	y1n   = (xn  * pp1d2) + (xnm1  * pp1d2) - (kp * y1n);
	y2n   = (y1n * pp1d2) + (y1nm1 * pp1d2) - (kp * y2n);
	y3n   = (y2n * pp1d2) + (y2nm1 * pp1d2) - (kp * y3n);
	y4n   = (y3n * pp1d2) + (y3nm1 * pp1d2) - (kp * y4n);
	/* Clipper band limited sigmoid */
	y4n   = y4n - (((y4n * y4n) * y4n) / 6.0);
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
    float fcon = (nextfco * 2.0) / SAMPLERATE;   // filt freq, normalized to 0 to Nyquist

    float fcoslope = CALCSLOPE(fcon, fco);

    float xnm1 = unit->m_xnm1;
    float y1nm1 = unit->m_y1nm1;
    float y2nm1 = unit->m_y2nm1;
    float y3nm1 = unit->m_y3nm1;
    float y1n = unit->m_y1n;
    float y2n = unit->m_y2n;
    float y3n = unit->m_y3n;
    float y4n = unit->m_y4n;

    float kp, pp1d2, scale, xn, k;

    for (int i = 0; i < inNumSamples; i++) {
	kp = (3.6 * fco) - ((1.6 * fco) * fco) - 1.0;  /* Emperical tuning     */
	pp1d2 = (kp + 1.0) * 0.5;              /* Timesaver                  */
	scale = (float)exp((1.0 - (double)pp1d2 ) * 1.386249); /* Scaling factor  */
	k = res[i] * scale;
	xn = in[i]; // make this similar to the CSound stuff for now... easier translation
	xn = xn - (k * y4n); /* Inverted feed back for corner peaking */
	
	/* Four cascaded onepole filters (bilinear transform) */
	y1n   = (xn  * pp1d2) + (xnm1  * pp1d2) - (kp * y1n);
	y2n   = (y1n * pp1d2) + (y1nm1 * pp1d2) - (kp * y2n);
	y3n   = (y2n * pp1d2) + (y2nm1 * pp1d2) - (kp * y3n);
	y4n   = (y3n * pp1d2) + (y3nm1 * pp1d2) - (kp * y4n);
	/* Clipper band limited sigmoid */
	y4n   = y4n - (((y4n * y4n) * y4n) / 6.0);
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
    float fcon = 2.0 / SAMPLERATE;   // multiplier for filt freq, normalized to 0 to Nyquist

    float resslope = CALCSLOPE(nextres, res);

    float xnm1 = unit->m_xnm1;
    float y1nm1 = unit->m_y1nm1;
    float y2nm1 = unit->m_y2nm1;
    float y3nm1 = unit->m_y3nm1;
    float y1n = unit->m_y1n;
    float y2n = unit->m_y2n;
    float y3n = unit->m_y3n;
    float y4n = unit->m_y4n;

    float kp, pp1d2, scale, xn, k, thisfco;

    for (int i = 0; i < inNumSamples; i++) {
	thisfco = fco[i] * fcon;
	kp = (3.6 * thisfco) - ((1.6 * thisfco) * thisfco) - 1.0;  /* Emperical tuning     */
	pp1d2 = (kp + 1.0) * 0.5;              /* Timesaver                  */
	scale = (float)exp((1.0 - (double)pp1d2 ) * 1.386249); /* Scaling factor  */
	k = res * scale;
	xn = in[i]; // make this similar to the CSound stuff for now... easier translation
	xn = xn - (k * y4n); /* Inverted feed back for corner peaking */
	
	/* Four cascaded onepole filters (bilinear transform) */
	y1n   = (xn  * pp1d2) + (xnm1  * pp1d2) - (kp * y1n);
	y2n   = (y1n * pp1d2) + (y1nm1 * pp1d2) - (kp * y2n);
	y3n   = (y2n * pp1d2) + (y2nm1 * pp1d2) - (kp * y3n);
	y4n   = (y3n * pp1d2) + (y3nm1 * pp1d2) - (kp * y4n);
	/* Clipper band limited sigmoid */
	y4n   = y4n - (((y4n * y4n) * y4n) / 6.0);
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
    float fcon = 2.0 / SAMPLERATE;   // multiplier for filt freq, normalized to 0 to Nyquist

    float xnm1 = unit->m_xnm1;
    float y1nm1 = unit->m_y1nm1;
    float y2nm1 = unit->m_y2nm1;
    float y3nm1 = unit->m_y3nm1;
    float y1n = unit->m_y1n;
    float y2n = unit->m_y2n;
    float y3n = unit->m_y3n;
    float y4n = unit->m_y4n;

    float kp, pp1d2, scale, xn, k, thisfco;

    for (int i = 0; i < inNumSamples; i++) {
	thisfco = fco[i] * fcon;
	kp = (3.6 * thisfco) - ((1.6 * thisfco) * thisfco) - 1.0;  /* Emperical tuning     */
	pp1d2 = (kp + 1.0) * 0.5;              /* Timesaver                  */
	scale = (float)exp((1.0 - (double)pp1d2 ) * 1.386249); /* Scaling factor  */
	k = res[i] * scale;
	xn = in[i]; // make this similar to the CSound stuff for now... easier translation
	xn = xn - (k * y4n); /* Inverted feed back for corner peaking */
	
	/* Four cascaded onepole filters (bilinear transform) */
	y1n   = (xn  * pp1d2) + (xnm1  * pp1d2) - (kp * y1n);
	y2n   = (y1n * pp1d2) + (y1nm1 * pp1d2) - (kp * y2n);
	y3n   = (y2n * pp1d2) + (y2nm1 * pp1d2) - (kp * y3n);
	y4n   = (y3n * pp1d2) + (y3nm1 * pp1d2) - (kp * y4n);
	/* Clipper band limited sigmoid */
	y4n   = y4n - (((y4n * y4n) * y4n) / 6.0);
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
	unit->m_maxdelaytime = ZIN0(2);
	unit->m_delaytime = ZIN0(3);
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
			float onepole = ((1. - fabs(thiscoef)) * value) + (thiscoef * lastsamp);
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
			float onepole = ((1. - fabs(thiscoef)) * value) + (thiscoef * lastsamp);
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
			float onepole = ((1. - fabs(thiscoef)) * value) + (thiscoef * lastsamp);
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
			float onepole = ((1. - fabs(thiscoef)) * value) + (thiscoef * lastsamp);
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
			float onepole = ((1. - fabs(coef)) * value) + (coef * lastsamp);
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
			float onepole = ((1. - fabs(curcoef)) * value) + (curcoef * lastsamp);
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
			float onepole = ((1. - fabs(coef)) * value) + (coef * lastsamp);
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
			float onepole = ((1. - fabs(curcoef)) * value) + (curcoef * lastsamp);
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
			float onepole = ((1. - fabs(coef)) * value) + (coef * lastsamp);
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
			float onepole = ((1. - fabs(curcoef)) * value) + (curcoef * lastsamp);
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
			float onepole = ((1. - fabs(coef)) * value) + (coef * lastsamp);
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
			float onepole = ((1. - fabs(curcoef)) * value) + (curcoef * lastsamp);
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
			float onepole = ((1. - fabs(thiscoef)) * value) + (thiscoef * lastsamp);
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
			float onepole = ((1. - fabs(thiscoef)) * value) + (thiscoef * lastsamp);
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
			float onepole = ((1. - fabs(thiscoef)) * value) + (thiscoef * lastsamp);
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
			float onepole = ((1. - fabs(thiscoef)) * value) + (thiscoef * lastsamp);
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


#define DELTAP_BUF \
	World *world = unit->mWorld;\
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
	float *bufData __attribute__((__unused__)) = buf->data; \
	uint32 bufChannels __attribute__((__unused__)) = buf->channels; \
	uint32 bufSamples = buf->samples; \
	uint32 bufFrames = buf->frames; \
	int guardFrame __attribute__((__unused__)) = bufFrames - 2; \
	double loopMax = (double)bufSamples; \


#define CHECK_DELTAP_BUF \
	if ((!bufData) || (bufChannels != 1)) { \
                unit->mDone = true; \
		ClearUnitOutputs(unit, inNumSamples); \
		return; \
	}

void DelTapWr_Ctor(DelTapWr *unit)
{
	SETCALC(DelTapWr_first);
	unit->m_phase = 0;
	unit->m_fbufnum = -1e9f;
	DelTapWr_first(unit, 1);
}

void DelTapWr_first(DelTapWr *unit, int inNumSamples)
{
	SETCALC(DelTapWr_next);
    
	float fbufnum  = IN0(0);
	uint32 bufnum = (uint32)fbufnum;
	float* in = IN(1);
	float* out = OUT(0);
	
	uint32 phase = unit->m_phase;
	
	DELTAP_BUF
	CHECK_DELTAP_BUF
	
	// zero out the buffer!
	for(uint32 i = 0; i < bufSamples; i++){
	    bufData[i] = 0.f;
	    }
	    
	for(int i = 0; i < inNumSamples; i++){
	    out[i] = (float)phase;
	    bufData[phase] = in[i];
	    phase++;
	    if(phase == loopMax) phase -= loopMax;
	    }
	
	unit->m_phase = phase;
}

void DelTapWr_next(DelTapWr *unit, int inNumSamples)
{
	float fbufnum  = IN0(0);
	uint32 bufnum = (uint32)fbufnum;
	float* in = IN(1);
	float* out = OUT(0);
	
	uint32 phase = unit->m_phase;
	
	DELTAP_BUF
	CHECK_DELTAP_BUF

	for(int i = 0; i < inNumSamples; i++){
	    out[i] = (float)phase;
	    bufData[phase] = in[i];
	    phase++;
	    if(phase == loopMax) phase -= loopMax;
	    }
	
	unit->m_phase = phase;
}

#define SETUP_TAPDELK \
	float delTime = unit->m_delTime; \
	float newDelTime = IN0(2) * SAMPLERATE; \
	float delTimeInc = CALCSLOPE(newDelTime, delTime); \
	float phaseIn = IN0(1); \
	float phase; \
	int32 iphase; \
	float fbufnum  = IN0(0); \
	uint32 bufnum = (uint32)fbufnum; \
	float* out = OUT(0); \
	    
#define SETUP_TAPDELA \
	float* delTime = IN(2); \
	float phaseIn = IN0(1); \
	float phase, curDelTimeSamps; \
	int32 iphase; \
	float fbufnum  = IN0(0); \
	uint32 bufnum = (uint32)fbufnum; \
	float* out = OUT(0); \
	    
void DelTapRd_Ctor(DelTapRd *unit)
{
    unit->m_fbufnum = -1e9f;
    unit->m_delTime = IN0(2) * SAMPLERATE;
    int interp = (int)IN0(3);
    if(INRATE(2) == calc_FullRate){
	if(interp == 2){
	    SETCALC(DelTapRd_next2_a);
	    DelTapRd_next2_a(unit, 1);
	    } else if(interp == 4){
		SETCALC(DelTapRd_next4_a);
		DelTapRd_next4_a(unit, 1);
		} else {
		SETCALC(DelTapRd_next1_a);
		DelTapRd_next1_a(unit, 1);
		}
	    } else {
	if(interp == 2){
	    SETCALC(DelTapRd_next2_k);
	    DelTapRd_next2_k(unit, 1);
	    } else if(interp == 4){
		SETCALC(DelTapRd_next4_k);
		DelTapRd_next4_k(unit, 1);
		} else {
		SETCALC(DelTapRd_next1_k);
		DelTapRd_next1_k(unit, 1);
		}
	    }
}


void DelTapRd_next1_a(DelTapRd *unit, int inNumSamples)
{
    SETUP_TAPDELA
    DELTAP_BUF
    CHECK_DELTAP_BUF
    
    for(int i = 0; i < inNumSamples; i++)
	{
	    curDelTimeSamps = delTime[i] * SAMPLERATE; 
	    phase = phaseIn - curDelTimeSamps; 
	    if(phase < 0.) phase += loopMax; 
	    if(phase >=loopMax) phase -= loopMax; 
	    iphase = (int32)phase; 
	    out[i] = bufData[iphase];
	    phaseIn += 1.;
	}
}

void DelTapRd_next1_k(DelTapRd *unit, int inNumSamples)
{
    SETUP_TAPDELK
    DELTAP_BUF
    CHECK_DELTAP_BUF
    
    for(int i = 0; i < inNumSamples; i++)
	{
	    phase = phaseIn - delTime;
	    if(phase < 0.) phase += loopMax; 
	    if(phase >=loopMax) phase -= loopMax; 
	    iphase = (int32)phase; 
	    out[i] = bufData[iphase]; 
	    delTime += delTimeInc;
	    phaseIn += 1.;
	}
    unit->m_delTime = delTime;
}

void DelTapRd_next2_k(DelTapRd *unit, int inNumSamples)
{
    int32 iloopMax, iphase1;
    float fracphase, b, c;
    
    SETUP_TAPDELK
    DELTAP_BUF
    CHECK_DELTAP_BUF
    
    iloopMax = (int32)loopMax;
    
    for(int i = 0; i < inNumSamples; i++)
	{
	    phase = phaseIn - delTime;
	    if(phase < 0.) phase += loopMax; 
	    if(phase >= loopMax) phase -= loopMax; 
	    iphase = (int32)phase; 
	    iphase1 = iphase + 1; 
	    if(iphase1 >= iloopMax) iphase1 -= iloopMax; 
	    fracphase = phase - (float)iphase; 
	    b = bufData[iphase]; 
	    c = bufData[iphase1]; 
	    out[i] = (b + fracphase * (c - b));
	    delTime += delTimeInc;
	    phaseIn += 1.;
	}
    unit->m_delTime = delTime;
}

void DelTapRd_next2_a(DelTapRd *unit, int inNumSamples)
{
    int32 iloopMax, iphase1;
    float fracphase, b, c;

    SETUP_TAPDELA
    DELTAP_BUF
    CHECK_DELTAP_BUF
    
    iloopMax = (int32)loopMax;
    
    for(int i = 0; i < inNumSamples; i++)
	{
	    curDelTimeSamps = delTime[i] * SAMPLERATE; 
	    phase = phaseIn - curDelTimeSamps; 
	    if(phase < 0.) phase += loopMax; 
	    if(phase >= loopMax) phase -= loopMax; 
	    iphase = (int32)phase; 
	    iphase1 = iphase + 1; 
	    if(iphase1 >= iloopMax) iphase1 -= iloopMax; 
	    fracphase = phase - (float)iphase; 
	    b = bufData[iphase]; 
	    c = bufData[iphase1]; 
	    out[i] = (b + fracphase * (c - b));
	    phaseIn += 1.;
	}
}

void DelTapRd_next4_k(DelTapRd *unit, int inNumSamples)
{
    int32 iloopMax, iphase1, iphase2, iphase0;
    float fracphase, a, b, c, d;
    

    SETUP_TAPDELK    
    DELTAP_BUF
    CHECK_DELTAP_BUF
    
    iloopMax = (int32)loopMax;
	    
    for(int i = 0; i < inNumSamples; i++)
	{
	    phase = phaseIn - delTime;
	    if(phase < 0.) phase += loopMax; 
	    if(phase >= loopMax) phase -= loopMax; 
	    iphase = (int32)phase; 
	    iphase0 = iphase - 1;
	    iphase1 = iphase + 1;
	    iphase2 = iphase + 2;
	    
	    if(iphase0 < 0) iphase0 += iloopMax;
	    if(iphase1 > iloopMax) iphase1 -=iloopMax;
	    if(iphase2 > iloopMax) iphase2 -=iloopMax;

	    fracphase = phase - (float)iphase;
	    a = bufData[iphase0]; 
	    b = bufData[iphase]; 
	    c = bufData[iphase1]; 
	    d = bufData[iphase2]; 
	    out[i] = cubicinterp(fracphase, a, b, c, d);
	    delTime += delTimeInc;
	    phaseIn += 1.;
	}
    unit->m_delTime = delTime;
}

void DelTapRd_next4_a(DelTapRd *unit, int inNumSamples)
{
    int32 iloopMax, iphase1, iphase2, iphase0;
    float fracphase, a, b, c, d;

    SETUP_TAPDELA
    DELTAP_BUF
    CHECK_DELTAP_BUF
    
    iloopMax = (int32)loopMax;
	    
    for(int i = 0; i < inNumSamples; i++)
	{
	    curDelTimeSamps = delTime[i] * SAMPLERATE; 
	    phase = phaseIn - curDelTimeSamps; 
	    if(phase < 0.) phase += loopMax; 
	    if(phase >= loopMax) phase -= loopMax; 
	    iphase = (int32)phase; 
	    iphase0 = iphase - 1;
	    iphase1 = iphase + 1;
	    iphase2 = iphase + 2;
	    
	    if(iphase0 < 0) iphase0 += iloopMax;
	    if(iphase1 > iloopMax) iphase1 -=iloopMax;
	    if(iphase2 > iloopMax) iphase2 -=iloopMax;

	    fracphase = phase - (float)iphase;
	    a = bufData[iphase0]; 
	    b = bufData[iphase]; 
	    c = bufData[iphase1]; 
	    d = bufData[iphase2]; 
	    out[i] = cubicinterp(fracphase, a, b, c, d);
	    phaseIn += 1.;
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
		    nextPulse = (int)sc_max(roundf((60.0 / bpm) * numBeats * SAMPLERATE), 2.0);
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

////////////////////////////////////////////////////////////////////////////////////////////////////////

void load(InterfaceTable *inTable)
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
	
	DefineSimpleCantAliasUnit(DelTapWr);
	DefineSimpleCantAliasUnit(DelTapRd);
    
#define DefineDelayUnit(name) \
    (*ft->fDefineUnit)(#name, sizeof(name), (UnitCtorFunc)&name##_Ctor, \
    (UnitDtorFunc)&DelayUnit_Dtor, 0);
    
	DefineDelayUnit(CombLP);
	DefineSimpleCantAliasUnit(PanX);
	DefineSimpleCantAliasUnit(WarpZ);
	DefineSimpleUnit(Metro);

	
}


	