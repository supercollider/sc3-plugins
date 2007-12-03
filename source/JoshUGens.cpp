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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


#include "SC_PlugIn.h"

#define ATS_BAND_WIDTH { 100.0, 100.0, 100.0, 100.0, 110.0, 120.0, 140.0, 150.0, 160.0, 190.0, 210.0, 240.0, 280.0, 320.0, 380.0, 450.0, 550.0, 700.0, 900.0, 1100.0, 1300.0, 1800.0, 2500.0, 3500.0, 4500.0}
#define ATS_CENTER_FREQ {50.0, 150.0, 250.0, 350.0, 455.0, 570.0, 700.0, 845.0, 1000.0, 1175.0, 1375.0, 1600.0, 1860.0, 2160.0, 2510.0, 2925.0, 3425.0, 4050.0, 4850.0, 5850.0, 7050.0, 8600.0, 10750.0, 13750.0, 17750.0} 

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
	int m_valindex; 
	float m_framestart;
	float m_storevals[100]; // this will handle up to 50 poles


};
struct AtsSynth : public Unit
{
	int32 *m_phase; 
	int32 m_phaseoffset, m_lomask;
	int m_numPartials, m_partialStart, m_partialSkip;
	float m_sinePct, m_noisePct, m_freqShift;
	float m_fbufnum, m_framestart;
	double m_cpstoinc, m_radtoinc;
	int phaseinit;
	SndBuf *m_buf;
};

struct PVSynth : AtsSynth
{
};

struct PVInfo : public Unit
{
	float m_partialNum;
	int m_numPartials, m_partialStart, m_partialSkip;
	float m_fbufnum, m_framestart;
	SndBuf *m_buf;
};

struct AtsNoiSynth : public Unit
{
	int32 *m_phase; 
	int32  m_lomask;
	int m_numPartials, m_partialStart, m_partialSkip;
	int m_numBands, m_bandStart, m_bandSkip;
	float m_sinePct, m_noisePct, m_freqShift;
	float m_fbufnum,  m_framestart;
	double m_cpstoinc, m_radtoinc;
	// for noise resynth
	int32 m_sinphase[25];
	int32 m_sinphaseoffset, m_sinlomask;
	double m_sincpstoinc, m_sinradtoinc;
	float m_atsBandFreq[25]; 
	float m_atsCenterFreq[25];
	float m_level[25];
	float m_slope[25];
	int32 m_counter[25];
	SndBuf *m_buf;	
	int phaseinit;
};

struct AtsFreq : public Unit
{
	int m_partialNum;
	float m_fbufnum, m_framestart;
	SndBuf *m_buf;
};

struct AtsAmp : public Unit
{
	int m_partialNum;
	float m_fbufnum, m_framestart;
	SndBuf *m_buf;
};

struct AtsNoise : public Unit
{
	int m_bandNum;
	float m_fbufnum, m_framestart;
	SndBuf *m_buf;
};

