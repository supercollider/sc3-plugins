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
#include <stdio.h>

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

struct Grain
{
	double phase, rate;
	double b1, y1, y2; // envelope
	int counter;
	int bufnum;
	int interp;
	SndBuf *m_buf;
};

struct WinGrain
{
	double phase, rate;
	double winPos, winInc; // tells the grain where to look in the winBuf for an amp value
	int counter;
	int bufnum;
	int interp;
	int mWindow;
	double curamp;
	SndBuf *m_buf;
};

struct WinGrainI
{
	double phase, rate;
	int counter;
	int bufnum;
	int interp;
	int mWindowA, mWindowB; // the two windows to interpolate between
	double winPosA, winIncA, winPosB, winIncB; // tells the grain where to look in the winBuf for an amp value
	float ifac; // the interpolation factor between the two windows
	double curamp;
	SndBuf *m_buf;
};

const int kMaxGrains = 64;

const int kMaxSynthGrains = 512;

struct MonoGrain : public Unit
{
	int mNumActive;
	int m_NextGrain;
	Grain mGrains[kMaxSynthGrains];
};

struct Grainbf
{
	double phase, rate;
	double b1, y1, y2; // envelope
	int counter;
	int bufnum;
	int interp;
	double loc, elevation, rho;
	float m_azimuth, m_elevation, m_rho, m_level, m_W_amp, m_X_amp, m_Y_amp, m_Z_amp;

};

struct MonoGrainBF : public Unit
{
	int mNumActive;
	int m_NextGrain;
	Grainbf mGrains[kMaxSynthGrains];
};

// for granular synthesis
struct IGrain
{
	double b1, y1, y2; // envelope
	int counter;
};

struct InGrain : public Unit
{
	int mNumActive;
	float curtrig;
	IGrain mGrains[kMaxSynthGrains];
};

struct IGrainB
{
	double curamp;
	int counter;
	int mWindow;
	double winPos, winInc; // tells the grain where to look in the winBuf for an amp value
};

struct InGrainB : public Unit
{
	int mNumActive;
	float curtrig;
	IGrainB mGrains[kMaxSynthGrains];
};

struct IGrainI
{
	double curamp;
	int counter;
	int mWindowA, mWindowB; // the two windows to interpolate between
	double winPosA, winIncA, winPosB, winIncB; // tells the grain where to look in the winBuf for an amp value
	float ifac; // the interpolation factor between the two windows
};

struct InGrainI : public Unit
{
	int mNumActive;
	float curtrig;
	IGrainI mGrains[kMaxSynthGrains];
};

struct SGrain
{
	double b1, y1, y2; // envelope
	int counter;
	int32 oscphase; // the phase of the osc inside this grain
	int32 freq; // the freq of the osc inside this grain in phase inc
};

struct SinGrain : public Unit
{
	int mNumActive;
	uint32 m_lomask;
	float curtrig;
	double m_cpstoinc, m_radtoinc;
	SGrain mGrains[kMaxSynthGrains];
};

struct SGrainB
{
	int32 oscphase; // the phase of the osc inside this grain
	int32 freq; // the freq of the osc inside this grain in phase inc
	double curamp;
	int counter;
	int mWindow;
	double winPos, winInc; // tells the grain where to look in the winBuf for an amp value
};

struct SinGrainB : public Unit
{
	int mNumActive;
	uint32 m_lomask;
	float curtrig;
	double m_cpstoinc, m_radtoinc;
	SGrainB mGrains[kMaxSynthGrains];
};

struct SGrainI
{
	int32 oscphase; // the phase of the osc inside this grain
	int32 freq; // the freq of the osc inside this grain in phase inc
	double curamp;
	int counter;
	int mWindowA, mWindowB; // the two windows to interpolate between
	double winPosA, winIncA, winPosB, winIncB; // tells the grain where to look in the winBuf for an amp value
	float ifac; // the interpolation factor between the two windows
};

struct SinGrainI : public Unit
{
	int mNumActive;
	uint32 m_lomask;
	float curtrig;
	double m_cpstoinc, m_radtoinc;
	SGrainI mGrains[kMaxSynthGrains];
};

struct FGrain
{
	int32 coscphase, moscphase; // the phase of the osc inside this grain
	int32 mfreq; // the freq of the osc inside this grain in phase inc
	double b1, y1, y2; // envelope
	float deviation, carbase;
	int counter;
};


struct FMGrain : public Unit
{
	int mNumActive;
	uint32 m_lomask;
	float curtrig;
	double m_cpstoinc, m_radtoinc;
	FGrain mGrains[kMaxSynthGrains];
};


struct FGrainB
{
	int32 coscphase, moscphase; // the phase of the osc inside this grain
	int32 mfreq; // the freq of the osc inside this grain in phase inc
	double curamp;
	float deviation, carbase;
	int counter;
	int mWindow;
	double winPos, winInc; // tells the grain where to look in the winBuf for an amp value
};

struct FMGrainB : public Unit
{
	int mNumActive;
	uint32 m_lomask;
	float curtrig;
	double m_cpstoinc, m_radtoinc;
	FGrainB mGrains[kMaxSynthGrains];
};


struct FGrainI
{
	int32 coscphase, moscphase; // the phase of the osc inside this grain
	int32 mfreq; // the freq of the osc inside this grain in phase inc
	double curamp;
	float deviation, carbase;
	int counter;
	int mWindowA, mWindowB; // the two windows to interpolate between
	double winPosA, winIncA, winPosB, winIncB; // tells the grain where to look in the winBuf for an amp value
	float ifac; // the interpolation factor between the two windows
};

struct FMGrainI : public Unit
{
	int mNumActive;
	uint32 m_lomask;
	float curtrig;
	double m_cpstoinc, m_radtoinc;
	FGrainI mGrains[kMaxSynthGrains];
};

struct BufGrain : public Unit
{
	int mNumActive;
	float curtrig;
	Grain mGrains[kMaxSynthGrains];
};

struct BufGrainB : public Unit
{
	int mNumActive;
	float curtrig;
	WinGrain mGrains[kMaxSynthGrains];
};

struct BufGrainI : public Unit
{
	int mNumActive;
	float curtrig;
	WinGrainI mGrains[kMaxSynthGrains];
};

///////////////////// Ambisonic versions of grain UGens ////////////////////

// for granular synthesis
struct IGrainBF
{
	double b1, y1, y2; // envelope
	int counter;
	float m_wamp, m_xamp, m_yamp, m_zamp;
};

struct InGrainBF : public Unit
{
	int mNumActive;
	float curtrig;
	float m_wComp;
	IGrainBF mGrains[kMaxSynthGrains];

};

struct IGrainBBF
{
	double curamp;
	int counter;
	int mWindow;
	double winPos, winInc; // tells the grain where to look in the winBuf for an amp value
	float m_wamp, m_xamp, m_yamp, m_zamp;

};

struct InGrainBBF : public Unit
{
	int mNumActive;
	float curtrig;
	float m_wComp;
	IGrainBBF mGrains[kMaxSynthGrains];
};

struct IGrainIBF
{
	double curamp;
	int counter;
	int mWindowA, mWindowB; // the two windows to interpolate between
	double winPosA, winIncA, winPosB, winIncB; // tells the grain where to look in the winBuf for an amp value
	float ifac; // the interpolation factor between the two windows
	float m_wamp, m_xamp, m_yamp, m_zamp;
};

struct InGrainIBF : public Unit
{
	int mNumActive;
	float curtrig;
	float m_wComp;
	IGrainIBF mGrains[kMaxSynthGrains];
};

struct SGrainBF
{
	double b1, y1, y2; // envelope
	int counter;
	int32 oscphase; // the phase of the osc inside this grain
	int32 freq; // the freq of the osc inside this grain in phase inc
	float m_wamp, m_xamp, m_yamp, m_zamp;

};

struct SinGrainBF : public Unit
{
	int mNumActive;
	uint32 m_lomask;
	float curtrig;
	double m_cpstoinc, m_radtoinc;
	float m_wComp;
	SGrainBF mGrains[kMaxSynthGrains];
};

struct SGrainBBF
{
	int32 oscphase; // the phase of the osc inside this grain
	int32 freq; // the freq of the osc inside this grain in phase inc
	double curamp;
	int counter;
	int mWindow;
	double winPos, winInc; // tells the grain where to look in the winBuf for an amp value
	float m_wamp, m_xamp, m_yamp, m_zamp;
};

struct SinGrainBBF : public Unit
{
	int mNumActive;
	uint32 m_lomask;
	float curtrig;
	double m_cpstoinc, m_radtoinc;
	float m_wComp;
	SGrainBBF mGrains[kMaxSynthGrains];
};

struct SGrainIBF
{
	int32 oscphase; // the phase of the osc inside this grain
	int32 freq; // the freq of the osc inside this grain in phase inc
	double curamp;
	int counter;
	int mWindowA, mWindowB; // the two windows to interpolate between
	double winPosA, winIncA, winPosB, winIncB; // tells the grain where to look in the winBuf for an amp value
	float ifac; // the interpolation factor between the two windows
	float m_wamp, m_xamp, m_yamp, m_zamp;

};

struct SinGrainIBF : public Unit
{
	int mNumActive;
	uint32 m_lomask;
	float curtrig;
	double m_cpstoinc, m_radtoinc;
	float m_wComp;
	SGrainIBF mGrains[kMaxSynthGrains];
};


struct FGrainBF
{
	int32 coscphase, moscphase; // the phase of the osc inside this grain
	int32 mfreq; // the freq of the osc inside this grain in phase inc
	double b1, y1, y2; // envelope
	float deviation, carbase;
	int counter;
	float m_wamp, m_xamp, m_yamp, m_zamp;
};


struct FMGrainBF : public Unit
{
	int mNumActive;
	uint32 m_lomask;
	float curtrig;
	double m_cpstoinc, m_radtoinc;
	float m_wComp;
	FGrainBF mGrains[kMaxSynthGrains];
};


struct FGrainBBF
{
	int32 coscphase, moscphase; // the phase of the osc inside this grain
	int32 mfreq; // the freq of the osc inside this grain in phase inc
	double curamp;
	float deviation, carbase;
	int counter;
	int mWindow;
	double winPos, winInc; // tells the grain where to look in the winBuf for an amp value
	float m_wamp, m_xamp, m_yamp, m_zamp;
};

struct FMGrainBBF : public Unit
{
	int mNumActive;
	uint32 m_lomask;
	float curtrig;
	double m_cpstoinc, m_radtoinc;
	float m_wComp;
	FGrainBBF mGrains[kMaxSynthGrains];
};


struct FGrainIBF
{
	int32 coscphase, moscphase; // the phase of the osc inside this grain
	int32 mfreq; // the freq of the osc inside this grain in phase inc
	double curamp;
	float deviation, carbase;
	int counter;
	int mWindowA, mWindowB; // the two windows to interpolate between
	double winPosA, winIncA, winPosB, winIncB; // tells the grain where to look in the winBuf for an amp value
	float ifac; // the interpolation factor between the two windows
	float m_wamp, m_xamp, m_yamp, m_zamp;
};

struct FMGrainIBF : public Unit
{
	int mNumActive;
	uint32 m_lomask;
	float curtrig;
	double m_cpstoinc, m_radtoinc;
	float m_wComp;
	FGrainIBF mGrains[kMaxSynthGrains];
};

struct GrainBF
{
	double phase, rate;
	double b1, y1, y2; // envelope
	int counter;
	int bufnum;
	int interp;
	float m_wamp, m_xamp, m_yamp, m_zamp;
	SndBuf *m_buf;
};

struct WinGrainBF
{
	double phase, rate;
	double winPos, winInc; // tells the grain where to look in the winBuf for an amp value
	int counter;
	int bufnum;
	int interp;
	int mWindow;
	double curamp;
	float m_wamp, m_xamp, m_yamp, m_zamp;
	SndBuf *m_buf;
};

struct WinGrainIBF
{
	double phase, rate;
	int counter;
	int bufnum;
	int interp;
	int mWindowA, mWindowB; // the two windows to interpolate between
	double winPosA, winIncA, winPosB, winIncB; // tells the grain where to look in the winBuf for an amp value
	float ifac; // the interpolation factor between the two windows
	double curamp;
	float m_wamp, m_xamp, m_yamp, m_zamp;
	SndBuf *m_buf;
};

struct BufGrainBF : public Unit
{
	int mNumActive;
	float curtrig;
	float m_wComp;
	GrainBF mGrains[kMaxSynthGrains];
};

struct BufGrainBBF : public Unit
{
	int mNumActive;
	float curtrig;
	float m_wComp;
	WinGrainBF mGrains[kMaxSynthGrains];
};

struct BufGrainIBF : public Unit
{
	int mNumActive;
	float curtrig;
	float m_wComp;
	WinGrainIBF mGrains[kMaxSynthGrains];
};
////////////////////////////////////////////////////////////////////////

extern "C"
{
	void load(InterfaceTable *inTable);

	void MonoGrain_next(MonoGrain *unit, int inNumSamples);
	void MonoGrain_Ctor(MonoGrain* unit);

	void MonoGrainBF_next(MonoGrainBF *unit, int inNumSamples);
	void MonoGrainBF_Ctor(MonoGrainBF* unit);

	void SinGrain_Ctor(SinGrain* unit);
	void SinGrain_next_a(SinGrain* unit, int inNumSamples);
	void SinGrain_next_k(SinGrain* unit, int inNumSamples);

	void SinGrainB_Ctor(SinGrainB* unit);
	void SinGrainB_next_a(SinGrainB* unit, int inNumSamples);
	void SinGrainB_next_k(SinGrainB* unit, int inNumSamples);

	void SinGrainI_Ctor(SinGrainI* unit);
	void SinGrainI_next_a(SinGrainI* unit, int inNumSamples);
	void SinGrainI_next_k(SinGrainI* unit, int inNumSamples);

	void InGrain_Ctor(InGrain* unit);
	void InGrain_next_a(InGrain* unit, int inNumSamples);
	void InGrain_next_k(InGrain* unit, int inNumSamples);

	void InGrainB_Ctor(InGrainB* unit);
	void InGrainB_next_a(InGrainB* unit, int inNumSamples);
	void InGrainB_next_k(InGrainB* unit, int inNumSamples);

	void InGrainI_Ctor(InGrainI* unit);
	void InGrainI_next_a(InGrainI* unit, int inNumSamples);
	void InGrainI_next_k(InGrainI* unit, int inNumSamples);

	void FMGrain_Ctor(FMGrain* unit);
	void FMGrain_next_a(FMGrain* unit, int inNumSamples);
	void FMGrain_next_k(FMGrain* unit, int inNumSamples);

	void FMGrainB_Ctor(FMGrainB* unit);
	void FMGrainB_next_a(FMGrainB* unit, int inNumSamples);
	void FMGrainB_next_k(FMGrainB* unit, int inNumSamples);

	void FMGrainI_Ctor(FMGrainI* unit);
	void FMGrainI_next_a(FMGrainI* unit, int inNumSamples);
	void FMGrainI_next_k(FMGrainI* unit, int inNumSamples);

	void BufGrain_Ctor(BufGrain* unit);
	void BufGrain_next_a(BufGrain *unit, int inNumSamples);
	void BufGrain_next_k(BufGrain *unit, int inNumSamples);

	void BufGrainB_Ctor(BufGrainB* unit);
	void BufGrainB_next_a(BufGrainB *unit, int inNumSamples);
	void BufGrainB_next_k(BufGrainB *unit, int inNumSamples);

	void BufGrainI_Ctor(BufGrainI* unit);
	void BufGrainI_next_a(BufGrainI *unit, int inNumSamples);
	void BufGrainI_next_k(BufGrainI *unit, int inNumSamples);

	/////// Ambisonic Versions of grains /////////////////

	void SinGrainBF_Ctor(SinGrainBF* unit);
	void SinGrainBF_next_a(SinGrainBF* unit, int inNumSamples);
	void SinGrainBF_next_k(SinGrainBF* unit, int inNumSamples);

	void SinGrainBBF_Ctor(SinGrainBBF* unit);
	void SinGrainBBF_next_a(SinGrainBBF* unit, int inNumSamples);
	void SinGrainBBF_next_k(SinGrainBBF* unit, int inNumSamples);

	void SinGrainIBF_Ctor(SinGrainIBF* unit);
	void SinGrainIBF_next_a(SinGrainIBF* unit, int inNumSamples);
	void SinGrainIBF_next_k(SinGrainIBF* unit, int inNumSamples);

	void FMGrainBF_Ctor(FMGrainBF* unit);
	void FMGrainBF_next_a(FMGrainBF* unit, int inNumSamples);
	void FMGrainBF_next_k(FMGrainBF* unit, int inNumSamples);

	void FMGrainBBF_Ctor(FMGrainBBF* unit);
	void FMGrainBBF_next_a(FMGrainBBF* unit, int inNumSamples);
	void FMGrainBBF_next_k(FMGrainBBF* unit, int inNumSamples);

	void FMGrainIBF_Ctor(FMGrainIBF* unit);
	void FMGrainIBF_next_a(FMGrainIBF* unit, int inNumSamples);
	void FMGrainIBF_next_k(FMGrainIBF* unit, int inNumSamples);

	void InGrainBF_Ctor(InGrainBF* unit);
	void InGrainBF_next_a(InGrainBF* unit, int inNumSamples);
	void InGrainBF_next_k(InGrainBF* unit, int inNumSamples);

	void InGrainIBF_Ctor(InGrainIBF* unit);
	void InGrainIBF_next_a(InGrainIBF* unit, int inNumSamples);
	void InGrainIBF_next_k(InGrainIBF* unit, int inNumSamples);

	void InGrainBBF_Ctor(InGrainBBF* unit);
	void InGrainBBF_next_a(InGrainBBF* unit, int inNumSamples);
	void InGrainBBF_next_k(InGrainBBF* unit, int inNumSamples);

	void BufGrainBF_Ctor(BufGrainBF* unit);
	void BufGrainBF_next_a(BufGrainBF *unit, int inNumSamples);
	void BufGrainBF_next_k(BufGrainBF *unit, int inNumSamples);

	void BufGrainBBF_Ctor(BufGrainBBF* unit);
	void BufGrainBBF_next_a(BufGrainBBF *unit, int inNumSamples);
	void BufGrainBBF_next_k(BufGrainBBF *unit, int inNumSamples);

	void BufGrainIBF_Ctor(BufGrainIBF* unit);
	void BufGrainIBF_next_a(BufGrainIBF *unit, int inNumSamples);
	void BufGrainIBF_next_k(BufGrainIBF *unit, int inNumSamples);

	// for standard dist /////////
	/*
	void GrainIn_Ctor(GrainIn* unit);
	void GrainIn_next_a(GrainIn *unit, int inNumSamples);
	void GrainIn_next_k(GrainIn *unit, int inNumSamples);

	void GrainSin_Ctor(GrainSin* unit);
	void GrainSin_next_a(GrainSin *unit, int inNumSamples);
	void GrainSin_next_k(GrainSin *unit, int inNumSamples);

	void GrainFM_Ctor(GrainFM* unit);
	void GrainFM_next_a(GrainFM *unit, int inNumSamples);
	void GrainFM_next_k(GrainFM *unit, int inNumSamples);

	void GrainBuf_Ctor(GrainBuf* unit);
	void GrainBuf_next_a(GrainBuf *unit, int inNumSamples);
	void GrainBuf_next_k(GrainBuf *unit, int inNumSamples);
	*/
	// end //
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// Granular UGens ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

//static float cubicinterp(float x, float y0, float y1, float y2, float y3)
//{
//	// 4-point, 3rd-order Hermite (x-form)
//	float c0 = y1;
//	float c1 = 0.5f * (y2 - y0);
//	float c2 = y0 - 2.5f * y1 + 2.f * y2 - 0.5f * y3;
//	float c3 = 0.5f * (y3 - y0) + 1.5f * (y1 - y2);
//
//	return ((c3 * x + c2) * x + c1) * x + c0;
//}

inline float IN_AT(Unit* unit, int index, int offset)
{
	if (INRATE(index) == calc_FullRate) return IN(index)[offset];
	if (INRATE(index) == calc_DemandRate) return DEMANDINPUT(index);
	return ZIN0(index);
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

// was 	SndBuf *buf = bufs + bufnum;
#define SETUP_GRAIN_BUF \
	if (bufnum >= world->mNumSndBufs) { \
		int localBufNum = bufnum - world->mNumSndBufs; \
		Graph *parent = unit->mParent; \
		if(localBufNum <= parent->localBufNum) { \
			grain->m_buf = parent->mLocalSndBufs + localBufNum; \
		} else { \
			bufnum = 0; \
			grain->m_buf = world->mSndBufs + bufnum; \
		} \
	} else { \
		grain->m_buf = world->mSndBufs + bufnum; \
	} \

#define GRAIN_BUF \
	SndBuf *buf = grain->m_buf; \
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

#define GRAIN_LOOP_BODY_4 \
		float amp = y1 * y1; \
		phase = sc_gloop(phase, loopMax); \
		int32 iphase = (int32)phase; \
		float* table1 = bufData + iphase; \
		float* table0 = table1 - 1; \
		float* table2 = table1 + 1; \
		float* table3 = table1 + 2; \
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
		float a = table0[0]; \
		float b = table1[0]; \
		float c = table2[0]; \
		float d = table3[0]; \
		float outval = amp * cubicinterp(fracphase, a, b, c, d); \
		ZXP(out1) += outval; \
		double y0 = b1 * y1 - y2; \
		y2 = y1; \
		y1 = y0; \


#define GRAIN_LOOP_BODY_2 \
		float amp = y1 * y1; \
		phase = sc_gloop(phase, loopMax); \
		int32 iphase = (int32)phase; \
		float* table1 = bufData + iphase; \
		float* table2 = table1 + 1; \
		if (iphase > guardFrame) { \
			table2 -= bufSamples; \
		} \
		float fracphase = phase - (double)iphase; \
		float b = table1[0]; \
		float c = table2[0]; \
		float outval = amp * (b + fracphase * (c - b)); \
		ZXP(out1) += outval; \
		double y0 = b1 * y1 - y2; \
		y2 = y1; \
		y1 = y0; \


#define GRAIN_LOOP_BODY_1 \
		float amp = y1 * y1; \
		phase = sc_gloop(phase, loopMax); \
		int32 iphase = (int32)phase; \
		float outval = amp * bufData[iphase]; \
		ZXP(out1) += outval; \
		double y0 = b1 * y1 - y2; \
		y2 = y1; \
		y1 = y0; \


#define GET_GRAIN_WIN \
		SndBuf *window = unit->mWorld->mSndBufs + (int)grain->mWindow; \
		float *windowData __attribute__((__unused__)) = window->data; \
		uint32 windowSamples __attribute__((__unused__)) = window->samples; \
		uint32 windowFrames = window->frames; \
		int windowGuardFrame __attribute__((__unused__)) = windowFrames - 1; \

#define BUF_GRAIN_AMP \
		winPos += winInc; \
		int iWinPos = (int)winPos; \
		double winFrac = winPos - (double)iWinPos;\
		float* winTable1 = windowData + iWinPos;\
		float* winTable2 = winTable1 + 1;\
		if (winPos > windowGuardFrame) {\
		    winTable2 -= windowSamples; \
		    } \
		amp = lininterp(winFrac, winTable1[0], winTable2[0]); \

#define GET_INTERP_GRAIN_WIN \
		SndBuf *windowA = unit->mWorld->mSndBufs + (int)grain->mWindowA; \
		float *windowDataA __attribute__((__unused__)) = windowA->data; \
		uint32 windowSamplesA __attribute__((__unused__)) = windowA->samples; \
		uint32 windowFramesA = windowA->frames; \
		int windowGuardFrameA __attribute__((__unused__)) = windowFramesA - 1; \
		SndBuf *windowB = unit->mWorld->mSndBufs + (int)grain->mWindowB; \
		float *windowDataB __attribute__((__unused__)) = windowB->data; \
		uint32 windowSamplesB __attribute__((__unused__)) = windowB->samples; \
		uint32 windowFramesB = windowB->frames; \
		int windowGuardFrameB __attribute__((__unused__)) = windowFramesB - 1; \

#define BUF_INTERP_GRAIN_AMP \
		winPosA += winIncA; \
		int iWinPosA = (int)winPosA; \
		double winFracA = winPosA - (double)iWinPosA;\
		float* winTableA1 = windowDataA + iWinPosA;\
		float* winTableA2 = winTableA1 + 1;\
		if (winPosA > windowGuardFrameA) {\
		    winTableA2 -= windowSamplesA; \
		    } \
		float ampA = lininterp(winFracA, winTableA1[0], winTableA2[0]); \
		winPosB += winIncB; \
		int iWinPosB = (int)winPosB; \
		double winFracB = winPosB - (double)iWinPosB;\
		float* winTableB1 = windowDataB + iWinPosB;\
		float* winTableB2 = winTableB1 + 1;\
		if (winPosB > windowGuardFrameB) {\
		    winTableB2 -= windowSamplesB; \
		    } \
		float ampB = lininterp(winFracB, winTableB1[0], winTableB2[0]); \
		amp = lininterp(grain->ifac, ampA, ampB);\

#define BUF_GRAIN_LOOP_BODY_4 \
		phase = sc_gloop(phase, loopMax); \
		int32 iphase = (int32)phase; \
		float* table1 = bufData + iphase; \
		float* table0 = table1 - 1; \
		float* table2 = table1 + 1; \
		float* table3 = table1 + 2; \
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
		float a = table0[0]; \
		float b = table1[0]; \
		float c = table2[0]; \
		float d = table3[0]; \
		float outval = amp * cubicinterp(fracphase, a, b, c, d); \
		ZXP(out1) += outval; \

#define BUF_GRAIN_LOOP_BODY_2 \
		phase = sc_gloop(phase, loopMax); \
		int32 iphase = (int32)phase; \
		float* table1 = bufData + iphase; \
		float* table2 = table1 + 1; \
		if (iphase > guardFrame) { \
			table2 -= bufSamples; \
		} \
		float fracphase = phase - (double)iphase; \
		float b = table1[0]; \
		float c = table2[0]; \
		float outval = amp * (b + fracphase * (c - b)); \
		ZXP(out1) += outval; \

// amp needs to be calculated by looking up values in window

#define BUF_GRAIN_LOOP_BODY_1 \
		phase = sc_gloop(phase, loopMax); \
		int32 iphase = (int32)phase; \
		float outval = amp * bufData[iphase]; \
		ZXP(out1) += outval; \

////////////////////////////////////////////////////////////////////////////////////

void MonoGrain_next(MonoGrain *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);
	float *out = OUT(0);
	float *in = IN(0);
	float winSize = IN0(1);
	float grainFreq = IN0(2);
	float randSize = IN0(3);
	for (int i=0; i < unit->mNumActive; ) {
		Grain *grain = unit->mGrains + i;
		double b1 = grain->b1;
		double y1 = grain->y1;
		double y2 = grain->y2;
		int nsmps = sc_min(grain->counter, inNumSamples);
		for (int j=0; j<nsmps; ++j) {
		    float amp = y1 * y1;
		    float outval = amp * in[j];
		    out[j] += outval;
		    double y0 = b1 * y1 - y2;
		    y2 = y1;
		    y1 = y0;
		    }
		grain->y1 = y1;
		grain->y2 = y2;
		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	}
	int nextGrain = unit->m_NextGrain;
	for (int i=0; i<inNumSamples; ++i) {
		--nextGrain;
		if (nextGrain <= 0) {
			// start a grain
			if (unit->mNumActive+1 >= kMaxSynthGrains) break;
			Grain *grain = unit->mGrains + unit->mNumActive++;
			RGET
//			s1 = trand(s1, s2, s3);
//			s2 = trand(s2, s3, s1);
//			s3 = trand(s3, s1, s2);
			if (grainFreq <= 0.0) {printf("GrainFreq must be greater than 0\n"); break;}
			double counter = winSize * SAMPLERATE;
			double winrandamt = frand2(s1, s2, s3) * randSize;
			counter = floor(counter + (counter * winrandamt));
			counter = sc_max(4., counter);
			grain->counter = (int)counter;
			nextGrain = (int)(SAMPLERATE / grainFreq);
			double w = pi / counter;
			double b1 = grain->b1 = 2. * cos(w);
			double y1 = sin(w);
			double y2 = 0.;
			int nsmps = sc_min(grain->counter, inNumSamples - i);
			for (int j=0; j<nsmps; ++j) {
			    float amp = y1 * y1;
			    float outval = amp * in[j];
			    out[j] += outval;
			    double y0 = b1 * y1 - y2;
			    y2 = y1;
			    y1 = y0;
			}
			grain->y1 = y1;
			grain->y2 = y2;
			// store random values
			RPUT
			// end change
			grain->counter -= nsmps;
			if (grain->counter <= 0) {
				// remove grain
				*grain = unit->mGrains[--unit->mNumActive];
			}
		}
	}
	unit->m_NextGrain = nextGrain;
}

void MonoGrain_Ctor(MonoGrain *unit)
{
	SETCALC(MonoGrain_next);
	unit->mNumActive = 0;
	unit->m_NextGrain = 1;
	ClearUnitOutputs(unit, 1);
}

// a BFormat version of the previous MonoGrain
void MonoGrainBF_next(MonoGrainBF *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);
	float *Wout = OUT(0);
	float *Xout = OUT(1);
	float *Yout = OUT(2);
	float *Zout = OUT(3);
	float *in = IN(0);

	for (int i=0; i < unit->mNumActive; ) {
		Grainbf *grainbf = unit->mGrains + i;
		double b1 = grainbf->b1;
		double y1 = grainbf->y1;
		double y2 = grainbf->y2;
		float W_amp = grainbf->m_W_amp;
		float X_amp = grainbf->m_X_amp;
		float Y_amp = grainbf->m_Y_amp;
		float Z_amp = grainbf->m_Z_amp;
		int nsmps = sc_min(grainbf->counter, inNumSamples);
		for (int j=0; j<nsmps; ++j) {
		    float amp = y1 * y1;
		    float outval = amp * in[j];
		    // do the BFormat encoding on the resultant signal
		    Wout[j] += outval * W_amp;
		    Xout[j] += outval * X_amp;
		    Yout[j] += outval * Y_amp;
		    Zout[j] += outval * Z_amp;
		    double y0 = b1 * y1 - y2;
		    y2 = y1;
		    y1 = y0;
		    }
		grainbf->y1 = y1;
		grainbf->y2 = y2;
		grainbf->counter -= nsmps;
		if (grainbf->counter <= 0) {
			// remove grain
			*grainbf = unit->mGrains[--unit->mNumActive];
		} else ++i;
	}
	int nextGrain = unit->m_NextGrain;
	if (nextGrain > inNumSamples) {
	    unit->m_NextGrain -=inNumSamples;
	    } else {
	    for (int i=0; i<inNumSamples; ++i) {
		    --nextGrain;
		    if (nextGrain <= 0) {
			    // start a grain
			    if (unit->mNumActive+1 >= kMaxSynthGrains) break;
			    float winSize = IN_AT(unit, 1, i);
			    float grainFreq = IN_AT(unit, 2, i);
			    float randSize = IN_AT(unit, 3, i);
			    float loc = IN_AT(unit, 4, i); // same as azimuth
			    float locrand = IN_AT(unit, 5, i);
			    float elevation = IN_AT(unit, 6, i);
			    float elevationrand = IN_AT(unit, 7, i);
			    float rho = IN_AT(unit, 8, i); // place at the speaker radius for now
			    float sinint, cosint;
			    Grainbf *grainbf = unit->mGrains + unit->mNumActive++;
			    RGET
			    if (grainFreq <= 0.0) {printf("GrainFreq must be greater than 0\n"); break;}
			    double counter = winSize * SAMPLERATE;
			    double ranval = frand2(s1, s2, s3);
			    double winrandamt = ranval * randSize;
			    counter = floor(counter + (counter * winrandamt));
			    counter = sc_max(4., counter);
			    grainbf->counter = (int)counter;
			    nextGrain = (int)(SAMPLERATE / grainFreq);
			    double w = pi / counter;
			    double b1 = grainbf->b1 = 2. * cos(w);
			    double y1 = sin(w);
			    double y2 = 0.;
			    double thisloc = loc + (locrand * (ranval * pi));
			    double thiselevation = elevation + (elevationrand * (ranval * 0.5 * pi));
			    float *Wout1 = Wout + i;
			    float *Xout1 = Xout + i;
			    float *Yout1 = Yout + i;
			    float *Zout1 = Zout + i;
			    float sina = sin(thisloc);
			    float sinb = sin(thiselevation);
			    float cosa = cos(thisloc);
			    float cosb = cos(thiselevation);
			    if(rho >= 1)
				{sinint = (rsqrt2 * (sin(0.78539816339745 * 1.0))) / pow(rho, (float)1.5);
				cosint =  (rsqrt2 * (cos(0.78539816339745 * 1.0))) / pow(rho, (float)1.5);}
			    else
				{sinint = rsqrt2 * (sin(0.78539816339745 * rho));
				cosint = rsqrt2 * (cos(0.78539816339745 * rho));};
			    float W_amp = rsqrt2 * cosint;
			    float X_amp = cosa * cosb * sinint;
			    float Y_amp = sina * cosb * sinint;
			    float Z_amp = sinb * sinint;
			    int nsmps = sc_min(grainbf->counter, inNumSamples - i);
			    for (int j=0; j<nsmps; ++j) {
				float amp = y1 * y1;
				float outval = amp * in[j];
				// do the BFormat encoding on the resultant signal
				Wout1[j] += outval * W_amp;
				Xout1[j] += outval * X_amp;
				Yout1[j] += outval * Y_amp;
				Zout1[j] += outval * Z_amp;
				double y0 = b1 * y1 - y2;
				y2 = y1;
				y1 = y0;
			    }
			    grainbf->y1 = y1;
			    grainbf->y2 = y2;
			    grainbf->m_W_amp = W_amp;
			    grainbf->m_X_amp = X_amp;
			    grainbf->m_Y_amp = Y_amp;
			    grainbf->m_Z_amp = Z_amp;
			    // store random values
			    RPUT
			    grainbf->counter -= nsmps;
			    if (grainbf->counter <= 0) {
				    // remove grain
				    *grainbf = unit->mGrains[--unit->mNumActive];
				}
			}
		}
	    unit->m_NextGrain = nextGrain;
	    }
}

