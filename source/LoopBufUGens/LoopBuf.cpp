/*
 *  LoopBuf.cpp
 *  xSC3plugins
 *
 *  Created by Lance Putnam on Sun Jun 27 2004.
 *  Copyright (c) 2004 __MyCompanyName__. All rights reserved.
 *
 */

#include "SC_PlugIn.h"

static InterfaceTable *ft;

struct LoopBuf : public Unit
{
	double m_phase;
	float m_prevgate;
	float m_fbufnum;
	bool m_playThrough;
	SndBuf *m_buf;
};

struct FLoopBuf : public Unit
{
	uint32 m_phase;
	float m_prevgate;
	float m_fbufnum;
	bool m_playThrough;
	SndBuf *m_buf;
};

extern "C"
{
	void load(InterfaceTable *inTable);

//	void LoopBuf_next_aa(LoopBuf *unit, int inNumSamples);
//	void LoopBuf_next_ak(LoopBuf *unit, int inNumSamples);
//	void LoopBuf_next_ka(LoopBuf *unit, int inNumSamples);
	void LoopBuf_next_kk(LoopBuf *unit, int inNumSamples);
	void LoopBuf_Ctor(LoopBuf* unit);

	void FLoopBuf_next(FLoopBuf *unit, int inNumSamples);
	void FLoopBuf_Ctor(FLoopBuf* unit);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
/*
static float cubicinterp(float x, float y0, float y1, float y2, float y3)
{
	// 4-point, 3rd-order Hermite (x-form)
	// 8 mul, 11 add
	float c0 = y1;
	float c1 = 0.5f * (y2 - y0);
	float c2 = y0 - (2.5f * y1) + y2 + y2 - 0.5f * y3;
	float c3 = 0.5f * (y3 - y0) + 1.5f * (y1 - y2);

	return ((c3 * x + c2) * x + c1) * x + c0;

//	float result = (0.5f * (y3 - y0) + 1.5f * (y1 - y2)) * x;
//	result = (result + (y0 - (2.5f * y1) + y2 + y2 - 0.5f * y3)) * x;
//	result = (result + ( 0.5f * (y2 - y0) )) * x;
//
//	return result + y1;

//	float hy0 = 0.5f * y0;
//	float hy1 = 0.5f * y1;
//	float hy2 = 0.5f * y2;
//	float hy3 = 0.5f * y3;
//	float c1 = hy2 - hy0;
//	float c2 = y0 + y2 + y2 - y1 - y1 - hy1 - hy3;
//	float c3 = hy3 - hy0 + y1 + hy1 - y2 - hy2;
//
//	return ((c3 * x + c2) * x + c1) * x + y1;

	// 4 mul, 15 add
	//float ty0 = y0 + y0;
//	float ty1 = y1 + y1;
//	float ty2 = y2 + y2;
//	float ty2mty1 = ty2 - ty1;
//	float y1py3 = y1 + y3;
//	//float c1 = y2 - y0;
//	//float c2 = y0 + y0 + ty2mty1 + ty2mty1 - y1py3;
//	//float c3 = y1py3 - y0 - y2 - ty2mty1;
//
//	//return 0.5f * (((c3 * x + c2) * x + c1) * x + ty1);
//	return 0.5f * ((( (y1py3 - y0 - y2 - ty2mty1) * x + (y0 + y0 + ty2mty1 + ty2mty1 - y1py3) ) * x + (y2 - y0) ) * x + ty1);

	// 6 mul, 9 add
//	float y0my3 = y0 - y3;
//	float y1my2 = y1 - y2;
//	float y0my2 = y0 - y2;
//	// -y0 + 3*y1 - 3*y2 + y3
//	float result = (3.f*y1my2 - y0my3) * x;
//
//	// 2*y0 - 5*y1 + 4*y2 - y3
//	result = (result + -5.f*y1my2 + y0my3 + y0my2) * x;
//
//	result = (result - y0my2) * x;
//	return (0.5f * result) + y1;

}
*/
//////////////////////////////////////////////////////////////////////////////////////////////////

//inline double sc_loop(Unit *unit, double in, double hi, int loop)
//{
//	// avoid the divide if possible
//	if (in >= hi) {			// forward playback
//		if (!loop) {
//			unit->mDone = true;
//			return hi;
//		}
//		in -= hi;
//		if (in < hi) return in;
//	} else if (in < 0.) {   // reverse playback
//		if (!loop) {
//			unit->mDone = true;
//			return 0.;
//		}
//		in += hi;
//		if (in >= 0.) return in;
//	} else return in;
//
//	return in - hi * floor(in/hi);
//}

// intel asm
//inline int truncate(float flt)
//{
//	int i;
//	_asm
//	{
//	fld flt
//	fistp i
//	}
//	return i
//}

#define CHECK_LOOP \
	if(endLoop >= fbufFrames)\
		endLoop = fbufFrames - 1.f;\
	else if(endLoop < 0.f)\
		endLoop = 0.f;\
	if(startLoop < 0.f)\
		startLoop = 0.f;\
	else if(startLoop >= fbufFrames)\
		startLoop = fbufFrames - 1.f;\
	if(startLoop > endLoop){\
		float temp = endLoop;\
		endLoop = startLoop;\
		startLoop = temp;\
	}\
	float loopFrames = endLoop - startLoop;\
	if(loopFrames < 1.f){\
		loopFrames = 1.f;\
		if(endLoop >= fbufFrames - 1.f)\
			startLoop = endLoop - 1.f;\
		else endLoop = startLoop + 1.f;\
	}

#define CHECK_GATE\
	bool gateIsPos = gate > 0.f;\
	bool gateWasNeg = unit->m_prevgate <= 0.f;\
	if (gateIsPos && gateWasNeg) {\
		unit->mDone = false;\
		unit->m_playThrough = false;\
		phase = ZIN0(3);\
	}\
	else if (!(gateIsPos || gateWasNeg))\
		unit->m_playThrough = true;\
	unit->m_prevgate = gate;\

#define WRAP_PHASE_FWD \
	if(phase > endLoop){\
		if(unit->m_playThrough){\
			unit->mDone = true;\
			phase = endLoop;\
		}\
		else{\
			phase -= loopFrames;\
			if(phase > endLoop){\
				phase -= loopFrames * floorf((phase-startLoop)/loopFrames);\
			}\
		}\
	}\
	int32 iphase = (int32)phase;

#define WRAP_PHASE_BWD \
	if(phase < startLoop){\
		if(unit->m_playThrough){\
			unit->mDone = true;\
			phase = startLoop;\
		}\
		else{\
			phase += loopFrames;\
			if(phase < startLoop){\
				phase += loopFrames * floorf((endLoop-phase)/loopFrames);\
			}\
		}\
	}\
	int32 iphase = (int32)phase;


#define CHECK_BUF \
	if (!bufData) { \
                unit->mDone = true; \
		ClearUnitOutputs(unit, inNumSamples); \
		return; \
	}

#define SETUP_OUT \
	uint32 numOutputs = unit->mNumOutputs; \
	if (numOutputs > bufChannels) { \
                unit->mDone = true; \
		ClearUnitOutputs(unit, inNumSamples); \
		return; \
	} \
	float *out[16]; \
	for (uint32 i=0; i<numOutputs; ++i) out[i] = ZOUT(i);

#define LOOP_BODY_4 \
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
		int32 index = 0; \
		float fracphase = phase - (double)iphase; \
		for (uint32 i=0; i<numOutputs; ++i) { \
			float a = table0[index]; \
			float b = table1[index]; \
			float c = table2[index]; \
			float d = table3[index]; \
			*++(out[i]) = cubicinterp(fracphase, a, b, c, d); \
			index++; \
		}\
		phase += rate;

#define LOOP_BODY_2 \
		float* table1 = bufData + iphase * bufChannels; \
		float* table2 = table1 + bufChannels; \
		if (iphase > guardFrame) { \
			table2 -= bufSamples; \
		} \
		int32 index = 0; \
		float fracphase = phase - (double)iphase; \
		for (uint32 i=0; i<numOutputs; ++i) { \
			float b = table1[index]; \
			float c = table2[index]; \
			*++(out[i]) = b + fracphase * (c - b); \
			index++; \
		}\
		phase += rate;

#define LOOP_BODY_1 \
		float* table1 = bufData + iphase * bufChannels; \
		int32 index = 0; \
		for (uint32 i=0; i<numOutputs; ++i) { \
			*++(out[i]) = table1[index++]; \
		}\
		phase += rate;

void LoopBuf_Ctor(LoopBuf *unit)
{
// input 1 => rate
// input 2 => gate
//	if (INRATE(1) == calc_FullRate) {
//		if (INRATE(2) == calc_FullRate) {
//			SETCALC(LoopBuf_next_aa);
//		} else {
//			SETCALC(LoopBuf_next_ak);
//		}
//	} else {
//		if (INRATE(2) == calc_FullRate) {
//			SETCALC(LoopBuf_next_ka);
//		} else {
			SETCALC(LoopBuf_next_kk);
//		}
//	}

	unit->m_fbufnum = -1e9f;
	unit->m_prevgate = 0.;
	unit->m_phase = ZIN0(3);
	unit->m_playThrough = false;

	ClearUnitOutputs(unit, 1);
}

void LoopBuf_next_kk(LoopBuf *unit, int inNumSamples)
{
	float rate		= ZIN0(1);
	float gate		= ZIN0(2);
	float startLoop = ZIN0(4);
	float endLoop   = ZIN0(5);
	int32 ipol		= (int32)ZIN0(6);

	double phase = unit->m_phase;
	bool isPlayingForward = rate >= 0.f;

	GET_BUF
	CHECK_BUF
	SETUP_OUT

	float fbufFrames = (float)bufFrames;

	CHECK_LOOP
	CHECK_GATE

	if(isPlayingForward){
		if( unit->m_playThrough )
			endLoop = fbufFrames;

		switch(ipol){
		case 2:
			for (int i=0; i<inNumSamples; ++i) {
				WRAP_PHASE_FWD
				LOOP_BODY_2
			}
			break;
		case 4:
			for (int i=0; i<inNumSamples; ++i) {
				WRAP_PHASE_FWD
				LOOP_BODY_4
			}
			break;
		default:
			for (int i=0; i<inNumSamples; ++i) {
				WRAP_PHASE_FWD
				LOOP_BODY_1
			}
			break;
		};
	}
	else {
		if( unit->m_playThrough )
			startLoop = 0.f;

		switch(ipol){
		case 2:
			for (int i=0; i<inNumSamples; ++i) {
				WRAP_PHASE_BWD
				LOOP_BODY_2
			}
			break;
		case 4:
			for (int i=0; i<inNumSamples; ++i) {
				WRAP_PHASE_BWD
				LOOP_BODY_4
			}
			break;
		default:
			for (int i=0; i<inNumSamples; ++i) {
				WRAP_PHASE_BWD
				LOOP_BODY_1
			}
			break;
		};
	}
	unit->m_phase = phase;
}

void FLoopBuf_Ctor(FLoopBuf *unit)
{
	SETCALC(FLoopBuf_next);

	unit->m_fbufnum = -1e9f;
	unit->m_prevgate = 0.;
	unit->m_phase = (uint32)ZIN0(3);
	unit->m_playThrough = false;

	ClearUnitOutputs(unit, 1);
}

void FLoopBuf_next(FLoopBuf *unit, int inNumSamples)
{
	float rate		= ZIN0(1);
	float gate		= ZIN0(2);
	//int32 ipol		= (int32)ZIN0(6);

	uint32 phase = unit->m_phase;
	//bool isPlayingForward = rate >= 0.f;

	GET_BUF
	CHECK_BUF
	SETUP_OUT

	uint32 intsPerFrame = 4294967295UL/bufFrames;
	double fintsPerFrame = (double)intsPerFrame;
	double framesPerInt = 1.f / fintsPerFrame;
	uint32 phaseInc = (uint32)(fintsPerFrame * rate);
	uint32 startLoop = (uint32)ZIN0(4) * intsPerFrame;
	uint32 endLoop   = (uint32)ZIN0(5) * intsPerFrame;

	// CHECK_LOOP
	if(startLoop > endLoop){	// ensure startLoop is always less than endLoop
		uint32 temp = endLoop;
		endLoop = startLoop;
		startLoop = temp;
	}

	uint32 loopFrames = endLoop - startLoop;

	if(loopFrames == 0){   // the loop needs to be at least 1 frame
		loopFrames = intsPerFrame;
		if(endLoop >= 4294967295UL - intsPerFrame)
			startLoop = endLoop - intsPerFrame;
		else endLoop = startLoop + intsPerFrame;
	}
	// END CHECK_LOOP

	// CHECK_GATE
	bool gateIsPos = gate > 0.f;
	bool gateWasNeg = unit->m_prevgate <= 0.f;
	if (gateIsPos && gateWasNeg) {			// gate +zc
		unit->mDone = false;
		unit->m_playThrough = false;
		phase = (uint32)ZIN0(3) * intsPerFrame;
	}
	else if (!(gateIsPos || gateWasNeg))	// gate -zc
		unit->m_playThrough = true;
	// END CHECK_GATE

	unit->m_prevgate = gate;

//	if(isPlayingForward){
		if( unit->m_playThrough )
			endLoop = 4294967295UL;

		for (int i=0; i<inNumSamples; ++i) {
			//IWRAP_PHASE_FWD
			if(phase > endLoop){
				if(unit->m_playThrough){
					unit->mDone = true;
					phase = endLoop;
				}
				else{
					phase -= loopFrames;
					if(phase > endLoop){
						phase -= loopFrames * (phase-startLoop)/loopFrames;
					}
				}
			}

			//ILOOP_BODY_1
//			uint32 iphase = phase / intsPerFrame;
//			float* table1 = bufData + iphase * bufChannels;
//			int32 index = 0;
//			for (uint32 i=0; i<numOutputs; ++i) {
//				*++(out[i]) = table1[index++];
//			}
//			phase += phaseInc;

			//ILOOP_BODY_2
			uint32 bufIndex = phase / intsPerFrame;
			float* table1 = bufData + bufIndex * bufChannels;
			float* table2 = table1 + bufChannels;
			if (bufIndex > guardFrame) {
				table2 -= bufSamples;
			}
			int32 index = 0;
			float fracphase = (double)(phase-(bufIndex*intsPerFrame)) * framesPerInt;
			//float fracphase = (double)(phase % intsPerFrame) * framesPerInt;
			for (uint32 i=0; i<numOutputs; ++i) {
				float b = table1[index];
				float c = table2[index];
				*++(out[i]) = b + fracphase * (c - b);
				index++;
			}
			phase += phaseInc;

//		float* table1 = bufData + iphase * bufChannels;
//		float* table2 = table1 + bufChannels;
//		if (iphase > guardFrame) {
//			table2 -= bufSamples;
//		}
//		int32 index = 0;
//		float fracphase = phase - (double)iphase;
//		for (uint32 i=0; i<numOutputs; ++i) {
//			float b = table1[index];
//			float c = table2[index];
//			*++(out[i]) = b + fracphase * (c - b);
//			index++;
//		}
//		phase += rate;
		}

//	}
//
//	else {
//		if( unit->m_playThrough )
//			startLoop = 0;
//
//		IWRAP_PHASE_BWD
//		ILOOP_BODY_1
//		phase += phaseInc;
//	}
	unit->m_phase = phase;
}

PluginLoad(LoopBuf)
{
	ft = inTable;
	DefineSimpleUnit(LoopBuf);
	DefineSimpleUnit(FLoopBuf);
}