struct AtsParInfo : public Unit
{
	int m_partialNum;
	float m_fbufnum, m_framestart;
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

extern "C"
{
	void load(InterfaceTable *inTable);
	
	void Maxamp_next(Maxamp *unit, int inNumSamples);
	void Maxamp_Ctor(Maxamp* unit);
	
	// being a little lazy for the time being... treat PVSynth as
	// a type 1 ATS file
	
	void PVSynth_Ctor(AtsSynth* unit);
	void PVSynth_Dtor(AtsSynth* unit);
	
	void PVInfo_next(AtsParInfo *unit, int inNumSamples);
	void PVInfo_Ctor(AtsParInfo* unit);
	
	void AtsSynth_next(AtsSynth *unit, int inNumSamples);
	void AtsSynth_Ctor(AtsSynth* unit);
	void AtsSynth_Dtor(AtsSynth* unit);
	
	void AtsNoiSynth_next(AtsNoiSynth *unit, int inNumSamples);
	void AtsNoiSynth_Ctor(AtsNoiSynth* unit);
	void AtsNoiSynth_Dtor(AtsNoiSynth* unit);
	
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
	
}

static float cubicinterp(float x, float y0, float y1, float y2, float y3)
{
	// 4-point, 3rd-order Hermite (x-form)
	float c0 = y1;
	float c1 = 0.5f * (y2 - y0);
	float c2 = y0 - 2.5f * y1 + 2.f * y2 - 0.5f * y3;
	float c3 = 0.5f * (y3 - y0) + 1.5f * (y1 - y2);

	return ((c3 * x + c2) * x + c1) * x + c0;
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
	// init the past sample's values to 0.0
	for (int i = 0; i < 100; ++i){
	    unit->m_storevals[i] = 0.0f;
	    }

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

	float *polevals = new float[npoles];
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
	delete [] polevals;
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

	float *polevals = new float[npoles];
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
	delete [] polevals;
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
	

////////////////////////////////////////////////////////////////////////////////////

void PVSynth_Ctor(AtsSynth* unit)
{
	SETCALC(AtsSynth_next);
	int tableSizeSin = ft->mSineSize;
	unit->m_numPartials = (int)ZIN0(1);
	unit->m_partialStart = (int)ZIN0(2);
	unit->m_partialSkip = (int)ZIN0(3);
	int numPartials = unit->m_numPartials;
	unit->m_lomask = (tableSizeSin - 1) << 3; 
	unit->m_radtoinc = tableSizeSin * (rtwopi * 65536.); 
	unit->m_cpstoinc = tableSizeSin * SAMPLEDUR * 65536.; 
	
	unit->m_phase = (int32*)RTAlloc(unit->mWorld, numPartials * sizeof(int32));
	unit->phaseinit = 1;
	unit->m_fbufnum = -1e9f;

	ClearUnitOutputs(unit, 1);

}

void PVSynth_Dtor(AtsSynth* unit)
{	
	RTFree(unit->mWorld, unit->m_phase);
}

void PVInfo_Ctor(AtsParInfo* unit)
{
	SETCALC(AtsParInfo_next);
	unit->m_partialNum = ZIN0(1);
	unit->m_framestart = ZIN0(2);
	unit->m_fbufnum = -1e9f;
	ClearUnitOutputs(unit, 1);

}


void AtsSynth_Ctor(AtsSynth* unit)
{
	SETCALC(AtsSynth_next);
	int tableSizeSin = ft->mSineSize;
	unit->m_numPartials = (int)ZIN0(1);
	unit->m_partialStart = (int)ZIN0(2);
	unit->m_partialSkip = (int)ZIN0(3);
	unit->m_framestart = ZIN0(4);
	unit->m_lomask = (tableSizeSin - 1) << 3; 
	unit->m_radtoinc = tableSizeSin * (rtwopi * 65536.); 
	unit->m_cpstoinc = tableSizeSin * SAMPLEDUR * 65536.;     
	unit->m_phase = (int32*)RTAlloc(unit->mWorld, unit->m_numPartials * sizeof(int32));
	unit->phaseinit = 1;
	unit->m_fbufnum = -1e9f;
	ClearUnitOutputs(unit, 1);
}

void AtsSynth_Dtor(AtsSynth* unit)
{	
	RTFree(unit->mWorld, unit->m_phase);
}


void AtsSynth_next(AtsSynth *unit, int inNumSamples)
{
	if(unit->phaseinit > 0)
	{
	for (int i = 0; i < unit->m_numPartials; i++){
	    unit->m_phase[i] = 0;
	    }
	unit->phaseinit = -1;
	}
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
	float *out = OUT(0);
	float freqMul = IN0(5);
	float freqAdd = IN0(6);
	float* filePartials = bufData + 1;
	float* fileFrames = bufData + 2;
	int ampDataStart = 4;
	int freqDataStart = ((int)filePartials[0] * (int)fileFrames[0]) + 4;
	//leaving phase out for now
//	int phaseDataStart = (((int)filePartials[0] * (int)fileFrames[0]) * 2) + 4;
	int numFrames = (int)fileFrames[0];
	float framestart = unit->m_framestart * (numFrames - 1);
	float frame = IN0(4);
	float frameend = frame * (numFrames - 1);
	// get the frame vals for the freq data
	int frameround1 = (int)frameend;
	int frameround2 = 1;
	// get frames vals for amp data (interpolated over the k-rate)
	int startframeround1 = (int)framestart;
	int startframeround2 = 1;
	
	float framepct = frameend - (float)frameround1;
	float startframepct = framestart - (float)startframeround1;
	
	int numPartials = unit->m_numPartials;
	int partialStart = unit->m_partialStart;
	int partialSkip = unit->m_partialSkip;
	int totalPartials = numPartials;

	int *partials = new int[numPartials]; //this will be the partials to synthesize
	
	startframeround2 = 1;

	if ((frameround1 + 1) > numFrames) frameround2 = 0;	

	if ((startframeround1 + 1) > numFrames) startframeround2 = 0;

	if (!bufData) { 
                unit->mDone = true; 
		ClearUnitOutputs(unit, inNumSamples); 
		return;
		};
	
	//fill partials with the correct partials, check if partials requested are actually present
	
	for (int j = 0; j < numPartials; ++j){
	    int aPartial = partialStart + (partialSkip * j);
	    if (aPartial < filePartials[0]) 
		partials[j] = aPartial;
		else
		--totalPartials;
	}    

	ClearUnitOutputs(unit, inNumSamples);

	float *table0 = ft->mSineWavetable;
	float *table1 = table0 + 1;
	int32 lomask = unit->m_lomask;
	
	int ampData, freqData;
	float startampin, endampin, ampslope, freqin;
	int32 freq, phase;
	float* startatsAmp1;
	float* endatsAmp1;
	float* atsFreq1;
	
	// this loop calulates the partial sample values for the each of the sins, stores them in an array
	for (int i = 0; i< totalPartials; ++i){
	    ampData = ampDataStart + (numFrames * partials[i]);
	    freqData = freqDataStart + (numFrames * partials[i]);
	    
	    startatsAmp1 = bufData + (ampData + startframeround1); //this is the position in the file of the amp

	    endatsAmp1 = bufData + (ampData + frameround1); 
	    	    
	    startampin = lininterp(startframepct, startatsAmp1[0], startatsAmp1[startframeround2]); 
	    endampin = lininterp(framepct, endatsAmp1[0], endatsAmp1[frameround2]); 
	    ampslope = CALCSLOPE(endampin, startampin);
	    
	    atsFreq1 = bufData + (freqData + frameround1); //this is the position in the file of the freq

	    freqin = (lininterp(framepct, atsFreq1[0], atsFreq1[frameround2]) * freqMul) + freqAdd; 
	    freq = (int32)(unit->m_cpstoinc * freqin);
	    
	    phase = unit->m_phase[i];
	
	    for (int j = 0; j < inNumSamples; ++j){
		out[j] += (lookupi1(table0, table1, phase, lomask) * startampin);
		phase += freq;
		startampin += ampslope;
	    };
	    unit->m_phase[i] = phase;
	    }
	
	unit->m_framestart = frame;

	delete [] partials;
}


/////////////////////////////////////////////////////////////////

void AtsNoiSynth_Ctor(AtsNoiSynth* unit)
{
	SETCALC(AtsNoiSynth_next);
	int i;
	int tableSizeSin = ft->mSineSize;
	int numPartials = unit->m_numPartials = (int)ZIN0(1);
	unit->m_partialStart = (int)ZIN0(2);
	unit->m_partialSkip = (int)ZIN0(3);
	unit->m_framestart = ZIN0(4);
	unit->m_numBands = (int)ZIN0(9);
	unit->m_bandStart = (int)ZIN0(10);
	unit->m_bandSkip = (int)ZIN0(11);
	//int numBands = (int)unit->m_numBands;
	unit->m_lomask = (tableSizeSin - 1) << 3; 
	unit->m_radtoinc = tableSizeSin * (rtwopi * 65536.); 
	unit->m_cpstoinc = tableSizeSin * SAMPLEDUR * 65536.; 
	
	// this allocates space to save phase data for each partial... 
	
	unit->m_phase = (int32*)RTAlloc(unit->mWorld, numPartials * sizeof(int32));
	unit->phaseinit = 1;
	unit->m_fbufnum = -1e9f;

	float atsBandFreq[25] = ATS_BAND_WIDTH;
	float atsCenterFreq[25] = ATS_CENTER_FREQ;

	for (i = 0; i < 25; i++){
	    unit->m_atsBandFreq[i] = atsBandFreq[i];
	    unit->m_atsCenterFreq[i] = atsCenterFreq[i];
	    unit->m_counter[i] = 0;
	    unit->m_level[i] = unit->mParent->mRGen->frand2();
	    unit->m_slope[i] = 0.f;
	    }
		
	unit->m_sinlomask = (tableSizeSin - 1) << 3; 
	unit->m_sinradtoinc = tableSizeSin * (rtwopi * 65536.); 
	unit->m_sincpstoinc = tableSizeSin * SAMPLEDUR * 65536.; 
	
	ClearUnitOutputs(unit, 1);

}

void AtsNoiSynth_Dtor(AtsNoiSynth* unit)
{	
	RTFree(unit->mWorld, unit->m_phase);
}

void AtsNoiSynth_next(AtsNoiSynth *unit, int inNumSamples)
{
	if(unit->phaseinit > 0)
	{
	    for (int i = 0; i < unit->m_numPartials; i++){
		unit->m_phase[i] = 0.;
		}
	    unit->phaseinit = -1;
	}
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
	float *out = OUT(0);
	float* filePartials = bufData + 1;
	float* fileFrames = bufData + 2;
	float* winSize = bufData + 3;
	int ampDataStart = 4;
	int freqDataStart = ((int)filePartials[0] * (int)fileFrames[0]) + 4;
	// leave phase out for now
	//int phaseDataStart = (((int)filePartials[0] * (int)fileFrames[0]) * 2) + 4;
	int noiseDataStart = (((int)filePartials[0] * (int)fileFrames[0]) * 3) + 4;
	int numFrames = (int)fileFrames[0];
	float framestart = unit->m_framestart * (numFrames - 1);
	float frame = IN0(4);
	float frameend = frame * (numFrames - 1);
	// get the frame vals for the freq data
	int frameround1 = (int)frameend;
	int frameround2 = 1;
	// get frames vals for amp data (interpolated over the k-rate)
	int startframeround1 = (int)framestart;
	int startframeround2 = 1;
	
	float framepct = frameend - (float)frameround1;
	float startframepct = framestart - (float)startframeround1;
	
	int numPartials = unit->m_numPartials;
	int partialStart = unit->m_partialStart;
	int partialSkip = unit->m_partialSkip;
	int numBands = unit->m_numBands;
	int bandStart = unit->m_bandStart;
	int bandSkip = unit->m_bandSkip;
	float sinePct = IN0(5);
	float noisePct = IN0(6);
	float freqMul = IN0(7);
	float freqAdd = IN0(8);
	int totalPartials = numPartials;
	int totalBands = numBands;
	
	int *partials = new int[totalPartials]; //this will be the partials to synthesize
	int *bands = new int[totalBands]; //this will be the bands to synthesize
	
	//frameround2 = frameround1 + 1;
	//startframeround2 = startframeround1 + 1;

	if ((frameround1 + 1) > numFrames) frameround2 = 0;	
	if ((startframeround1 + 1) > numFrames) startframeround2 = 0;
	
	if (!bufData) { 
                unit->mDone = true; 
		ClearUnitOutputs(unit, inNumSamples); 
		return;
		};
		
	//fill partials with the correct partials, check if partials requested are actually present
	
	for (int i = 0; i < numPartials; ++i){
	    int aPartial = partialStart + (partialSkip * i);
	    if (aPartial < (int)filePartials[0]) 
		partials[i] = aPartial;
		else
		--totalPartials;
	}    
	
    	ClearUnitOutputs(unit, inNumSamples);

	float *table0 = ft->mSineWavetable;
	float *table1 = table0 + 1;
	int32 lomask = unit->m_lomask;
	    
	// this loop calulates the partial sample values for the sins, stores them in an array
	for (int i = 0; i< totalPartials; ++i){
	    
	    int ampData = ampDataStart + (numFrames * partials[i]);
	    int freqData = freqDataStart + (numFrames * partials[i]);
	    
	    float* startatsAmp1 = bufData + (int)(ampData + startframeround1); //this is the position in the file of the amp

	    float* endatsAmp1 = bufData + (int)(ampData + frameround1); 
	    	    
	    float startampin = lininterp(startframepct, startatsAmp1[0], startatsAmp1[startframeround2]) * sinePct;
	    float endampin = lininterp(framepct, endatsAmp1[0], endatsAmp1[frameround2]) * sinePct;
	    float ampslope = CALCSLOPE(endampin, startampin);
	    
	    float* atsFreq1 = bufData + (int)(freqData + frameround1); //this is the position in the file of the freq

	    float freqin = ((lininterp(framepct, atsFreq1[0], atsFreq1[frameround2]) * freqMul) + freqAdd);
	    int32 freq = (int32)(unit->m_cpstoinc * freqin);
	    
	    int32 phase = unit->m_phase[i];
	
	    for (int j = 0; j < inNumSamples; ++j){
		out[j] += (lookupi1(table0, table1, phase, lomask) * startampin);
		phase += freq;
		startampin += ampslope;
	    };
	    unit->m_phase[i] = phase;
	    }
	
	for (int i = 0; i < numBands; ++i){
	    int aBand = bandStart + (bandSkip * i);
	    if (aBand < 25) 
		bands[i] = aBand;
		else
		--totalBands;
	}    
	
	float scale = (winSize[0] * 0.33166610955984); 
	
	//calculate the noise
	for (int i = 0; i < totalBands; i++){
	    int bandnum = bands[i];
		
	    int noiseData = noiseDataStart + ((int)numFrames * bandnum);
	    float* startatsnoise1 = bufData + (int)(noiseData + startframeround1); //this is the position in the file of the noise
	    float* endatsnoise1 = bufData + (int)(noiseData + frameround1); 
	    float startnoisein = sqrt((lininterp(startframepct, startatsnoise1[0], startatsnoise1[startframeround2]) * noisePct) / scale);
	    float endnoisein = sqrt((lininterp(framepct, endatsnoise1[0], endatsnoise1[frameround2]) * noisePct) / scale);
	    float noiseslope = CALCSLOPE(endnoisein, startnoisein); 
	    
	    int32 sinphase = unit->m_sinphase[i];
		
	    float noisefreq = unit->m_atsBandFreq[bandnum];
	    float sinfreq = unit->m_atsCenterFreq[bandnum];
	    int32 freq = (int32)(unit->m_sincpstoinc * sinfreq); 

	    float level = unit->m_level[i];
	    float slope = unit->m_slope[i];
	    int32 counter = unit->m_counter[i];
		
	    RGET
		
	    for (int j = 0; j < inNumSamples; j++){
		if (counter<=0) {
		    counter = (int32)(unit->mRate->mSampleRate / sc_max(noisefreq, .001f));
		    counter = sc_max(1, counter);
		    float nextlevel = frand2(s1, s2, s3);
		    slope = (nextlevel - level) / counter;
		} else --counter;
		out[j] += lookupi1(table0, table1, sinphase, lomask) * startnoisein * level;
		startnoisein += noiseslope;
		level += slope;
    		sinphase += freq;
		}
		
	    RPUT
	    unit->m_level[i] = level;
	    unit->m_slope[i] = slope;
	    unit->m_counter[i] = counter;
	    unit->m_sinphase[i] = sinphase;
	    }
	
    	unit->m_framestart = frame;
		delete [] partials;
		delete [] bands;
}

/////////////////////////////////////////////////////////////////

void AtsFreq_Ctor(AtsFreq* unit)
{
	SETCALC(AtsFreq_next);
	unit->m_partialNum = (int)ZIN0(1);
	unit->m_fbufnum = -1e9f;
	unit->m_framestart = ZIN0(2);
	ClearUnitOutputs(unit, 1);

}

void AtsFreq_next(AtsFreq *unit, int inNumSamples)
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
	float *out = OUT(0);
	float* filePartials = bufData + 1;
	float* fileFrames = bufData + 2;
	int freqDataStart = ((int)filePartials[0] * (int)fileFrames[0]) + 4;
	int numFrames = (int)fileFrames[0];
	float framestart = unit->m_framestart * (numFrames - 1);
	float frame = IN0(2);
	float frameend = frame * (numFrames - 1);
	int startframeround1 = (int)framestart;
	int startframeround2;
	float startframepct = framestart-(float)startframeround1;
	int endframeround1 = (int)frameend;
	int endframeround2;
	float endframepct = frameend-(float)endframeround1;
		
	int partialNum = unit->m_partialNum;
	
	startframeround2 = startframeround1 + 1;
	endframeround2 = endframeround1 + 1;
	
	if (partialNum >= filePartials[0]){ 
                unit->mDone = true; 
		ClearUnitOutputs(unit, inNumSamples); 
		return;
		};
	    
	if (startframeround2 > numFrames) startframeround2 = startframeround1;
	if (endframeround2 > numFrames) endframeround2 = endframeround1;
	
	if (!bufData) { 
                unit->mDone = true; 
		ClearUnitOutputs(unit, inNumSamples); 
		return;
		};
	
	int freqData = freqDataStart + (numFrames * partialNum);
	int startfreqData1 = freqData + startframeround1;
	int startfreqData2 = freqData  + startframeround2;
	float* startatsFreq1 = bufData + startfreqData1; //this is the position in the file of the freq
	float* startatsFreq2 = bufData + startfreqData2;
	float startfreqin = (startatsFreq1[0] + ((startatsFreq2[0] - startatsFreq1[0]) * startframepct));
	
	int endfreqData1 = freqData + endframeround1;
	int endfreqData2 = freqData + endframeround2;
	float* endatsFreq1 = bufData + endfreqData1; //this is the position in the file of the freq
	float* endatsFreq2 = bufData + endfreqData2;
	float endfreqin = (endatsFreq1[0] + ((endatsFreq2[0] - endatsFreq1[0]) * endframepct));
		
	float freqslope = CALCSLOPE(endfreqin, startfreqin);
	
	for (int j = 0; j < inNumSamples; ++ j){
	    out[j] = startfreqin;
	    startfreqin += freqslope;
	    }
	
	unit->m_framestart = frame;
}
/////////////////////////////////////////////////////////////////

void AtsAmp_Ctor(AtsAmp* unit)
{
	SETCALC(AtsAmp_next);
	unit->m_partialNum = (int)ZIN0(1);
	unit->m_fbufnum = -1e9f;
	ClearUnitOutputs(unit, 1);
	unit->m_framestart = ZIN0(2);

}

void AtsAmp_next(AtsAmp *unit, int inNumSamples)
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
	float *out = OUT(0);
	float* filePartials = bufData + 1;
	float* fileFrames = bufData + 2;
	int ampDataStart = 4;
	int numFrames = (int)fileFrames[0];
	float framestart = unit->m_framestart * ((int)numFrames - 1);
	float frame = IN0(2);
	float frameend = frame * ((int)numFrames - 1);
	int startframeround1 = (int)framestart;
	int startframeround2;
	float startframepct = framestart-(float)startframeround1;
	int endframeround1 = (int)frameend;
	int endframeround2;
	float endframepct = frameend - (float)endframeround1;
	int partialNum = unit->m_partialNum;

	
	startframeround2 = startframeround1 + 1;
	endframeround2 = endframeround1 + 1;
	