void MonoGrainBF_Ctor(MonoGrainBF *unit)
{
	SETCALC(MonoGrainBF_next);
	unit->mNumActive = 0;
	unit->m_NextGrain = 1;
	ClearUnitOutputs(unit, 1);
}

///////////////////////////////////////////////////////////////////////////////
/* granular synthesis UGens - SinOsc / FM / Buffer / In */
///////////////////////////////////SinGrain ///////////////////////////////////

void SinGrain_next_a(SinGrain *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);
	float *out = OUT(0);
	float *trig = IN(0);
	float winSize, freq;
	float *table0 = ft->mSineWavetable;
	float *table1 = table0 + 1;

	for (int i=0; i < unit->mNumActive; ) {
		SGrain *grain = unit->mGrains + i;
		double b1 = grain->b1;
		double y1 = grain->y1;
		double y2 = grain->y2;
		int32 thisfreq = grain->freq;
		int32 oscphase = grain->oscphase;

		int nsmps = sc_min(grain->counter, inNumSamples);
		for (int j=0; j<nsmps; ++j) {
		    float amp = y1 * y1;
		    float outval = amp * lookupi1(table0, table1, oscphase, unit->m_lomask);
		    out[j] += outval;
		    double y0 = b1 * y1 - y2;
		    y2 = y1;
		    y1 = y0;
		    oscphase += thisfreq;
		    }
		grain->y1 = y1;
		grain->y2 = y2;
		grain->oscphase = oscphase;
		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	}

	for (int i=0; i<inNumSamples; ++i) {
		if ((unit->curtrig <= 0) && (trig[i] > 0.0)) {
			// start a grain
			if (unit->mNumActive+1 >= kMaxSynthGrains) {Print("Too many grains!\n"); break;}
			SGrain *grain = unit->mGrains + unit->mNumActive++;
			// INRATE(1) == calcFullRate
			freq = IN_AT(unit, 2, i);
			winSize = IN_AT(unit, 1, i);
			int32 thisfreq = grain->freq = (int32)(unit->m_cpstoinc * freq);
			int32 oscphase = 0;
			double counter = winSize * SAMPLERATE;
			counter = sc_max(4., counter);
			grain->counter = (int)counter;
			double w = pi / counter;
			double b1 = grain->b1 = 2. * cos(w);
			double y1 = sin(w);
			double y2 = 0.;
			float *out1 = out + i;
			int nsmps = sc_min(grain->counter, inNumSamples - i);
			for (int j=0; j<nsmps; ++j) {
			    float amp = y1 * y1;
			    float outval = amp * lookupi1(table0, table1, oscphase, unit->m_lomask);
			    out1[j] += outval;
			    double y0 = b1 * y1 - y2;
			    y2 = y1;
			    y1 = y0;
			    oscphase += thisfreq;
			}
			grain->oscphase = oscphase;
			grain->y1 = y1;
			grain->y2 = y2;
			grain->counter -= nsmps;
			if (grain->counter <= 0) {
				// remove grain
				*grain = unit->mGrains[--unit->mNumActive];
			}
		}
		unit->curtrig = trig[i];
	}
}

void SinGrain_next_k(SinGrain *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);
	float *out = OUT(0);
	float trig = IN0(0);
	float winSize, freq;
	float *table0 = ft->mSineWavetable;
	float *table1 = table0 + 1;

	for (int i=0; i < unit->mNumActive; ) {
		SGrain *grain = unit->mGrains + i;
		double b1 = grain->b1;
		double y1 = grain->y1;
		double y2 = grain->y2;
		int32 thisfreq = grain->freq;
		int32 oscphase = grain->oscphase;

		int nsmps = sc_min(grain->counter, inNumSamples);
		for (int j=0; j<nsmps; ++j) {
		    float amp = y1 * y1;
		    float outval = amp * lookupi1(table0, table1, oscphase, unit->m_lomask);
		    out[j] += outval;
		    double y0 = b1 * y1 - y2;
		    y2 = y1;
		    y1 = y0;
		    oscphase += thisfreq;
		    }
		grain->y1 = y1;
		grain->y2 = y2;
		grain->oscphase = oscphase;
		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	}

	if ((unit->curtrig <= 0) && (trig > 0.0)) {
		    // start a grain
		    if (unit->mNumActive+1 >= kMaxSynthGrains)
		    {
		    Print("Too many grains!\n");
		    } else {
		    SGrain *grain = unit->mGrains + unit->mNumActive++;
		    freq = IN0(2);
		    winSize = IN0(1);
		    int32 thisfreq = grain->freq = (int32)(unit->m_cpstoinc * freq);
		    int32 oscphase = 0;
		    double counter = winSize * SAMPLERATE;
		    counter = sc_max(4., counter);
		    grain->counter = (int)counter;
		    double w = pi / counter;
		    double b1 = grain->b1 = 2. * cos(w);
		    double y1 = sin(w);
		    double y2 = 0.;
			int nsmps = sc_min(grain->counter, inNumSamples);
			for (int j=0; j<nsmps; ++j) {
			    float amp = y1 * y1;
			    float outval = amp * lookupi1(table0, table1, oscphase, unit->m_lomask);
			    out[j] += outval;
			    double y0 = b1 * y1 - y2;
			    y2 = y1;
			    y1 = y0;
			    oscphase += thisfreq;
			}
			grain->oscphase = oscphase;
			grain->y1 = y1;
			grain->y2 = y2;
			grain->counter -= nsmps;
			if (grain->counter <= 0) {
				// remove grain
				*grain = unit->mGrains[--unit->mNumActive];
			}
		}
	    }
	    unit->curtrig = trig;
}


void SinGrain_Ctor(SinGrain *unit)
{

	if (INRATE(0) == calc_FullRate)
	    SETCALC(SinGrain_next_a);
	    else
	    SETCALC(SinGrain_next_k);
	int tableSizeSin = ft->mSineSize;
	unit->m_lomask = (tableSizeSin - 1) << 3;
	unit->m_radtoinc = tableSizeSin * (rtwopi * 65536.);
	unit->m_cpstoinc = tableSizeSin * SAMPLEDUR * 65536.;
	unit->curtrig = 0.f;
	unit->mNumActive = 0;
	SinGrain_next_k(unit, 1);
}

///////////////////////// SinGrainB ////////////////////////////

void SinGrainB_next_a(SinGrainB *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);
	float *out = OUT(0);
	float *trig = IN(0);
	float winSize, freq;
	float *table0 = ft->mSineWavetable;
	float *table1 = table0 + 1;

	for (int i=0; i < unit->mNumActive; ) {
		SGrainB *grain = unit->mGrains + i;
		int32 thisfreq = grain->freq;
		int32 oscphase = grain->oscphase;
		GET_GRAIN_WIN

		double winInc = grain->winInc;
		double winPos = grain->winPos;
		double amp = grain->curamp;
		int nsmps = sc_min(grain->counter, inNumSamples);
		for (int j=0; j<nsmps; ++j) {
		    float outval = amp * lookupi1(table0, table1, oscphase, unit->m_lomask);
		    out[j] += outval;
		    oscphase += thisfreq;
		    BUF_GRAIN_AMP
		    }
		grain->winPos = winPos;
		grain->curamp = amp;
		grain->oscphase = oscphase;
		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	}

	for (int i=0; i<inNumSamples; ++i) {
		if ((unit->curtrig <= 0) && (trig[i] > 0.0)) {
			// start a grain
			if (unit->mNumActive+1 >= kMaxSynthGrains) {Print("Too many grains!\n"); break;}
			SGrainB *grain = unit->mGrains + unit->mNumActive++;
			freq = IN_AT(unit, 2, i);
			winSize = IN_AT(unit, 1, i);
			grain->mWindow = (int)IN_AT(unit, 3, i);
			double winPos = grain->winPos = 0.f;
			GET_GRAIN_WIN
			double counter = winSize * SAMPLERATE;
			double winInc = grain->winInc = (double)windowSamples / counter;
			int32 thisfreq = grain->freq = (int32)(unit->m_cpstoinc * freq);
			int32 oscphase = 0;
			counter = sc_max(4., counter);
			grain->counter = (int)counter;
			double amp = windowData[0];
			float *out1 = out + i;
			int nsmps = sc_min(grain->counter, inNumSamples - i);
			for (int j=0; j<nsmps; ++j) {
			    float outval = amp * lookupi1(table0, table1, oscphase, unit->m_lomask);
			    out1[j] += outval;
			    oscphase += thisfreq;
			    BUF_GRAIN_AMP
			}
			grain->curamp = amp;
			grain->winPos = winPos;
			grain->oscphase = oscphase;
			grain->counter -= nsmps;
			if (grain->counter <= 0) {
				// remove grain
				*grain = unit->mGrains[--unit->mNumActive];
			}
		}
		unit->curtrig = trig[i];
	}
}

void SinGrainB_next_k(SinGrainB *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);
	float *out = OUT(0);
	float trig = IN0(0);
	float winSize, freq;
	float *table0 = ft->mSineWavetable;
	float *table1 = table0 + 1;

	for (int i=0; i < unit->mNumActive; ) {
		SGrainB *grain = unit->mGrains + i;
		double amp = grain->curamp;
		int32 thisfreq = grain->freq;
		int32 oscphase = grain->oscphase;
		GET_GRAIN_WIN
		double winInc = grain->winInc;
		double winPos = grain->winPos;
		int nsmps = sc_min(grain->counter, inNumSamples);
		for (int j=0; j<nsmps; ++j) {
		    float outval = amp * lookupi1(table0, table1, oscphase, unit->m_lomask);
		    out[j] += outval;
		    oscphase += thisfreq;
		    BUF_GRAIN_AMP
		    }
		grain->winPos = winPos;
		grain->curamp = amp;
		grain->oscphase = oscphase;
		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	}

	    if ((unit->curtrig <= 0) && (trig > 0.0)) {
		    // start a grain
		    if (unit->mNumActive+1 >= kMaxSynthGrains) {
			Print("Too many grains!\n");
			} else {
			SGrainB *grain = unit->mGrains + unit->mNumActive++;
			// INRATE(1) == calcFullRate
			freq = IN0(2);
			winSize = IN0(1);
			grain->mWindow = (int)IN0(3);
			double winPos = grain->winPos = 0.f;
			GET_GRAIN_WIN
			double counter = winSize * SAMPLERATE;
			double winInc = grain->winInc = (double)windowSamples / counter;
			int32 thisfreq = grain->freq = (int32)(unit->m_cpstoinc * freq);
			int32 oscphase = 0;
			counter = sc_max(4., counter);
			grain->counter = (int)counter;
			double amp = windowData[0];
			int nsmps = sc_min(grain->counter, inNumSamples);
			for (int j=0; j<nsmps; ++j) {
			    float outval = amp * lookupi1(table0, table1, oscphase, unit->m_lomask);
			    out[j] += outval;
			    oscphase += thisfreq;
			    BUF_GRAIN_AMP
			}
			grain->winPos = winPos;
			grain->oscphase = oscphase;
			grain->counter -= inNumSamples;
			grain->curamp = amp;
			if (grain->counter <= 0) {
				// remove grain
				*grain = unit->mGrains[--unit->mNumActive];
			}
		}
	    }
	unit->curtrig = trig;

}


void SinGrainB_Ctor(SinGrainB *unit)
{
	if (INRATE(0) == calc_FullRate)
	    SETCALC(SinGrainB_next_a);
	    else
	    SETCALC(SinGrainB_next_k);
	int tableSizeSin = ft->mSineSize;
	unit->m_lomask = (tableSizeSin - 1) << 3;
	unit->m_radtoinc = tableSizeSin * (rtwopi * 65536.);
	unit->m_cpstoinc = tableSizeSin * SAMPLEDUR * 65536.;
	unit->curtrig = 0.f;
	unit->mNumActive = 0;
	SinGrainB_next_k(unit, 1);
}

///////////////////////// SinGrainI ////////////////////////////

void SinGrainI_next_a(SinGrainI *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);
	float *out = OUT(0);
	float *trig = IN(0);
	float winSize, freq;
	float *table0 = ft->mSineWavetable;
	float *table1 = table0 + 1;

	for (int i=0; i < unit->mNumActive; ) {
		SGrainI *grain = unit->mGrains + i;
		int32 thisfreq = grain->freq;
		int32 oscphase = grain->oscphase;
		GET_INTERP_GRAIN_WIN

		double winIncA = grain->winIncA;
		double winPosA = grain->winPosA;
		double winIncB = grain->winIncB;
		double winPosB = grain->winPosB;
		double amp = grain->curamp;
		int nsmps = sc_min(grain->counter, inNumSamples);
		for (int j=0; j<nsmps; ++j) {
		    float outval = amp * lookupi1(table0, table1, oscphase, unit->m_lomask);
		    out[j] += outval;
		    oscphase += thisfreq;
		    BUF_INTERP_GRAIN_AMP
		    }
		grain->winPosA = winPosA;
		grain->winPosB = winPosB;
		grain->curamp = amp;
		grain->oscphase = oscphase;
		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	}

	for (int i=0; i<inNumSamples; ++i) {
		if ((unit->curtrig <= 0) && (trig[i] > 0.0)) {
			// start a grain
			if (unit->mNumActive+1 >= kMaxSynthGrains) {Print("Too many grains!\n"); break;}
			SGrainI *grain = unit->mGrains + unit->mNumActive++;
			freq = IN_AT(unit, 2, i);
			winSize = IN_AT(unit, 1, i);
			grain->mWindowA = (int)IN_AT(unit, 3, i);
			grain->mWindowB = (int)IN_AT(unit, 4, i);
			float ifac = grain->ifac = IN_AT(unit, 5, i);
			double winPosA = grain->winPosA = 0.f;
			double winPosB = grain->winPosB = 0.f;
			GET_INTERP_GRAIN_WIN
			double counter = winSize * SAMPLERATE;
			double winIncA = grain->winIncA = (double)windowSamplesA / counter;
			double winIncB = grain->winIncB = (double)windowSamplesB / counter;
			int32 thisfreq = grain->freq = (int32)(unit->m_cpstoinc * freq);
			int32 oscphase = 0;
			counter = sc_max(4., counter);
			grain->counter = (int)counter;
			double amp = lininterp(ifac, windowDataA[0], windowDataB[0]);
			float *out1 = out + i;
			int nsmps = sc_min(grain->counter, inNumSamples - i);
			for (int j=0; j<nsmps; ++j) {
			    float outval = amp * lookupi1(table0, table1, oscphase, unit->m_lomask);
			    out1[j] += outval;
			    oscphase += thisfreq;
			    BUF_INTERP_GRAIN_AMP
			}
			grain->curamp = amp;
			grain->winPosA = winPosA;
			grain->winPosB = winPosB;
			grain->oscphase = oscphase;
			grain->counter -= nsmps;
			if (grain->counter <= 0) {
				// remove grain
				*grain = unit->mGrains[--unit->mNumActive];
			}
		}
		unit->curtrig = trig[i];
	}
}

void SinGrainI_next_k(SinGrainI *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);
	float *out = OUT(0);
	float trig = IN0(0);
	float winSize, freq;
	float *table0 = ft->mSineWavetable;
	float *table1 = table0 + 1;

	for (int i=0; i < unit->mNumActive; ) {
		SGrainI *grain = unit->mGrains + i;
		int32 thisfreq = grain->freq;
		int32 oscphase = grain->oscphase;
		GET_INTERP_GRAIN_WIN

		double winIncA = grain->winIncA;
		double winPosA = grain->winPosA;
		double winIncB = grain->winIncB;
		double winPosB = grain->winPosB;
		double amp = grain->curamp;
		int nsmps = sc_min(grain->counter, inNumSamples);
		for (int j=0; j<nsmps; ++j) {
		    float outval = amp * lookupi1(table0, table1, oscphase, unit->m_lomask);
		    out[j] += outval;
		    oscphase += thisfreq;
		    BUF_INTERP_GRAIN_AMP
		    }
		grain->winPosA = winPosA;
		grain->winPosB = winPosB;
		grain->curamp = amp;
		grain->oscphase = oscphase;
		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	}

		if ((unit->curtrig <= 0) && (trig > 0.0)) {
			// start a grain
			if (unit->mNumActive+1 >= kMaxSynthGrains) {
			    Print("Too many grains!\n");
			    } else {
			SGrainI *grain = unit->mGrains + unit->mNumActive++;
			freq = IN0(2);
			winSize = IN0(1);
			grain->mWindowA = (int)IN0(3);
			grain->mWindowB = (int)IN0(4);
			float ifac = grain->ifac = IN0(5);
			double winPosA = grain->winPosA = 0.f;
			double winPosB = grain->winPosB = 0.f;
			GET_INTERP_GRAIN_WIN
			double counter = winSize * SAMPLERATE;
			double winIncA = grain->winIncA = (double)windowSamplesA / counter;
			double winIncB = grain->winIncB = (double)windowSamplesB / counter;
			int32 thisfreq = grain->freq = (int32)(unit->m_cpstoinc * freq);
			int32 oscphase = 0;
			counter = sc_max(4., counter);
			grain->counter = (int)counter;
			double amp = lininterp(ifac, windowDataA[0], windowDataB[0]);
			int nsmps = sc_min(grain->counter, inNumSamples);
			for (int j=0; j<nsmps; ++j) {
			    float outval = amp * lookupi1(table0, table1, oscphase, unit->m_lomask);
			    out[j] += outval;
			    oscphase += thisfreq;
			    BUF_INTERP_GRAIN_AMP
			}
			grain->curamp = amp;
			grain->winPosA = winPosA;
			grain->winPosB = winPosB;
			grain->oscphase = oscphase;
			grain->counter -= inNumSamples;
			if (grain->counter <= 0) {
				// remove grain
				*grain = unit->mGrains[--unit->mNumActive];
			}
		}
	    }
	    unit->curtrig = trig;
}


void SinGrainI_Ctor(SinGrainI *unit)
{
	if (INRATE(0) == calc_FullRate)
	    SETCALC(SinGrainI_next_a);
	    else
	    SETCALC(SinGrainI_next_k);
	int tableSizeSin = ft->mSineSize;
	unit->m_lomask = (tableSizeSin - 1) << 3;
	unit->m_radtoinc = tableSizeSin * (rtwopi * 65536.);
	unit->m_cpstoinc = tableSizeSin * SAMPLEDUR * 65536.;
	unit->curtrig = 0.f;
	unit->mNumActive = 0;
	SinGrainI_next_k(unit, 1);
}

//////////////////////// FM grains /////////////////////////////

void FMGrain_next_a(FMGrain *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);
	float *out = OUT(0);
	float *trig = IN(0);
	float winSize, carfreq, modfreq, index;

	float *table0 = ft->mSineWavetable;
	float *table1 = table0 + 1;

	for (int i=0; i < unit->mNumActive; ) {
		FGrain *grain = unit->mGrains + i;
		double b1 = grain->b1;
		double y1 = grain->y1;
		double y2 = grain->y2;
		int32 mfreq = grain->mfreq;
		int32 moscphase = grain->moscphase;
		int32 coscphase = grain->coscphase;
		float deviation = grain->deviation;
		float carbase = grain->carbase;

		int nsmps = sc_min(grain->counter, inNumSamples);
		for (int j=0; j<nsmps; ++j) {
			float amp = y1 * y1;
			float thismod = lookupi1(table0, table1, moscphase, unit->m_lomask) * deviation;
			float outval = amp * lookupi1(table0, table1, coscphase, unit->m_lomask);
			out[j] += outval;
			double y0 = b1 * y1 - y2;
			y2 = y1;
			y1 = y0;
			int32 cfreq = (int32)(unit->m_cpstoinc * (carbase + thismod)); // needs to be calced in the loop!
			coscphase += cfreq;
			moscphase += mfreq;
		    } // need to save float carbase, int32 mfreq, float deviation
		grain->coscphase = coscphase;
		grain->moscphase = moscphase;
		grain->y1 = y1;
		grain->y2 = y2;
		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	}

	for (int i=0; i<inNumSamples; ++i) {
	    if ((unit->curtrig <= 0) && (trig[i] > 0.0)) {
		// start a grain
		if (unit->mNumActive+1 >= kMaxSynthGrains) {Print("Too many grains!\n"); break;}
		FGrain *grain = unit->mGrains + unit->mNumActive++;
		winSize = IN_AT(unit, 1, i);
		carfreq = IN_AT(unit, 2, i);
		modfreq = IN_AT(unit, 3, i);
		index = IN_AT(unit, 4, i);
		float deviation = grain->deviation = index * modfreq;
		int32 mfreq = grain->mfreq = (int32)(unit->m_cpstoinc * modfreq);
		grain->carbase = carfreq;
		int32 coscphase = 0;
		int32 moscphase = 0;
		double counter = winSize * SAMPLERATE;
		counter = sc_max(4., counter);
		grain->counter = (int)counter;
		double w = pi / counter;
		double b1 = grain->b1 = 2. * cos(w);
		double y1 = sin(w);
		double y2 = 0.;
		float *out1 = out + i;
		int nsmps = sc_min(grain->counter, inNumSamples - i);
		for (int j=0; j<nsmps; ++j) {
		    float amp = y1 * y1;
		    float thismod = lookupi1(table0, table1, moscphase, unit->m_lomask) * deviation;
		    float outval = amp * lookupi1(table0, table1, coscphase, unit->m_lomask);
		    out1[j] += outval;
		    double y0 = b1 * y1 - y2;
		    y2 = y1;
		    y1 = y0;
		    int32 cfreq = (int32)(unit->m_cpstoinc * (carfreq + thismod)); // needs to be calced in the loop!
		    coscphase += cfreq;
		    moscphase += mfreq;
		} // need to save float carbase, int32 mfreq, float deviation
		grain->coscphase = coscphase;
		grain->moscphase = moscphase;
		grain->y1 = y1;
		grain->y2 = y2;
		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		}
		}
		unit->curtrig = trig[i];
	}
}

void FMGrain_next_k(FMGrain *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);
	float *out = OUT(0);
	float trig = IN0(0);
	float winSize, carfreq, modfreq, index;

	float *table0 = ft->mSineWavetable;
	float *table1 = table0 + 1;

	for (int i=0; i < unit->mNumActive; ) {
		FGrain *grain = unit->mGrains + i;
		double b1 = grain->b1;
		double y1 = grain->y1;
		double y2 = grain->y2;
		int32 mfreq = grain->mfreq;
		int32 moscphase = grain->moscphase;
		int32 coscphase = grain->coscphase;
		float deviation = grain->deviation;
		float carbase = grain->carbase;

		int nsmps = sc_min(grain->counter, inNumSamples);
		for (int j=0; j<nsmps; ++j) {
			float amp = y1 * y1;
			float thismod = lookupi1(table0, table1, moscphase, unit->m_lomask) * deviation;
			float outval = amp * lookupi1(table0, table1, coscphase, unit->m_lomask);
			out[j] += outval;
			double y0 = b1 * y1 - y2;
			y2 = y1;
			y1 = y0;
			int32 cfreq = (int32)(unit->m_cpstoinc * (carbase + thismod)); // needs to be calced in the loop!
			coscphase += cfreq;
			moscphase += mfreq;
		    } // need to save float carbase, int32 mfreq, float deviation
		grain->coscphase = coscphase;
		grain->moscphase = moscphase;
		grain->y1 = y1;
		grain->y2 = y2;
		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	    }

	    if ((unit->curtrig <= 0) && (trig > 0.0)) {
		// start a grain
		if (unit->mNumActive+1 >= kMaxSynthGrains) {Print("Too many grains!\n");
		} else {
		FGrain *grain = unit->mGrains + unit->mNumActive++;
		winSize = IN0(1);
		carfreq = IN0(2);
		modfreq = IN0(3);
		index = IN0(4);
		float deviation = grain->deviation = index * modfreq;
		int32 mfreq = grain->mfreq = (int32)(unit->m_cpstoinc * modfreq);
		grain->carbase = carfreq;
		int32 coscphase = 0;
		int32 moscphase = 0;
		double counter = winSize * SAMPLERATE;
		counter = sc_max(4., counter);
		grain->counter = (int)counter;
		double w = pi / counter;
		double b1 = grain->b1 = 2. * cos(w);
		double y1 = sin(w);
		double y2 = 0.;
		int nsmps = sc_min(grain->counter, inNumSamples);
		for (int j=0; j<nsmps; ++j) {
		    float amp = y1 * y1;
		    float thismod = lookupi1(table0, table1, moscphase, unit->m_lomask) * deviation;
		    float outval = amp * lookupi1(table0, table1, coscphase, unit->m_lomask);
		    out[j] += outval;
		    double y0 = b1 * y1 - y2;
		    y2 = y1;
		    y1 = y0;
		    int32 cfreq = (int32)(unit->m_cpstoinc * (carfreq + thismod)); // needs to be calced in the loop!
		    coscphase += cfreq;
		    moscphase += mfreq;
		} // need to save float carbase, int32 mfreq, float deviation
		grain->coscphase = coscphase;
		grain->moscphase = moscphase;
		grain->y1 = y1;
		grain->y2 = y2;
		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		}
	    }
	}
	unit->curtrig = trig;
}

void FMGrain_Ctor(FMGrain *unit)
{
	if (INRATE(0) == calc_FullRate)
	    SETCALC(FMGrain_next_a);
	    else
	    SETCALC(FMGrain_next_k);
	int tableSizeSin = ft->mSineSize;
	unit->m_lomask = (tableSizeSin - 1) << 3;
	unit->m_radtoinc = tableSizeSin * (rtwopi * 65536.);
	unit->m_cpstoinc = tableSizeSin * SAMPLEDUR * 65536.;
	unit->curtrig = 0.f;
	unit->mNumActive = 0;
	FMGrain_next_k(unit, 1);
}

///////////////////////  FMGrainB //////////////////////////////////////////

void FMGrainB_next_a(FMGrainB *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);
	float *out = OUT(0);
	float *trig = IN(0);
	float winSize, carfreq, modfreq, index;

	float *table0 = ft->mSineWavetable;
	float *table1 = table0 + 1;

	for (int i=0; i < unit->mNumActive; ) {
		FGrainB *grain = unit->mGrains + i;
		int32 mfreq = grain->mfreq;
		int32 moscphase = grain->moscphase;
		int32 coscphase = grain->coscphase;
		float deviation = grain->deviation;
		float carbase = grain->carbase;
		GET_GRAIN_WIN
		double amp = grain->curamp;
		double winInc = grain->winInc;
		double winPos = grain->winPos;
		int nsmps = sc_min(grain->counter, inNumSamples);
		for (int j=0; j<nsmps; ++j) {
			float thismod = lookupi1(table0, table1, moscphase, unit->m_lomask) * deviation;
			float outval = amp * lookupi1(table0, table1, coscphase, unit->m_lomask);
			out[j] += outval;
			BUF_GRAIN_AMP
			int32 cfreq = (int32)(unit->m_cpstoinc * (carbase + thismod)); // needs to be calced in the loop!
			coscphase += cfreq;
			moscphase += mfreq;
		    } // need to save float carbase, int32 mfreq, float deviation
		grain->coscphase = coscphase;
		grain->moscphase = moscphase;
		grain->curamp = amp;
		grain->winPos = winPos;
		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	}

	for (int i=0; i<inNumSamples; ++i) {
	    if ((unit->curtrig <= 0) && (trig[i] > 0.0)) {
		// start a grain
		if (unit->mNumActive+1 >= kMaxSynthGrains) {Print("Too many grains!\n"); break;}
		FGrainB *grain = unit->mGrains + unit->mNumActive++;
		winSize = IN_AT(unit, 1, i);
		carfreq = IN_AT(unit, 2, i);
		modfreq = IN_AT(unit, 3, i);
		index = IN_AT(unit, 4, i);
		grain->mWindow = (int)IN_AT(unit, 5, i);
		double winPos = grain->winPos = 0.f;
		GET_GRAIN_WIN
		double counter = winSize * SAMPLERATE;
		double winInc = grain->winInc = (double)windowSamples / counter;
		double amp = windowData[0];
		float deviation = grain->deviation = index * modfreq;
		int32 mfreq = grain->mfreq = (int32)(unit->m_cpstoinc * modfreq);
		grain->carbase = carfreq;
		int32 coscphase = 0;
		int32 moscphase = 0;
		counter = sc_max(4., counter);
		grain->counter = (int)counter;

		float *out1 = out + i;
		int nsmps = sc_min(grain->counter, inNumSamples - i);
		for (int j=0; j<nsmps; ++j) {
		    float thismod = lookupi1(table0, table1, moscphase, unit->m_lomask) * deviation;
		    float outval = amp * lookupi1(table0, table1, coscphase, unit->m_lomask);
		    out1[j] += outval;
		    BUF_GRAIN_AMP
		    int32 cfreq = (int32)(unit->m_cpstoinc * (carfreq + thismod)); // needs to be calced in the loop!
		    coscphase += cfreq;
		    moscphase += mfreq;
		} // need to save float carbase, int32 mfreq, float deviation
		grain->coscphase = coscphase;
		grain->moscphase = moscphase;
		grain->curamp = amp;
		grain->winPos = winPos;
		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		}
		}
		unit->curtrig = trig[i];
	}
}

