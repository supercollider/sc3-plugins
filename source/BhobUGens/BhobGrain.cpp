/* granular synthesis ugens by bhob rainey.  http://www.bhobrainey.net
TGrains2 and TGrains3 use the framework of SuperCollider's TGrains ugen with some additions to provide
the user with more control over the grain shape

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

static InterfaceTable *ft;

struct Grain2
{
	double phase, rate, attPhase, decPhase;
	double attIncr, decIncr;
	float pan1, pan2;
	int counter, decCount;
	int bufnum;
	int chan;
	int interp;
};

const int kMaxGrains = 64;

struct TGrains2 : public Unit
{
	float mPrevTrig;
	int mNumActive;
	Grain2 mGrains[kMaxGrains];
};

struct TGrains3 : public Unit
{
	float mPrevTrig;
	int mNumActive, mWindowSize;
	float* mWindow;
	Grain2 mGrains[kMaxGrains];
};


// declare unit generator functions
extern "C"
{
	void TGrains2_next(TGrains2 *unit, int inNumSamples);
	void TGrains2_Ctor(TGrains2* unit);

	void TGrains3_next(TGrains3 *unit, int inNumSamples);
	void TGrains3_Ctor(TGrains3* unit);
};
/*
static float cubicinterp(float x, float y0, float y1, float y2, float y3)
{
	float c0 = y1;
	float c1 = 0.5f * (y2 - y0);
	float c2 = y0 - 2.5f * y1 + 2.f * y2 - 0.5f * y3;
	float c3 = 0.5f * (y3 - y0) + 1.5f * (y1 - y2);

	return ((c3 * x + c2) * x + c1) * x + c0;
}
*/
#define GRAIN_BUF \
	SndBuf *buf = bufs + bufnum; \
	float *bufData __attribute__((__unused__)) = buf->data; \
	uint32 bufChannels __attribute__((__unused__)) = buf->channels; \
	uint32 bufSamples __attribute__((__unused__)) = buf->samples; \
	uint32 bufFrames = buf->frames; \
	int guardFrame __attribute__((__unused__)) = bufFrames - 2; \