	if (partialNum >= filePartials[0]){ 
		unit->mDone = true; 
		ClearUnitOutputs(unit, inNumSamples); 
		return;
		};
		
	if (startframeround2 > numFrames) startframeround2 = startframeround1;
	if (endframeround2 > numFrames) endframeround2 = endframeround1;
	
	if (!bufData) { 
                unit->mDone = true; 
		ClearUnitOutputs(unit, inNumSamples); 
		return;
		};
		
	int ampData = ampDataStart + (numFrames * partialNum);
	int startampData1 = ampData + startframeround1;
	int startampData2 = ampData + startframeround2;
	float* startatsAmp1 = bufData + startampData1; //this is the position in the file of the amp
	float* startatsAmp2 = bufData + startampData2;
	float startampin = (startatsAmp1[0] + ((startatsAmp2[0]- startatsAmp1[0]) * startframepct));

	int endampData1 = ampData + endframeround1;
	int endampData2 = ampData + endframeround2;
	float* endatsAmp1 = bufData + endampData1; //this is the position in the file of the amp
	float* endatsAmp2 = bufData + endampData2;
	float endampin = (endatsAmp1[0] + ((endatsAmp2[0]- endatsAmp1[0]) * endframepct));
		   
	float ampin = startampin;
	float ampslope = CALCSLOPE(endampin, startampin);
	for (int j = 0; j < inNumSamples; ++ j){
	    out[j] = ampin;
	    ampin += ampslope;
	    }
	