void FMGrainB_next_k(FMGrainB *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);
	float *out = OUT(0);
	float trig = IN0(0);
	float winSize, carfreq, modfreq, index;

	float *table0 = ft->mSineWavetable;
	float *table1 = table0 + 1;

	for (int i=0; i < unit->mNumActive; ) {
		FGrainB *grain = unit->mGrains + i;
		GET_GRAIN_WIN
		double amp = grain->curamp;
		double winInc = grain->winInc;
		double winPos = grain->winPos;
		int32 mfreq = grain->mfreq;
		int32 moscphase = grain->moscphase;
		int32 coscphase = grain->coscphase;
		float deviation = grain->deviation;
		float carbase = grain->carbase;

		int nsmps = sc_min(grain->counter, inNumSamples);
		for (int j=0; j<nsmps; ++j) {
			float thismod = lookupi1(table0, table1, moscphase, unit->m_lomask) * deviation;
			float outval = amp * lookupi1(table0, table1, coscphase, unit->m_lomask);
			out[j] += outval;
			BUF_GRAIN_AMP
			int32 cfreq = (int32)(unit->m_cpstoinc * (carbase + thismod)); // needs to be calced in the loop!
			coscphase += cfreq;
			moscphase += mfreq;
		    } // need to save float carbase, int32 mfreq, float deviation
		grain->coscphase = coscphase;
		grain->moscphase = moscphase;
		grain->curamp = amp;
		grain->winPos = winPos;
		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	    }

	    if ((unit->curtrig <= 0) && (trig > 0.0)) {
		// start a grain
		if (unit->mNumActive+1 >= kMaxSynthGrains) {Print("Too many grains!\n");
		} else {
		FGrainB *grain = unit->mGrains + unit->mNumActive++;
		winSize = IN0(1);
		carfreq = IN0(2);
		modfreq = IN0(3);
		index = IN0(4);
		grain->mWindow = (int)IN0(5);
		double winPos = grain->winPos = 0.f;
		GET_GRAIN_WIN
		double counter = winSize * SAMPLERATE;
		double winInc = grain->winInc = (double)windowSamples / counter;
		float amp = windowData[0];

		float deviation = grain->deviation = index * modfreq;
		int32 mfreq = grain->mfreq = (int32)(unit->m_cpstoinc * modfreq);
		grain->carbase = carfreq;
		int32 coscphase = 0;
		int32 moscphase = 0;
		counter = sc_max(4., counter);
		grain->counter = (int)counter;
		int nsmps = sc_min(grain->counter, inNumSamples);
		for (int j=0; j<nsmps; ++j) {
		    float thismod = lookupi1(table0, table1, moscphase, unit->m_lomask) * deviation;
		    float outval = amp * lookupi1(table0, table1, coscphase, unit->m_lomask);
		    out[j] += outval;
		    BUF_GRAIN_AMP
		    int32 cfreq = (int32)(unit->m_cpstoinc * (carfreq + thismod)); // needs to be calced in the loop!
		    coscphase += cfreq;
		    moscphase += mfreq;
		} // need to save float carbase, int32 mfreq, float deviation
		grain->coscphase = coscphase;
		grain->moscphase = moscphase;
		grain->curamp = amp;
		grain->winPos = winPos;
		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		}
	    }
	}
	unit->curtrig = trig;
}

void FMGrainB_Ctor(FMGrainB *unit)
{
	if (INRATE(0) == calc_FullRate)
	    SETCALC(FMGrainB_next_a);
	    else
	    SETCALC(FMGrainB_next_k);
	int tableSizeSin = ft->mSineSize;
	unit->m_lomask = (tableSizeSin - 1) << 3;
	unit->m_radtoinc = tableSizeSin * (rtwopi * 65536.);
	unit->m_cpstoinc = tableSizeSin * SAMPLEDUR * 65536.;
	unit->curtrig = 0.f;
	unit->mNumActive = 0;
	FMGrainB_next_k(unit, 1);
}


///////////////////////  FMGrainI //////////////////////////////////////////

void FMGrainI_next_a(FMGrainI *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);
	float *out = OUT(0);
	float *trig = IN(0);
	float winSize, carfreq, modfreq, index;

	float *table0 = ft->mSineWavetable;
	float *table1 = table0 + 1;

	for (int i=0; i < unit->mNumActive; ) {
		FGrainI *grain = unit->mGrains + i;
		int32 mfreq = grain->mfreq;
		int32 moscphase = grain->moscphase;
		int32 coscphase = grain->coscphase;
		float deviation = grain->deviation;
		float carbase = grain->carbase;
		GET_INTERP_GRAIN_WIN
		double amp = grain->curamp;
		double winIncA = grain->winIncA;
		double winPosA = grain->winPosA;
		double winIncB = grain->winIncB;
		double winPosB = grain->winPosB;
		int nsmps = sc_min(grain->counter, inNumSamples);
		for (int j=0; j<nsmps; ++j) {
			float thismod = lookupi1(table0, table1, moscphase, unit->m_lomask) * deviation;
			float outval = amp * lookupi1(table0, table1, coscphase, unit->m_lomask);
			out[j] += outval;
			BUF_INTERP_GRAIN_AMP
			int32 cfreq = (int32)(unit->m_cpstoinc * (carbase + thismod)); // needs to be calced in the loop!
			coscphase += cfreq;
			moscphase += mfreq;
		    } // need to save float carbase, int32 mfreq, float deviation
		grain->coscphase = coscphase;
		grain->moscphase = moscphase;
		grain->curamp = amp;
		grain->winPosA = winPosA;
		grain->winPosB = winPosB;
		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	}

	for (int i=0; i<inNumSamples; ++i) {
	    if ((unit->curtrig <= 0) && (trig[i] > 0.0)) {
		// start a grain
		if (unit->mNumActive+1 >= kMaxSynthGrains) {Print("Too many grains!\n"); break;}
		FGrainI *grain = unit->mGrains + unit->mNumActive++;
		winSize = IN_AT(unit, 1, i);
		carfreq = IN_AT(unit, 2, i);
		modfreq = IN_AT(unit, 3, i);
		index = IN_AT(unit, 4, i);
		grain->mWindowA = (int)IN_AT(unit, 5, i);
		grain->mWindowB = (int)IN_AT(unit, 6, i);
		float ifac = grain->ifac = IN_AT(unit, 7, i);
		double winPosA = grain->winPosA = 0.f;
		double winPosB = grain->winPosB = 0.f;
		GET_INTERP_GRAIN_WIN
		double counter = winSize * SAMPLERATE;
		double winIncA = grain->winIncA = (double)windowSamplesA / counter;
		double winIncB = grain->winIncB = (double)windowSamplesB / counter;
		double amp = lininterp(ifac, windowDataA[0], windowDataB[0]);
		float deviation = grain->deviation = index * modfreq;
		int32 mfreq = grain->mfreq = (int32)(unit->m_cpstoinc * modfreq);
		grain->carbase = carfreq;
		int32 coscphase = 0;
		int32 moscphase = 0;
		counter = sc_max(4., counter);
		grain->counter = (int)counter;

		float *out1 = out + i;
		int nsmps = sc_min(grain->counter, inNumSamples - i);
		for (int j=0; j<nsmps; ++j) {
		    float thismod = lookupi1(table0, table1, moscphase, unit->m_lomask) * deviation;
		    float outval = amp * lookupi1(table0, table1, coscphase, unit->m_lomask);
		    out1[j] += outval;
		    BUF_INTERP_GRAIN_AMP
		    int32 cfreq = (int32)(unit->m_cpstoinc * (carfreq + thismod)); // needs to be calced in the loop!
		    coscphase += cfreq;
		    moscphase += mfreq;
		} // need to save float carbase, int32 mfreq, float deviation
		grain->coscphase = coscphase;
		grain->moscphase = moscphase;
		grain->curamp = amp;
		grain->winPosA = winPosA;
		grain->winPosB = winPosB;
		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		}
		}
		unit->curtrig = trig[i];
	}
}

void FMGrainI_next_k(FMGrainI *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);
	float *out = OUT(0);
	float trig = IN0(0);
	float winSize, carfreq, modfreq, index;

	float *table0 = ft->mSineWavetable;
	float *table1 = table0 + 1;

	for (int i=0; i < unit->mNumActive; ) {
		FGrainI *grain = unit->mGrains + i;
		int32 mfreq = grain->mfreq;
		int32 moscphase = grain->moscphase;
		int32 coscphase = grain->coscphase;
		float deviation = grain->deviation;
		float carbase = grain->carbase;
		GET_INTERP_GRAIN_WIN
		double amp = grain->curamp;
		double winIncA = grain->winIncA;
		double winPosA = grain->winPosA;
		double winIncB = grain->winIncB;
		double winPosB = grain->winPosB;
		int nsmps = sc_min(grain->counter, inNumSamples);
		for (int j=0; j<nsmps; ++j) {
			float thismod = lookupi1(table0, table1, moscphase, unit->m_lomask) * deviation;
			float outval = amp * lookupi1(table0, table1, coscphase, unit->m_lomask);
			out[j] += outval;
			BUF_INTERP_GRAIN_AMP
			int32 cfreq = (int32)(unit->m_cpstoinc * (carbase + thismod)); // needs to be calced in the loop!
			coscphase += cfreq;
			moscphase += mfreq;
		    } // need to save float carbase, int32 mfreq, float deviation
		grain->coscphase = coscphase;
		grain->moscphase = moscphase;
		grain->curamp = amp;
		grain->winPosA = winPosA;
		grain->winPosB = winPosB;
		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	}

	    if ((unit->curtrig <= 0) && (trig > 0.0)) {
		// start a grain
		if (unit->mNumActive+1 >= kMaxSynthGrains) {
		    Print("Too many grains!\n");
		    } else {
		FGrainI *grain = unit->mGrains + unit->mNumActive++;
		winSize = IN0(1);
		carfreq = IN0(2);
		modfreq = IN0(3);
		index = IN0(4);
		grain->mWindowA = (int)IN0(5);
		grain->mWindowB = (int)IN0(6);
		float ifac = grain->ifac = IN0(7);
		double winPosA = grain->winPosA = 0.f;
		double winPosB = grain->winPosB = 0.f;
		GET_INTERP_GRAIN_WIN
		double counter = winSize * SAMPLERATE;
		double winIncA = grain->winIncA = (double)windowSamplesA / counter;
		double winIncB = grain->winIncB = (double)windowSamplesB / counter;
		double amp = lininterp(ifac, windowDataA[0], windowDataB[0]);
		float deviation = grain->deviation = index * modfreq;
		int32 mfreq = grain->mfreq = (int32)(unit->m_cpstoinc * modfreq);
		grain->carbase = carfreq;
		int32 coscphase = 0;
		int32 moscphase = 0;
		counter = sc_max(4., counter);
		grain->counter = (int)counter;
		int nsmps = sc_min(grain->counter, inNumSamples);
		for (int j=0; j<nsmps; ++j) {
		    float thismod = lookupi1(table0, table1, moscphase, unit->m_lomask) * deviation;
		    float outval = amp * lookupi1(table0, table1, coscphase, unit->m_lomask);
		    out[j] += outval;
		    BUF_INTERP_GRAIN_AMP
		    int32 cfreq = (int32)(unit->m_cpstoinc * (carfreq + thismod)); // needs to be calced in the loop!
		    coscphase += cfreq;
		    moscphase += mfreq;
		} // need to save float carbase, int32 mfreq, float deviation
		grain->coscphase = coscphase;
		grain->moscphase = moscphase;
		grain->curamp = amp;
		grain->winPosA = winPosA;
		grain->winPosB = winPosB;
		grain->counter -= inNumSamples;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		    }
		}
	    }
	    unit->curtrig = trig;
}

void FMGrainI_Ctor(FMGrainI *unit)
{
	if (INRATE(0) == calc_FullRate)
	    SETCALC(FMGrainI_next_a);
	    else
	    SETCALC(FMGrainI_next_k);
	int tableSizeSin = ft->mSineSize;
	unit->m_lomask = (tableSizeSin - 1) << 3;
	unit->m_radtoinc = tableSizeSin * (rtwopi * 65536.);
	unit->m_cpstoinc = tableSizeSin * SAMPLEDUR * 65536.;
	unit->curtrig = 0.f;
	unit->mNumActive = 0;
	FMGrainI_next_k(unit, 1);
}
///////////////////// BufGrain (basically Warp1 with a trigger... which is basically TGrains in Mono???) //////////////

void BufGrain_next_a(BufGrain *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);
	float *out;
	out = ZOUT(0);
	float *trig = IN(0);

	World *world = unit->mWorld;
//	SndBuf *bufs = world->mSndBufs;
//	uint32 numBufs = world->mNumSndBufs;

	for (int i=0; i < unit->mNumActive; ) {
		Grain *grain = unit->mGrains + i;
//		uint32 bufnum = grain->bufnum;

		GRAIN_BUF

		if (bufChannels != 1) {
			 ++i;
			 continue;
		}
		double loopMax = (double)bufFrames;
		double rate = grain->rate;
		double phase = grain->phase;
		double b1 = grain->b1;
		double y1 = grain->y1;
		double y2 = grain->y2;

		float *out1 = out;

		int nsmps = sc_min(grain->counter, inNumSamples);
		if (grain->interp >= 4) {
			for (int j=0; j<nsmps; ++j) {
				GRAIN_LOOP_BODY_4;
				phase += rate;
			}
		} else if (grain->interp >= 2) {
			for (int j=0; j<nsmps; ++j) {
				GRAIN_LOOP_BODY_2;
				phase += rate;
			}
		} else {
			for (int j=0; j<nsmps; ++j) {
				GRAIN_LOOP_BODY_1;
				phase += rate;
			}
		}

		grain->phase = phase;
		grain->y1 = y1;
		grain->y2 = y2;

		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	}

	for (int i=0; i<inNumSamples; ++i) {
		if ((trig[i] > 0) && (unit->curtrig <=0)) {
			// start a grain
			if (unit->mNumActive+1 >= kMaxSynthGrains) {Print("Too many grains!\n");
			} else {
			uint32 bufnum = (uint32)IN_AT(unit, 2, i);
//			if (bufnum >= numBufs) continue;
			Grain *grain = unit->mGrains + unit->mNumActive++;

			SETUP_GRAIN_BUF
			GRAIN_BUF

			if (bufChannels != 1) continue;

			float bufSampleRate = buf->samplerate;
			float bufRateScale = bufSampleRate * SAMPLEDUR;
			double loopMax = (double)bufFrames;

//			Grain *grain = unit->mGrains + unit->mNumActive++;
//			grain->bufnum = bufnum;

			double counter = IN_AT(unit, 1, i) * SAMPLERATE;
			counter = sc_max(4., counter);
			grain->counter = (int)counter;

			double rate = grain->rate = IN_AT(unit, 3, i) * bufRateScale;
			double phase = IN_AT(unit, 4, i) * bufFrames;
			grain->interp = (int)IN_AT(unit, 5, i);

			double w = pi / counter;
			double b1 = grain->b1 = 2. * cos(w);
			double y1 = sin(w);
			double y2 = 0.;

			float *out1 = out + i;

			int nsmps = sc_min(grain->counter, inNumSamples - i);
			if (grain->interp >= 4) {
				for (int j=0; j<nsmps; ++j) {
					GRAIN_LOOP_BODY_4;
					phase += rate;
				}
			} else if (grain->interp >= 2) {
				for (int j=0; j<nsmps; ++j) {
					GRAIN_LOOP_BODY_2;
					phase += rate;
				}
			} else {
				for (int j=0; j<nsmps; ++j) {
					GRAIN_LOOP_BODY_1;
					phase += rate;
				}
			}

			grain->phase = phase;
			grain->y1 = y1;
			grain->y2 = y2;
			// end change
			grain->counter -= nsmps;
			if (grain->counter <= 0) {
				// remove grain
				*grain = unit->mGrains[--unit->mNumActive];
			}
		    }
		}
		unit->curtrig = trig[i];
	    }

}

void BufGrain_next_k(BufGrain *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);
	float *out;
	out = ZOUT(0);
	float trig = IN0(0);

	World *world = unit->mWorld;
//	SndBuf *bufs = world->mSndBufs;
	//uint32 numBufs = world->mNumSndBufs;

	for (int i=0; i < unit->mNumActive; ) {
		Grain *grain = unit->mGrains + i;
//		uint32 bufnum = grain->bufnum;

		GRAIN_BUF
		CHECK_BUF

		if (bufChannels != 1) {
			 ++i;
			 continue;
		}

		double loopMax = (double)bufFrames;

		double rate = grain->rate;
		double phase = grain->phase;
		double b1 = grain->b1;
		double y1 = grain->y1;
		double y2 = grain->y2;

		float *out1 = out;

		int nsmps = sc_min(grain->counter, inNumSamples);
		if (grain->interp >= 4) {
			for (int j=0; j<nsmps; ++j) {
				GRAIN_LOOP_BODY_4;
				phase += rate;
			}
		} else if (grain->interp >= 2) {
			for (int j=0; j<nsmps; ++j) {
				GRAIN_LOOP_BODY_2;
				phase += rate;
			}
		} else {
			for (int j=0; j<nsmps; ++j) {
				GRAIN_LOOP_BODY_1;
				phase += rate;
			}
		}

		grain->phase = phase;
		grain->y1 = y1;
		grain->y2 = y2;

		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	    }

		if ((trig > 0) && (unit->curtrig <=0)) {
			// start a grain
			if (unit->mNumActive+1 >= kMaxSynthGrains) {Print("Too many grains!\n");
			} else {
			uint32 bufnum = (uint32)IN0(2);
			Grain *grain = unit->mGrains + unit->mNumActive++;

			SETUP_GRAIN_BUF
			GRAIN_BUF
			CHECK_BUF

			float bufSampleRate = buf->samplerate;
			float bufRateScale = bufSampleRate * SAMPLEDUR;
			double loopMax = (double)bufFrames;

//			Grain *grain = unit->mGrains + unit->mNumActive++;
//			grain->bufnum = bufnum;

			double counter = IN0(1) * SAMPLERATE;
			counter = sc_max(4., counter);
			grain->counter = (int)counter;

			double rate = grain->rate = IN0(3) * bufRateScale;
			double phase = IN0(4) * bufFrames;
			grain->interp = (int)IN0(5);

			double w = pi / counter;
			double b1 = grain->b1 = 2. * cos(w);
			double y1 = sin(w);
			double y2 = 0.;

			float *out1 = out;

			int nsmps = sc_min(grain->counter, inNumSamples);
			if (grain->interp >= 4) {
				for (int j=0; j<nsmps; ++j) {
					GRAIN_LOOP_BODY_4;
					phase += rate;
				}
			} else if (grain->interp >= 2) {
				for (int j=0; j<nsmps; ++j) {
					GRAIN_LOOP_BODY_2;
					phase += rate;
				}
			} else {
				for (int j=0; j<nsmps; ++j) {
					GRAIN_LOOP_BODY_1;
					phase += rate;
				}
			}

			grain->phase = phase;
			grain->y1 = y1;
			grain->y2 = y2;
			// end change
			grain->counter -= nsmps;
			if (grain->counter <= 0) {
				// remove grain
				*grain = unit->mGrains[--unit->mNumActive];
			}
		}
	    }
	    unit->curtrig = trig;

}

void BufGrain_Ctor(BufGrain *unit)
{
	if (INRATE(0) == calc_FullRate)
	    SETCALC(BufGrain_next_a);
	    else
	    SETCALC(BufGrain_next_k);
	unit->mNumActive = 0;
	unit->curtrig = 0.f;
	BufGrain_next_k(unit, 1); // should be _k
}

////////////////////// BufGrainsB ///////////////////////////////

void BufGrainB_next_a(BufGrainB *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);
	float *out;
	out = ZOUT(0);
	float *trig = IN(0);

	World *world = unit->mWorld;
//	SndBuf *bufs = world->mSndBufs;
//	uint32 numBufs = world->mNumSndBufs;

	for (int i=0; i < unit->mNumActive; ) {
		WinGrain *grain = unit->mGrains + i;
//		uint32 bufnum = grain->bufnum;

		GRAIN_BUF
		CHECK_BUF

		if (bufChannels != 1) {
			 ++i;
			 continue;
		}
		GET_GRAIN_WIN
		double amp = grain->curamp;
		double winInc = grain->winInc;
		double winPos = grain->winPos;

		double loopMax = (double)bufFrames;

		double rate = grain->rate;
		double phase = grain->phase;

		float *out1 = out;

		int nsmps = sc_min(grain->counter, inNumSamples);
		if (grain->interp >= 4) {
			for (int j=0; j<nsmps; ++j) {
				BUF_GRAIN_LOOP_BODY_4;
				phase += rate;
				BUF_GRAIN_AMP

			}
		} else if (grain->interp >= 2) {
			for (int j=0; j<nsmps; ++j) {
				BUF_GRAIN_LOOP_BODY_2;
				phase += rate;
				BUF_GRAIN_AMP
			}
		} else {
			for (int j=0; j<nsmps; ++j) {
				BUF_GRAIN_LOOP_BODY_1;
				phase += rate;
				BUF_GRAIN_AMP
			}
		}

		grain->phase = phase;
		grain->curamp = amp;
		grain->winPos = winPos;

		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	}

	for (int i=0; i<inNumSamples; ++i) {
		if ((trig[i] > 0) && (unit->curtrig <=0)) {
			// start a grain
			if (unit->mNumActive+1 >= kMaxSynthGrains) {Print("Too many grains!\n");
			} else {
			uint32 bufnum = (uint32)IN_AT(unit, 2, i);
//			if (bufnum >= numBufs) continue;

			WinGrain *grain = unit->mGrains + unit->mNumActive++;

			SETUP_GRAIN_BUF
			GRAIN_BUF
			CHECK_BUF

			float winSize = IN_AT(unit, 1, i);

			if (bufChannels != 1) continue;

			float bufSampleRate = buf->samplerate;
			float bufRateScale = bufSampleRate * SAMPLEDUR;
			double loopMax = (double)bufFrames;

//			WinGrain *grain = unit->mGrains + unit->mNumActive++;
//			grain->bufnum = bufnum;

			double rate = grain->rate = IN_AT(unit, 3, i) * bufRateScale;
			double phase = IN_AT(unit, 4, i) * bufFrames;
			grain->interp = (int)IN_AT(unit, 5, i);

			grain->mWindow = (int)IN_AT(unit, 6, i);
			double winPos = grain->winPos = 0.f;
			GET_GRAIN_WIN
			double counter = winSize * SAMPLERATE;
			counter = sc_max(4., counter);
			grain->counter = (int)counter;
			double winInc = grain->winInc = (double)windowSamples / counter;
			double amp = windowData[0];

			float *out1 = out + i;

			int nsmps = sc_min(grain->counter, inNumSamples - i);
			if (grain->interp >= 4) {
				for (int j=0; j<nsmps; ++j) {
					BUF_GRAIN_LOOP_BODY_4;
					phase += rate;
					BUF_GRAIN_AMP
				}
			} else if (grain->interp >= 2) {
				for (int j=0; j<nsmps; ++j) {
					BUF_GRAIN_LOOP_BODY_2;
					phase += rate;
					BUF_GRAIN_AMP
				}
			} else {
				for (int j=0; j<nsmps; ++j) {
					BUF_GRAIN_LOOP_BODY_1;
					phase += rate;
					BUF_GRAIN_AMP
				}
			}

			grain->phase = phase;
			grain->curamp = amp;
			grain->winPos = winPos;
			// end change
			grain->counter -= nsmps;
			if (grain->counter <= 0) {
				// remove grain
				*grain = unit->mGrains[--unit->mNumActive];
			}
		    }
		}
		unit->curtrig = trig[i];
	    }

}

void BufGrainB_next_k(BufGrainB *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);
	float *out;
	out = ZOUT(0);
	float trig = IN0(0);

	World *world = unit->mWorld;
//	SndBuf *bufs = world->mSndBufs;
	//uint32 numBufs = world->mNumSndBufs;

	for (int i=0; i < unit->mNumActive; ) {
		WinGrain *grain = unit->mGrains + i;
//		uint32 bufnum = grain->bufnum;

		GRAIN_BUF
		CHECK_BUF

		if (bufChannels != 1) {
			 ++i;
			 continue;
		}

		double loopMax = (double)bufFrames;

		double rate = grain->rate;
		double phase = grain->phase;
		GET_GRAIN_WIN
		double amp = grain->curamp;
		double winInc = grain->winInc;
		double winPos = grain->winPos;


		float *out1 = out;

		int nsmps = sc_min(grain->counter, inNumSamples);
		if (grain->interp >= 4) {
			for (int j=0; j<nsmps; ++j) {
				BUF_GRAIN_LOOP_BODY_4;
				phase += rate;
				BUF_GRAIN_AMP
			}
		} else if (grain->interp >= 2) {
			for (int j=0; j<nsmps; ++j) {
				BUF_GRAIN_LOOP_BODY_2;
				phase += rate;
				BUF_GRAIN_AMP
			}
		} else {
			for (int j=0; j<nsmps; ++j) {
				BUF_GRAIN_LOOP_BODY_1;
				phase += rate;
				BUF_GRAIN_AMP
			}
		}

		grain->phase = phase;
		grain->curamp = amp;
		grain->winPos = winPos;

		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	    }

		if ((trig > 0) && (unit->curtrig <=0)) {
			// start a grain
			if (unit->mNumActive+1 >= kMaxSynthGrains) {Print("Too many grains!\n");
			} else {

			uint32 bufnum = (uint32)IN0(2);
			WinGrain *grain = unit->mGrains + unit->mNumActive++;

			SETUP_GRAIN_BUF
			GRAIN_BUF
			CHECK_BUF

			float winSize = IN0(1);


			float bufSampleRate = buf->samplerate;
			float bufRateScale = bufSampleRate * SAMPLEDUR;
			double loopMax = (double)bufFrames;

//			WinGrain *grain = unit->mGrains + unit->mNumActive++;
//			grain->bufnum = bufnum;


			double rate = grain->rate = IN0(3) * bufRateScale;
			double phase = IN0(4) * bufFrames;
			grain->interp = (int)IN0(5);

			grain->mWindow = (int)IN0(6);
			double winPos = grain->winPos = 0.f;
			GET_GRAIN_WIN
			double counter = winSize * SAMPLERATE;
			counter = sc_max(4., counter);
			grain->counter = (int)counter;

			double winInc = grain->winInc = (double)windowSamples / counter;
			float amp = windowData[0];

			float *out1 = out;
			int nsmps = sc_min(grain->counter, inNumSamples);

			if (grain->interp >= 4) {
				for (int j=0; j<nsmps; ++j) {
					BUF_GRAIN_LOOP_BODY_4;
					phase += rate;
					BUF_GRAIN_AMP
				}
			} else if (grain->interp >= 2) {
				for (int j=0; j<nsmps; ++j) {
					BUF_GRAIN_LOOP_BODY_2;
					phase += rate;
					BUF_GRAIN_AMP
				}
			} else {
				for (int j=0; j<nsmps; ++j) {
					BUF_GRAIN_LOOP_BODY_1;
					phase += rate;
					BUF_GRAIN_AMP
				}
			}

			grain->phase = phase;
			grain->curamp = amp;
			grain->winPos = winPos;
			// end change
			grain->counter -= inNumSamples;
			if (grain->counter <= 0) {
				// remove grain
				*grain = unit->mGrains[--unit->mNumActive];
			}
		}
	    }
	    unit->curtrig = trig;

}

void BufGrainB_Ctor(BufGrainB *unit)
{
	if (INRATE(0) == calc_FullRate)
	    SETCALC(BufGrainB_next_a);
	    else
	    SETCALC(BufGrainB_next_k);
	unit->mNumActive = 0;
	unit->curtrig = 0.f;
	BufGrainB_next_k(unit, 1); // should be _k
}


////////////////////// BufGrainsI ///////////////////////////////

void BufGrainI_next_a(BufGrainI *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);
	float *out;
	out = ZOUT(0);
	float *trig = IN(0);

	World *world = unit->mWorld;
//	SndBuf *bufs = world->mSndBufs;
//	uint32 numBufs = world->mNumSndBufs;

	for (int i=0; i < unit->mNumActive; ) {
		WinGrainI *grain = unit->mGrains + i;
//		uint32 bufnum = grain->bufnum;

		GRAIN_BUF
		CHECK_BUF

		if (bufChannels != 1) {
			 ++i;
			 continue;
		}
		GET_INTERP_GRAIN_WIN
		double amp = grain->curamp;
		double winIncA = grain->winIncA;
		double winPosA = grain->winPosA;
		double winIncB = grain->winIncB;
		double winPosB = grain->winPosB;

		double loopMax = (double)bufFrames;

		double rate = grain->rate;
		double phase = grain->phase;

		float *out1 = out;

		int nsmps = sc_min(grain->counter, inNumSamples);
			if (grain->interp >= 4) {
				for (int j=0; j<nsmps; ++j) {
					BUF_GRAIN_LOOP_BODY_4;
					phase += rate;
					BUF_INTERP_GRAIN_AMP
				}
			} else if (grain->interp >= 2) {
				for (int j=0; j<nsmps; ++j) {
					BUF_GRAIN_LOOP_BODY_2;
					phase += rate;
					BUF_INTERP_GRAIN_AMP
				}
			} else {
				for (int j=0; j<nsmps; ++j) {
					BUF_GRAIN_LOOP_BODY_1;
					phase += rate;
					BUF_INTERP_GRAIN_AMP
				}
			}


		grain->phase = phase;
		grain->curamp = amp;
		grain->winPosA = winPosA;
		grain->winPosB = winPosB;

		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	}

	for (int i=0; i<inNumSamples; ++i) {
		if ((trig[i] > 0) && (unit->curtrig <=0)) {
			// start a grain
			if (unit->mNumActive+1 >= kMaxSynthGrains) {Print("Too many grains!\n");
			} else {
			uint32 bufnum = (uint32)IN_AT(unit, 2, i);

//			if (bufnum >= numBufs) continue;
			float winSize = IN_AT(unit, 1, i);

			WinGrainI *grain = unit->mGrains + unit->mNumActive++;

			SETUP_GRAIN_BUF
			GRAIN_BUF
			CHECK_BUF

			if (bufChannels != 1) continue;

			float bufSampleRate = buf->samplerate;
			float bufRateScale = bufSampleRate * SAMPLEDUR;
			double loopMax = (double)bufFrames;

//			WinGrainI *grain = unit->mGrains + unit->mNumActive++;
//			grain->bufnum = bufnum;

			double rate = grain->rate = IN_AT(unit, 3, i) * bufRateScale;
			double phase = IN_AT(unit, 4, i) * bufFrames;
			grain->interp = (int)IN_AT(unit, 5, i);

			grain->mWindowA = (int)IN_AT(unit, 6, i);
			grain->mWindowB = (int)IN_AT(unit, 7, i);
			float ifac = grain->ifac = IN_AT(unit, 8, i);

			double winPosA = grain->winPosA = 0.f;
			double winPosB = grain->winPosB = 0.f;
			GET_INTERP_GRAIN_WIN
			double counter = winSize * SAMPLERATE;
			counter = sc_max(4., counter);
			grain->counter = (int)counter;
			double winIncA = grain->winIncA = (double)windowSamplesA / counter;
			double winIncB = grain->winIncB = (double)windowSamplesB / counter;
			double amp = lininterp(ifac, windowDataA[0], windowDataB[0]);

			float *out1 = out + i;

			int nsmps = sc_min(grain->counter, inNumSamples - i);
			if (grain->interp >= 4) {
				for (int j=0; j<nsmps; ++j) {
					BUF_GRAIN_LOOP_BODY_4;
					phase += rate;
					BUF_INTERP_GRAIN_AMP
				}
			} else if (grain->interp >= 2) {
				for (int j=0; j<nsmps; ++j) {
					BUF_GRAIN_LOOP_BODY_2;
					phase += rate;
					BUF_INTERP_GRAIN_AMP
				}
			} else {
				for (int j=0; j<nsmps; ++j) {
					BUF_GRAIN_LOOP_BODY_1;
					phase += rate;
					BUF_INTERP_GRAIN_AMP
				}
			}

			grain->phase = phase;
			grain->curamp = amp;
			grain->winPosA = winPosA;
			grain->winPosB = winPosB;
			// end change
			grain->counter -= nsmps;
			if (grain->counter <= 0) {
				// remove grain
				*grain = unit->mGrains[--unit->mNumActive];
			}
		    }
		}
		unit->curtrig = trig[i];
	    }

}