inline float IN_AT(Unit* unit, int index, int offset)
{
	if (INRATE(index) == calc_FullRate) return IN(index)[offset];
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

#define GRAIN_AMP_2 \
		double amp; \
		double attPhase = grain->attPhase; \
		double decPhase = grain->decPhase; \
		if (counter > grain->decCount) { \
			amp = attPhase; \
			if (attPhase < 1.f) { \
				grain->attPhase += grain->attIncr; \
			} \
		} else { \
			amp = decPhase; \
			grain->decPhase -= grain->decIncr; \
		} \

#define GRAIN2_LOOP_BODY_4 \
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
		ZXP(out1) += outval * pan1; \
		ZXP(out2) += outval * pan2; \
		counter--; \

#define GRAIN2_LOOP_BODY_2 \
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
		ZXP(out1) += outval * pan1; \
		ZXP(out2) += outval * pan2; \
		counter--; \

#define GRAIN2_LOOP_BODY_1 \
		phase = sc_gloop(phase, loopMax); \
		int32 iphase = (int32)phase; \
		float outval = amp * bufData[iphase]; \
		ZXP(out1) += outval * pan1; \
		ZXP(out2) += outval * pan2; \
		counter--; \


void TGrains2_next(TGrains2 *unit, int inNumSamples)
{
	float *trigin = IN(0);
	float prevtrig = unit->mPrevTrig;

	uint32 numOutputs = unit->mNumOutputs;
	ClearUnitOutputs(unit, inNumSamples);
	float *out[16];
	for (uint32 i=0; i<numOutputs; ++i) out[i] = ZOUT(i);

	World *world = unit->mWorld;
	SndBuf *bufs = world->mSndBufs;
	uint32 numBufs = world->mNumSndBufs;

	for (int i=0; i < unit->mNumActive; ) {
		Grain2 *grain = unit->mGrains + i;
		uint32 bufnum = grain->bufnum;

		GRAIN_BUF

		if (bufChannels != 1) {
			 ++i;
			 continue;
		}

		double loopMax = (double)bufFrames;

		float pan1 = grain->pan1;
		float pan2 = grain->pan2;
		double rate = grain->rate;
		double phase = grain->phase;
		int counter = grain->counter;

		uint32 chan1 = grain->chan;
		uint32 chan2 = chan1 + 1;
		if (chan2 >= numOutputs) chan2 = 0;

		float *out1 = out[chan1];
		float *out2 = out[chan2];

		int nsmps = sc_min(grain->counter, inNumSamples);
		if (grain->interp >= 4) {
			for (int j=0; j<nsmps; ++j) {
				GRAIN_AMP_2;
				GRAIN2_LOOP_BODY_4;
				phase += rate;
			}
		} else if (grain->interp >= 2) {
			for (int j=0; j<nsmps; ++j) {
				GRAIN_AMP_2;
				GRAIN2_LOOP_BODY_2;
				phase += rate;
			}
		} else {
			for (int j=0; j<nsmps; ++j) {
				GRAIN_AMP_2;
				GRAIN2_LOOP_BODY_1;
				phase += rate;
			}
		}

		grain->phase = phase;

		grain->counter = counter;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	}

	int trigSamples = INRATE(0) == calc_FullRate ? inNumSamples : 1;

	for (int i=0; i<trigSamples; ++i) {
		float trig = trigin[i];

		if (trig > 0.f && prevtrig <= 0.f) {
			// start a grain
			if (unit->mNumActive+1 >= kMaxGrains) break;
			uint32 bufnum = (uint32)IN_AT(unit, 1, i);
			if (bufnum >= numBufs) continue;
			GRAIN_BUF

			if (bufChannels != 1) continue;

			float bufSampleRate = buf->samplerate;
			float bufRateScale = bufSampleRate * SAMPLEDUR;
			double loopMax = (double)bufFrames;

			Grain2 *grain = unit->mGrains + unit->mNumActive++;
			grain->bufnum = bufnum;

			double dur = IN_AT(unit, 4, i);
			double counter = floor(dur * SAMPLERATE);
			counter = sc_max(4., counter);

			double rate = grain->rate = IN_AT(unit, 2, i) * bufRateScale;
			double centerPhase = IN_AT(unit, 3, i) * bufSampleRate;
			double phase = centerPhase - 0.5 * counter * rate;

			float pan = IN_AT(unit, 5, i);
			float amp = IN_AT(unit, 6, i);
			double att = sc_max((double)IN_AT(unit, 7, i), SAMPLEDUR);
			double dec = IN_AT(unit, 8, i);

			if ((att + dec) > dur) {
				double sum=att + dec;
				double norm=dur/sum;
				att *= norm;
				dec *= norm;
			}

			grain->interp = (int)IN_AT(unit, 9, i);

			float panangle;
			float pan1, pan2;
			if (numOutputs > 1) {
				if (numOutputs > 2) {
					pan = sc_wrap(pan * 0.5f, 0.f, 1.f);
					float cpan = numOutputs * pan + 0.5f;
					float ipan = floor(cpan);
					float panfrac = cpan - ipan;
					panangle = panfrac * pi2_f;
					grain->chan = (int)ipan;
					if (grain->chan >= (int)numOutputs)
						grain->chan -= numOutputs;
				} else {
					grain->chan = 0;
					pan = sc_clip(pan * 0.5f + 0.5f, 0.f, 1.f);
					panangle = pan * pi2_f;
				}
				pan1 = grain->pan1 = cos(panangle);
				pan2 = grain->pan2 = sin(panangle);
			} else {
				grain->chan = 0;
				pan1 = grain->pan1 = 1.;
				pan2 = grain->pan2 = 0.;
			}
			int attCount = sc_max((int)(SAMPLERATE * att), 2);
			int decCount = sc_max((int)(SAMPLERATE * dec), 2);
			grain->attIncr = 1.0/attCount;
			grain->decIncr = 1.0/decCount;
			grain->decCount = decCount;
			grain->attPhase = 0.f;
			grain->decPhase = 1.f;

			uint32 chan1 = grain->chan;
			uint32 chan2 = chan1 + 1;
			if (chan2 >= numOutputs) chan2 = 0;

			float *out1 = out[chan1] + i;
			float *out2 = out[chan2] + i;

			int nsmps = sc_min(grain->counter, inNumSamples - i);
			if (grain->interp >= 4) {
				for (int j=0; j<nsmps; ++j) {
					GRAIN_AMP_2;
					GRAIN2_LOOP_BODY_4;
					phase += rate;
				}
			} else if (grain->interp >= 2) {
				for (int j=0; j<nsmps; ++j) {
					GRAIN_AMP_2;
					GRAIN2_LOOP_BODY_2;
					phase += rate;
				}
			} else {
				for (int j=0; j<nsmps; ++j) {
					GRAIN_AMP_2;
					GRAIN2_LOOP_BODY_1;
					phase += rate;
				}
			}

			grain->phase = phase;

			grain->counter = (int)counter;
			if (grain->counter <= 0) {
				// remove grain
				*grain = unit->mGrains[--unit->mNumActive];
			}
		}
		prevtrig = trig;
	}

	unit->mPrevTrig = prevtrig;
}

void TGrains2_Ctor(TGrains2 *unit)
{
	SETCALC(TGrains2_next);

	unit->mNumActive = 0;
	unit->mPrevTrig = 0.;

	ClearUnitOutputs(unit, 1);
}

#define GRAIN_AMP_3 \
		float amp; \
		int i_attPhase = (int)grain->attPhase; \
		int i_decPhase = (int)grain->decPhase; \
		if (counter > grain->decCount) { \
			amp = window[sc_min(i_attPhase, windowSize)]; \
			if (i_attPhase < windowSize) { \
				grain->attPhase += grain->attIncr; \
			} \
		} else { \
			amp = window[sc_max(i_decPhase, 0)]; \
			grain->decPhase -= grain->decIncr; \
		} \

void TGrains3_next(TGrains3 *unit, int inNumSamples)
{
	float *trigin = IN(0);
	float prevtrig = unit->mPrevTrig;
	float* window = unit->mWindow;
	int windowSize = unit->mWindowSize;

	uint32 numOutputs = unit->mNumOutputs;
	ClearUnitOutputs(unit, inNumSamples);
	float *out[16];
	for (uint32 i=0; i<numOutputs; ++i) out[i] = ZOUT(i);

	World *world = unit->mWorld;
	SndBuf *bufs = world->mSndBufs;
	uint32 numBufs = world->mNumSndBufs;

	for (int i=0; i < unit->mNumActive; ) {
		Grain2 *grain = unit->mGrains + i;
		uint32 bufnum = grain->bufnum;

		GRAIN_BUF

		if (bufChannels != 1) {
			 ++i;
			 continue;
		}

		double loopMax = (double)bufFrames;

		float pan1 = grain->pan1;
		float pan2 = grain->pan2;
		double rate = grain->rate;
		double phase = grain->phase;
		int counter = grain->counter;

		uint32 chan1 = grain->chan;
		uint32 chan2 = chan1 + 1;
		if (chan2 >= numOutputs) chan2 = 0;

		float *out1 = out[chan1];
		float *out2 = out[chan2];
		//printf("B chan %d %d  %08X %08X", chan1, chan2, out1, out2);

		int nsmps = sc_min(grain->counter, inNumSamples);
		if (grain->interp >= 4) {
			for (int j=0; j<nsmps; ++j) {
				GRAIN_AMP_3;
				GRAIN2_LOOP_BODY_4;
				phase += rate;
			}
		} else if (grain->interp >= 2) {
			for (int j=0; j<nsmps; ++j) {
				GRAIN_AMP_3;
				GRAIN2_LOOP_BODY_2;
				phase += rate;
			}
		} else {
			for (int j=0; j<nsmps; ++j) {
				GRAIN_AMP_3;
				GRAIN2_LOOP_BODY_1;
				phase += rate;
			}
		}

		grain->phase = phase;

		grain->counter = counter;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	}

	int trigSamples = INRATE(0) == calc_FullRate ? inNumSamples : 1;

	for (int i=0; i<trigSamples; ++i) {
		float trig = trigin[i];

		if (trig > 0.f && prevtrig <= 0.f) {
			// start a grain
			if (unit->mNumActive+1 >= kMaxGrains) break;
			uint32 bufnum = (uint32)IN_AT(unit, 1, i);
			if (bufnum >= numBufs) continue;
			GRAIN_BUF

			if (bufChannels != 1) continue;

			float bufSampleRate = buf->samplerate;
			float bufRateScale = bufSampleRate * SAMPLEDUR;
			double loopMax = (double)bufFrames;

			Grain2 *grain = unit->mGrains + unit->mNumActive++;
			grain->bufnum = bufnum;

			double dur=IN_AT(unit, 4, i);
			double counter = floor(dur * SAMPLERATE);
			counter = sc_max(4., counter);

			double rate = grain->rate = IN_AT(unit, 2, i) * bufRateScale;
			double centerPhase = IN_AT(unit, 3, i) * bufSampleRate;
			double phase = centerPhase - 0.5 * counter * rate;

			float pan = IN_AT(unit, 5, i);
			float amp = IN_AT(unit, 6, i);
			double att = IN_AT(unit, 7, i);
			double dec = IN_AT(unit, 8, i);

			if ((att + dec) > dur) {
				double sum=att + dec;
				double norm=dur/sum;
				att *= norm;
				dec *= norm;
			}

			grain->interp = (int)IN_AT(unit, 10, i);

			float panangle;
			float pan1, pan2;
			if (numOutputs > 1) {
				if (numOutputs > 2) {
					pan = sc_wrap(pan * 0.5f, 0.f, 1.f);
					float cpan = numOutputs * pan + 0.5f;
					float ipan = floor(cpan);
					float panfrac = cpan - ipan;
					panangle = panfrac * pi2_f;
					grain->chan = (int)ipan;
					if (grain->chan >= (int)numOutputs)
						grain->chan -= numOutputs;
				} else {
					grain->chan = 0;
					pan = sc_clip(pan * 0.5f + 0.5f, 0.f, 1.f);
					panangle = pan * pi2_f;
				}
				pan1 = grain->pan1 = cos(panangle);
				pan2 = grain->pan2 = sin(panangle);
			} else {
				grain->chan = 0;
				pan1 = grain->pan1 = 1.;
				pan2 = grain->pan2 = 0.;
			}
			int attCount = sc_max((int)(SAMPLERATE * att), 2);
			int decCount = sc_max((int)(SAMPLERATE * dec), 2);
			grain->attIncr = (float)windowSize/attCount;
			grain->decIncr = (float)windowSize/decCount;
			grain->decCount = decCount;
			grain->attPhase = 0.f;
			grain->decPhase = (float)windowSize;

			uint32 chan1 = grain->chan;
			uint32 chan2 = chan1 + 1;
			if (chan2 >= numOutputs) chan2 = 0;

			float *out1 = out[chan1] + i;
			float *out2 = out[chan2] + i;

			int nsmps = sc_min(grain->counter, inNumSamples - i);
			if (grain->interp >= 4) {
				for (int j=0; j<nsmps; ++j) {
					GRAIN_AMP_3;
					GRAIN2_LOOP_BODY_4;
					phase += rate;
				}
			} else if (grain->interp >= 2) {
				for (int j=0; j<nsmps; ++j) {
					GRAIN_AMP_3;
					GRAIN2_LOOP_BODY_2;
					phase += rate;
				}
			} else {
				for (int j=0; j<nsmps; ++j) {
					GRAIN_AMP_3;
					GRAIN2_LOOP_BODY_1;
					phase += rate;
				}
			}

			grain->phase = phase;

			grain->counter = (int)counter;
			if (grain->counter <= 0) {
				// remove grain
				*grain = unit->mGrains[--unit->mNumActive];
			}
		}
		prevtrig = trig;
	}

	unit->mPrevTrig = prevtrig;
}

void TGrains3_Ctor(TGrains3 *unit)
{
	SETCALC(TGrains3_next);

	unit->mNumActive = 0;
	unit->mPrevTrig = 0.;
	SndBuf* window=unit->mWorld->mSndBufs + (int)IN0(9);
	unit->mWindow=window->data;
	unit->mWindowSize=window->samples - 1;

	ClearUnitOutputs(unit, 1);
}

PluginLoad(BhobGrains)
{
	ft = inTable;

	DefineSimpleUnit(TGrains2);
	DefineSimpleUnit(TGrains3);
}

////////////////////////////////////////////////////////////////////