	unit->m_framestart = frame;
}


void AtsNoise_Ctor(AtsNoise* unit)
{
	SETCALC(AtsNoise_next);
	unit->m_bandNum = (int)IN0(1);
	unit->m_fbufnum = -1e9f;
	ClearUnitOutputs(unit, 1);
	unit->m_framestart = IN0(2);

}

void AtsNoise_next(AtsNoise *unit, int inNumSamples)
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
	float *out = OUT(0);
	float* filePartials = bufData + 1;
	float* fileFrames = bufData + 2;
	int noiseDataStart = (((int)filePartials[0] * (int)fileFrames[0]) * 3) + 4;
	int numFrames = (int)fileFrames[0];
	float framestart = unit->m_framestart * (numFrames - 1);
	float frame = IN0(2);
	float frameend = frame * (numFrames - 1);
	int startframeround1 = (int)framestart;
	int startframeround2;
	float startframepct = framestart-(float)startframeround1;
	int endframeround1 = (int)frameend;
	int endframeround2;
	float endframepct = frameend - (float)endframeround1;
	int bandNum = unit->m_bandNum;

	
	startframeround2 = startframeround1 + 1;
	endframeround2 = endframeround1 + 1;
	
	if (bandNum >= 25){ 
		unit->mDone = true; 
		ClearUnitOutputs(unit, inNumSamples); 
		return;
		};
		
	if (startframeround2 > numFrames) startframeround2 = startframeround1;
	if (endframeround2 > numFrames) endframeround2 = endframeround1;
	
	if (!bufData) { 
                unit->mDone = true; 
		ClearUnitOutputs(unit, inNumSamples); 
		return;
		};
	
	int noiseData = noiseDataStart + (numFrames * bandNum);		
	int startnoiseData1 =  noiseData + startframeround1;
	int startnoiseData2 = noiseData + startframeround2;
	float* startatsnoise1 = bufData + startnoiseData1; //this is the position in the file of the noise
	float* startatsnoise2 = bufData + startnoiseData2;
	float startnoisein = (startatsnoise1[0] + ((startatsnoise2[0]- startatsnoise1[0]) * startframepct));

	int endnoiseData1 = noiseData + endframeround1;
	int endnoiseData2 = noiseData + endframeround2;
	float* endatsnoise1 = bufData + endnoiseData1; //this is the position in the file of the noise
	float* endatsnoise2 = bufData + endnoiseData2;
	float endnoisein = (endatsnoise1[0] + ((endatsnoise2[0]- endatsnoise1[0]) * endframepct));
		   
	float noisein = startnoisein;
	float noiseslope = CALCSLOPE(endnoisein, startnoisein);
	for (int j = 0; j < inNumSamples; ++ j){
	    out[j] = noisein;
	    noisein += noiseslope;
	    }
	
	unit->m_framestart = frame;
}