void BufGrainI_next_k(BufGrainI *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);
	float *out;
	out = ZOUT(0);
	float trig = IN0(0);

	World *world = unit->mWorld;
//	SndBuf *bufs = world->mSndBufs;

	for (int i=0; i < unit->mNumActive; ) {
		WinGrainI *grain = unit->mGrains + i;
//		uint32 bufnum = grain->bufnum;

		GRAIN_BUF
		CHECK_BUF

		if (bufChannels != 1) {
			 ++i;
			 continue;
		}
		GET_INTERP_GRAIN_WIN
		double amp = grain->curamp;
		double winIncA = grain->winIncA;
		double winPosA = grain->winPosA;
		double winIncB = grain->winIncB;
		double winPosB = grain->winPosB;

		double loopMax = (double)bufFrames;

		double rate = grain->rate;
		double phase = grain->phase;

		float *out1 = out;

		int nsmps = sc_min(grain->counter, inNumSamples);
			if (grain->interp >= 4) {
				for (int j=0; j<nsmps; ++j) {
					BUF_GRAIN_LOOP_BODY_4;
					phase += rate;
					BUF_INTERP_GRAIN_AMP
				}
			} else if (grain->interp >= 2) {
				for (int j=0; j<nsmps; ++j) {
					BUF_GRAIN_LOOP_BODY_2;
					phase += rate;
					BUF_INTERP_GRAIN_AMP
				}
			} else {
				for (int j=0; j<nsmps; ++j) {
					BUF_GRAIN_LOOP_BODY_1;
					phase += rate;
					BUF_INTERP_GRAIN_AMP
				}
			}


		grain->phase = phase;
		grain->curamp = amp;
		grain->winPosA = winPosA;
		grain->winPosB = winPosB;

		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	}

		if ((trig > 0) && (unit->curtrig <=0)) {
			// start a grain
			if (unit->mNumActive+1 >= kMaxSynthGrains) {
			    Print("Too many grains!\n");
			} else {
			uint32 bufnum = (uint32)IN0(2);

			WinGrainI *grain = unit->mGrains + unit->mNumActive++;

			SETUP_GRAIN_BUF
			GRAIN_BUF
			CHECK_BUF

			float winSize = IN0(1);

			float bufSampleRate = buf->samplerate;
			float bufRateScale = bufSampleRate * SAMPLEDUR;
			double loopMax = (double)bufFrames;

//			WinGrainI *grain = unit->mGrains + unit->mNumActive++;
//			grain->bufnum = bufnum;

			double rate = grain->rate = IN0(3) * bufRateScale;
			double phase = IN0(4) * bufFrames;
			grain->interp = (int)IN0(5);

			grain->mWindowA = (int)IN0(6);
			grain->mWindowB = (int)IN0(7);
			float ifac = grain->ifac = IN0(8);

			double winPosA = grain->winPosA = 0.f;
			double winPosB = grain->winPosB = 0.f;
			GET_INTERP_GRAIN_WIN
			double counter = winSize * SAMPLERATE;
			counter = sc_max(4., counter);
			grain->counter = (int)counter;
			double winIncA = grain->winIncA = (double)windowSamplesA / counter;
			double winIncB = grain->winIncB = (double)windowSamplesB / counter;
			double amp = lininterp(ifac, windowDataA[0], windowDataB[0]);
			float *out1 = out;
			int nsmps = sc_min(grain->counter, inNumSamples);
			if (grain->interp >= 4) {
				for (int j=0; j<nsmps; ++j) {
					BUF_GRAIN_LOOP_BODY_4;
					phase += rate;
					BUF_INTERP_GRAIN_AMP
				}
			} else if (grain->interp >= 2) {
				for (int j=0; j<nsmps; ++j) {
					BUF_GRAIN_LOOP_BODY_2;
					phase += rate;
					BUF_INTERP_GRAIN_AMP
				}
			} else {
				for (int j=0; j<nsmps; ++j) {
					BUF_GRAIN_LOOP_BODY_1;
					phase += rate;
					BUF_INTERP_GRAIN_AMP
				}
			}

			grain->phase = phase;
			grain->curamp = amp;
			grain->winPosA = winPosA;
			grain->winPosB = winPosB;
			// end change
			grain->counter -= inNumSamples;
			if (grain->counter <= 0) {
				// remove grain
				*grain = unit->mGrains[--unit->mNumActive];
			}
		    }
		}
		unit->curtrig = trig;
	    }

void BufGrainI_Ctor(BufGrainI *unit)
{
	if (INRATE(0) == calc_FullRate)
	    SETCALC(BufGrainI_next_a);
	    else
	    SETCALC(BufGrainI_next_k);
	unit->mNumActive = 0;
	unit->curtrig = 0.f;
	BufGrainI_next_k(unit, 1); // should be _k
}

//////////////////// InGrain ////////////////////

void InGrain_next_a(InGrain *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);
	float *out = OUT(0);
	float *trig = IN(0);
	float *in = IN(2);
	float winSize;

	for (int i=0; i < unit->mNumActive; ) {
		IGrain *grain = unit->mGrains + i;
		double b1 = grain->b1;
		double y1 = grain->y1;
		double y2 = grain->y2;

		int nsmps = sc_min(grain->counter, inNumSamples);
		for (int j=0; j<nsmps; ++j) {
		    float amp = y1 * y1;
		    float outval = amp * in[j];
		    out[j] += outval;
		    double y0 = b1 * y1 - y2;
		    y2 = y1;
		    y1 = y0;
		    }
		grain->y1 = y1;
		grain->y2 = y2;
		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	}

	for (int i=0; i<inNumSamples; ++i) {
		if ((unit->curtrig <= 0) && (trig[i] > 0.0)) {
			// start a grain
			if (unit->mNumActive+1 >= kMaxSynthGrains) {Print("Too many grains!\n"); break;}
			IGrain *grain = unit->mGrains + unit->mNumActive++;
			// INRATE(1) == calcFullRate
			winSize = IN_AT(unit, 1, i);
			double counter = winSize * SAMPLERATE;
			counter = sc_max(4., counter);
			grain->counter = (int)counter;
			double w = pi / counter;
			double b1 = grain->b1 = 2. * cos(w);
			double y1 = sin(w);
			double y2 = 0.;
			float *out1 = out + i;
			float *in1 = in + i;
			int nsmps = sc_min(grain->counter, inNumSamples - i);
			for (int j=0; j<nsmps; ++j) {
			    float amp = y1 * y1;
			    float outval = amp * in1[j];
			    out1[j] += outval;
			    double y0 = b1 * y1 - y2;
			    y2 = y1;
			    y1 = y0;
			}
			grain->y1 = y1;
			grain->y2 = y2;
			grain->counter -= nsmps;
			if (grain->counter <= 0) {
				// remove grain
				*grain = unit->mGrains[--unit->mNumActive];
			}
		}
		unit->curtrig = trig[i];
	}
}

void InGrain_next_k(InGrain *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);
	float *out = OUT(0);
	float trig = IN0(0);
	float *in = IN(2);
	float winSize;

	for (int i=0; i < unit->mNumActive; ) {
		IGrain *grain = unit->mGrains + i;
		double b1 = grain->b1;
		double y1 = grain->y1;
		double y2 = grain->y2;

		int nsmps = sc_min(grain->counter, inNumSamples);
		for (int j=0; j<nsmps; ++j) {
		    float amp = y1 * y1;
		    float outval = amp * in[j];
		    out[j] += outval;
		    double y0 = b1 * y1 - y2;
		    y2 = y1;
		    y1 = y0;
		    }
		grain->y1 = y1;
		grain->y2 = y2;
		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	}

	if ((unit->curtrig <= 0) && (trig > 0.0)) {
		    // start a grain
		    if (unit->mNumActive+1 >= kMaxSynthGrains)
		    {
		    Print("Too many grains!\n");
		    } else {
		    IGrain *grain = unit->mGrains + unit->mNumActive++;
		    winSize = IN0(1);
		    double counter = winSize * SAMPLERATE;
		    counter = sc_max(4., counter);
		    grain->counter = (int)counter;
		    double w = pi / counter;
		    double b1 = grain->b1 = 2. * cos(w);
		    double y1 = sin(w);
		    double y2 = 0.;
		    int nsmps = sc_min(grain->counter, inNumSamples);
		    for (int j=0; j<nsmps; ++j) {
			float amp = y1 * y1;
			float outval = amp * in[j];
			out[j] += outval;
			double y0 = b1 * y1 - y2;
			y2 = y1;
			y1 = y0;
		    }
		    grain->y1 = y1;
		    grain->y2 = y2;
		    grain->counter -= inNumSamples;
		    if (grain->counter <= 0) {
			    // remove grain
			    *grain = unit->mGrains[--unit->mNumActive];
		    }
		}
	    }
	    unit->curtrig = trig;
}


void InGrain_Ctor(InGrain *unit)
{

	if (INRATE(0) == calc_FullRate)
	    SETCALC(InGrain_next_a);
	    else
	    SETCALC(InGrain_next_k);
	unit->mNumActive = 0;
	unit->curtrig = 0.f;
	InGrain_next_k(unit, 1);
}

///////////////////////// InGrainB ////////////////////////////

void InGrainB_next_a(InGrainB *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);
	float *out = OUT(0);
	float *trig = IN(0);
	float *in = IN(2);
	float winSize;

	for (int i=0; i < unit->mNumActive; ) {
		IGrainB *grain = unit->mGrains + i;
		GET_GRAIN_WIN

		double winInc = grain->winInc;
		double winPos = grain->winPos;
		double amp = grain->curamp;
		int nsmps = sc_min(grain->counter, inNumSamples);
		for (int j=0; j<nsmps; ++j) {
		    float outval = amp * in[j];
		    out[j] += outval;
		    BUF_GRAIN_AMP
		    }
		grain->winPos = winPos;
		grain->curamp = amp;
		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	}

	for (int i=0; i<inNumSamples; ++i) {
		if ((unit->curtrig <= 0) && (trig[i] > 0.0)) {
			// start a grain
			if (unit->mNumActive+1 >= kMaxSynthGrains) {Print("Too many grains!\n"); break;}
			IGrainB *grain = unit->mGrains + unit->mNumActive++;
			winSize = IN_AT(unit, 1, i);
			grain->mWindow = (int)IN_AT(unit, 3, i);
			double winPos = grain->winPos = 0.f;
			GET_GRAIN_WIN
			double counter = winSize * SAMPLERATE;
			double winInc = grain->winInc = (double)windowSamples / counter;
			counter = sc_max(4., counter);
			grain->counter = (int)counter;
			double amp = windowData[0];
			float *out1 = out + i;
			float *in1 = in + i;
			int nsmps = sc_min(grain->counter, inNumSamples - i);
			for (int j=0; j<nsmps; ++j) {
			    float outval = amp * in1[j];
			    out1[j] += outval;
			    BUF_GRAIN_AMP
			}
			grain->curamp = amp;
			grain->winPos = winPos;
			grain->counter -= nsmps;
			if (grain->counter <= 0) {
				// remove grain
				*grain = unit->mGrains[--unit->mNumActive];
			}
		}
		unit->curtrig = trig[i];
	}
}

void InGrainB_next_k(InGrainB *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);
	float *out = OUT(0);
	float trig = IN0(0);
	float *in = IN(2);
	float winSize;

	for (int i=0; i < unit->mNumActive; ) {
		IGrainB *grain = unit->mGrains + i;
		double amp = grain->curamp;
		GET_GRAIN_WIN
		double winInc = grain->winInc;
		double winPos = grain->winPos;
		int nsmps = sc_min(grain->counter, inNumSamples);
		for (int j=0; j<nsmps; ++j) {
		    float outval = amp * in[j];
		    out[j] += outval;
		    BUF_GRAIN_AMP
		    }
		grain->winPos = winPos;
		grain->curamp = amp;
		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	}

	    if ((unit->curtrig <= 0) && (trig > 0.0)) {
		    // start a grain
		    if (unit->mNumActive+1 >= kMaxSynthGrains) {
			Print("Too many grains!\n");
			} else {
			IGrainB *grain = unit->mGrains + unit->mNumActive++;
			// INRATE(1) == calcFullRate
			winSize = IN0(1);
			grain->mWindow = IN0(3);
			double winPos = grain->winPos = 0.f;
			GET_GRAIN_WIN
			double counter = winSize * SAMPLERATE;
			double winInc = grain->winInc = (double)windowSamples / counter;
			counter = sc_max(4., counter);
			grain->counter = (int)counter;
			double amp = windowData[0];
			int nsmps = sc_min(grain->counter, inNumSamples);
			for (int j=0; j<nsmps; ++j) {
			    float outval = amp * in[j];
			    out[j] += outval;
			    BUF_GRAIN_AMP
			}
			grain->winPos = winPos;
			grain->counter -= inNumSamples;
			grain->curamp = amp;
			if (grain->counter <= 0) {
				// remove grain
				*grain = unit->mGrains[--unit->mNumActive];
			}
		    }
		}
	    unit->curtrig = trig;

}


void InGrainB_Ctor(InGrainB *unit)
{
	if (INRATE(0) == calc_FullRate)
	    SETCALC(InGrainB_next_a);
	    else
	    SETCALC(InGrainB_next_k);
	unit->curtrig = 0.f;
	unit->mNumActive = 0;
	InGrainB_next_k(unit, 1);
}

///////////////////////// SinGrainI ////////////////////////////

void InGrainI_next_a(InGrainI *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);
	float *out = OUT(0);
	float *trig = IN(0);
	float *in = IN(2);
	float winSize;

	for (int i=0; i < unit->mNumActive; ) {
		IGrainI *grain = unit->mGrains + i;
		GET_INTERP_GRAIN_WIN

		double winIncA = grain->winIncA;
		double winPosA = grain->winPosA;
		double winIncB = grain->winIncB;
		double winPosB = grain->winPosB;
		double amp = grain->curamp;
		int nsmps = sc_min(grain->counter, inNumSamples);
		for (int j=0; j<nsmps; ++j) {
		    float outval = amp * in[j];
		    out[j] += outval;
		    BUF_INTERP_GRAIN_AMP
		    }
		grain->winPosA = winPosA;
		grain->winPosB = winPosB;
		grain->curamp = amp;
		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	}

	for (int i=0; i<inNumSamples; ++i) {
		if ((unit->curtrig <= 0) && (trig[i] > 0.0)) {
			// start a grain
			if (unit->mNumActive+1 >= kMaxSynthGrains) {Print("Too many grains!\n"); break;}
			IGrainI *grain = unit->mGrains + unit->mNumActive++;
			winSize = IN_AT(unit, 1, i);
			grain->mWindowA = (int)IN_AT(unit, 3, i);
			grain->mWindowB = (int)IN_AT(unit, 4, i);
			float ifac = grain->ifac = IN_AT(unit, 5, i);
			double winPosA = grain->winPosA = 0.f;
			double winPosB = grain->winPosB = 0.f;
			GET_INTERP_GRAIN_WIN
			double counter = winSize * SAMPLERATE;
			double winIncA = grain->winIncA = (double)windowSamplesA / counter;
			double winIncB = grain->winIncB = (double)windowSamplesB / counter;
			counter = sc_max(4., counter);
			grain->counter = (int)counter;
			double amp = lininterp(ifac, windowDataA[0], windowDataB[0]);
			float *out1 = out + i;
			float *in1 = in + i;
			int nsmps = sc_min(grain->counter, inNumSamples - i);
			for (int j=0; j<nsmps; ++j) {
			    float outval = amp * in1[j];
			    out1[j] += outval;
			    BUF_INTERP_GRAIN_AMP
			}
			grain->curamp = amp;
			grain->winPosA = winPosA;
			grain->winPosB = winPosB;
			grain->counter -= nsmps;
			if (grain->counter <= 0) {
				// remove grain
				*grain = unit->mGrains[--unit->mNumActive];
			}
		}
		unit->curtrig = trig[i];
	}
}

void InGrainI_next_k(InGrainI *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);
	float *out = OUT(0);
	float trig = IN0(0);
	float *in = IN(2);
	float winSize;

	for (int i=0; i < unit->mNumActive; ) {
		IGrainI *grain = unit->mGrains + i;
		GET_INTERP_GRAIN_WIN

		double winIncA = grain->winIncA;
		double winPosA = grain->winPosA;
		double winIncB = grain->winIncB;
		double winPosB = grain->winPosB;
		double amp = grain->curamp;
		int nsmps = sc_min(grain->counter, inNumSamples);
		for (int j=0; j<nsmps; ++j) {
		    float outval = amp * in[j];
		    out[j] += outval;
		    BUF_INTERP_GRAIN_AMP
		    }
		grain->winPosA = winPosA;
		grain->winPosB = winPosB;
		grain->curamp = amp;
		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	}

	    if ((unit->curtrig <= 0) && (trig > 0.0)) {
		    // start a grain
		    if (unit->mNumActive+1 >= kMaxSynthGrains) {
			Print("Too many grains!\n");
			} else {
			IGrainI *grain = unit->mGrains + unit->mNumActive++;
			winSize = IN0(1);
			grain->mWindowA = (int)IN0(3);
			grain->mWindowB = (int)IN0(4);
			float ifac = grain->ifac = IN0(5);
			double winPosA = grain->winPosA = 0.f;
			double winPosB = grain->winPosB = 0.f;
			GET_INTERP_GRAIN_WIN
			double counter = winSize * SAMPLERATE;
			double winIncA = grain->winIncA = (double)windowSamplesA / counter;
			double winIncB = grain->winIncB = (double)windowSamplesB / counter;
			counter = sc_max(4., counter);
			grain->counter = (int)counter;
			double amp = lininterp(ifac, windowDataA[0], windowDataB[0]);
			int nsmps = sc_min(grain->counter, inNumSamples);
			for (int j=0; j<nsmps; ++j) {
			    float outval = amp * in[j];
			    out[j] += outval;
			    BUF_INTERP_GRAIN_AMP
			}
			grain->curamp = amp;
			grain->winPosA = winPosA;
			grain->winPosB = winPosB;
			grain->counter -= inNumSamples;
			if (grain->counter <= 0) {
				// remove grain
				*grain = unit->mGrains[--unit->mNumActive];
			}
		    }
		}
	    unit->curtrig = trig;

}


void InGrainI_Ctor(InGrainI *unit)
{
	if (INRATE(0) == calc_FullRate)
	    SETCALC(InGrainI_next_a);
	    else
	    SETCALC(InGrainI_next_k);
	unit->curtrig = 0.f;
	unit->mNumActive = 0;
	InGrainI_next_k(unit, 1);
}


///////////////////// Granular synths with Ambisonic output ///////////////////////

///////////// First, define some macros! ///////////////////

// sets up the pointers to the output
#define SETUP_BF_OUTS \
    float *Wout = OUT(0); \
    float *Xout = OUT(1); \
    float *Yout = OUT(2); \
    float *Zout = OUT(3); \

// sets up the initial output pointers
#define SETUP_INIT_BF_OUTS \
    float *Wout1 = Wout + i; \
    float *Xout1 = Xout + i; \
    float *Yout1 = Yout + i; \
    float *Zout1 = Zout + i; \

// calculates the position in space of the grain. saves vals to struct inside each grain
#define CALC_BF_COEFS \
    float sina = sin(azimuth); \
    float sinb = sin(elevation); \
    float cosa = cos(azimuth); \
    float cosb = cos(elevation); \
    float sinint, cosint; \
	    if(rho >= 1) { \
		    float intrho = 1 / pow(rho, 1.5); \
		    sinint = (rsqrt2 * (sin(0.78539816339745))) * intrho; \
		    cosint =  (rsqrt2 * (cos(0.78539816339745))) * intrho; \
		    }  else {  \
		    sinint = rsqrt2 * (sin(0.78539816339745 * rho)); \
		    cosint = rsqrt2 * (cos(0.78539816339745 * rho)); \
		    }; \
    float X_amp = grain->m_xamp = cosa * cosb * sinint; \
    float Y_amp = grain->m_yamp = sina * cosb  * sinint; \
    float Z_amp = grain->m_zamp = sinb * sinint; \
    float W_amp = 1.;\
    if(wComp > 0.){ \
	W_amp = grain->m_wamp = cosint * (1 - (0.293 * ((X_amp * X_amp) + (Y_amp * Y_amp) + (Z_amp * Z_amp)))); \
    } else { \
	W_amp = grain->m_wamp = cosint * 0.707; \
    }

// grab and setup the _amp scalers;
#define GET_BF_AMPS \
    float W_amp = grain->m_wamp; \
    float X_amp = grain->m_xamp; \
    float Y_amp = grain->m_yamp; \
    float Z_amp = grain->m_zamp; \

// write to the four outputs
#define OUT_BF \
    Wout[j] += outval * W_amp; \
    Xout[j] += outval * X_amp; \
    Yout[j] += outval * Y_amp; \
    Zout[j] += outval * Z_amp; \

// for the initial start of the grain, write to the outputs
#define OUT_INIT_BF \
    Wout1[j] += outval * W_amp; \
    Xout1[j] += outval * X_amp; \
    Yout1[j] += outval * Y_amp; \
    Zout1[j] += outval * Z_amp; \

#define GRAIN_LOOP_BF_BODY_4 \
		float amp = y1 * y1; \
		phase = sc_gloop(phase, loopMax); \
		int32 iphase = (int32)phase; \
		float* table1 = bufData + iphase; \
		float* table0 = table1 - 1; \
		float* table2 = table1 + 1; \
		float* table3 = table1 + 2; \
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
		float a = table0[0]; \
		float b = table1[0]; \
		float c = table2[0]; \
		float d = table3[0]; \
		float outval = amp * cubicinterp(fracphase, a, b, c, d); \
		ZXP(Wout1) += outval * W_amp; \
		ZXP(Xout1) += outval * X_amp; \
		ZXP(Yout1) += outval * Y_amp; \
		ZXP(Zout1) += outval * Z_amp; \
		double y0 = b1 * y1 - y2; \
		y2 = y1; \
		y1 = y0; \


#define GRAIN_LOOP_BF_BODY_2 \
		float amp = y1 * y1; \
		phase = sc_gloop(phase, loopMax); \
		int32 iphase = (int32)phase; \
		float* table1 = bufData + iphase; \
		float* table2 = table1 + 1; \
		if (iphase > guardFrame) { \
			table2 -= bufSamples; \
		} \
		float fracphase = phase - (double)iphase; \
		float b = table1[0]; \
		float c = table2[0]; \
		float outval = amp * (b + fracphase * (c - b)); \
		ZXP(Wout1) += outval * W_amp; \
		ZXP(Xout1) += outval * X_amp; \
		ZXP(Yout1) += outval * Y_amp; \
		ZXP(Zout1) += outval * Z_amp; \
		double y0 = b1 * y1 - y2; \
		y2 = y1; \
		y1 = y0; \


#define GRAIN_LOOP_BF_BODY_1 \
		float amp = y1 * y1; \
		phase = sc_gloop(phase, loopMax); \
		int32 iphase = (int32)phase; \
		float outval = amp * bufData[iphase]; \
		ZXP(Wout1) += outval * W_amp; \
		ZXP(Xout1) += outval * X_amp; \
		ZXP(Yout1) += outval * Y_amp; \
		ZXP(Zout1) += outval * Z_amp; \
		double y0 = b1 * y1 - y2; \
		y2 = y1; \
		y1 = y0; \

#define BUF_GRAIN_LOOP_BF_BODY_4 \
		phase = sc_gloop(phase, loopMax); \
		int32 iphase = (int32)phase; \
		float* table1 = bufData + iphase; \
		float* table0 = table1 - 1; \
		float* table2 = table1 + 1; \
		float* table3 = table1 + 2; \
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
		float a = table0[0]; \
		float b = table1[0]; \
		float c = table2[0]; \
		float d = table3[0]; \
		float outval = amp * cubicinterp(fracphase, a, b, c, d); \
		ZXP(Wout1) += outval * W_amp; \
		ZXP(Xout1) += outval * X_amp; \
		ZXP(Yout1) += outval * Y_amp; \
		ZXP(Zout1) += outval * Z_amp; \

#define BUF_GRAIN_LOOP_BF_BODY_2 \
		phase = sc_gloop(phase, loopMax); \
		int32 iphase = (int32)phase; \
		float* table1 = bufData + iphase; \
		float* table2 = table1 + 1; \
		if (iphase > guardFrame) { \
			table2 -= bufSamples; \
		} \
		float fracphase = phase - (double)iphase; \
		float b = table1[0]; \
		float c = table2[0]; \
		float outval = amp * (b + fracphase * (c - b)); \
		ZXP(Wout1) += outval * W_amp; \
		ZXP(Xout1) += outval * X_amp; \
		ZXP(Yout1) += outval * Y_amp; \
		ZXP(Zout1) += outval * Z_amp; \

// amp needs to be calculated by looking up values in window

#define BUF_GRAIN_LOOP_BF_BODY_1 \
		phase = sc_gloop(phase, loopMax); \
		int32 iphase = (int32)phase; \
		float outval = amp * bufData[iphase]; \
		ZXP(Wout1) += outval * W_amp; \
		ZXP(Xout1) += outval * X_amp; \
		ZXP(Yout1) += outval * Y_amp; \
		ZXP(Zout1) += outval * Z_amp; \



//////////////////////////////////////////////////

//////////////////// InGrainBF ///////////////////

void InGrainBF_next_a(InGrainBF *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);

	SETUP_BF_OUTS

	float *trig = IN(0);
	float *in = IN(2);
	float winSize;
	float wComp = unit->m_wComp;
	for (int i=0; i < unit->mNumActive; ) {
		IGrainBF *grain = unit->mGrains + i;
		double b1 = grain->b1;
		double y1 = grain->y1;
		double y2 = grain->y2;

		GET_BF_AMPS

		int nsmps = sc_min(grain->counter, inNumSamples);
		for (int j=0; j<nsmps; ++j) {
		    float amp = y1 * y1;
		    float outval = amp * in[j];

		    OUT_BF

		    double y0 = b1 * y1 - y2;
		    y2 = y1;
		    y1 = y0;
		    }
		grain->y1 = y1;
		grain->y2 = y2;
		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	}
	for (int i=0; i<inNumSamples; ++i) {
		if ((unit->curtrig <= 0) && (trig[i] > 0.0)) {
			// start a grain
			if (unit->mNumActive+1 >= kMaxSynthGrains) {Print("Too many grains!\n"); break;}
			IGrainBF *grain = unit->mGrains + unit->mNumActive++;
			// INRATE(1) == calcFullRate
			winSize = IN_AT(unit, 1, i);
			double counter = winSize * SAMPLERATE;
			counter = sc_max(4., counter);
			grain->counter = (int)counter;
			double w = pi / counter;
			double b1 = grain->b1 = 2. * cos(w);
			double y1 = sin(w);
			double y2 = 0.;
			float azimuth = IN_AT(unit, 3, i);
			float elevation = IN_AT(unit, 4, i);
			float rho = IN_AT(unit, 5, i);

			CALC_BF_COEFS
			SETUP_INIT_BF_OUTS

			float *in1 = in + i;
			int nsmps = sc_min(grain->counter, inNumSamples - i);
			for (int j=0; j<nsmps; ++j) {
			    float amp = y1 * y1;
			    float outval = amp * in1[j];

			    OUT_INIT_BF

			    double y0 = b1 * y1 - y2;
			    y2 = y1;
			    y1 = y0;
			}
			grain->y1 = y1;
			grain->y2 = y2;
			grain->counter -= nsmps;
			if (grain->counter <= 0) {
				// remove grain
				*grain = unit->mGrains[--unit->mNumActive];
			}
		}
		unit->curtrig = trig[i];
	}
}

void InGrainBF_next_k(InGrainBF *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);

	SETUP_BF_OUTS

	float trig = IN0(0);
	float *in = IN(2);
	float winSize;
	float wComp = unit->m_wComp;

	for (int i=0; i < unit->mNumActive; ) {
		IGrainBF *grain = unit->mGrains + i;
		double b1 = grain->b1;
		double y1 = grain->y1;
		double y2 = grain->y2;

		GET_BF_AMPS

		int nsmps = sc_min(grain->counter, inNumSamples);
		for (int j=0; j<nsmps; ++j) {
		    float amp = y1 * y1;
		    float outval = amp * in[j];

		    OUT_BF

		    double y0 = b1 * y1 - y2;
		    y2 = y1;
		    y1 = y0;
		    }
		grain->y1 = y1;
		grain->y2 = y2;
		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	}

	if ((unit->curtrig <= 0) && (trig > 0.0)) {
		    // start a grain
		    if (unit->mNumActive+1 >= kMaxSynthGrains)
		    {
		    Print("Too many grains!\n");
		    } else {
		    IGrainBF *grain = unit->mGrains + unit->mNumActive++;
		    winSize = IN0(1);
		    double counter = winSize * SAMPLERATE;
		    counter = sc_max(4., counter);
		    grain->counter = (int)counter;
		    double w = pi / counter;
		    double b1 = grain->b1 = 2. * cos(w);
		    double y1 = sin(w);
		    double y2 = 0.;
		    float azimuth = IN0(3);
		    float elevation = IN0(4);
		    float rho = IN0(5);

		    CALC_BF_COEFS

		    int nsmps = sc_min(grain->counter, inNumSamples);
		    for (int j=0; j<nsmps; ++j) {
			float amp = y1 * y1;
			float outval = amp * in[j];

			OUT_BF

			double y0 = b1 * y1 - y2;
			y2 = y1;
			y1 = y0;
		    }
		    grain->y1 = y1;
		    grain->y2 = y2;
		    grain->counter -= nsmps;
		    if (grain->counter <= 0) {
			    // remove grain
			    *grain = unit->mGrains[--unit->mNumActive];
		    }
		}
	    }
	    unit->curtrig = trig;
}


void InGrainBF_Ctor(InGrainBF *unit)
{

	if (INRATE(0) == calc_FullRate)
	    SETCALC(InGrainBF_next_a);
	    else
	    SETCALC(InGrainBF_next_k);
	unit->mNumActive = 0;
	unit->curtrig = 0.f;
	InGrainBF_next_k(unit, 1);
	unit->m_wComp = IN0(6);
}


///////////////////////// InGrainBBF ////////////////////////////

void InGrainBBF_next_a(InGrainBBF *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);

	SETUP_BF_OUTS

	float *trig = IN(0);
	float *in = IN(2);
	float winSize;
	float wComp = unit->m_wComp;

	for (int i=0; i < unit->mNumActive; ) {
		IGrainBBF *grain = unit->mGrains + i;

		GET_GRAIN_WIN
		GET_BF_AMPS

		double winInc = grain->winInc;
		double winPos = grain->winPos;
		double amp = grain->curamp;
		int nsmps = sc_min(grain->counter, inNumSamples);
		for (int j=0; j<nsmps; ++j) {
		    float outval = amp * in[j];
		    OUT_BF
		    BUF_GRAIN_AMP
		    }
		grain->winPos = winPos;
		grain->curamp = amp;
		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	}

	for (int i=0; i<inNumSamples; ++i) {
		if ((unit->curtrig <= 0) && (trig[i] > 0.0)) {
			// start a grain
			if (unit->mNumActive+1 >= kMaxSynthGrains) {Print("Too many grains!\n"); break;}
			IGrainBBF *grain = unit->mGrains + unit->mNumActive++;
			winSize = IN_AT(unit, 1, i);
			grain->mWindow = (int)IN_AT(unit, 3, i);
			double winPos = grain->winPos = 0.f;
			GET_GRAIN_WIN
			float azimuth = IN_AT(unit, 4, i);
			float elevation = IN_AT(unit, 5, i);
			float rho = IN_AT(unit, 6, i);

			CALC_BF_COEFS
			SETUP_INIT_BF_OUTS

			double counter = winSize * SAMPLERATE;
			double winInc = grain->winInc = (double)windowSamples / counter;
			counter = sc_max(4., counter);
			grain->counter = (int)counter;
			double amp = windowData[0];
			float *in1 = in + i;
			int nsmps = sc_min(grain->counter, inNumSamples - i);
			for (int j=0; j<nsmps; ++j) {
			    float outval = amp * in1[j];

			    OUT_INIT_BF
			    BUF_GRAIN_AMP
			}
			grain->curamp = amp;
			grain->winPos = winPos;
			grain->counter -= nsmps;
			if (grain->counter <= 0) {
				// remove grain
				*grain = unit->mGrains[--unit->mNumActive];
			}
		}
		unit->curtrig = trig[i];
	}
}

void InGrainBBF_next_k(InGrainBBF *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);

	SETUP_BF_OUTS

	float trig = IN0(0);
	float *in = IN(2);
	float winSize;
	float wComp = unit->m_wComp;

	for (int i=0; i < unit->mNumActive; ) {
		IGrainBBF *grain = unit->mGrains + i;
		double amp = grain->curamp;

		GET_GRAIN_WIN
		GET_BF_AMPS

		double winInc = grain->winInc;
		double winPos = grain->winPos;
		int nsmps = sc_min(grain->counter, inNumSamples);
		for (int j=0; j<nsmps; ++j) {
		    float outval = amp * in[j];
		    OUT_BF
		    BUF_GRAIN_AMP
		    }
		grain->winPos = winPos;
		grain->curamp = amp;
		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	}

	    if ((unit->curtrig <= 0) && (trig > 0.0)) {
		    // start a grain
		    if (unit->mNumActive+1 >= kMaxSynthGrains) {
			Print("Too many grains!\n");
			} else {
			IGrainBBF *grain = unit->mGrains + unit->mNumActive++;
			// INRATE(1) == calcFullRate
			winSize = IN0(1);
			grain->mWindow = IN0(3);
			double winPos = grain->winPos = 0.f;
			GET_GRAIN_WIN
			float azimuth = IN0(4);
			float elevation = IN0(5);
			float rho = IN0(6);

			CALC_BF_COEFS

			double counter = winSize * SAMPLERATE;
			double winInc = grain->winInc = (double)windowSamples / counter;
			counter = sc_max(4., counter);
			grain->counter = (int)counter;
			double amp = windowData[0];
			int nsmps = sc_min(grain->counter, inNumSamples);
			for (int j=0; j<nsmps; ++j) {
			    float outval = amp * in[j];
			    OUT_BF

			    BUF_GRAIN_AMP
			}
			grain->winPos = winPos;
			grain->counter -= inNumSamples;
			grain->curamp = amp;
			if (grain->counter <= 0) {
				// remove grain
				*grain = unit->mGrains[--unit->mNumActive];
			}
		    }
		}
	    unit->curtrig = trig;

}


void InGrainBBF_Ctor(InGrainBBF *unit)
{
	if (INRATE(0) == calc_FullRate)
	    SETCALC(InGrainBBF_next_a);
	    else
	    SETCALC(InGrainBBF_next_k);
	unit->curtrig = 0.f;
	unit->mNumActive = 0;
	InGrainBBF_next_k(unit, 1);
	unit->m_wComp = IN0(7);
}

///////////////////////// InGrainIBF ////////////////////////////

void InGrainIBF_next_a(InGrainIBF *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);

	SETUP_BF_OUTS

	float *trig = IN(0);
	float *in = IN(2);
	float winSize;
	float wComp = unit->m_wComp;

	for (int i=0; i < unit->mNumActive; ) {
		IGrainIBF *grain = unit->mGrains + i;
		GET_INTERP_GRAIN_WIN

		double winIncA = grain->winIncA;
		double winPosA = grain->winPosA;
		double winIncB = grain->winIncB;
		double winPosB = grain->winPosB;
		double amp = grain->curamp;

		GET_BF_AMPS

		int nsmps = sc_min(grain->counter, inNumSamples);
		for (int j=0; j<nsmps; ++j) {
		    float outval = amp * in[j];
		    OUT_BF
		    BUF_INTERP_GRAIN_AMP
		    }
		grain->winPosA = winPosA;
		grain->winPosB = winPosB;
		grain->curamp = amp;
		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	}

	for (int i=0; i<inNumSamples; ++i) {
		if ((unit->curtrig <= 0) && (trig[i] > 0.0)) {
			// start a grain
			if (unit->mNumActive+1 >= kMaxSynthGrains) {Print("Too many grains!\n"); break;}
			IGrainIBF *grain = unit->mGrains + unit->mNumActive++;
			winSize = IN_AT(unit, 1, i);
			grain->mWindowA = (int)IN_AT(unit, 3, i);
			grain->mWindowB = (int)IN_AT(unit, 4, i);
			float ifac = grain->ifac = IN_AT(unit, 5, i);
			double winPosA = grain->winPosA = 0.f;
			double winPosB = grain->winPosB = 0.f;
			GET_INTERP_GRAIN_WIN
			double counter = winSize * SAMPLERATE;
			double winIncA = grain->winIncA = (double)windowSamplesA / counter;
			double winIncB = grain->winIncB = (double)windowSamplesB / counter;
			counter = sc_max(4., counter);
			grain->counter = (int)counter;
			double amp = lininterp(ifac, windowDataA[0], windowDataB[0]);
			float azimuth = IN_AT(unit, 6, i);
			float elevation = IN_AT(unit, 7, i);
			float rho = IN_AT(unit, 8, i);

			CALC_BF_COEFS
			SETUP_INIT_BF_OUTS

			float *in1 = in + i;
			int nsmps = sc_min(grain->counter, inNumSamples - i);
			for (int j=0; j<nsmps; ++j) {
			    float outval = amp * in1[j];
			    OUT_INIT_BF
			    BUF_INTERP_GRAIN_AMP
			}
			grain->curamp = amp;
			grain->winPosA = winPosA;
			grain->winPosB = winPosB;
			grain->counter -= nsmps;
			if (grain->counter <= 0) {
				// remove grain
				*grain = unit->mGrains[--unit->mNumActive];
			}
		}
		unit->curtrig = trig[i];
	}
}

void InGrainIBF_next_k(InGrainIBF *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);
	SETUP_BF_OUTS
	float trig = IN0(0);
	float *in = IN(2);
	float winSize;
	float wComp = unit->m_wComp;

	for (int i=0; i < unit->mNumActive; ) {
		IGrainIBF *grain = unit->mGrains + i;
		GET_INTERP_GRAIN_WIN
		GET_BF_AMPS

		double winIncA = grain->winIncA;
		double winPosA = grain->winPosA;
		double winIncB = grain->winIncB;
		double winPosB = grain->winPosB;
		double amp = grain->curamp;
		int nsmps = sc_min(grain->counter, inNumSamples);
		for (int j=0; j<nsmps; ++j) {
		    float outval = amp * in[j];
		    OUT_BF
		    BUF_INTERP_GRAIN_AMP
		    }
		grain->winPosA = winPosA;
		grain->winPosB = winPosB;
		grain->curamp = amp;
		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	}
	    if ((unit->curtrig <= 0) && (trig > 0.0)) {
		    // start a grain
		    if (unit->mNumActive+1 >= kMaxSynthGrains) {
			Print("Too many grains!\n");
			} else {
			IGrainIBF *grain = unit->mGrains + unit->mNumActive++;
			winSize = IN0(1);
			grain->mWindowA = (int)IN0(3);
			grain->mWindowB = (int)IN0(4);
			float ifac = grain->ifac = IN0(5);
			double winPosA = grain->winPosA = 0.f;
			double winPosB = grain->winPosB = 0.f;
			GET_INTERP_GRAIN_WIN
			double counter = winSize * SAMPLERATE;
			double winIncA = grain->winIncA = (double)windowSamplesA / counter;
			double winIncB = grain->winIncB = (double)windowSamplesB / counter;
			counter = sc_max(4., counter);
			grain->counter = (int)counter;
			float azimuth = IN0(6);
			float elevation = IN0(7);
			float rho = IN0(8);

			CALC_BF_COEFS
			double amp = lininterp(ifac, windowDataA[0], windowDataB[0]);
			int nsmps = sc_min(grain->counter, inNumSamples);
			for (int j=0; j<nsmps; ++j) {
			    float outval = amp * in[j];
			    OUT_BF
			    BUF_INTERP_GRAIN_AMP
			}
			grain->curamp = amp;
			grain->winPosA = winPosA;
			grain->winPosB = winPosB;
			grain->counter -= inNumSamples;
			if (grain->counter <= 0) {
				// remove grain
				*grain = unit->mGrains[--unit->mNumActive];
			}
		    }
		}
	    unit->curtrig = trig;

}


void InGrainIBF_Ctor(InGrainIBF *unit)
{
	if (INRATE(0) == calc_FullRate)
	    SETCALC(InGrainIBF_next_a);
	    else
	    SETCALC(InGrainIBF_next_k);
	unit->curtrig = 0.f;
	unit->mNumActive = 0;
	unit->m_wComp = IN0(9);
	InGrainIBF_next_k(unit, 1);
}

//////////////////////////////////////////////////

///////////////////////////////////SinGrainBF ///////////////////////////////////

void SinGrainBF_next_a(SinGrainBF *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);

	SETUP_BF_OUTS

	float *trig = IN(0);
	float winSize, freq;
	float *table0 = ft->mSineWavetable;
	float *table1 = table0 + 1;
	float wComp = unit->m_wComp;

	for (int i=0; i < unit->mNumActive; ) {
		SGrainBF *grain = unit->mGrains + i;
		double b1 = grain->b1;
		double y1 = grain->y1;
		double y2 = grain->y2;

		GET_BF_AMPS

		int32 thisfreq = grain->freq;
		int32 oscphase = grain->oscphase;

		int nsmps = sc_min(grain->counter, inNumSamples);
		for (int j=0; j<nsmps; ++j) {
		    float amp = y1 * y1;
		    float outval = amp * lookupi1(table0, table1, oscphase, unit->m_lomask);

		    OUT_BF

		    double y0 = b1 * y1 - y2;
		    y2 = y1;
		    y1 = y0;
		    oscphase += thisfreq;
		    }
		grain->y1 = y1;
		grain->y2 = y2;
		grain->oscphase = oscphase;
		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	}

	for (int i=0; i<inNumSamples; ++i) {
		if ((unit->curtrig <= 0) && (trig[i] > 0.0)) {
			// start a grain
			if (unit->mNumActive+1 >= kMaxSynthGrains) {Print("Too many grains!\n"); break;}
			SGrainBF *grain = unit->mGrains + unit->mNumActive++;
			// INRATE(1) == calcFullRate
			freq = IN_AT(unit, 2, i);
			winSize = IN_AT(unit, 1, i);
			int32 thisfreq = grain->freq = (int32)(unit->m_cpstoinc * freq);
			int32 oscphase = 0;
			double counter = winSize * SAMPLERATE;
			counter = sc_max(4., counter);
			grain->counter = (int)counter;
			double w = pi / counter;
			double b1 = grain->b1 = 2. * cos(w);
			double y1 = sin(w);
			double y2 = 0.;

			float azimuth = IN_AT(unit, 3, i);
			float elevation = IN_AT(unit, 4, i);
			float rho = IN_AT(unit, 5, i);

			CALC_BF_COEFS
			SETUP_INIT_BF_OUTS

			int nsmps = sc_min(grain->counter, inNumSamples - i);
			for (int j=0; j<nsmps; ++j) {
			    float amp = y1 * y1;
			    float outval = amp * lookupi1(table0, table1, oscphase, unit->m_lomask);

			    OUT_INIT_BF

			    double y0 = b1 * y1 - y2;
			    y2 = y1;
			    y1 = y0;
			    oscphase += thisfreq;
			}
			grain->oscphase = oscphase;
			grain->y1 = y1;
			grain->y2 = y2;
			grain->counter -= nsmps;
			if (grain->counter <= 0) {
				// remove grain
				*grain = unit->mGrains[--unit->mNumActive];
			}
		}
		unit->curtrig = trig[i];
	}
}

void SinGrainBF_next_k(SinGrainBF *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);

	SETUP_BF_OUTS

	float trig = IN0(0);
	float winSize, freq;
	float *table0 = ft->mSineWavetable;
	float *table1 = table0 + 1;
	float wComp = unit->m_wComp;
	for (int i=0; i < unit->mNumActive; ) {
		SGrainBF *grain = unit->mGrains + i;
		double b1 = grain->b1;
		double y1 = grain->y1;
		double y2 = grain->y2;

		GET_BF_AMPS

		int32 thisfreq = grain->freq;
		int32 oscphase = grain->oscphase;

		int nsmps = sc_min(grain->counter, inNumSamples);
		for (int j=0; j<nsmps; ++j) {
		    float amp = y1 * y1;
		    float outval = amp * lookupi1(table0, table1, oscphase, unit->m_lomask);

		    OUT_BF

		    double y0 = b1 * y1 - y2;
		    y2 = y1;
		    y1 = y0;
		    oscphase += thisfreq;
		    }
		grain->y1 = y1;
		grain->y2 = y2;
		grain->oscphase = oscphase;
		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	}

	if ((unit->curtrig <= 0) && (trig > 0.0)) {
		    // start a grain
		    if (unit->mNumActive+1 >= kMaxSynthGrains)
		    {
		    Print("Too many grains!\n");
		    } else {
		    SGrainBF *grain = unit->mGrains + unit->mNumActive++;
		    freq = IN0(2);
		    winSize = IN0(1);
		    int32 thisfreq = grain->freq = (int32)(unit->m_cpstoinc * freq);
		    int32 oscphase = 0;
		    double counter = winSize * SAMPLERATE;
		    counter = sc_max(4., counter);
		    grain->counter = (int)counter;
		    double w = pi / counter;
		    double b1 = grain->b1 = 2. * cos(w);
		    double y1 = sin(w);
		    double y2 = 0.;

		    float azimuth = IN0(3);
		    float elevation = IN0(4);
		    float rho = IN0(5);

		    CALC_BF_COEFS

		    int nsmps = sc_min(grain->counter, inNumSamples);
		    for (int j=0; j<nsmps; ++j) {
			float amp = y1 * y1;
			float outval = amp * lookupi1(table0, table1, oscphase, unit->m_lomask);

			OUT_BF

			double y0 = b1 * y1 - y2;
			y2 = y1;
			y1 = y0;
			oscphase += thisfreq;
		    }
		    grain->oscphase = oscphase;
		    grain->y1 = y1;
		    grain->y2 = y2;
		    grain->counter -= nsmps;
		    if (grain->counter <= 0) {
			    // remove grain
			    *grain = unit->mGrains[--unit->mNumActive];
		    }
		}
	    }
	    unit->curtrig = trig;
}


void SinGrainBF_Ctor(SinGrainBF *unit)
{

	if (INRATE(0) == calc_FullRate)
	    SETCALC(SinGrainBF_next_a);
	    else
	    SETCALC(SinGrainBF_next_k);
	int tableSizeSin = ft->mSineSize;
	unit->m_lomask = (tableSizeSin - 1) << 3;
	unit->m_radtoinc = tableSizeSin * (rtwopi * 65536.);
	unit->m_cpstoinc = tableSizeSin * SAMPLEDUR * 65536.;
	unit->curtrig = 0.f;
	unit->mNumActive = 0;
	unit->m_wComp = IN0(6);
	SinGrainBF_next_k(unit, 1);
}

///////////////////////// SinGrainB ////////////////////////////

void SinGrainBBF_next_a(SinGrainBBF *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);

	SETUP_BF_OUTS

	float *trig = IN(0);
	float winSize, freq;
	float *table0 = ft->mSineWavetable;
	float *table1 = table0 + 1;
	float wComp = unit->m_wComp;

	for (int i=0; i < unit->mNumActive; ) {
		SGrainBBF *grain = unit->mGrains + i;
		int32 thisfreq = grain->freq;
		int32 oscphase = grain->oscphase;

		GET_GRAIN_WIN
		GET_BF_AMPS

		double winInc = grain->winInc;
		double winPos = grain->winPos;
		double amp = grain->curamp;
		int nsmps = sc_min(grain->counter, inNumSamples);
		for (int j=0; j<nsmps; ++j) {
		    float outval = amp * lookupi1(table0, table1, oscphase, unit->m_lomask);

		    OUT_BF

		    oscphase += thisfreq;
		    BUF_GRAIN_AMP
		    }
		grain->winPos = winPos;
		grain->curamp = amp;
		grain->oscphase = oscphase;
		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	}

	for (int i=0; i<inNumSamples; ++i) {
		if ((unit->curtrig <= 0) && (trig[i] > 0.0)) {
			// start a grain
			if (unit->mNumActive+1 >= kMaxSynthGrains) {Print("Too many grains!\n"); break;}
			SGrainBBF *grain = unit->mGrains + unit->mNumActive++;
			freq = IN_AT(unit, 2, i);
			winSize = IN_AT(unit, 1, i);
			grain->mWindow = (int)IN_AT(unit, 3, i);
			double winPos = grain->winPos = 0.f;
			GET_GRAIN_WIN

			float azimuth = IN_AT(unit, 4, i);
			float elevation = IN_AT(unit, 5, i);
			float rho = IN_AT(unit, 6, i);

			CALC_BF_COEFS
			SETUP_INIT_BF_OUTS

			double counter = winSize * SAMPLERATE;
			double winInc = grain->winInc = (double)windowSamples / counter;
			int32 thisfreq = grain->freq = (int32)(unit->m_cpstoinc * freq);
			int32 oscphase = 0;
			counter = sc_max(4., counter);
			grain->counter = (int)counter;
			double amp = windowData[0];
			int nsmps = sc_min(grain->counter, inNumSamples - i);
			for (int j=0; j<nsmps; ++j) {
			    float outval = amp * lookupi1(table0, table1, oscphase, unit->m_lomask);

			    OUT_INIT_BF

			    oscphase += thisfreq;
			    BUF_GRAIN_AMP
			}
			grain->curamp = amp;
			grain->winPos = winPos;
			grain->oscphase = oscphase;
			grain->counter -= nsmps;
			if (grain->counter <= 0) {
				// remove grain
				*grain = unit->mGrains[--unit->mNumActive];
			}
		}
		unit->curtrig = trig[i];
	}
}

void SinGrainBBF_next_k(SinGrainBBF *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);

	SETUP_BF_OUTS

	float trig = IN0(0);
	float winSize, freq;
	float *table0 = ft->mSineWavetable;
	float *table1 = table0 + 1;
	float wComp = unit->m_wComp;

	for (int i=0; i < unit->mNumActive; ) {
		SGrainBBF *grain = unit->mGrains + i;
		double amp = grain->curamp;
		int32 thisfreq = grain->freq;
		int32 oscphase = grain->oscphase;
		GET_GRAIN_WIN

		GET_BF_AMPS

		double winInc = grain->winInc;
		double winPos = grain->winPos;
		int nsmps = sc_min(grain->counter, inNumSamples);
		for (int j=0; j<nsmps; ++j) {
		    float outval = amp * lookupi1(table0, table1, oscphase, unit->m_lomask);
		    OUT_BF
		    oscphase += thisfreq;
		    BUF_GRAIN_AMP
		    }
		grain->winPos = winPos;
		grain->curamp = amp;
		grain->oscphase = oscphase;
		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	}

	    if ((unit->curtrig <= 0) && (trig > 0.0)) {
		    // start a grain
		    if (unit->mNumActive+1 >= kMaxSynthGrains) {
			Print("Too many grains!\n");
			} else {
			SGrainBBF *grain = unit->mGrains + unit->mNumActive++;
			// INRATE(1) == calcFullRate
			freq = IN0(2);
			winSize = IN0(1);
			grain->mWindow = (int)IN0(3);
			double winPos = grain->winPos = 0.f;
			GET_GRAIN_WIN

			float azimuth = IN0(4);
			float elevation = IN0(5);
			float rho = IN0(6);

			CALC_BF_COEFS

			double counter = winSize * SAMPLERATE;
			double winInc = grain->winInc = (double)windowSamples / counter;
			int32 thisfreq = grain->freq = (int32)(unit->m_cpstoinc * freq);
			int32 oscphase = 0;
			counter = sc_max(4., counter);
			grain->counter = (int)counter;
			double amp = windowData[0];
			int nsmps = sc_min(grain->counter, inNumSamples);
			for (int j=0; j<nsmps; ++j) {
			    float outval = amp * lookupi1(table0, table1, oscphase, unit->m_lomask);
			    OUT_BF
			    oscphase += thisfreq;
			    BUF_GRAIN_AMP
			}
			grain->winPos = winPos;
			grain->oscphase = oscphase;
			grain->counter -= inNumSamples;
			grain->curamp = amp;
			if (grain->counter <= 0) {
				// remove grain
				*grain = unit->mGrains[--unit->mNumActive];
			}
		}
	    }
	unit->curtrig = trig;

}


void SinGrainBBF_Ctor(SinGrainBBF *unit)
{
	if (INRATE(0) == calc_FullRate)
	    SETCALC(SinGrainBBF_next_a);
	    else
	    SETCALC(SinGrainBBF_next_k);
	int tableSizeSin = ft->mSineSize;
	unit->m_lomask = (tableSizeSin - 1) << 3;
	unit->m_radtoinc = tableSizeSin * (rtwopi * 65536.);
	unit->m_cpstoinc = tableSizeSin * SAMPLEDUR * 65536.;
	unit->curtrig = 0.f;
	unit->mNumActive = 0;
	unit->m_wComp = IN0(7);
	SinGrainBBF_next_k(unit, 1);
}

///////////////////////// SinGrainIBF ////////////////////////////

void SinGrainIBF_next_a(SinGrainIBF *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);

	SETUP_BF_OUTS

	float *trig = IN(0);
	float winSize, freq;
	float *table0 = ft->mSineWavetable;
	float *table1 = table0 + 1;
	float wComp = unit->m_wComp;

	for (int i=0; i < unit->mNumActive; ) {
		SGrainIBF *grain = unit->mGrains + i;
		int32 thisfreq = grain->freq;
		int32 oscphase = grain->oscphase;
		GET_INTERP_GRAIN_WIN

		double winIncA = grain->winIncA;
		double winPosA = grain->winPosA;
		double winIncB = grain->winIncB;
		double winPosB = grain->winPosB;
		double amp = grain->curamp;

		GET_BF_AMPS

		int nsmps = sc_min(grain->counter, inNumSamples);
		for (int j=0; j<nsmps; ++j) {
		    float outval = amp * lookupi1(table0, table1, oscphase, unit->m_lomask);
		    OUT_BF
		    oscphase += thisfreq;
		    BUF_INTERP_GRAIN_AMP
		    }
		grain->winPosA = winPosA;
		grain->winPosB = winPosB;
		grain->curamp = amp;
		grain->oscphase = oscphase;
		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	}

	for (int i=0; i<inNumSamples; ++i) {
		if ((unit->curtrig <= 0) && (trig[i] > 0.0)) {
			// start a grain
			if (unit->mNumActive+1 >= kMaxSynthGrains) {Print("Too many grains!\n"); break;}
			SGrainIBF *grain = unit->mGrains + unit->mNumActive++;
			freq = IN_AT(unit, 2, i);
			winSize = IN_AT(unit, 1, i);
			grain->mWindowA = (int)IN_AT(unit, 3, i);
			grain->mWindowB = (int)IN_AT(unit, 4, i);
			float ifac = grain->ifac = IN_AT(unit, 5, i);
			double winPosA = grain->winPosA = 0.f;
			double winPosB = grain->winPosB = 0.f;
			GET_INTERP_GRAIN_WIN
			double counter = winSize * SAMPLERATE;
			double winIncA = grain->winIncA = (double)windowSamplesA / counter;
			double winIncB = grain->winIncB = (double)windowSamplesB / counter;
			int32 thisfreq = grain->freq = (int32)(unit->m_cpstoinc * freq);
			int32 oscphase = 0;
			counter = sc_max(4., counter);
			grain->counter = (int)counter;
			double amp = lininterp(ifac, windowDataA[0], windowDataB[0]);
			float azimuth = IN_AT(unit, 6, i);
			float elevation = IN_AT(unit, 7, i);
			float rho = IN_AT(unit, 8, i);

			CALC_BF_COEFS
			SETUP_INIT_BF_OUTS

			int nsmps = sc_min(grain->counter, inNumSamples - i);
			for (int j=0; j<nsmps; ++j) {
			    float outval = amp * lookupi1(table0, table1, oscphase, unit->m_lomask);
			    OUT_INIT_BF
			    oscphase += thisfreq;
			    BUF_INTERP_GRAIN_AMP
			}
			grain->curamp = amp;
			grain->winPosA = winPosA;
			grain->winPosB = winPosB;
			grain->oscphase = oscphase;
			grain->counter -= nsmps;
			if (grain->counter <= 0) {
				// remove grain
				*grain = unit->mGrains[--unit->mNumActive];
			}
		}
		unit->curtrig = trig[i];
	}
}

void SinGrainIBF_next_k(SinGrainIBF *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);
	SETUP_BF_OUTS
	float trig = IN0(0);
	float winSize, freq;
	float *table0 = ft->mSineWavetable;
	float *table1 = table0 + 1;
	float wComp = unit->m_wComp;

	for (int i=0; i < unit->mNumActive; ) {
		SGrainIBF *grain = unit->mGrains + i;
		int32 thisfreq = grain->freq;
		int32 oscphase = grain->oscphase;
		GET_INTERP_GRAIN_WIN
		GET_BF_AMPS

		double winIncA = grain->winIncA;
		double winPosA = grain->winPosA;
		double winIncB = grain->winIncB;
		double winPosB = grain->winPosB;
		double amp = grain->curamp;
		int nsmps = sc_min(grain->counter, inNumSamples);
		for (int j=0; j<nsmps; ++j) {
		    float outval = amp * lookupi1(table0, table1, oscphase, unit->m_lomask);
		    OUT_BF
		    oscphase += thisfreq;
		    BUF_INTERP_GRAIN_AMP
		    }
		grain->winPosA = winPosA;
		grain->winPosB = winPosB;
		grain->curamp = amp;
		grain->oscphase = oscphase;
		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	}

		if ((unit->curtrig <= 0) && (trig > 0.0)) {
			// start a grain
			if (unit->mNumActive+1 >= kMaxSynthGrains) {
			    Print("Too many grains!\n");
			    } else {
			SGrainIBF *grain = unit->mGrains + unit->mNumActive++;
			freq = IN0(2);
			winSize = IN0(1);
			grain->mWindowA = (int)IN0(3);
			grain->mWindowB = (int)IN0(4);
			float ifac = grain->ifac = IN0(5);
			double winPosA = grain->winPosA = 0.f;
			double winPosB = grain->winPosB = 0.f;
			GET_INTERP_GRAIN_WIN
			double counter = winSize * SAMPLERATE;
			double winIncA = grain->winIncA = (double)windowSamplesA / counter;
			double winIncB = grain->winIncB = (double)windowSamplesB / counter;
			int32 thisfreq = grain->freq = (int32)(unit->m_cpstoinc * freq);
			int32 oscphase = 0;
			counter = sc_max(4., counter);
			grain->counter = (int)counter;

			float azimuth = IN0(6);
			float elevation = IN0(7);
			float rho = IN0(8);

			CALC_BF_COEFS

			double amp = lininterp(ifac, windowDataA[0], windowDataB[0]);
			int nsmps = sc_min(grain->counter, inNumSamples);
			for (int j=0; j<nsmps; ++j) {
			    float outval = amp * lookupi1(table0, table1, oscphase, unit->m_lomask);
			    OUT_BF
			    oscphase += thisfreq;
			    BUF_INTERP_GRAIN_AMP
			}
			grain->curamp = amp;
			grain->winPosA = winPosA;
			grain->winPosB = winPosB;
			grain->oscphase = oscphase;
			grain->counter -= inNumSamples;
			if (grain->counter <= 0) {
				// remove grain
				*grain = unit->mGrains[--unit->mNumActive];
			}
		}
	    }
	    unit->curtrig = trig;
}


void SinGrainIBF_Ctor(SinGrainIBF *unit)
{
	if (INRATE(0) == calc_FullRate)
	    SETCALC(SinGrainIBF_next_a);
	    else
	    SETCALC(SinGrainIBF_next_k);
	int tableSizeSin = ft->mSineSize;
	unit->m_lomask = (tableSizeSin - 1) << 3;
	unit->m_radtoinc = tableSizeSin * (rtwopi * 65536.);
	unit->m_cpstoinc = tableSizeSin * SAMPLEDUR * 65536.;
	unit->curtrig = 0.f;
	unit->mNumActive = 0;
	unit->m_wComp = IN0(9);
	SinGrainIBF_next_k(unit, 1);
}

//////////////////////// FM grains /////////////////////////////

void FMGrainBF_next_a(FMGrainBF *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);

	SETUP_BF_OUTS

	float *trig = IN(0);
	float winSize, carfreq, modfreq, index;

	float *table0 = ft->mSineWavetable;
	float *table1 = table0 + 1;
	float wComp = unit->m_wComp;

	for (int i=0; i < unit->mNumActive; ) {
		FGrainBF *grain = unit->mGrains + i;
		double b1 = grain->b1;
		double y1 = grain->y1;
		double y2 = grain->y2;

		GET_BF_AMPS

		int32 mfreq = grain->mfreq;
		int32 moscphase = grain->moscphase;
		int32 coscphase = grain->coscphase;
		float deviation = grain->deviation;
		float carbase = grain->carbase;

		int nsmps = sc_min(grain->counter, inNumSamples);
		for (int j=0; j<nsmps; ++j) {
			float amp = y1 * y1;
			float thismod = lookupi1(table0, table1, moscphase, unit->m_lomask) * deviation;
			float outval = amp * lookupi1(table0, table1, coscphase, unit->m_lomask);

			OUT_BF

			double y0 = b1 * y1 - y2;
			y2 = y1;
			y1 = y0;
			int32 cfreq = (int32)(unit->m_cpstoinc * (carbase + thismod)); // needs to be calced in the loop!
			coscphase += cfreq;
			moscphase += mfreq;
		    } // need to save float carbase, int32 mfreq, float deviation
		grain->coscphase = coscphase;
		grain->moscphase = moscphase;
		grain->y1 = y1;
		grain->y2 = y2;
		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	}

	for (int i=0; i<inNumSamples; ++i) {
	    if ((unit->curtrig <= 0) && (trig[i] > 0.0)) {
		// start a grain
		if (unit->mNumActive+1 >= kMaxSynthGrains) {Print("Too many grains!\n"); break;}
		FGrainBF *grain = unit->mGrains + unit->mNumActive++;
		winSize = IN_AT(unit, 1, i);
		carfreq = IN_AT(unit, 2, i);
		modfreq = IN_AT(unit, 3, i);
		index = IN_AT(unit, 4, i);
		float deviation = grain->deviation = index * modfreq;
		int32 mfreq = grain->mfreq = (int32)(unit->m_cpstoinc * modfreq);
		grain->carbase = carfreq;
		int32 coscphase = 0;
		int32 moscphase = 0;
		double counter = winSize * SAMPLERATE;
		counter = sc_max(4., counter);
		grain->counter = (int)counter;
		double w = pi / counter;
		double b1 = grain->b1 = 2. * cos(w);
		double y1 = sin(w);
		double y2 = 0.;

		float azimuth = IN_AT(unit, 5, i);
		float elevation = IN_AT(unit, 6, i);
		float rho = IN_AT(unit, 7, i);

		CALC_BF_COEFS
		SETUP_INIT_BF_OUTS

		int nsmps = sc_min(grain->counter, inNumSamples - i);
		for (int j=0; j<nsmps; ++j) {
		    float amp = y1 * y1;
		    float thismod = lookupi1(table0, table1, moscphase, unit->m_lomask) * deviation;
		    float outval = amp * lookupi1(table0, table1, coscphase, unit->m_lomask);

		    OUT_INIT_BF

		    double y0 = b1 * y1 - y2;
		    y2 = y1;
		    y1 = y0;
		    int32 cfreq = (int32)(unit->m_cpstoinc * (carfreq + thismod)); // needs to be calced in the loop!
		    coscphase += cfreq;
		    moscphase += mfreq;
		} // need to save float carbase, int32 mfreq, float deviation
		grain->coscphase = coscphase;
		grain->moscphase = moscphase;
		grain->y1 = y1;
		grain->y2 = y2;
		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		}
		}
		unit->curtrig = trig[i];
	}
}