void AtsParInfo_Ctor(AtsParInfo* unit)
{
	SETCALC(AtsParInfo_next);
	unit->m_partialNum = (int)ZIN0(1);
	unit->m_framestart = ZIN0(2);
	unit->m_fbufnum = -1e9f;
	ClearUnitOutputs(unit, 1);

}

void AtsParInfo_next(AtsParInfo *unit, int inNumSamples)
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
	float *out1 = OUT(0);
	float *out2 = OUT(1);
	float* filePartials = bufData + 1;
	float* fileFrames = bufData + 2;
	int ampDataStart = 4;
	int freqDataStart = ((int)filePartials[0] * (int)fileFrames[0]) + 4;
	int numFrames = (int)fileFrames[0];
	float framestart = unit->m_framestart * ((int)numFrames - 1);
	float frame = IN0(2);
	float frameend = frame * ((int)numFrames - 1);
	int startframeround1 = (int)framestart;
	int startframeround2;
	float startframepct = framestart-(float)startframeround1;

	int endframeround1 = (int)frameend;
	int endframeround2;
	float endframepct = frameend-(float)endframeround1;
		
	int partialNum = unit->m_partialNum;

	
	startframeround2 = startframeround1 + 1;
	endframeround2 = endframeround1 + 1;
	
	if (partialNum >= filePartials[0]){ 
		unit->mDone = true; 
		ClearUnitOutputs(unit, inNumSamples); 
		return;
		};
		
	if (startframeround2 > numFrames) startframeround2 = startframeround1;
	if (endframeround2 > numFrames) endframeround2 = endframeround1;
	
	if (!bufData) { 
                unit->mDone = true; 
		ClearUnitOutputs(unit, inNumSamples); 
		return;
		};
	
	int partialFrame = numFrames * partialNum;
	int startampData1 = ampDataStart + (partialFrame + startframeround1);
	int startampData2 = ampDataStart + (partialFrame + startframeround2);
	float* startatsAmp1 = bufData + startampData1; //this is the position in the file of the amp
	float* startatsAmp2 = bufData + startampData2;
	float startampin = (startatsAmp1[0] + ((startatsAmp2[0] - startatsAmp1[0]) * startframepct));

	int endampData1 = ampDataStart + (partialFrame + endframeround1);
	int endampData2 = ampDataStart + (partialFrame + endframeround2);
	float* endatsAmp1 = bufData + endampData1; //this is the position in the file of the amp
	float* endatsAmp2 = bufData + endampData2;
	float endampin = (endatsAmp1[0] + ((endatsAmp2[0] - endatsAmp1[0]) * endframepct));
	
	float ampslope = CALCSLOPE(endampin, startampin);
	
	int startfreqData1 = freqDataStart + (partialFrame + startframeround1);
	int startfreqData2 = freqDataStart + (partialFrame + startframeround2);
	float* startatsFreq1 = bufData + startfreqData1; //this is the position in the file of the amp
	float* startatsFreq2 = bufData + startfreqData2;
	float startfreqin = (startatsFreq1[0] + ((startatsFreq2[0] - startatsFreq1[0]) * startframepct));

	int endfreqData1 = freqDataStart + (partialFrame + endframeround1);
	int endfreqData2 = freqDataStart + (partialFrame + endframeround2);
	float* endatsFreq1 = bufData + endfreqData1; //this is the position in the file of the amp
	float* endatsFreq2 = bufData + endfreqData2;
	float endfreqin = (endatsFreq1[0] + ((endatsFreq2[0]- endatsFreq1[0]) * endframepct));
			   
	float freqslope = CALCSLOPE(endfreqin, startfreqin);
	
	for (int j = 0; j < inNumSamples; ++ j){
	    out1[j] = startampin;
	    out2[j] = startfreqin;
	    startampin += ampslope;
	    startfreqin += freqslope;
	    }
	
	unit->m_framestart = frame;
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