void FMGrainBF_next_k(FMGrainBF *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);

	SETUP_BF_OUTS

	float trig = IN0(0);
	float winSize, carfreq, modfreq, index;

	float *table0 = ft->mSineWavetable;
	float *table1 = table0 + 1;
	float wComp = unit->m_wComp;

	for (int i=0; i < unit->mNumActive; ) {
		FGrainBF *grain = unit->mGrains + i;
		double b1 = grain->b1;
		double y1 = grain->y1;
		double y2 = grain->y2;

		GET_BF_AMPS

		int32 mfreq = grain->mfreq;
		int32 moscphase = grain->moscphase;
		int32 coscphase = grain->coscphase;
		float deviation = grain->deviation;
		float carbase = grain->carbase;

		int nsmps = sc_min(grain->counter, inNumSamples);
		for (int j=0; j<nsmps; ++j) {
			float amp = y1 * y1;
			float thismod = lookupi1(table0, table1, moscphase, unit->m_lomask) * deviation;
			float outval = amp * lookupi1(table0, table1, coscphase, unit->m_lomask);

			OUT_BF

			double y0 = b1 * y1 - y2;
			y2 = y1;
			y1 = y0;
			int32 cfreq = (int32)(unit->m_cpstoinc * (carbase + thismod)); // needs to be calced in the loop!
			coscphase += cfreq;
			moscphase += mfreq;
		    } // need to save float carbase, int32 mfreq, float deviation
		grain->coscphase = coscphase;
		grain->moscphase = moscphase;
		grain->y1 = y1;
		grain->y2 = y2;
		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	    }

	    if ((unit->curtrig <= 0) && (trig > 0.0)) {
		// start a grain
		if (unit->mNumActive+1 >= kMaxSynthGrains) {Print("Too many grains!\n");
		} else {
		FGrainBF *grain = unit->mGrains + unit->mNumActive++;
		winSize = IN0(1);
		carfreq = IN0(2);
		modfreq = IN0(3);
		index = IN0(4);
		float deviation = grain->deviation = index * modfreq;
		int32 mfreq = grain->mfreq = (int32)(unit->m_cpstoinc * modfreq);
		grain->carbase = carfreq;
		int32 coscphase = 0;
		int32 moscphase = 0;
		double counter = winSize * SAMPLERATE;
		counter = sc_max(4., counter);
		grain->counter = (int)counter;
		double w = pi / counter;
		double b1 = grain->b1 = 2. * cos(w);
		double y1 = sin(w);
		double y2 = 0.;

		float azimuth = IN0(5);
		float elevation = IN0(6);
		float rho = IN0(7);

		CALC_BF_COEFS

		int nsmps = sc_min(grain->counter, inNumSamples);
		for (int j=0; j<nsmps; ++j) {
		    float amp = y1 * y1;
		    float thismod = lookupi1(table0, table1, moscphase, unit->m_lomask) * deviation;
		    float outval = amp * lookupi1(table0, table1, coscphase, unit->m_lomask);

		    OUT_BF

		    double y0 = b1 * y1 - y2;
		    y2 = y1;
		    y1 = y0;
		    int32 cfreq = (int32)(unit->m_cpstoinc * (carfreq + thismod)); // needs to be calced in the loop!
		    coscphase += cfreq;
		    moscphase += mfreq;
		} // need to save float carbase, int32 mfreq, float deviation
		grain->coscphase = coscphase;
		grain->moscphase = moscphase;
		grain->y1 = y1;
		grain->y2 = y2;
		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		}
	    }
	}
	unit->curtrig = trig;
}

void FMGrainBF_Ctor(FMGrainBF *unit)
{
	if (INRATE(0) == calc_FullRate)
	    SETCALC(FMGrainBF_next_a);
	    else
	    SETCALC(FMGrainBF_next_k);
	int tableSizeSin = ft->mSineSize;
	unit->m_lomask = (tableSizeSin - 1) << 3;
	unit->m_radtoinc = tableSizeSin * (rtwopi * 65536.);
	unit->m_cpstoinc = tableSizeSin * SAMPLEDUR * 65536.;
	unit->curtrig = 0.f;
	unit->mNumActive = 0;
	unit->m_wComp = IN0(8);
	FMGrainBF_next_k(unit, 1);
}

///////////////////////  FMGrainB //////////////////////////////////////////

void FMGrainBBF_next_a(FMGrainBBF *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);

	SETUP_BF_OUTS

	float *trig = IN(0);
	float winSize, carfreq, modfreq, index;

	float *table0 = ft->mSineWavetable;
	float *table1 = table0 + 1;
	float wComp = unit->m_wComp;

	for (int i=0; i < unit->mNumActive; ) {
		FGrainBBF *grain = unit->mGrains + i;
		int32 mfreq = grain->mfreq;
		int32 moscphase = grain->moscphase;
		int32 coscphase = grain->coscphase;
		float deviation = grain->deviation;
		float carbase = grain->carbase;
		GET_GRAIN_WIN
		GET_BF_AMPS

		double amp = grain->curamp;
		double winInc = grain->winInc;
		double winPos = grain->winPos;
		int nsmps = sc_min(grain->counter, inNumSamples);
		for (int j=0; j<nsmps; ++j) {
			float thismod = lookupi1(table0, table1, moscphase, unit->m_lomask) * deviation;
			float outval = amp * lookupi1(table0, table1, coscphase, unit->m_lomask);
			OUT_BF
			BUF_GRAIN_AMP
			int32 cfreq = (int32)(unit->m_cpstoinc * (carbase + thismod)); // needs to be calced in the loop!
			coscphase += cfreq;
			moscphase += mfreq;
		    } // need to save float carbase, int32 mfreq, float deviation
		grain->coscphase = coscphase;
		grain->moscphase = moscphase;
		grain->curamp = amp;
		grain->winPos = winPos;
		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	}

	for (int i=0; i<inNumSamples; ++i) {
	    if ((unit->curtrig <= 0) && (trig[i] > 0.0)) {
		// start a grain
		if (unit->mNumActive+1 >= kMaxSynthGrains) {Print("Too many grains!\n"); break;}
		FGrainBBF *grain = unit->mGrains + unit->mNumActive++;
		winSize = IN_AT(unit, 1, i);
		carfreq = IN_AT(unit, 2, i);
		modfreq = IN_AT(unit, 3, i);
		index = IN_AT(unit, 4, i);
		grain->mWindow = (int)IN_AT(unit, 5, i);
		double winPos = grain->winPos = 0.f;
		GET_GRAIN_WIN

		float azimuth = IN_AT(unit, 6, i);
		float elevation = IN_AT(unit, 7, i);
		float rho = IN_AT(unit, 8, i);

		CALC_BF_COEFS
		SETUP_INIT_BF_OUTS

		double counter = winSize * SAMPLERATE;
		double winInc = grain->winInc = (double)windowSamples / counter;
		double amp = windowData[0];
		float deviation = grain->deviation = index * modfreq;
		int32 mfreq = grain->mfreq = (int32)(unit->m_cpstoinc * modfreq);
		grain->carbase = carfreq;
		int32 coscphase = 0;
		int32 moscphase = 0;
		counter = sc_max(4., counter);
		grain->counter = (int)counter;

		int nsmps = sc_min(grain->counter, inNumSamples - i);
		for (int j=0; j<nsmps; ++j) {
		    float thismod = lookupi1(table0, table1, moscphase, unit->m_lomask) * deviation;
		    float outval = amp * lookupi1(table0, table1, coscphase, unit->m_lomask);
		    OUT_INIT_BF
		    BUF_GRAIN_AMP
		    int32 cfreq = (int32)(unit->m_cpstoinc * (carfreq + thismod)); // needs to be calced in the loop!
		    coscphase += cfreq;
		    moscphase += mfreq;
		} // need to save float carbase, int32 mfreq, float deviation
		grain->coscphase = coscphase;
		grain->moscphase = moscphase;
		grain->curamp = amp;
		grain->winPos = winPos;
		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		}
		}
		unit->curtrig = trig[i];
	}
}

void FMGrainBBF_next_k(FMGrainBBF *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);

	SETUP_BF_OUTS

	float trig = IN0(0);
	float winSize, carfreq, modfreq, index;

	float *table0 = ft->mSineWavetable;
	float *table1 = table0 + 1;
	float wComp = unit->m_wComp;

	for (int i=0; i < unit->mNumActive; ) {
		FGrainBBF *grain = unit->mGrains + i;
		GET_GRAIN_WIN
		GET_BF_AMPS

		double amp = grain->curamp;
		double winInc = grain->winInc;
		double winPos = grain->winPos;
		int32 mfreq = grain->mfreq;
		int32 moscphase = grain->moscphase;
		int32 coscphase = grain->coscphase;
		float deviation = grain->deviation;
		float carbase = grain->carbase;

		int nsmps = sc_min(grain->counter, inNumSamples);
		for (int j=0; j<nsmps; ++j) {
			float thismod = lookupi1(table0, table1, moscphase, unit->m_lomask) * deviation;
			float outval = amp * lookupi1(table0, table1, coscphase, unit->m_lomask);
			OUT_BF
			BUF_GRAIN_AMP
			int32 cfreq = (int32)(unit->m_cpstoinc * (carbase + thismod)); // needs to be calced in the loop!
			coscphase += cfreq;
			moscphase += mfreq;
		    } // need to save float carbase, int32 mfreq, float deviation
		grain->coscphase = coscphase;
		grain->moscphase = moscphase;
		grain->curamp = amp;
		grain->winPos = winPos;
		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	    }

	    if ((unit->curtrig <= 0) && (trig > 0.0)) {
		// start a grain
		if (unit->mNumActive+1 >= kMaxSynthGrains) {Print("Too many grains!\n");
		} else {
		FGrainBBF *grain = unit->mGrains + unit->mNumActive++;
		winSize = IN0(1);
		carfreq = IN0(2);
		modfreq = IN0(3);
		index = IN0(4);
		grain->mWindow = (int)IN0(5);
		double winPos = grain->winPos = 0.f;
		GET_GRAIN_WIN

		float azimuth = IN0(6);
		float elevation = IN0(7);
		float rho = IN0(8);

		CALC_BF_COEFS

		double counter = winSize * SAMPLERATE;
		double winInc = grain->winInc = (double)windowSamples / counter;
		float amp = windowData[0];

		float deviation = grain->deviation = index * modfreq;
		int32 mfreq = grain->mfreq = (int32)(unit->m_cpstoinc * modfreq);
		grain->carbase = carfreq;
		int32 coscphase = 0;
		int32 moscphase = 0;
		counter = sc_max(4., counter);
		grain->counter = (int)counter;
		int nsmps = sc_min(grain->counter, inNumSamples);
		for (int j=0; j<nsmps; ++j) {
		    float thismod = lookupi1(table0, table1, moscphase, unit->m_lomask) * deviation;
		    float outval = amp * lookupi1(table0, table1, coscphase, unit->m_lomask);
		    OUT_BF

		    BUF_GRAIN_AMP
		    int32 cfreq = (int32)(unit->m_cpstoinc * (carfreq + thismod)); // needs to be calced in the loop!
		    coscphase += cfreq;
		    moscphase += mfreq;
		} // need to save float carbase, int32 mfreq, float deviation
		grain->coscphase = coscphase;
		grain->moscphase = moscphase;
		grain->curamp = amp;
		grain->winPos = winPos;
		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		}
	    }
	}
	unit->curtrig = trig;
}

void FMGrainBBF_Ctor(FMGrainBBF *unit)
{
	if (INRATE(0) == calc_FullRate)
	    SETCALC(FMGrainBBF_next_a);
	    else
	    SETCALC(FMGrainBBF_next_k);
	int tableSizeSin = ft->mSineSize;
	unit->m_lomask = (tableSizeSin - 1) << 3;
	unit->m_radtoinc = tableSizeSin * (rtwopi * 65536.);
	unit->m_cpstoinc = tableSizeSin * SAMPLEDUR * 65536.;
	unit->curtrig = 0.f;
	unit->mNumActive = 0;
	unit->m_wComp = IN0(9);
	FMGrainBBF_next_k(unit, 1);
}


///////////////////////  FMGrainI //////////////////////////////////////////

void FMGrainIBF_next_a(FMGrainIBF *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);

	SETUP_BF_OUTS

	float *trig = IN(0);
	float winSize, carfreq, modfreq, index;

	float *table0 = ft->mSineWavetable;
	float *table1 = table0 + 1;
	float wComp = unit->m_wComp;

	for (int i=0; i < unit->mNumActive; ) {
		FGrainIBF *grain = unit->mGrains + i;
		int32 mfreq = grain->mfreq;
		int32 moscphase = grain->moscphase;
		int32 coscphase = grain->coscphase;
		float deviation = grain->deviation;
		float carbase = grain->carbase;
		GET_INTERP_GRAIN_WIN
		double amp = grain->curamp;

		GET_BF_AMPS

		double winIncA = grain->winIncA;
		double winPosA = grain->winPosA;
		double winIncB = grain->winIncB;
		double winPosB = grain->winPosB;
		int nsmps = sc_min(grain->counter, inNumSamples);
		for (int j=0; j<nsmps; ++j) {
			float thismod = lookupi1(table0, table1, moscphase, unit->m_lomask) * deviation;
			float outval = amp * lookupi1(table0, table1, coscphase, unit->m_lomask);
			OUT_BF
			BUF_INTERP_GRAIN_AMP
			int32 cfreq = (int32)(unit->m_cpstoinc * (carbase + thismod)); // needs to be calced in the loop!
			coscphase += cfreq;
			moscphase += mfreq;
		    } // need to save float carbase, int32 mfreq, float deviation
		grain->coscphase = coscphase;
		grain->moscphase = moscphase;
		grain->curamp = amp;
		grain->winPosA = winPosA;
		grain->winPosB = winPosB;
		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	}

	for (int i=0; i<inNumSamples; ++i) {
	    if ((unit->curtrig <= 0) && (trig[i] > 0.0)) {
		// start a grain
		if (unit->mNumActive+1 >= kMaxSynthGrains) {Print("Too many grains!\n"); break;}
		FGrainIBF *grain = unit->mGrains + unit->mNumActive++;
		winSize = IN_AT(unit, 1, i);
		carfreq = IN_AT(unit, 2, i);
		modfreq = IN_AT(unit, 3, i);
		index = IN_AT(unit, 4, i);
		grain->mWindowA = (int)IN_AT(unit, 5, i);
		grain->mWindowB = (int)IN_AT(unit, 6, i);
		float ifac = grain->ifac = IN_AT(unit, 7, i);
		double winPosA = grain->winPosA = 0.f;
		double winPosB = grain->winPosB = 0.f;
		GET_INTERP_GRAIN_WIN
		double counter = winSize * SAMPLERATE;
		double winIncA = grain->winIncA = (double)windowSamplesA / counter;
		double winIncB = grain->winIncB = (double)windowSamplesB / counter;
		double amp = lininterp(ifac, windowDataA[0], windowDataB[0]);

		float azimuth = IN_AT(unit, 8, i);
		float elevation = IN_AT(unit, 9, i);
		float rho = IN_AT(unit, 10, i);

		CALC_BF_COEFS
		SETUP_INIT_BF_OUTS


		float deviation = grain->deviation = index * modfreq;
		int32 mfreq = grain->mfreq = (int32)(unit->m_cpstoinc * modfreq);
		grain->carbase = carfreq;
		int32 coscphase = 0;
		int32 moscphase = 0;
		counter = sc_max(4., counter);
		grain->counter = (int)counter;

		int nsmps = sc_min(grain->counter, inNumSamples - i);
		for (int j=0; j<nsmps; ++j) {
		    float thismod = lookupi1(table0, table1, moscphase, unit->m_lomask) * deviation;
		    float outval = amp * lookupi1(table0, table1, coscphase, unit->m_lomask);
		    OUT_INIT_BF
		    BUF_INTERP_GRAIN_AMP
		    int32 cfreq = (int32)(unit->m_cpstoinc * (carfreq + thismod)); // needs to be calced in the loop!
		    coscphase += cfreq;
		    moscphase += mfreq;
		} // need to save float carbase, int32 mfreq, float deviation
		grain->coscphase = coscphase;
		grain->moscphase = moscphase;
		grain->curamp = amp;
		grain->winPosA = winPosA;
		grain->winPosB = winPosB;
		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		}
		}
		unit->curtrig = trig[i];
	}
}

void FMGrainIBF_next_k(FMGrainIBF *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);
	SETUP_BF_OUTS
	float trig = IN0(0);
	float winSize, carfreq, modfreq, index;

	float *table0 = ft->mSineWavetable;
	float *table1 = table0 + 1;
	float wComp = unit->m_wComp;

	for (int i=0; i < unit->mNumActive; ) {
		FGrainIBF *grain = unit->mGrains + i;
		int32 mfreq = grain->mfreq;
		int32 moscphase = grain->moscphase;
		int32 coscphase = grain->coscphase;
		float deviation = grain->deviation;
		float carbase = grain->carbase;
		GET_INTERP_GRAIN_WIN
		GET_BF_AMPS

		double amp = grain->curamp;
		double winIncA = grain->winIncA;
		double winPosA = grain->winPosA;
		double winIncB = grain->winIncB;
		double winPosB = grain->winPosB;
		int nsmps = sc_min(grain->counter, inNumSamples);
		for (int j=0; j<nsmps; ++j) {
			float thismod = lookupi1(table0, table1, moscphase, unit->m_lomask) * deviation;
			float outval = amp * lookupi1(table0, table1, coscphase, unit->m_lomask);
			OUT_BF
			BUF_INTERP_GRAIN_AMP
			int32 cfreq = (int32)(unit->m_cpstoinc * (carbase + thismod)); // needs to be calced in the loop!
			coscphase += cfreq;
			moscphase += mfreq;
		    } // need to save float carbase, int32 mfreq, float deviation
		grain->coscphase = coscphase;
		grain->moscphase = moscphase;
		grain->curamp = amp;
		grain->winPosA = winPosA;
		grain->winPosB = winPosB;
		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	}

	    if ((unit->curtrig <= 0) && (trig > 0.0)) {
		// start a grain
		if (unit->mNumActive+1 >= kMaxSynthGrains) {
		    Print("Too many grains!\n");
		    } else {
		FGrainIBF *grain = unit->mGrains + unit->mNumActive++;
		winSize = IN0(1);
		carfreq = IN0(2);
		modfreq = IN0(3);
		index = IN0(4);
		grain->mWindowA = (int)IN0(5);
		grain->mWindowB = (int)IN0(6);
		float ifac = grain->ifac = IN0(7);
		double winPosA = grain->winPosA = 0.f;
		double winPosB = grain->winPosB = 0.f;
		GET_INTERP_GRAIN_WIN
		double counter = winSize * SAMPLERATE;
		double winIncA = grain->winIncA = (double)windowSamplesA / counter;
		double winIncB = grain->winIncB = (double)windowSamplesB / counter;
		double amp = lininterp(ifac, windowDataA[0], windowDataB[0]);
		float deviation = grain->deviation = index * modfreq;
		int32 mfreq = grain->mfreq = (int32)(unit->m_cpstoinc * modfreq);
		grain->carbase = carfreq;
		int32 coscphase = 0;
		int32 moscphase = 0;
		counter = sc_max(4., counter);
		grain->counter = (int)counter;
		float azimuth = IN0(8);
		float elevation = IN0(9);
		float rho = IN0(10);

		CALC_BF_COEFS

		int nsmps = sc_min(grain->counter, inNumSamples);
		for (int j=0; j<nsmps; ++j) {
		    float thismod = lookupi1(table0, table1, moscphase, unit->m_lomask) * deviation;
		    float outval = amp * lookupi1(table0, table1, coscphase, unit->m_lomask);
		    OUT_BF
		    BUF_INTERP_GRAIN_AMP
		    int32 cfreq = (int32)(unit->m_cpstoinc * (carfreq + thismod)); // needs to be calced in the loop!
		    coscphase += cfreq;
		    moscphase += mfreq;
		} // need to save float carbase, int32 mfreq, float deviation
		grain->coscphase = coscphase;
		grain->moscphase = moscphase;
		grain->curamp = amp;
		grain->winPosA = winPosA;
		grain->winPosB = winPosB;
		grain->counter -= inNumSamples;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		    }
		}
	    }
	    unit->curtrig = trig;
}

void FMGrainIBF_Ctor(FMGrainIBF *unit)
{
	if (INRATE(0) == calc_FullRate)
	    SETCALC(FMGrainIBF_next_a);
	    else
	    SETCALC(FMGrainIBF_next_k);
	int tableSizeSin = ft->mSineSize;
	unit->m_lomask = (tableSizeSin - 1) << 3;
	unit->m_radtoinc = tableSizeSin * (rtwopi * 65536.);
	unit->m_cpstoinc = tableSizeSin * SAMPLEDUR * 65536.;
	unit->curtrig = 0.f;
	unit->mNumActive = 0;
	unit->m_wComp = IN0(11);
	FMGrainIBF_next_k(unit, 1);
}

///////////////////// BufGrain (basically Warp1 with a trigger... which is basically TGrains in Mono???) //////////////

void BufGrainBF_next_a(BufGrainBF *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);
	float *Wout = ZOUT(0);
	float *Xout = ZOUT(1);
	float *Yout = ZOUT(2);
	float *Zout = ZOUT(3);

	float *trig = IN(0);

	World *world = unit->mWorld;
//	SndBuf *bufs = world->mSndBufs;
//	uint32 numBufs = world->mNumSndBufs;
	float wComp = unit->m_wComp;

	for (int i=0; i < unit->mNumActive; ) {
		GrainBF *grain = unit->mGrains + i;
//		uint32 bufnum = grain->bufnum;

		GRAIN_BUF
		CHECK_BUF

		GET_BF_AMPS

		if (bufChannels != 1) {
			 ++i;
			 continue;
		}
		double loopMax = (double)bufFrames;
		double rate = grain->rate;
		double phase = grain->phase;
		double b1 = grain->b1;
		double y1 = grain->y1;
		double y2 = grain->y2;

		float *Wout1 = Wout;
		float *Xout1 = Xout;
		float *Yout1 = Yout;
		float *Zout1 = Zout;

		int nsmps = sc_min(grain->counter, inNumSamples);
		if (grain->interp >= 4) {
			for (int j=0; j<nsmps; ++j) {
				GRAIN_LOOP_BF_BODY_4;
				phase += rate;
			}
		} else if (grain->interp >= 2) {
			for (int j=0; j<nsmps; ++j) {
				GRAIN_LOOP_BF_BODY_2;
				phase += rate;
			}
		} else {
			for (int j=0; j<nsmps; ++j) {
				GRAIN_LOOP_BF_BODY_1;
				phase += rate;
			}
		}

		grain->phase = phase;
		grain->y1 = y1;
		grain->y2 = y2;

		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	}

	for (int i=0; i<inNumSamples; ++i) {
		if ((trig[i] > 0) && (unit->curtrig <=0)) {
			// start a grain
			if (unit->mNumActive+1 >= kMaxSynthGrains) {Print("Too many grains!\n");
			} else {
			uint32 bufnum = (uint32)IN_AT(unit, 2, i);
//			if (bufnum >= numBufs) continue;

			GrainBF *grain = unit->mGrains + unit->mNumActive++;

			SETUP_GRAIN_BUF
			GRAIN_BUF
			CHECK_BUF


			if (bufChannels != 1) continue;

			float bufSampleRate = buf->samplerate;
			float bufRateScale = bufSampleRate * SAMPLEDUR;
			double loopMax = (double)bufFrames;

//			GrainBF *grain = unit->mGrains + unit->mNumActive++;
//			grain->bufnum = bufnum;

			double counter = IN_AT(unit, 1, i) * SAMPLERATE;
			counter = sc_max(4., counter);
			grain->counter = (int)counter;

			double rate = grain->rate = IN_AT(unit, 3, i) * bufRateScale;
			double phase = IN_AT(unit, 4, i) * bufFrames;
			grain->interp = (int)IN_AT(unit, 5, i);

			double w = pi / counter;
			double b1 = grain->b1 = 2. * cos(w);
			double y1 = sin(w);
			double y2 = 0.;

			float azimuth = IN_AT(unit, 6, i);
			float elevation = IN_AT(unit, 7, i);
			float rho = IN_AT(unit, 8, i);

			CALC_BF_COEFS

			float *Wout1 = Wout + i;
			float *Xout1 = Xout + i;
			float *Yout1 = Yout + i;
			float *Zout1 = Zout + i;

			int nsmps = sc_min(grain->counter, inNumSamples - i);
			if (grain->interp >= 4) {
				for (int j=0; j<nsmps; ++j) {
					GRAIN_LOOP_BF_BODY_4;
					phase += rate;
				}
			} else if (grain->interp >= 2) {
				for (int j=0; j<nsmps; ++j) {
					GRAIN_LOOP_BF_BODY_2;
					phase += rate;
				}
			} else {
				for (int j=0; j<nsmps; ++j) {
					GRAIN_LOOP_BF_BODY_1;
					phase += rate;
				}
			}

			grain->phase = phase;
			grain->y1 = y1;
			grain->y2 = y2;
			// end change
			grain->counter -= nsmps;
			if (grain->counter <= 0) {
				// remove grain
				*grain = unit->mGrains[--unit->mNumActive];
			}
		    }
		}
		unit->curtrig = trig[i];
	    }
	}

void BufGrainBF_next_k(BufGrainBF *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);
	float *Wout = ZOUT(0);
	float *Xout = ZOUT(1);
	float *Yout = ZOUT(2);
	float *Zout = ZOUT(3);

	float trig = ZIN0(0);
	float wComp = unit->m_wComp;

	World *world = unit->mWorld;
//	SndBuf *bufs = world->mSndBufs;
	//uint32 numBufs = world->mNumSndBufs;

	for (int i=0; i < unit->mNumActive; ) {
		GrainBF *grain = unit->mGrains + i;
//		uint32 bufnum = grain->bufnum;

		GRAIN_BUF
		CHECK_BUF

		if (bufChannels != 1) {
			 ++i;
			 continue;
		}

		double loopMax = (double)bufFrames;

		double rate = grain->rate;
		double phase = grain->phase;
		double b1 = grain->b1;
		double y1 = grain->y1;
		double y2 = grain->y2;

		float *Wout1 = Wout;
		float *Xout1 = Xout;
		float *Yout1 = Yout;
		float *Zout1 = Zout;

		GET_BF_AMPS

		int nsmps = sc_min(grain->counter, inNumSamples);
		if (grain->interp >= 4) {
			for (int j=0; j<nsmps; ++j) {
				GRAIN_LOOP_BF_BODY_4;
				phase += rate;
			}
		} else if (grain->interp >= 2) {
			for (int j=0; j<nsmps; ++j) {
				GRAIN_LOOP_BF_BODY_2;
				phase += rate;
			}
		} else {
			for (int j=0; j<nsmps; ++j) {
				GRAIN_LOOP_BF_BODY_1;
				phase += rate;
			}
		}

		grain->phase = phase;
		grain->y1 = y1;
		grain->y2 = y2;

		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	    }

		if ((trig > 0.) && (unit->curtrig <= 0.)) {
			// start a grain
			if (unit->mNumActive+1 >= kMaxSynthGrains) {Print("Too many grains!\n");
			} else {
			uint32 bufnum = (uint32)ZIN0(2);

			GrainBF *grain = unit->mGrains + unit->mNumActive++;

			SETUP_GRAIN_BUF
			GRAIN_BUF
			CHECK_BUF

			float bufSampleRate = buf->samplerate;
			float bufRateScale = bufSampleRate * SAMPLEDUR;
			double loopMax = (double)bufFrames;

//			GrainBF *grain = unit->mGrains + unit->mNumActive++;
//			grain->bufnum = bufnum;

			double counter = ZIN0(1) * SAMPLERATE;
			counter = sc_max(4., counter);
			grain->counter = (int)counter;

			double rate = grain->rate = ZIN0(3) * bufRateScale;
			double phase = ZIN0(4) * bufFrames;
			grain->interp = (int)ZIN0(5);

			double w = pi / counter;
			double b1 = grain->b1 = 2. * cos(w);
			double y1 = sin(w);
			double y2 = 0.;

			float azimuth = ZIN0(6);
			float elevation = ZIN0(7);
			float rho = ZIN0(8);

			CALC_BF_COEFS

			float *Wout1 = Wout;
			float *Xout1 = Xout;
			float *Yout1 = Yout;
			float *Zout1 = Zout;

			int nsmps = sc_min(grain->counter, inNumSamples);
			if (grain->interp >= 4) {
				for (int j=0; j<nsmps; ++j) {
					GRAIN_LOOP_BF_BODY_4;
					phase += rate;
				}
			} else if (grain->interp >= 2) {
				for (int j=0; j<nsmps; ++j) {
					GRAIN_LOOP_BF_BODY_2;
					phase += rate;
				}
			} else {
				for (int j=0; j<nsmps; ++j) {
					GRAIN_LOOP_BF_BODY_1;
					phase += rate;
				}
			}

			grain->phase = phase;
			grain->y1 = y1;
			grain->y2 = y2;
			grain->counter -= nsmps;
			if (grain->counter <= 0) {
				// remove grain
				*grain = unit->mGrains[--unit->mNumActive];
			}
		}
	    }
	    unit->curtrig = trig;

}

void BufGrainBF_Ctor(BufGrainBF *unit)
{
	if (INRATE(0) == calc_FullRate)
	    SETCALC(BufGrainBF_next_a);
	    else
	    SETCALC(BufGrainBF_next_k);
	unit->mNumActive = 0;
	unit->curtrig = 0.f;
	unit->m_wComp = IN0(9);
	BufGrainBF_next_k(unit, 1); // should be _k
}

////////////////////// BufGrainsB ///////////////////////////////

void BufGrainBBF_next_a(BufGrainBBF *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);
	float *Wout;
	float *Xout;
	float *Yout;
	float *Zout;

	Wout = ZOUT(0);
	Xout = ZOUT(1);
	Yout = ZOUT(2);
	Zout = ZOUT(3);

	float *trig = IN(0);

	World *world = unit->mWorld;
//	SndBuf *bufs = world->mSndBufs;
//	uint32 numBufs = world->mNumSndBufs;
	float wComp = unit->m_wComp;

	for (int i=0; i < unit->mNumActive; ) {
		WinGrainBF *grain = unit->mGrains + i;
//		uint32 bufnum = grain->bufnum;

		GRAIN_BUF
		CHECK_BUF

		if (bufChannels != 1) {
			 ++i;
			 continue;
		}
		GET_GRAIN_WIN
		double amp = grain->curamp;
		double winInc = grain->winInc;
		double winPos = grain->winPos;

		double loopMax = (double)bufFrames;

		double rate = grain->rate;
		double phase = grain->phase;

		GET_BF_AMPS

		float *Wout1 = Wout;
		float *Xout1 = Xout;
		float *Yout1 = Yout;
		float *Zout1 = Zout;

		int nsmps = sc_min(grain->counter, inNumSamples);
		if (grain->interp >= 4) {
			for (int j=0; j<nsmps; ++j) {
				BUF_GRAIN_LOOP_BF_BODY_4;
				phase += rate;
				BUF_GRAIN_AMP

			}
		} else if (grain->interp >= 2) {
			for (int j=0; j<nsmps; ++j) {
				BUF_GRAIN_LOOP_BF_BODY_2;
				phase += rate;
				BUF_GRAIN_AMP
			}
		} else {
			for (int j=0; j<nsmps; ++j) {
				BUF_GRAIN_LOOP_BF_BODY_1;
				phase += rate;
				BUF_GRAIN_AMP
			}
		}

		grain->phase = phase;
		grain->curamp = amp;
		grain->winPos = winPos;

		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	}

	for (int i=0; i<inNumSamples; ++i) {
		if ((trig[i] > 0) && (unit->curtrig <=0)) {
			// start a grain
			if (unit->mNumActive+1 >= kMaxSynthGrains) {Print("Too many grains!\n");
			} else {
			uint32 bufnum = (uint32)IN_AT(unit, 2, i);
//			if (bufnum >= numBufs) continue;
			float winSize = IN_AT(unit, 1, i);
			WinGrainBF *grain = unit->mGrains + unit->mNumActive++;

			SETUP_GRAIN_BUF
			GRAIN_BUF
			CHECK_BUF

			if (bufChannels != 1) continue;

			float bufSampleRate = buf->samplerate;
			float bufRateScale = bufSampleRate * SAMPLEDUR;
			double loopMax = (double)bufFrames;

//			WinGrainBF *grain = unit->mGrains + unit->mNumActive++;
//			grain->bufnum = bufnum;

			double rate = grain->rate = IN_AT(unit, 3, i) * bufRateScale;
			double phase = IN_AT(unit, 4, i) * bufFrames;
			grain->interp = (int)IN_AT(unit, 5, i);

			grain->mWindow = (int)IN_AT(unit, 6, i);
			double winPos = grain->winPos = 0.f;
			GET_GRAIN_WIN
			double counter = winSize * SAMPLERATE;
			counter = sc_max(4., counter);
			grain->counter = (int)counter;
			double winInc = grain->winInc = (double)windowSamples / counter;
			double amp = windowData[0];

			float azimuth = IN_AT(unit, 7, i);
			float elevation = IN_AT(unit, 8, i);
			float rho = IN_AT(unit, 9, i);

			float *Wout1 = Wout + i;
			float *Xout1 = Xout + i;
			float *Yout1 = Yout + i;
			float *Zout1 = Zout + i;

			CALC_BF_COEFS

			int nsmps = sc_min(grain->counter, inNumSamples - i);
			if (grain->interp >= 4) {
				for (int j=0; j<nsmps; ++j) {
					BUF_GRAIN_LOOP_BF_BODY_4;
					phase += rate;
					BUF_GRAIN_AMP
				}
			} else if (grain->interp >= 2) {
				for (int j=0; j<nsmps; ++j) {
					BUF_GRAIN_LOOP_BF_BODY_2;
					phase += rate;
					BUF_GRAIN_AMP
				}
			} else {
				for (int j=0; j<nsmps; ++j) {
					BUF_GRAIN_LOOP_BF_BODY_1;
					phase += rate;
					BUF_GRAIN_AMP
				}
			}

			grain->phase = phase;
			grain->curamp = amp;
			grain->winPos = winPos;
			// end change
			grain->counter -= nsmps;
			if (grain->counter <= 0) {
				// remove grain
				*grain = unit->mGrains[--unit->mNumActive];
			}
		    }
		}
		unit->curtrig = trig[i];
	    }

}

void BufGrainBBF_next_k(BufGrainBBF *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);
	float *Wout;
	float *Xout;
	float *Yout;
	float *Zout;
	Wout = ZOUT(0);
	Xout = ZOUT(1);
	Yout = ZOUT(2);
	Zout = ZOUT(3);
	float trig = IN0(0);

	World *world = unit->mWorld;
//	SndBuf *bufs = world->mSndBufs;
	//uint32 numBufs = world->mNumSndBufs;
	float wComp = unit->m_wComp;

	for (int i=0; i < unit->mNumActive; ) {
		WinGrainBF *grain = unit->mGrains + i;
//		uint32 bufnum = grain->bufnum;

		GRAIN_BUF
		CHECK_BUF

		if (bufChannels != 1) {
			 ++i;
			 continue;
		}

		double loopMax = (double)bufFrames;

		double rate = grain->rate;
		double phase = grain->phase;
		GET_GRAIN_WIN
		double amp = grain->curamp;
		double winInc = grain->winInc;
		double winPos = grain->winPos;

		GET_BF_AMPS

		float *Wout1 = Wout;
		float *Xout1 = Xout;
		float *Yout1 = Yout;
		float *Zout1 = Zout;

		int nsmps = sc_min(grain->counter, inNumSamples);
		if (grain->interp >= 4) {
			for (int j=0; j<nsmps; ++j) {
				BUF_GRAIN_LOOP_BF_BODY_4;
				phase += rate;
				BUF_GRAIN_AMP
			}
		} else if (grain->interp >= 2) {
			for (int j=0; j<nsmps; ++j) {
				BUF_GRAIN_LOOP_BF_BODY_2;
				phase += rate;
				BUF_GRAIN_AMP
			}
		} else {
			for (int j=0; j<nsmps; ++j) {
				BUF_GRAIN_LOOP_BF_BODY_1;
				phase += rate;
				BUF_GRAIN_AMP
			}
		}

		grain->phase = phase;
		grain->curamp = amp;
		grain->winPos = winPos;

		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	    }

		if ((trig > 0) && (unit->curtrig <=0)) {
			// start a grain
			if (unit->mNumActive+1 >= kMaxSynthGrains) {Print("Too many grains!\n");
			} else {
			uint32 bufnum = (uint32)IN0(2);
			float winSize = IN0(1);

			WinGrainBF *grain = unit->mGrains + unit->mNumActive++;

			SETUP_GRAIN_BUF
			GRAIN_BUF
			CHECK_BUF

			float bufSampleRate = buf->samplerate;
			float bufRateScale = bufSampleRate * SAMPLEDUR;
			double loopMax = (double)bufFrames;

//			WinGrainBF *grain = unit->mGrains + unit->mNumActive++;
//			grain->bufnum = bufnum;


			double rate = grain->rate = IN0(3) * bufRateScale;
			double phase = IN0(4) * bufFrames;
			grain->interp = (int)IN0(5);

			grain->mWindow = (int)IN0(6);
			double winPos = grain->winPos = 0.f;
			GET_GRAIN_WIN
			double counter = winSize * SAMPLERATE;
			counter = sc_max(4., counter);
			grain->counter = (int)counter;

			double winInc = grain->winInc = (double)windowSamples / counter;

			float azimuth = IN0(7);
			float elevation = IN0(8);
			float rho = IN0(9);

			CALC_BF_COEFS

			float amp = windowData[0];

			float *Wout1 = Wout;
			float *Xout1 = Xout;
			float *Yout1 = Yout;
			float *Zout1 = Zout;

			if (grain->interp >= 4) {
				for (int j=0; j<inNumSamples; ++j) {
					BUF_GRAIN_LOOP_BF_BODY_4;
					phase += rate;
					BUF_GRAIN_AMP
				}
			} else if (grain->interp >= 2) {
				for (int j=0; j<inNumSamples; ++j) {
					BUF_GRAIN_LOOP_BF_BODY_2;
					phase += rate;
					BUF_GRAIN_AMP
				}
			} else {
				for (int j=0; j<inNumSamples; ++j) {
					BUF_GRAIN_LOOP_BF_BODY_1;
					phase += rate;
					BUF_GRAIN_AMP
				}
			}

			grain->phase = phase;
			grain->curamp = amp;
			grain->winPos = winPos;
			// end change
			grain->counter -= inNumSamples;
			if (grain->counter <= 0) {
				// remove grain
				*grain = unit->mGrains[--unit->mNumActive];
			}
		}
	    }
	    unit->curtrig = trig;

}

void BufGrainBBF_Ctor(BufGrainBBF *unit)
{
	if (INRATE(0) == calc_FullRate)
	    SETCALC(BufGrainBBF_next_a);
	    else
	    SETCALC(BufGrainBBF_next_k);
	unit->mNumActive = 0;
	unit->curtrig = 0.f;
	unit->m_wComp = IN0(10);
	BufGrainBBF_next_k(unit, 1); // should be _k
}


////////////////////// BufGrainsI ///////////////////////////////

void BufGrainIBF_next_a(BufGrainIBF *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);
	float *Wout;
	float *Xout;
	float *Yout;
	float *Zout;
	Wout = ZOUT(0);
	Xout = ZOUT(1);
	Yout = ZOUT(2);
	Zout = ZOUT(3);

	float *trig = IN(0);

	World *world = unit->mWorld;
//	SndBuf *bufs = world->mSndBufs;
//	uint32 numBufs = world->mNumSndBufs;
	float wComp = unit->m_wComp;

	for (int i=0; i < unit->mNumActive; ) {
		WinGrainIBF *grain = unit->mGrains + i;
//		uint32 bufnum = grain->bufnum;

		GRAIN_BUF
		CHECK_BUF

		if (bufChannels != 1) {
			 ++i;
			 continue;
		}
		GET_INTERP_GRAIN_WIN
		double amp = grain->curamp;
		double winIncA = grain->winIncA;
		double winPosA = grain->winPosA;
		double winIncB = grain->winIncB;
		double winPosB = grain->winPosB;

		GET_BF_AMPS

		double loopMax = (double)bufFrames;

		double rate = grain->rate;
		double phase = grain->phase;

		float *Wout1 = Wout;
		float *Xout1 = Xout;
		float *Yout1 = Yout;
		float *Zout1 = Zout;

		int nsmps = sc_min(grain->counter, inNumSamples);
			if (grain->interp >= 4) {
				for (int j=0; j<nsmps; ++j) {
					BUF_GRAIN_LOOP_BF_BODY_4;
					phase += rate;
					BUF_INTERP_GRAIN_AMP
				}
			} else if (grain->interp >= 2) {
				for (int j=0; j<nsmps; ++j) {
					BUF_GRAIN_LOOP_BF_BODY_2;
					phase += rate;
					BUF_INTERP_GRAIN_AMP
				}
			} else {
				for (int j=0; j<nsmps; ++j) {
					BUF_GRAIN_LOOP_BF_BODY_1;
					phase += rate;
					BUF_INTERP_GRAIN_AMP
				}
			}


		grain->phase = phase;
		grain->curamp = amp;
		grain->winPosA = winPosA;
		grain->winPosB = winPosB;

		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	}

	for (int i=0; i<inNumSamples; ++i) {
		if ((trig[i] > 0) && (unit->curtrig <=0)) {
			// start a grain
			if (unit->mNumActive+1 >= kMaxSynthGrains) {Print("Too many grains!\n");
			} else {
			uint32 bufnum = (uint32)IN_AT(unit, 2, i);
//			if (bufnum >= numBufs) continue;
			float winSize = IN_AT(unit, 1, i);

			WinGrainIBF *grain = unit->mGrains + unit->mNumActive++;

			SETUP_GRAIN_BUF
			GRAIN_BUF
			CHECK_BUF

			if (bufChannels != 1) continue;

			float bufSampleRate = buf->samplerate;
			float bufRateScale = bufSampleRate * SAMPLEDUR;
			double loopMax = (double)bufFrames;

//			WinGrainIBF *grain = unit->mGrains + unit->mNumActive++;
//			grain->bufnum = bufnum;

			double rate = grain->rate = IN_AT(unit, 3, i) * bufRateScale;
			double phase = IN_AT(unit, 4, i) * bufFrames;
			grain->interp = (int)IN_AT(unit, 5, i);

			grain->mWindowA = (int)IN_AT(unit, 6, i);
			grain->mWindowB = (int)IN_AT(unit, 7, i);
			float ifac = grain->ifac = IN_AT(unit, 8, i);

			double winPosA = grain->winPosA = 0.f;
			double winPosB = grain->winPosB = 0.f;
			GET_INTERP_GRAIN_WIN
			double counter = winSize * SAMPLERATE;
			counter = sc_max(4., counter);
			grain->counter = (int)counter;
			double winIncA = grain->winIncA = (double)windowSamplesA / counter;
			double winIncB = grain->winIncB = (double)windowSamplesB / counter;
			double amp = lininterp(ifac, windowDataA[0], windowDataB[0]);

			float azimuth = IN_AT(unit, 9, i);
			float elevation = IN_AT(unit, 10, i);
			float rho = IN_AT(unit, 11, i);

			CALC_BF_COEFS

			float *Wout1 = Wout + i;
			float *Xout1 = Xout + i;
			float *Yout1 = Yout + i;
			float *Zout1 = Zout + i;

			int nsmps = sc_min(grain->counter, inNumSamples - i);
			if (grain->interp >= 4) {
				for (int j=0; j<nsmps; ++j) {
					BUF_GRAIN_LOOP_BF_BODY_4;
					phase += rate;
					BUF_INTERP_GRAIN_AMP
				}
			} else if (grain->interp >= 2) {
				for (int j=0; j<nsmps; ++j) {
					BUF_GRAIN_LOOP_BF_BODY_2;
					phase += rate;
					BUF_INTERP_GRAIN_AMP
				}
			} else {
				for (int j=0; j<nsmps; ++j) {
					BUF_GRAIN_LOOP_BF_BODY_1;
					phase += rate;
					BUF_INTERP_GRAIN_AMP
				}
			}

			grain->phase = phase;
			grain->curamp = amp;
			grain->winPosA = winPosA;
			grain->winPosB = winPosB;
			// end change
			grain->counter -= nsmps;
			if (grain->counter <= 0) {
				// remove grain
				*grain = unit->mGrains[--unit->mNumActive];
			}
		    }
		}
		unit->curtrig = trig[i];
	    }

}

void BufGrainIBF_next_k(BufGrainIBF *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);
	float *Wout;
	float *Xout;
	float *Yout;
	float *Zout;
	Wout = ZOUT(0);
	Xout = ZOUT(1);
	Yout = ZOUT(2);
	Zout = ZOUT(3);

	float trig = IN0(0);

	World *world = unit->mWorld;
//	SndBuf *bufs = world->mSndBufs;
	float wComp = unit->m_wComp;

	for (int i=0; i < unit->mNumActive; ) {
		WinGrainIBF *grain = unit->mGrains + i;
//		uint32 bufnum = grain->bufnum;

		GRAIN_BUF
		CHECK_BUF


		if (bufChannels != 1) {
			 ++i;
			 continue;
		}
		GET_INTERP_GRAIN_WIN
		double amp = grain->curamp;
		double winIncA = grain->winIncA;
		double winPosA = grain->winPosA;
		double winIncB = grain->winIncB;
		double winPosB = grain->winPosB;

		double loopMax = (double)bufFrames;

		double rate = grain->rate;
		double phase = grain->phase;
		GET_BF_AMPS
		float *Wout1 = Wout;
		float *Xout1 = Xout;
		float *Yout1 = Yout;
		float *Zout1 = Zout;

		int nsmps = sc_min(grain->counter, inNumSamples);
			if (grain->interp >= 4) {
				for (int j=0; j<nsmps; ++j) {
					BUF_GRAIN_LOOP_BF_BODY_4;
					phase += rate;
					BUF_INTERP_GRAIN_AMP
				}
			} else if (grain->interp >= 2) {
				for (int j=0; j<nsmps; ++j) {
					BUF_GRAIN_LOOP_BF_BODY_2;
					phase += rate;
					BUF_INTERP_GRAIN_AMP
				}
			} else {
				for (int j=0; j<nsmps; ++j) {
					BUF_GRAIN_LOOP_BF_BODY_1;
					phase += rate;
					BUF_INTERP_GRAIN_AMP
				}
			}


		grain->phase = phase;
		grain->curamp = amp;
		grain->winPosA = winPosA;
		grain->winPosB = winPosB;

		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	}

		if ((trig > 0) && (unit->curtrig <=0)) {
			// start a grain
			if (unit->mNumActive+1 >= kMaxSynthGrains) {
			    Print("Too many grains!\n");
			} else {
			uint32 bufnum = (uint32)IN0(2);
			float winSize = IN0(1);

			WinGrainIBF *grain = unit->mGrains + unit->mNumActive++;

			SETUP_GRAIN_BUF
			GRAIN_BUF
			CHECK_BUF

			float bufSampleRate = buf->samplerate;
			float bufRateScale = bufSampleRate * SAMPLEDUR;
			double loopMax = (double)bufFrames;

//			WinGrainIBF *grain = unit->mGrains + unit->mNumActive++;
//			grain->bufnum = bufnum;

			double rate = grain->rate = IN0(3) * bufRateScale;
			double phase = IN0(4) * bufFrames;
			grain->interp = (int)IN0(5);

			grain->mWindowA = (int)IN0(6);
			grain->mWindowB = (int)IN0(7);
			float ifac = grain->ifac = IN0(8);

			double winPosA = grain->winPosA = 0.f;
			double winPosB = grain->winPosB = 0.f;
			GET_INTERP_GRAIN_WIN
			double counter = winSize * SAMPLERATE;
			counter = sc_max(4., counter);
			grain->counter = (int)counter;
			double winIncA = grain->winIncA = (double)windowSamplesA / counter;
			double winIncB = grain->winIncB = (double)windowSamplesB / counter;
			double amp = lininterp(ifac, windowDataA[0], windowDataB[0]);
			float azimuth = IN0(9);
			float elevation = IN0(10);
			float rho = IN0(11);

			CALC_BF_COEFS

			float *Wout1 = Wout;
			float *Xout1 = Xout;
			float *Yout1 = Yout;
			float *Zout1 = Zout;
			int nsmps = sc_min(grain->counter, inNumSamples);
			if (grain->interp >= 4) {
				for (int j=0; j<nsmps; ++j) {
					BUF_GRAIN_LOOP_BF_BODY_4;
					phase += rate;
					BUF_INTERP_GRAIN_AMP
				}
			} else if (grain->interp >= 2) {
				for (int j=0; j<nsmps; ++j) {
					BUF_GRAIN_LOOP_BF_BODY_2;
					phase += rate;
					BUF_INTERP_GRAIN_AMP
				}
			} else {
				for (int j=0; j<nsmps; ++j) {
					BUF_GRAIN_LOOP_BF_BODY_1;
					phase += rate;
					BUF_INTERP_GRAIN_AMP
				}
			}

			grain->phase = phase;
			grain->curamp = amp;
			grain->winPosA = winPosA;
			grain->winPosB = winPosB;
			// end change
			grain->counter -= inNumSamples;
			if (grain->counter <= 0) {
				// remove grain
				*grain = unit->mGrains[--unit->mNumActive];
			}
		    }
		}
		unit->curtrig = trig;
	    }

void BufGrainIBF_Ctor(BufGrainIBF *unit)
{
	if (INRATE(0) == calc_FullRate)
	    SETCALC(BufGrainIBF_next_a);
	    else
	    SETCALC(BufGrainIBF_next_k);
	unit->mNumActive = 0;
	unit->curtrig = 0.f;
	unit->m_wComp = IN0(12);
	BufGrainIBF_next_k(unit, 1); // should be _k
}


/* begin add */
struct GrainInGJ
{
	double b1, y1, y2, curamp, winPos, winInc, grainAmp; // envelope
	int counter, chan;
	float pan1, pan2, winType;
};

struct GrainInJ : public Unit
{
	int mNumActive, m_channels, mMaxGrains;
	float curtrig;
	bool mFirst;
	GrainInGJ *mGrains;
};

struct GrainSinGJ
{
	double b1, y1, y2, curamp, winPos, winInc, grainAmp; // envelope
	int counter, chan;
	float pan1, pan2, winType;
	int32 oscphase; // the phase of the osc inside this grain
	int32 freq; // the freq of the osc inside this grain in phase inc
};

struct GrainSinJ : public Unit
{
	int mNumActive, m_channels, mMaxGrains;
	uint32 m_lomask;
	float curtrig;
	bool mFirst;
	double m_cpstoinc, m_radtoinc;
	GrainSinGJ *mGrains;
};

struct GrainFMGJ
{
	int32 coscphase, moscphase; // the phase of the osc inside this grain
	int32 mfreq; // the freq of the osc inside this grain in phase inc
	double b1, y1, y2, curamp, winPos, winInc, grainAmp; // envelope
	float deviation, carbase, pan1, pan2, winType;
	int counter, chan;
};


struct GrainFMJ : public Unit
{
	int mNumActive, m_channels, mMaxGrains;
	uint32 m_lomask;
	float curtrig;
	bool mFirst;
	double m_cpstoinc, m_radtoinc;
	GrainFMGJ *mGrains;
};

struct GrainBufGJ
{
	double phase, rate;
	double b1, y1, y2, curamp, winPos, winInc, grainAmp;
	float pan1, pan2, winType;
	int counter, chan, bufnum, interp, bufLoop;
};

struct GrainBufJ : public Unit
{
	int mNumActive, m_channels, mMaxGrains;
	float curtrig;
	GrainBufGJ *mGrains;
};

////////////////////////////////////////////////////////////////////////

extern "C"
{

	void GrainInJ_Ctor(GrainInJ* unit);
	void GrainInJ_Dtor(GrainInJ* unit);
	void GrainInJ_next_a(GrainInJ *unit, int inNumSamples);
	void GrainInJ_next_k(GrainInJ *unit, int inNumSamples);

	void GrainSinJ_Ctor(GrainSinJ* unit);
	void GrainSinJ_Dtor(GrainSinJ* unit);
	void GrainSinJ_next_a(GrainSinJ *unit, int inNumSamples);
	void GrainSinJ_next_k(GrainSinJ *unit, int inNumSamples);

	void GrainFMJ_Ctor(GrainFMJ* unit);
	void GrainFMJ_Dtor(GrainFMJ* unit);
	void GrainFMJ_next_a(GrainFMJ *unit, int inNumSamples);
	void GrainFMJ_next_k(GrainFMJ *unit, int inNumSamples);

	void GrainBufJ_Ctor(GrainBufJ* unit);
	void GrainBufJ_Dtor(GrainBufJ* unit);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// Granular UGens ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

inline float GRAIN_IN_AT(Unit* unit, int index, int offset)
{
	if (INRATE(index) == calc_FullRate) return IN(index)[offset];
	if (INRATE(index) == calc_DemandRate) return DEMANDINPUT_A(index, offset + 1);
	return IN0(index);
}

template <bool full_rate>
inline float grain_in_at(Unit* unit, int index, int offset)
{
	if (full_rate)
		return GRAIN_IN_AT(unit, index, offset);

	if (INRATE(index) == calc_DemandRate)
		return DEMANDINPUT_A(index, offset + 1);
	else
		return IN0(index);
}


#define GRAIN_BUF													\
	const SndBuf *buf;												\
	if (bufnum >= world->mNumSndBufs) {								\
		int localBufNum = bufnum - world->mNumSndBufs;				\
		Graph *parent = unit->mParent;								\
		if(localBufNum <= parent->localBufNum) {					\
			buf = parent->mLocalSndBufs + localBufNum;				\
		} else {													\
			bufnum = 0;												\
			buf = world->mSndBufs + bufnum;							\
		}															\
	} else {														\
		if (bufnum < 0) { bufnum = 0; }								\
		buf = world->mSndBufs + bufnum;								\
	}																\
	LOCK_SNDBUF_SHARED(buf);										\
	const float *bufData __attribute__((__unused__)) = buf->data;	\
	uint32 bufChannels __attribute__((__unused__)) = buf->channels;	\
	uint32 bufSamples __attribute__((__unused__)) = buf->samples;	\
	uint32 bufFrames = buf->frames;									\
	int guardFrame __attribute__((__unused__)) = bufFrames - 2;

#define DECLARE_WINDOW                                      \
	double winPos, winInc, w, b1, y1, y2, y0;               \
	float amp;												\
	winPos = winInc = w = b1 = y1 = y2 = y0 = amp = 0.;		\
	SndBuf *window;											\
	const float *windowData __attribute__((__unused__)) = 0;\
	uint32 windowSamples __attribute__((__unused__)) = 0;	\
	uint32 windowFrames __attribute__((__unused__)) = 0;	\
	int windowGuardFrame = 0;


#define CHECK_BUF								\
	if (!bufData) {								\
		unit->mDone = true;						\
		ClearUnitOutputs(unit, inNumSamples);	\
		return;									\
	}

#define GET_GRAIN_WIN_RELAXED(WINTYPE)					\
	do {												\
		assert(WINTYPE < unit->mWorld->mNumSndBufs);	\
		window = unit->mWorld->mSndBufs + (int)WINTYPE;	\
		windowData = window->data;						\
		windowSamples = window->samples;				\
		windowFrames = window->frames;					\
		windowGuardFrame = windowFrames - 1;			\
	} while (0);


static inline bool getGrainWin(Unit * unit, float wintype, SndBuf *& window, const float * & windowData,
							   uint32 & windowSamples, uint32 & windowFrames, int & windowGuardFrame)
{
	if (wintype >= unit->mWorld->mNumSndBufs) {
		Print("Envelope buffer out of range!\n");
		return false;
	}

	assert(wintype < unit->mWorld->mNumSndBufs);

	if (wintype < 0)
		return true; // use default hann window

	window = unit->mWorld->mSndBufs + (int)wintype;
	windowData = window->data;
	if (!windowData)
		return false;

	windowSamples = window->samples;
	windowFrames = window->frames;
	windowGuardFrame = windowFrames - 1;

	return true;
}

#define GRAIN_LOOP_BODY_4										\
		float amp = y1 * y1;									\
		phase = sc_gloop(phase, loopMax);						\
		int32 iphase = (int32)phase;							\
		float* table1 = bufData + iphase;						\
		float* table0 = table1 - 1;								\
		float* table2 = table1 + 1;								\
		float* table3 = table1 + 2;								\
		if (iphase == 0) {										\
			table0 += bufSamples;								\
		} else if (iphase >= guardFrame) {						\
			if (iphase == guardFrame) {							\
				table3 -= bufSamples;							\
			} else {											\
				table2 -= bufSamples;							\
				table3 -= bufSamples;							\
			}													\
		}														\
		float fracphase = phase - (double)iphase;				\
		float a = table0[0];									\
		float b = table1[0];									\
		float c = table2[0];									\
		float d = table3[0];									\
		float outval = amp * cubicinterp(fracphase, a, b, c, d)	\
		ZXP(out1) += outval;									\
		double y0 = b1 * y1 - y2;								\
		y2 = y1;												\
		y1 = y0;

#define GRAIN_LOOP_BODY_2								\
		float amp = y1 * y1;							\
		phase = sc_gloop(phase, loopMax);				\
		int32 iphase = (int32)phase;					\
		float* table1 = bufData + iphase;				\
		float* table2 = table1 + 1;						\
		if (iphase > guardFrame) {						\
			table2 -= bufSamples;						\
		}												\
		double fracphase = phase - (double)iphase;		\
		float b = table1[0];							\
		float c = table2[0];							\
		float outval = amp * (b + fracphase * (c - b));	\
		ZXP(out1) += outval;							\
		double y0 = b1 * y1 - y2;						\
		y2 = y1;										\
		y1 = y0;


#define GRAIN_LOOP_BODY_1						\
		float amp = y1 * y1;					\
		phase = sc_gloop(phase, loopMax);		\
		int32 iphase = (int32)phase;			\
		float outval = amp * bufData[iphase];	\
		ZXP(out1) += outval;					\
		double y0 = b1 * y1 - y2;				\
		y2 = y1;								\
		y1 = y0;

#define BUF_GRAIN_AMP											\
		winPos += winInc;										\
		int iWinPos = (int)winPos;								\
		double winFrac = winPos - (double)iWinPos;				\
		float* winTable1 = windowData + iWinPos;				\
		float* winTable2 = winTable1 + 1;						\
		if (winPos > windowGuardFrame) {						\
			winTable2 -= windowSamples;							\
		}														\
		amp = lininterp(winFrac, winTable1[0], winTable2[0]);

#define BUF_GRAIN_LOOP_BODY_4										\
		phase = sc_gloop(phase, loopMax);							\
		int32 iphase = (int32)phase;								\
		float* table1 = bufData + iphase;							\
		float* table0 = table1 - 1;									\
		float* table2 = table1 + 1;									\
		float* table3 = table1 + 2;									\
		if (iphase == 0) {											\
			table0 += bufSamples;									\
		} else if (iphase >= guardFrame) {							\
			if (iphase == guardFrame) {								\
				table3 -= bufSamples;								\
			} else {												\
				table2 -= bufSamples;								\
				table3 -= bufSamples;								\
			}														\
		}															\
		float fracphase = phase - (double)iphase;					\
		float a = table0[0];										\
		float b = table1[0];										\
		float c = table2[0];										\
		float d = table3[0];										\
		float outval = amp * cubicinterp(fracphase, a, b, c, d);	\
		ZXP(out1) += outval;

#define BUF_GRAIN_LOOP_BODY_2							\
		phase = sc_gloop(phase, loopMax);				\
		int32 iphase = (int32)phase;					\
		float* table1 = bufData + iphase;				\
		float* table2 = table1 + 1;						\
		if (iphase > guardFrame) {						\
			table2 -= bufSamples;						\
		}												\
		float fracphase = phase - (double)iphase;		\
		float b = table1[0];							\
		float c = table2[0];							\
		float outval = amp * (b + fracphase * (c - b)); \
		ZXP(out1) += outval;

// amp needs to be calculated by looking up values in window

#define BUF_GRAIN_LOOP_BODY_1					\
		phase = sc_gloop(phase, loopMax);		\
		int32 iphase = (int32)phase;			\
		float outval = amp * bufData[iphase];	\
		ZXP(out1) += outval;


#define SETUP_OUT											\
	uint32 numOutputs = unit->mNumOutputs;					\
	if (numOutputs > bufChannels) {							\
		unit->mDone = true;									\
		ClearUnitOutputs(unit, inNumSamples);				\
		return;												\
	}														\
	float *out[16];											\
	for (uint32 i=0; i<numOutputs; ++i) out[i] = ZOUT(i);


#define CALC_GRAIN_PAN													\
	float panangle, pan1, pan2;											\
	float *out1, *out2;													\
	if (numOutputs > 1) {												\
		if (numOutputs == 2) pan = pan * 0.5f;							\
		pan = sc_wrap(pan * 0.5f, 0.f, 1.f);							\
		float cpan = numOutputs * pan + 0.5f;							\
		float ipan = floor(cpan);										\
		float panfrac = cpan - ipan;									\
		panangle = panfrac * pi2_f;										\
		grain->chan = (int)ipan;										\
		if (grain->chan >= (int)numOutputs)								\
			grain->chan -= numOutputs;									\
		pan1 = grain->pan1 = cos(panangle);								\
		pan2 = grain->pan2 = sin(panangle);								\
	} else {															\
		grain->chan = 0;												\
		pan1 = grain->pan1 = 1.;										\
		pan2 = grain->pan2 = 0.;										\
	}

#define GET_GRAIN_INIT_AMP											\
	if(grain->winType < 0.){										\
		w = pi / counter;											\
		b1 = grain->b1 = 2. * cos(w);								\
		y1 = sin(w);												\
		y2 = 0.;													\
		amp = y1 * y1;												\
	} else {														\
		amp = windowData[0];										\
		winPos = grain->winPos = 0.f;								\
		winInc = grain->winInc = (double)windowSamples / counter;	\
	}

#define CALC_NEXT_GRAIN_AMP_INTERNAL						\
	do {													\
		y0 = b1 * y1 - y2;									\
		y2 = y1;											\
		y1 = y0;											\
		amp = y1 * y1;										\
	} while(0)

#define CALC_NEXT_GRAIN_AMP_CUSTOM								\
	do {														\
		winPos += winInc;										\
		int iWinPos = (int)winPos;								\
		double winFrac = winPos - (double)iWinPos;				\
		const float* winTable1 = windowData + iWinPos;			\
		const float* winTable2 = winTable1 + 1;					\
		if (!windowData)										\
			break;												\
		if (winPos > windowGuardFrame)							\
			winTable2 -= windowSamples;							\
		amp = lininterp(winFrac, winTable1[0], winTable2[0]);	\
	} while (0);												\
	if (!windowData)											\
		break;													\

#define CALC_NEXT_GRAIN_AMP									\
	if(grain->winType < 0.) {								\
		CALC_NEXT_GRAIN_AMP_INTERNAL;						\
	} else {												\
		CALC_NEXT_GRAIN_AMP_CUSTOM;							\
	}


#define GET_GRAIN_AMP_PARAMS					\
	if(grain->winType < 0.){					\
		b1 = grain->b1;							\
		y1 = grain->y1;							\
		y2 = grain->y2;							\
		amp = grain->curamp;					\
	} else {									\
		GET_GRAIN_WIN_RELAXED(grain->winType);	\
		if (!windowData) break;					\
		winPos = grain->winPos;					\
		winInc = grain->winInc;					\
		amp = grain->curamp;					\
	}

#define SAVE_GRAIN_AMP_PARAMS					\
	grain->y1 = y1;								\
	grain->y2 = y2;								\
	grain->winPos = winPos;						\
	grain->winInc = winInc;						\
	grain->curamp = amp;						\
	grain->counter -= nsmps;

#define WRAP_CHAN(offset)							\
	out1 = OUT(grain->chan) + offset;				\
	if(numOutputs > 1) {							\
		if((grain->chan + 1) >= (int)numOutputs)	\
			out2 = OUT(0) + offset;					\
		else										\
			out2 = OUT(grain->chan + 1) + offset;	\
	}

#define GET_PAN_PARAMS							\
	float pan1 = grain->pan1;					\
	uint32 chan1 = grain->chan;					\
	float *out1 = OUT(chan1);					\
	if(numOutputs > 1){							\
		pan2 = grain->pan2;						\
		uint32 chan2 = chan1 + 1;				\
		if (chan2 >= numOutputs) chan2 = 0;		\
		out2 = OUT(chan2);						\
	}


//////////////////// InGrainJ ////////////////////

inline void GrainInJ_next_play_active(GrainInJ * unit, int inNumSamples)
{
	const uint32 numOutputs = unit->mNumOutputs;
	float *in = IN(2);
	for (int i=0; i < unit->mNumActive; ) {
		GrainInGJ *grain = unit->mGrains + i;
		DECLARE_WINDOW
		GET_GRAIN_AMP_PARAMS
		// begin add //

		float grainAmp = grain->grainAmp;
		float pan2 = 0.f;
		float *out2;

		GET_PAN_PARAMS
		// end add //
		int nsmps = sc_min(grain->counter, inNumSamples);
		for (int j=0; j<nsmps; ++j) {
			float outval = grainAmp * amp * in[j];
			// begin change / add //
			out1[j] += outval * pan1;
			if(numOutputs > 1) out2[j] += outval * pan2;
			// end change //

			CALC_NEXT_GRAIN_AMP
		}

		SAVE_GRAIN_AMP_PARAMS

		if (grain->counter <= 0)
			*grain = unit->mGrains[--unit->mNumActive]; // remove grain
		else
			++i;
	}
}

template <bool full_rate>
inline void GrainInJ_next_start_new(GrainInJ * unit, int inNumSamples, int position)
{
	if (unit->mNumActive+1 >= unit->mMaxGrains) {
		Print("Too many grains!\n");
		return;
	}

	float winType = grain_in_at<full_rate>(unit, 4, position);
	DECLARE_WINDOW
	bool success = getGrainWin(unit, winType, window, windowData, windowSamples, windowFrames, windowGuardFrame);
	if (!success)
		return;

	GrainInGJ *grain = unit->mGrains + unit->mNumActive++;
	float winSize = grain_in_at<full_rate>(unit, 1, position);
	double counter = winSize * SAMPLERATE;
	counter = sc_max(4., counter);
	grain->counter = (int)counter;
	grain->winType = winType;
	float grainAmp = grain_in_at<full_rate>(unit, 6, position);
	grain->grainAmp = grainAmp;

	GET_GRAIN_INIT_AMP

	const uint32 numOutputs = unit->mNumOutputs;
	float *in = IN(2);

	float *in1 = in + position;
	// begin add //
	float pan = grain_in_at<full_rate>(unit, 3, position);

	CALC_GRAIN_PAN

	WRAP_CHAN(position)

	int nsmps = sc_min(grain->counter, inNumSamples - position);
	for (int j=0; j<nsmps; ++j) {
		float outval = grainAmp * amp * in1[j];
		// begin add / change
		out1[j] += outval * pan1;
		if(numOutputs > 1) out2[j] += outval * pan2;
		// end add / change
		CALC_NEXT_GRAIN_AMP
	}

	SAVE_GRAIN_AMP_PARAMS

	if (grain->counter <= 0)
		*grain = unit->mGrains[--unit->mNumActive]; // remove grain
}

void GrainInJ_next_a(GrainInJ *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);
	//begin add
	// end add

	GrainInJ_next_play_active(unit, inNumSamples);

	float *trig = IN(0);
	for (int i=0; i<inNumSamples; ++i) {
		if ((unit->curtrig <= 0) && (trig[i] > 0.0))
			GrainInJ_next_start_new<true>(unit, inNumSamples, i);
		unit->curtrig = trig[i];
	}
}