/*
void Balance_Ctor(Balance* unit)
{
	unit->m_scaler = 0.f;
	if (INRATE(1) == calc_FullRate) {
	    SETCALC(Balance_next_a);
	    } else {
	    SETCALC(Balance_next_k);
	    }
	unit->m_scaler = 0.f;
	ClearUnitOutputs(unit, 1);
}
*/

void Balance_Ctor(Balance* unit)
{
	unit->m_scaler = 0.f;
	if (INRATE(1) == calc_FullRate) {
	    SETCALC(Balance_next_a);
	    } else {
	    SETCALC(Balance_next_k);
	    }
	unit->m_hp = IN0(2); // 10.; // these are the csound defaults
	unit->m_stor = IN0(3); // 0.; 
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

//void Balance_next_a(Balance* unit, int inNumSamples)
//{
//	float* insig = IN(0);
//	float* testsig = IN(1);
//	float* out = OUT(0);
//	float insum = 0.f;
//	float testsum = 0.f;
//	float scaler = unit->m_scaler;
//	float newscale, levelinc;
//	
//	for(int i = 0; i < inNumSamples; i++){
//	    insum += powf(insig[i], 2);
//	    testsum += powf(testsig[i], 2);
//	    }
//	
//	float newinrms = sqrt(insum/inNumSamples);
//	float newtestrms = sqrt(testsum /inNumSamples);
//		
//	// protect a divide by zero... perhaps there is a better way to do this
//	if(newinrms <= 0.f) {
//	    newscale = 0.f; levelinc = 0.f;
//	    } else {
//	    newscale = newtestrms / newinrms;
//	    levelinc = CALCSLOPE(newscale, scaler);
//	    }
//		
//	for(int i = 0; i < inNumSamples; i++){
//	    out[i] = insig[i] * scaler;
//	    // overwrite old data with new data
//	    scaler += levelinc;
//	    }
//	    
//	unit->m_scaler = scaler;	
//}
//
//void Balance_next_k(Balance* unit, int inNumSamples)
//{
//	float* insig = IN(0);
//	float testsig = IN0(1);
//	float* out = OUT(0);
//	float insum = 0.f;
//	float scaler = unit->m_scaler;
//	float newscale, levelinc;
//	
//	for(int i = 0; i < inNumSamples; i++){
//	    insum += powf(insig[i], 2);
//	    }
//	
//	float newinrms = sqrt(insum/inNumSamples);
//	float newtestrms = sqrt(powf(testsig, 2));
//	// protect a divide by zero... perhaps there is a better way to do this
//	if(newinrms <= 0.f) {
//	    newscale = 0.f; levelinc = 0.f;
//	    } else {
//	    newscale = newtestrms / newinrms;
//	    levelinc = CALCSLOPE(newscale, scaler);
//	    }
//		
//	for(int i = 0; i < inNumSamples; i++){
//	    out[i] = insig[i] * scaler;
//	    // overwrite old data with new data
//	    scaler += levelinc;
//	    }
//	    
//	unit->m_scaler = scaler;	
//}


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


////////////////////////////////////////////////////////////////////////////////////////////////////////

void load(InterfaceTable *inTable)
{
	ft = inTable;
	
	DefineDtorUnit(AtsSynth);
	DefineDtorUnit(AtsNoiSynth);
	DefineSimpleUnit(AtsFreq);
	DefineSimpleUnit(AtsAmp);
	DefineSimpleUnit(AtsNoise);
	DefineSimpleUnit(AtsParInfo);
	DefineSimpleUnit(SinTone);
	DefineSimpleCantAliasUnit(Maxamp);

	DefineSimpleUnit(LPCSynth);
	DefineSimpleUnit(LPCVals);
	DefineSimpleUnit(AudioMSG);
	DefineDtorUnit(PVSynth);
	DefineSimpleUnit(PVInfo);
	DefineSimpleCantAliasUnit(Balance);
	DefineSimpleUnit(MoogVCF);
	DefineSimpleUnit(PosRatio);
    
#define DefineDelayUnit(name) \
    (*ft->fDefineUnit)(#name, sizeof(name), (UnitCtorFunc)&name##_Ctor, \
    (UnitDtorFunc)&DelayUnit_Dtor, 0);
    
	DefineDelayUnit(CombLP);
	
}


	