void GrainInJ_next_k(GrainInJ *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);
	if(unit->mFirst){
		unit->mFirst = false;
		float maxGrains = IN0(5);
		unit->mMaxGrains = (int)maxGrains;
		unit->mGrains = (GrainInGJ*)RTAlloc(unit->mWorld, unit->mMaxGrains * sizeof(GrainInGJ));
	}

	GrainInJ_next_play_active(unit, inNumSamples);

	float trig = IN0(0);

	if ((unit->curtrig <= 0) && (trig > 0.0))
		GrainInJ_next_start_new<false>(unit, inNumSamples, 0);
	unit->curtrig = trig;
}


void GrainInJ_Ctor(GrainInJ *unit)
{
	if (INRATE(0) == calc_FullRate)
		SETCALC(GrainInJ_next_a);
	else
		SETCALC(GrainInJ_next_k);
	unit->mFirst = true;
	unit->mNumActive = 0;
	unit->curtrig = 0.f;
	GrainInJ_next_k(unit, 1);
}

void GrainInJ_Dtor(GrainInJ *unit)
{
	RTFree(unit->mWorld, unit->mGrains);
}

inline void GrainSinJ_next_play_active(GrainSinJ * unit, int inNumSamples)
{
	const unsigned int numOutputs = unit->mNumOutputs;

	float *table0 = ft->mSineWavetable;
	float *table1 = table0 + 1;

	for (int i=0; i < unit->mNumActive; ) {
		GrainSinGJ *grain = unit->mGrains + i;
		DECLARE_WINDOW
		GET_GRAIN_AMP_PARAMS

		// begin add //

		float pan2 = 0.f;
		float *out2;

		GET_PAN_PARAMS

		// end add //

		int32 thisfreq = grain->freq;
		int32 oscphase = grain->oscphase;
		float grainAmp = grain->grainAmp;

		int nsmps = sc_min(grain->counter, inNumSamples);
		for (int j=0; j<nsmps; ++j) {
			float outval = grainAmp * amp * lookupi1(table0, table1, oscphase, unit->m_lomask);
			// begin change / add //
			out1[j] += outval * pan1;
			if(numOutputs > 1) out2[j] += outval * pan2;
			// end change //
			CALC_NEXT_GRAIN_AMP

			oscphase += thisfreq;
		}

		SAVE_GRAIN_AMP_PARAMS

		grain->oscphase = oscphase;
		if (grain->counter <= 0)
			*grain = unit->mGrains[--unit->mNumActive]; // remove grain
		else
			++i;
	}
}

template <bool full_rate>
inline void GrainSinJ_next_start_new(GrainSinJ * unit, int inNumSamples, int position)
{
	if (unit->mNumActive+1 >= unit->mMaxGrains) {
		Print("Too many grains!\n");
		return;
	}

	float winType = grain_in_at<full_rate>(unit, 4, position);
	DECLARE_WINDOW
	bool success = getGrainWin(unit, winType, window, windowData, windowSamples, windowFrames, windowGuardFrame);
	if (!success)
		return;

	GrainSinGJ *grain = unit->mGrains + unit->mNumActive++;
	float freq = grain_in_at<full_rate>(unit, 2, position);
	float winSize = grain_in_at<full_rate>(unit, 1, position);
	float grainAmp = grain_in_at<full_rate>(unit, 6, position);
	grain->grainAmp = grainAmp;
	int32 thisfreq = grain->freq = (int32)(unit->m_cpstoinc * freq);
	int32 oscphase = 0;
	double counter = winSize * SAMPLERATE;
	counter = sc_max(4., counter);
	grain->counter = (int)counter;
	grain->winType = winType;

	GET_GRAIN_INIT_AMP

	const uint32 numOutputs = unit->mNumOutputs;

	float *table0 = ft->mSineWavetable;
	float *table1 = table0 + 1;

	// begin add //
	float pan = grain_in_at<full_rate>(unit, 3, position);

	CALC_GRAIN_PAN

	WRAP_CHAN(position)

	// end add //

	int nsmps = sc_min(grain->counter, inNumSamples - position);
	for (int j=0; j<nsmps; ++j) {
		float outval = grainAmp * amp * lookupi1(table0, table1, oscphase, unit->m_lomask);
		// begin add / change
		out1[j] += outval * pan1;
		if(numOutputs > 1) out2[j] += outval * pan2;
		// end add / change
		CALC_NEXT_GRAIN_AMP

		oscphase += thisfreq;
	}
	grain->oscphase = oscphase;

	SAVE_GRAIN_AMP_PARAMS

	if (grain->counter <= 0)
		*grain = unit->mGrains[--unit->mNumActive]; // remove grain
}

void GrainSinJ_next_a(GrainSinJ *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);

	GrainSinJ_next_play_active(unit, inNumSamples);

	float *trig = IN(0);
	for (int i=0; i<inNumSamples; ++i) {
		if ((unit->curtrig <= 0) && (trig[i] > 0.0))
			GrainSinJ_next_start_new<true>(unit, inNumSamples, i);
		unit->curtrig = trig[i];
	}
}

void GrainSinJ_next_k(GrainSinJ *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);
	if(unit->mFirst){
		unit->mFirst = false;
		float maxGrains = IN0(5);
		unit->mMaxGrains = (int)maxGrains;
		unit->mGrains = (GrainSinGJ*)RTAlloc(unit->mWorld, unit->mMaxGrains * sizeof(GrainSinGJ));
	}

	GrainSinJ_next_play_active(unit, inNumSamples);

	float trig = IN0(0);


	if ((unit->curtrig <= 0) && (trig > 0.0))
		GrainSinJ_next_start_new<false>(unit, inNumSamples, 0);
	unit->curtrig = trig;
}

void GrainSinJ_Ctor(GrainSinJ *unit)
{
	if (INRATE(0) == calc_FullRate)
		SETCALC(GrainSinJ_next_a);
	else
		SETCALC(GrainSinJ_next_k);
	int tableSizeSin = ft->mSineSize;
	unit->m_lomask = (tableSizeSin - 1) << 3;
	unit->m_radtoinc = tableSizeSin * (rtwopi * 65536.);
	unit->m_cpstoinc = tableSizeSin * SAMPLEDUR * 65536.;
	unit->curtrig = 0.f;
	unit->mNumActive = 0;
	unit->mFirst = true;
	GrainSinJ_next_k(unit, 1);
}

void GrainSinJ_Dtor(GrainSinJ *unit)
{
	RTFree(unit->mWorld, unit->mGrains);
}


inline void GrainFMJ_next_play_active(GrainFMJ *unit, int inNumSamples)
{
	const uint32 numOutputs = unit->mNumOutputs;
	// end add

	float *table0 = ft->mSineWavetable;
	float *table1 = table0 + 1;

	for (int i=0; i < unit->mNumActive; ) {
		GrainFMGJ *grain = unit->mGrains + i;
		DECLARE_WINDOW
		GET_GRAIN_AMP_PARAMS

		int32 mfreq = grain->mfreq;
		int32 moscphase = grain->moscphase;
		int32 coscphase = grain->coscphase;
		float deviation = grain->deviation;
		float carbase = grain->carbase;
		float grainAmp = grain->grainAmp;
		// begin add //

		float pan2 = 0.f;
		float *out2;

		GET_PAN_PARAMS

		// end add //
		int nsmps = sc_min(grain->counter, inNumSamples);
		for (int j=0; j<nsmps; ++j) {
			float thismod = lookupi1(table0, table1, moscphase, unit->m_lomask) * deviation;
			float outval = grainAmp * amp * lookupi1(table0, table1, coscphase, unit->m_lomask);
			// begin change / add //
			out1[j] += outval * pan1;
			if(numOutputs > 1) out2[j] += outval * pan2;
			// end change //
			CALC_NEXT_GRAIN_AMP

			int32 cfreq = (int32)(unit->m_cpstoinc * (carbase + thismod)); // needs to be calced in the loop!
			coscphase += cfreq;
			moscphase += mfreq;
		} // need to save float carbase, int32 mfreq, float deviation
		grain->coscphase = coscphase;
		grain->moscphase = moscphase;

		SAVE_GRAIN_AMP_PARAMS

		if (grain->counter <= 0)
			*grain = unit->mGrains[--unit->mNumActive]; // remove grain
		else
			++i;
	}
}

template <bool full_rate>
inline void GrainFMJ_next_start_new(GrainFMJ * unit, int inNumSamples, int position)
{
	if (unit->mNumActive+1 >= unit->mMaxGrains) {
		Print("Too many grains!\n");
		return;
	}

	float winType = grain_in_at<full_rate>(unit, 6, position);
	DECLARE_WINDOW
	bool success = getGrainWin(unit, winType, window, windowData, windowSamples, windowFrames, windowGuardFrame);
	if (!success)
		return;

	GrainFMGJ *grain = unit->mGrains + unit->mNumActive++;
	float winSize = GRAIN_IN_AT(unit, 1, position);
	float carfreq = GRAIN_IN_AT(unit, 2, position);
	float modfreq = GRAIN_IN_AT(unit, 3, position);
	float index = GRAIN_IN_AT(unit, 4, position);
	float grainAmp = GRAIN_IN_AT(unit, 8, position);
	float deviation = grain->deviation = index * modfreq;
	int32 mfreq = grain->mfreq = (int32)(unit->m_cpstoinc * modfreq);
	grain->carbase = carfreq;
	grain->grainAmp = grainAmp;
	int32 coscphase = 0;
	int32 moscphase = 0;
	double counter = winSize * SAMPLERATE;
	counter = sc_max(4., counter);
	grain->counter = (int)counter;
	grain->winType = winType; //GRAIN_IN_AT(unit, 6, i);

	GET_GRAIN_INIT_AMP
	const uint32 numOutputs = unit->mNumOutputs;

	float *table0 = ft->mSineWavetable;
	float *table1 = table0 + 1;

	// begin add //
	float pan = GRAIN_IN_AT(unit, 5, position);

	CALC_GRAIN_PAN

	WRAP_CHAN(position)

	// end add //
	int nsmps = sc_min(grain->counter, inNumSamples - position);
	for (int j=0; j<nsmps; ++j) {
		float thismod = lookupi1(table0, table1, moscphase, unit->m_lomask) * deviation;
		float outval = grainAmp * amp * lookupi1(table0, table1, coscphase, unit->m_lomask);
		// begin add / change
		out1[j] += outval * pan1;
		if(numOutputs > 1) out2[j] += outval * pan2;
		// end add / change
		CALC_NEXT_GRAIN_AMP

		int32 cfreq = (int32)(unit->m_cpstoinc * (carfreq + thismod)); // needs to be calced in the loop!
		coscphase += cfreq;
		moscphase += mfreq;
	} // need to save float carbase, int32 mfreq, float deviation
	grain->coscphase = coscphase;
	grain->moscphase = moscphase;

	SAVE_GRAIN_AMP_PARAMS

	if (grain->counter <= 0)
		*grain = unit->mGrains[--unit->mNumActive]; // remove grain
}

void GrainFMJ_next_a(GrainFMJ *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);

	GrainFMJ_next_play_active(unit, inNumSamples);

	float *trig = IN(0);
	for (int i=0; i<inNumSamples; ++i) {
		if ((unit->curtrig <= 0) && (trig[i] > 0.0))
			GrainFMJ_next_start_new<true>(unit, inNumSamples, i);
		unit->curtrig = trig[i];
	}
}

void GrainFMJ_next_k(GrainFMJ *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);

	if (unit->mFirst) {
		unit->mFirst = false;
		float maxGrains = IN0(7);
		unit->mMaxGrains = (int)maxGrains;
		unit->mGrains = (GrainFMGJ*)RTAlloc(unit->mWorld, unit->mMaxGrains * sizeof(GrainFMGJ));
	}

	GrainFMJ_next_play_active(unit, inNumSamples);

	float trig = IN0(0);
	if ((unit->curtrig <= 0) && (trig > 0.0))
		GrainFMJ_next_start_new<false>(unit, inNumSamples, 0);

	unit->curtrig = trig;
}

void GrainFMJ_Ctor(GrainFMJ *unit)
{
	if (INRATE(0) == calc_FullRate)
		SETCALC(GrainFMJ_next_a);
	else
		SETCALC(GrainFMJ_next_k);
	int tableSizeSin = ft->mSineSize;
	unit->m_lomask = (tableSizeSin - 1) << 3;
	unit->m_radtoinc = tableSizeSin * (rtwopi * 65536.);
	unit->m_cpstoinc = tableSizeSin * SAMPLEDUR * 65536.;
	unit->curtrig = 0.f;
	unit->mNumActive = 0;
	unit->mFirst = true;
	GrainFMJ_next_k(unit, 1);
}

void GrainFMJ_Dtor(GrainFMJ *unit)
{
	RTFree(unit->mWorld, unit->mGrains);
}

#define GRAIN_BUF_LOOP_BODY_4_MONO \
		float outval = 0.0f; \
		if(bufLoop || (phase < loopMax)) { \
			phase = sc_gloop(phase, loopMax); \
			int32 iphase = (int32)phase; \
			const float* table1 = bufData + iphase; \
			const float* table0 = table1 - 1; \
			const float* table2 = table1 + 1; \
			const float* table3 = table1 + 2; \
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
			float a = table0[0]; \
			float b = table1[0]; \
			float c = table2[0]; \
			float d = table3[0]; \
			outval = grainAmp * amp * cubicinterp(fracphase, a, b, c, d); \
		} \
		out1[j] += outval * pan1; \

#define GRAIN_BUF_LOOP_BODY_4_STEREO \
		GRAIN_BUF_LOOP_BODY_4_MONO \
		out2[j] += outval * pan2;

#define GRAIN_BUF_LOOP_BODY_2_MONO \
		float outval = 0.0f; \
		if(bufLoop || (phase < loopMax)) { \
			phase = sc_gloop(phase, loopMax); \
			int32 iphase = (int32)phase; \
			const float* table1 = bufData + iphase; \
			const float* table2 = table1 + 1; \
			if (iphase > guardFrame) { \
				table2 -= bufSamples; \
			} \
			float fracphase = phase - (double)iphase; \
			float b = table1[0]; \
			float c = table2[0]; \
			outval = grainAmp * amp * (b + fracphase * (c - b)); \
		} \
		out1[j] += outval * pan1; \

#define GRAIN_BUF_LOOP_BODY_2_STEREO \
		GRAIN_BUF_LOOP_BODY_2_MONO \
		out2[j] += outval * pan2;

#define GRAIN_BUF_LOOP_BODY_1_MONO \
		float outval = 0.0f; \
		if(bufLoop || (phase < loopMax)) { \
			phase = sc_gloop(phase, loopMax); \
			int32 iphase = (int32)phase; \
			outval = grainAmp * amp * bufData[iphase]; \
		} \
		out1[j] += outval * pan1;

#define GRAIN_BUF_LOOP_BODY_1_STEREO \
		GRAIN_BUF_LOOP_BODY_1_MONO \
		out2[j] += outval * pan2;


#define GRAIN_BUF_PLAY_GRAIN(WINDOW) \
	do {\
		if (numOutputs == 1) \
		{ \
			if (grain->interp >= 4) { \
				for (int j=0; j<nsmps; j++) { \
					GRAIN_BUF_LOOP_BODY_4_MONO \
					CALC_NEXT_GRAIN_AMP_##WINDOW; \
					phase += rate; \
				} \
			} else if (grain->interp >= 2) { \
				for (int j=0; j<nsmps; j++) { \
					GRAIN_BUF_LOOP_BODY_2_MONO \
					CALC_NEXT_GRAIN_AMP_##WINDOW; \
					phase += rate; \
				} \
			} else { \
				for (int j=0; j<nsmps; j++) { \
					GRAIN_BUF_LOOP_BODY_1_MONO \
					CALC_NEXT_GRAIN_AMP_##WINDOW; \
					phase += rate; \
				} \
			} \
		} else { \
			if (grain->interp >= 4) { \
				for (int j=0; j<nsmps; j++) { \
					GRAIN_BUF_LOOP_BODY_4_STEREO \
					CALC_NEXT_GRAIN_AMP_##WINDOW; \
					phase += rate; \
				} \
			} else if (grain->interp >= 2) { \
				for (int j=0; j<nsmps; j++) { \
					GRAIN_BUF_LOOP_BODY_2_STEREO \
					CALC_NEXT_GRAIN_AMP_##WINDOW; \
					phase += rate; \
				} \
			} else { \
				for (int j=0; j<nsmps; j++) { \
					GRAIN_BUF_LOOP_BODY_1_STEREO \
					CALC_NEXT_GRAIN_AMP_##WINDOW; \
					phase += rate; \
				} \
			} \
		} \
	} while (0)


static inline bool GrainBufJ_grain_cleanup(GrainBufJ * unit, GrainBufGJ * grain)
{
	if (grain->counter <= 0) {
		*grain = unit->mGrains[--unit->mNumActive]; // remove grain
		return true;
	} else
		return false;
}

template <bool IsMono>
static inline void GrainBufJ_next_play_active(GrainBufJ *unit, int inNumSamples)
{
	const uint32 numOutputs = IsMono ? 1 : unit->mNumOutputs;

	World *world = unit->mWorld;

	for (int i=0; i < unit->mNumActive; ) {
		GrainBufGJ *grain = unit->mGrains + i;
		uint32 bufnum = grain->bufnum;
		float grainAmp = grain->grainAmp;
		int bufLoop = grain->bufLoop;
		
		GRAIN_BUF

		if (!bufData || (bufChannels != 1)) {
			grain->counter -= inNumSamples;
			if (!GrainBufJ_grain_cleanup(unit, grain))
				++i;
			continue;
		}

		double loopMax = (double)bufFrames;
		double rate = grain->rate;
		double phase = grain->phase;

		DECLARE_WINDOW
		GET_GRAIN_AMP_PARAMS



		// begin add //
		float pan2 = 0.f;
		float *out2;
		GET_PAN_PARAMS
		// end add //
		int nsmps = sc_min(grain->counter, inNumSamples);

		if (grain->winType < 0.)
			GRAIN_BUF_PLAY_GRAIN(INTERNAL);
		else
			GRAIN_BUF_PLAY_GRAIN(CUSTOM);

		if (GrainBufJ_grain_cleanup(unit, grain))
			continue;
		++i;

		grain->phase = phase;

		SAVE_GRAIN_AMP_PARAMS
	}
}

template <bool full_rate, bool IsMono>
static inline void GrainBufJ_next_start_new(GrainBufJ *unit, int inNumSamples, int position)
{
	World *world = unit->mWorld;

	if (unit->mNumActive+1 >= unit->mMaxGrains) {
		Print("Too many grains!\n");
		return;
	}
	float grainAmp = grain_in_at<full_rate>(unit, 9, position);
	int bufLoop = (int)grain_in_at<full_rate>(unit, 10, position);
	
	float winType = grain_in_at<full_rate>(unit, 7, position);
	DECLARE_WINDOW
	bool success = getGrainWin(unit, winType, window, windowData, windowSamples, windowFrames, windowGuardFrame);
	if (!success)
		return;

	GrainBufGJ *grain = unit->mGrains + unit->mNumActive++;
	grain->grainAmp = grainAmp;
	grain->bufLoop = bufLoop;
	int32 bufnum = grain_in_at<full_rate>(unit, 2, position);
	grain->bufnum = bufnum;

	GRAIN_BUF

	if ( (bufChannels != 1) || (!bufData) ) {
		GrainBufJ_grain_cleanup(unit, grain);
		return;
	}

	float bufSampleRate = buf->samplerate;
	float bufRateScale = bufSampleRate * SAMPLEDUR;
	double loopMax = (double)bufFrames;

	double rate = grain->rate = grain_in_at<full_rate>(unit, 3, position) * bufRateScale;
	double phase = grain_in_at<full_rate>(unit, 4, position) * bufFrames;
	if (!sc_isfinite(phase)) {
		GrainBufJ_grain_cleanup(unit, grain);
		return;
	}

	double counter = grain_in_at<full_rate>(unit, 1, position) * SAMPLERATE;
	counter = sc_max(4., counter);
	grain->counter = (int)counter;

	grain->interp = (int)grain_in_at<full_rate>(unit, 5, position);
	grain->winType = winType;

	GET_GRAIN_INIT_AMP

	const uint32 numOutputs = IsMono ? 1 : unit->mNumOutputs;

	// begin add //
	float pan = grain_in_at<full_rate>(unit, 6, position);

	CALC_GRAIN_PAN

	WRAP_CHAN(position)

	// end add //

	int nsmps = sc_min(grain->counter, inNumSamples - position);

	if (grain->winType < 0.)
		GRAIN_BUF_PLAY_GRAIN(INTERNAL);
	else
		GRAIN_BUF_PLAY_GRAIN(CUSTOM);

	grain->phase = phase;

	SAVE_GRAIN_AMP_PARAMS

	GrainBufJ_grain_cleanup(unit, grain);
}


template <bool MultiChannel>
static void GrainBufJ_next_a(GrainBufJ *unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);

	GrainBufJ_next_play_active<MultiChannel>(unit, inNumSamples);

	float *trig = IN(0);
	for (int i=0; i<inNumSamples; i++) {
		if ((trig[i] > 0) && (unit->curtrig <=0))
			GrainBufJ_next_start_new<true, MultiChannel>(unit, inNumSamples, i);
		unit->curtrig = trig[i];
	}
}

template <bool MultiChannel>
static void GrainBufJ_next_k(GrainBufJ * unit, int inNumSamples)
{
	ClearUnitOutputs(unit, inNumSamples);

	GrainBufJ_next_play_active<MultiChannel>(unit, inNumSamples);

	float trig = IN0(0);
	if ((trig > 0) && (unit->curtrig <=0))
		GrainBufJ_next_start_new<false, MultiChannel>(unit, inNumSamples, 0);
	unit->curtrig = trig;
}

static void GrainBufJ_next_k_1(GrainBufJ * unit, int inNumSamples)
{
	GrainBufJ_next_k<true>(unit, inNumSamples);
}

static void GrainBufJ_next_k_2(GrainBufJ * unit, int inNumSamples)
{
	GrainBufJ_next_k<false>(unit, inNumSamples);
}

static void GrainBufJ_next_a_1(GrainBufJ * unit, int inNumSamples)
{
	GrainBufJ_next_a<true>(unit, inNumSamples);
}

static void GrainBufJ_next_a_2(GrainBufJ * unit, int inNumSamples)
{
	GrainBufJ_next_a<false>(unit, inNumSamples);
}

void GrainBufJ_Ctor(GrainBufJ *unit)
{
	unit->mNumActive = 0;
	unit->curtrig = 0.f;

	float maxGrains = IN0(8);
	unit->mMaxGrains = (int)maxGrains;
	unit->mGrains = (GrainBufGJ*)RTAlloc(unit->mWorld, unit->mMaxGrains * sizeof(GrainBufGJ));

	if (unit->mNumOutputs == 1) {
		if (INRATE(0) == calc_FullRate)
			SETCALC(GrainBufJ_next_a_1);
		else
			SETCALC(GrainBufJ_next_k_1);
	} else {
		if (INRATE(0) == calc_FullRate)
			SETCALC(GrainBufJ_next_a_2);
		else
			SETCALC(GrainBufJ_next_k_2);
	}

	(unit->mCalcFunc)(unit, 1);
}

void GrainBufJ_Dtor(GrainBufJ *unit)
{
	RTFree(unit->mWorld, unit->mGrains);
}


/* end add */
////////////////////////////////////////////////////////////////////////////////////////////////////////

PluginLoad(JoshGrain)
{
	ft = inTable;

	DefineSimpleCantAliasUnit(MonoGrain);
	DefineSimpleCantAliasUnit(MonoGrainBF);
	DefineSimpleCantAliasUnit(SinGrain);
	DefineSimpleCantAliasUnit(SinGrainB);
	DefineSimpleCantAliasUnit(SinGrainI);
	DefineSimpleCantAliasUnit(InGrain);
	DefineSimpleCantAliasUnit(InGrainB);
	DefineSimpleCantAliasUnit(InGrainI);
	DefineSimpleCantAliasUnit(FMGrain);
	DefineSimpleCantAliasUnit(FMGrainB);
	DefineSimpleCantAliasUnit(FMGrainI);
	DefineSimpleCantAliasUnit(BufGrain);
	DefineSimpleCantAliasUnit(BufGrainB);
	DefineSimpleCantAliasUnit(BufGrainI);
	DefineSimpleCantAliasUnit(InGrainBF);
	DefineSimpleCantAliasUnit(InGrainBBF);
	DefineSimpleCantAliasUnit(InGrainIBF);
	DefineSimpleCantAliasUnit(SinGrainBF);
	DefineSimpleCantAliasUnit(SinGrainBBF);
	DefineSimpleCantAliasUnit(SinGrainIBF);
	DefineSimpleCantAliasUnit(FMGrainBF);
	DefineSimpleCantAliasUnit(FMGrainBBF);
	DefineSimpleCantAliasUnit(FMGrainIBF);
	DefineSimpleCantAliasUnit(BufGrainBF);
	DefineSimpleCantAliasUnit(BufGrainBBF);
	DefineSimpleCantAliasUnit(BufGrainIBF);
	DefineDtorCantAliasUnit(GrainInJ);
	DefineDtorCantAliasUnit(GrainSinJ);
	DefineDtorCantAliasUnit(GrainBufJ);
	DefineDtorCantAliasUnit(GrainFMJ);

}


