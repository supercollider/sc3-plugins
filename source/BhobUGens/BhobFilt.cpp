/*
filter plugins by bhob rainey.  http://www.bhobrainey.net
MoogLadder is based on work by Victor Lazzarini and Antti Huovilainen.
RLPFD is based on a csound macro by Josep M Comajuncos
Streson is a near copy of code by Victor Lazzarini
NLFilt is an implementation of a formula by Dobson and Fitch

SuperCollider real time aud synthesis system
    Copyright (c) 2002 James McCartney. All rights reserved.
	http://www.audsynth.com

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

struct MoogLadder : public Unit
{
	float m_fco, m_k2v;
	float m_res;
	float m_xnm1, m_y1nm1, m_y2nm1, m_y3nm1, m_y4nm1;
	float m_y1n, m_y2n, m_y3n, m_y4n, m_y1n1;
};

struct RLPFD : public Unit
{
	float m_fco, m_res, m_ap, m_scl;
	float m_x1, m_y1, m_y2, m_y3;
	float m_x1nm1, m_y1nm1, m_y2nm1;
};

struct MultiFilt : public Unit
{
	float m_hp, m_lp, m_bp, m_halfsamp;
	float m_cf, m_f, m_rq;
};

struct FilterDelayUnit : public Unit
{
	float *m_dlybuf;

	float m_dsamp, m_fdelaylen;
	float m_delaytime, m_maxdelaytime;
	long m_iwrphase, m_idelaylen, m_mask;
	long m_numoutput;
};

struct Streson : public FilterDelayUnit
{
	float m_lp, m_ap, m_a;
};

struct NLFiltN : public FilterDelayUnit
{
	float m_a, m_b, m_d, m_c, m_l;
};

struct NLFiltL : public FilterDelayUnit
{
	float m_a, m_b, m_d, m_c, m_l;
};

struct NLFiltC : public FilterDelayUnit
{
	float m_a, m_b, m_d, m_c, m_l;
};

struct NestedAllpassN : public Unit
{
	float m_dt1, m_dt2;
	float *m_buf1, *m_buf2;
	long m_phase, m_maxdel1, m_maxdel2, m_maxdel, m_numoutput, m_dsamp1, m_dsamp2;
};

struct NestedAllpassL : public Unit
{
	float m_dt1, m_dt2, m_dsamp1, m_dsamp2;
	float *m_buf1, *m_buf2;
	long m_phase, m_maxdel1, m_maxdel2, m_maxdel, m_numoutput;
};

struct NestedAllpassC : public Unit
{
	float m_dt1, m_dt2, m_dsamp1, m_dsamp2;
	float *m_buf1, *m_buf2;
	long m_phase, m_maxdel1, m_maxdel2, m_maxdel, m_numoutput;
};

struct DoubleNestedAllpassN : public Unit
{
	float m_dt1, m_dt2, m_dt3;
	float *m_buf1, *m_buf2, *m_buf3;
	long m_phase, m_maxdel1, m_maxdel2, m_maxdel3, m_dsamp1, m_dsamp2, m_dsamp3, m_maxdel, m_numoutput;
};

struct DoubleNestedAllpassL : public Unit
{
	float m_dt1, m_dt2, m_dt3, m_dsamp1, m_dsamp2, m_dsamp3;
	float *m_buf1, *m_buf2, *m_buf3;
	long m_phase, m_maxdel1, m_maxdel2, m_maxdel3, m_maxdel, m_numoutput;
};

struct DoubleNestedAllpassC : public Unit
{
	float m_dt1, m_dt2, m_dt3, m_dsamp1, m_dsamp2, m_dsamp3;
	float *m_buf1, *m_buf2, *m_buf3;
	long m_phase, m_maxdel1, m_maxdel2, m_maxdel3, m_maxdel, m_numoutput;
};

// declare unit generator functions
extern "C"
{
	void load(InterfaceTable *inTable);

	void MoogLadder_Ctor(MoogLadder* unit);
	void MoogLadder_next_kk(MoogLadder *unit, int inNumSamples);
	void MoogLadder_next_ka(MoogLadder *unit, int inNumSamples);
	void MoogLadder_next_ak(MoogLadder *unit, int inNumSamples);
	void MoogLadder_next_aa(MoogLadder *unit, int inNumSamples);

	void RLPFD_Ctor(RLPFD* unit);
	void RLPFD_next_kk(RLPFD *unit, int inNumSamples);
	void RLPFD_next_ka(RLPFD *unit, int inNumSamples);
	void RLPFD_next_ak(RLPFD *unit, int inNumSamples);
	void RLPFD_next_aa(RLPFD *unit, int inNumSamples);

	void MultiFilt_Ctor(MultiFilt* unit);
	void MultiFilt_next_kk(MultiFilt *unit, int inNumSamples);
	void MultiFilt_next_ak(MultiFilt *unit, int inNumSamples);
	void MultiFilt_next_ka(MultiFilt *unit, int inNumSamples);
	void MultiFilt_next_aa(MultiFilt *unit, int inNumSamples);

	void FiltDelay_Reset(FilterDelayUnit *unit);
	void FilterDelay_Dtor(FilterDelayUnit *unit);
	void FilterDelay_AllocDelayLine(FilterDelayUnit *unit);

	void Streson_Ctor(Streson* unit);
	void Streson_next_a(Streson *unit, int inNumSamples);
	void Streson_next_k(Streson *unit, int inNumSamples);
	void Streson_next_a_z(Streson *unit, int inNumSamples);
	void Streson_next_k_z(Streson *unit, int inNumSamples);

	void NLFiltN_Ctor(NLFiltN* unit);
	void NLFiltN_next(NLFiltN *unit, int inNumSamples);
	void NLFiltN_next_z(NLFiltN *unit, int inNumSamples);

	void NLFiltL_Ctor(NLFiltL* unit);
	void NLFiltL_next(NLFiltL *unit, int inNumSamples);
	void NLFiltL_next_z(NLFiltL *unit, int inNumSamples);

	void NLFiltC_Ctor(NLFiltC* unit);
	void NLFiltC_next(NLFiltC *unit, int inNumSamples);
	void NLFiltC_next_z(NLFiltC *unit, int inNumSamples);

	void NestedAllpassN_Ctor(NestedAllpassN* unit);
	void NestedAllpassN_Dtor(NestedAllpassN *unit);
	void NestedAllpassN_next(NestedAllpassN *unit, int inNumSamples);
	void NestedAllpassN_next_z(NestedAllpassN *unit, int inNumSamples);

	void NestedAllpassL_Ctor(NestedAllpassL* unit);
	void NestedAllpassL_Dtor(NestedAllpassL *unit);
	void NestedAllpassL_next(NestedAllpassL *unit, int inNumSamples);
	void NestedAllpassL_next_z(NestedAllpassL *unit, int inNumSamples);

	void NestedAllpassC_Ctor(NestedAllpassC* unit);
	void NestedAllpassC_Dtor(NestedAllpassC *unit);
	void NestedAllpassC_next(NestedAllpassC *unit, int inNumSamples);
	void NestedAllpassC_next_z(NestedAllpassC *unit, int inNumSamples);

	void DoubleNestedAllpassN_Ctor(DoubleNestedAllpassN* unit);
	void DoubleNestedAllpassN_Dtor(DoubleNestedAllpassN *unit);
	void DoubleNestedAllpassN_next(DoubleNestedAllpassN *unit, int inNumSamples);
	void DoubleNestedAllpassN_next_z(DoubleNestedAllpassN *unit, int inNumSamples);

	void DoubleNestedAllpassL_Ctor(DoubleNestedAllpassL* unit);
	void DoubleNestedAllpassL_Dtor(DoubleNestedAllpassL *unit);
	void DoubleNestedAllpassL_next(DoubleNestedAllpassL *unit, int inNumSamples);
	void DoubleNestedAllpassL_next_z(DoubleNestedAllpassL *unit, int inNumSamples);

	void DoubleNestedAllpassC_Ctor(DoubleNestedAllpassC* unit);
	void DoubleNestedAllpassC_Dtor(DoubleNestedAllpassC *unit);
	void DoubleNestedAllpassC_next(DoubleNestedAllpassC *unit, int inNumSamples);
	void DoubleNestedAllpassC_next_z(DoubleNestedAllpassC *unit, int inNumSamples);
};

#define SET2BUF \
	long bufsize1=(long)ceil(ZIN0(1) * unit->mRate->mSampleRate + 1.f); \
	long bufsize2=(long)ceil(ZIN0(4) * unit->mRate->mSampleRate + 1.f); \
	long maxdelay1=bufsize1 + BUFLENGTH; \
	maxdelay1 = NEXTPOWEROFTWO(maxdelay1); \
	long maxdelay2=bufsize2 + BUFLENGTH; \
	maxdelay2 = NEXTPOWEROFTWO(maxdelay2); \
	unit->m_buf1=0; \
	unit->m_buf2=0; \
	RTFree(unit->mWorld, unit->m_buf1); \
	unit->m_buf1=(float*)RTAlloc(unit->mWorld, maxdelay1 * sizeof(float)); \
	RTFree(unit->mWorld, unit->m_buf2); \
	unit->m_buf2=(float*)RTAlloc(unit->mWorld, maxdelay2 * sizeof(float)); \
	unit->m_maxdel1=maxdelay1-1; \
	unit->m_maxdel2=maxdelay2-1; \
	unit->m_maxdel=sc_max(unit->m_maxdel1, unit->m_maxdel2); \
	unit->m_numoutput=0; \
	unit->m_dt1=ZIN0(2); \
	unit->m_dt2=ZIN0(5); \

#define SET3BUF \
	long bufsize1=(long)ceil(ZIN0(1) * unit->mRate->mSampleRate + 1.f); \
	long bufsize2=(long)ceil(ZIN0(4) * unit->mRate->mSampleRate + 1.f); \
	long bufsize3=(long)ceil(ZIN0(7) * unit->mRate->mSampleRate + 1.f); \
	long maxdelay1=bufsize1 + BUFLENGTH; \
	maxdelay1 = NEXTPOWEROFTWO(maxdelay1); \
	long maxdelay2=bufsize2 + BUFLENGTH; \
	maxdelay2 = NEXTPOWEROFTWO(maxdelay2); \
	long maxdelay3=bufsize3 + BUFLENGTH; \
	maxdelay3 = NEXTPOWEROFTWO(maxdelay3); \
	unit->m_buf1=0; \
	unit->m_buf2=0; \
	unit->m_buf3=0; \
	RTFree(unit->mWorld, unit->m_buf1); \
	unit->m_buf1=(float*)RTAlloc(unit->mWorld, maxdelay1 * sizeof(float)); \
	RTFree(unit->mWorld, unit->m_buf2); \
	unit->m_buf2=(float*)RTAlloc(unit->mWorld, maxdelay2 * sizeof(float)); \
	RTFree(unit->mWorld, unit->m_buf3); \
	unit->m_buf3=(float*)RTAlloc(unit->mWorld, maxdelay3 * sizeof(float)); \
	unit->m_maxdel1=maxdelay1-1; \
	unit->m_maxdel2=maxdelay2-1; \
	unit->m_maxdel3=maxdelay3-1; \
	unit->m_maxdel=sc_max(unit->m_maxdel1, unit->m_maxdel2); \
	unit->m_maxdel=sc_max(unit->m_maxdel, unit->m_maxdel3); \
	unit->m_numoutput=0; \
	unit->m_dt1=ZIN0(2); \
	unit->m_dt2=ZIN0(5); \
	unit->m_dt3=ZIN0(8); \
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
float CalcDelay(FilterDelayUnit *unit, float delaytime);
float CalcDelay(FilterDelayUnit *unit, float delaytime)
{
	float next_dsamp = delaytime * SAMPLERATE;
	return sc_clip(next_dsamp, 1.f, unit->m_fdelaylen);
}

void FilterDelay_AllocDelayLine(FilterDelayUnit *unit)
{
	long delaybufsize = (long)ceil(unit->m_maxdelaytime * SAMPLERATE + 1.f);
	delaybufsize = delaybufsize + BUFLENGTH;
	delaybufsize = NEXTPOWEROFTWO(delaybufsize);
	unit->m_fdelaylen = unit->m_idelaylen = delaybufsize;

	RTFree(unit->mWorld, unit->m_dlybuf);
	int size = delaybufsize * sizeof(float);
	unit->m_dlybuf = (float*)RTAlloc(unit->mWorld, size);
	unit->m_mask = delaybufsize - 1;
}

void FilterDelay_Reset(FilterDelayUnit *unit)
{
	unit->m_dlybuf = 0;

	FilterDelay_AllocDelayLine(unit);

	unit->m_dsamp = CalcDelay(unit, unit->m_delaytime);

	unit->m_numoutput = 0;
	unit->m_iwrphase = 0;
}


void FilterDelay_Dtor(FilterDelayUnit *unit)
{
	RTFree(unit->mWorld, unit->m_dlybuf);
}

float Mirroring (float lower, float upper, float in);
float Mirroring (float lower, float upper, float in)
{
	if(in > upper || in < lower) {
		float infrac=fabs(in-(long)in);
		if (in<lower) in=lower + infrac;
		else in=upper - infrac;
	}
	return in;
}

void NLFiltN_Ctor(NLFiltN *unit)
{
	unit->m_maxdelaytime=0.05;
	FilterDelay_Reset(unit);
	SETCALC(NLFiltN_next_z);
	unit->m_a=ZIN0(1);
	unit->m_b=ZIN0(2);
	unit->m_d=ZIN0(3);
	unit->m_c=ZIN0(4);
	unit->m_l=ZIN0(5);
	ZOUT0(0) = 0.f;
}

void NLFiltN_next(NLFiltN *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float next_a = ZIN0(1);
	float next_b = ZIN0(2);
	float next_d = ZIN0(3);
	float next_c = ZIN0(4);
	float next_l = ZIN0(5);
	float a = unit->m_a;
	float b = unit->m_b;
	float d = unit->m_d;
	float c = unit->m_c;
	float l = unit->m_l;
	float *dlybuf = unit->m_dlybuf;
	long iwrphase = unit->m_iwrphase;
	long mask = unit->m_mask;

	if (next_a != a || next_b != b || next_d != d || next_c != c || next_l != l) {
		float lslope=CALCSLOPE(next_l, l);
		float aslope=CALCSLOPE(next_a, a);
		float bslope=CALCSLOPE(next_b, b);
		float dslope=CALCSLOPE(next_d, d);
		float cslope=CALCSLOPE(next_c, c);
		LOOP(inNumSamples,
			a += aslope;
			b += bslope;
			d += dslope;
			c += cslope;
			l += lslope;
			float delay1 = dlybuf[(iwrphase - 1) & mask];
			float delay2 = dlybuf[(iwrphase - 2) & mask];
			float delayL = dlybuf[(iwrphase - (long)l) & mask];

			float output = ZXP(in) + a*delay1 + b*delay2 + d*delayL*delayL - c;
			output = output - (output * output * output * 0.166666667);
			output = Mirroring(-1.0, 1.0, output);
			ZXP(out) = dlybuf[iwrphase & mask] = output;
			iwrphase++;
		);
	} else {
		LOOP(inNumSamples,
			float delay1 = dlybuf[(iwrphase - 1) & mask];
			float delay2 = dlybuf[(iwrphase - 2) & mask];
			float delayL = dlybuf[(iwrphase - (long)l) & mask];

			float output = ZXP(in) + a*delay1 + b*delay2 + d*delayL*delayL - c;
			output = output - (output * output * output * 0.166666667);
			output = Mirroring(-1.0, 1.0, output);
			ZXP(out) = dlybuf[iwrphase & mask] = output;
			iwrphase++;
		);
	}

	unit->m_iwrphase=iwrphase;
	unit->m_a = a;
	unit->m_b = b;
	unit->m_d = d;
	unit->m_c = c;
	unit->m_l = l;
}

void NLFiltN_next_z(NLFiltN *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float next_a = ZIN0(1);
	float next_b = ZIN0(2);
	float next_d = ZIN0(3);
	float next_c = ZIN0(4);
	float next_l = ZIN0(5);
	float a = unit->m_a;
	float b = unit->m_b;
	float d = unit->m_d;
	float c = unit->m_c;
	float l = unit->m_l;
	float *dlybuf = unit->m_dlybuf;
	long iwrphase = unit->m_iwrphase;
	long mask = unit->m_mask;

	if (next_a != a || next_b != b || next_d != d || next_c != c || next_l != l) {
		float lslope=CALCSLOPE(next_l, l);
		float aslope=CALCSLOPE(next_a, a);
		float bslope=CALCSLOPE(next_b, b);
		float dslope=CALCSLOPE(next_d, d);
		float cslope=CALCSLOPE(next_c, c);
		LOOP(inNumSamples,
			a += aslope;
			b += bslope;
			d += dslope;
			c += cslope;
			l += lslope;
			long irphase = iwrphase - (long)l;
			float delay1 = dlybuf[(iwrphase - 1) & mask];
			float delay2 = dlybuf[(iwrphase - 2) & mask];
			float delayL = dlybuf[irphase & mask];

			float input = ZXP(in);
			float output = input + a*delay1 + b*delay2 + d*delayL*delayL - c;
			output = output - (output * output * output * 0.166666667);
			output = Mirroring(-1.0, 1.0, output);
			if (irphase < 0) {
				dlybuf[iwrphase & mask] = input;
				ZXP(out)=0.f;
			} else {
				ZXP(out) = dlybuf[iwrphase & mask] = output;
			}
			iwrphase++;
		);
	} else {
		LOOP(inNumSamples,
			long irphase = iwrphase - (long)l;
			float delay1 = dlybuf[(iwrphase - 1) & mask];
			float delay2 = dlybuf[(iwrphase - 2) & mask];
			float delayL = dlybuf[irphase & mask];

			float input = ZXP(in);
			float output = input + a*delay1 + b*delay2 + d*delayL*delayL - c;
			output = output - (output * output * output * 0.166666667);
			output = Mirroring(-1.0, 1.0, output);
			if (irphase < 0) {
				dlybuf[iwrphase & mask] = input;
				ZXP(out)=0.f;
			} else {
				ZXP(out) = dlybuf[iwrphase & mask] = output;
			}
			iwrphase++;
		);
	}

	unit->m_iwrphase=iwrphase;
	unit->m_a = a;
	unit->m_b = b;
	unit->m_d = d;
	unit->m_c = c;
	unit->m_l = l;

	unit->m_numoutput += inNumSamples;
	if (unit->m_numoutput >= unit->m_idelaylen) {
		SETCALC(NLFiltN_next);
	}
}


void NLFiltL_Ctor(NLFiltL *unit)
{
	unit->m_maxdelaytime=0.05;
	FilterDelay_Reset(unit);
	SETCALC(NLFiltL_next_z);
	unit->m_a=ZIN0(1);
	unit->m_b=ZIN0(2);
	unit->m_d=ZIN0(3);
	unit->m_c=ZIN0(4);
	unit->m_l=ZIN0(5);
	ZOUT0(0) = 0.f;
}

void NLFiltL_next(NLFiltL *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float next_a = ZIN0(1);
	float next_b = ZIN0(2);
	float next_d = ZIN0(3);
	float next_c = ZIN0(4);
	float next_l = ZIN0(5);
	float a = unit->m_a;
	float b = unit->m_b;
	float d = unit->m_d;
	float c = unit->m_c;
	float l = unit->m_l;
	float *dlybuf = unit->m_dlybuf;
	long iwrphase = unit->m_iwrphase;
	long mask = unit->m_mask;

	if (next_a != a || next_b != b || next_d != d || next_c != c || next_l != l) {
		float lslope=CALCSLOPE(next_l, l);
		float aslope=CALCSLOPE(next_a, a);
		float bslope=CALCSLOPE(next_b, b);
		float dslope=CALCSLOPE(next_d, d);
		float cslope=CALCSLOPE(next_c, c);
		LOOP(inNumSamples,
			l += lslope;
			a += aslope;
			b += bslope;
			d += dslope;
			c += cslope;
			float delay1 = dlybuf[(iwrphase - 1) & mask];
			float delay2 = dlybuf[(iwrphase - 2) & mask];
			long l1 = (long)l;
			float frac = l-l1;
			float delayL1 = dlybuf[(iwrphase - l1) & mask];
			float delayL2 = dlybuf[(iwrphase - l1 - 1) & mask];
			float delayL = lininterp(frac, delayL1, delayL2);

			float output = ZXP(in) + a*delay1 + b*delay2 + d*delayL*delayL - c;
			output = output - (output * output * output * 0.166666667);
			output = Mirroring(-1.0, 1.0, output);
			ZXP(out) = dlybuf[iwrphase & mask] = output;
			iwrphase++;
		)
	} else {
		LOOP(inNumSamples,
			float delay1 = dlybuf[(iwrphase - 1) & mask];
			float delay2 = dlybuf[(iwrphase - 2) & mask];
			float delayL = dlybuf[(iwrphase - (long)l) & mask];

			float output = ZXP(in) + a*delay1 + b*delay2 + d*delayL*delayL - c;
			output = output - (output * output * output * 0.166666667);
			output = Mirroring(-1.0, 1.0, output);
			ZXP(out) = dlybuf[iwrphase & mask] = output;
			iwrphase++;
		)
	}

	unit->m_iwrphase=iwrphase;
	unit->m_a = a;
	unit->m_b = b;
	unit->m_d = d;
	unit->m_c = c;
	unit->m_l = l;
}

void NLFiltL_next_z(NLFiltL *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float next_a = ZIN0(1);
	float next_b = ZIN0(2);
	float next_d = ZIN0(3);
	float next_c = ZIN0(4);
	float next_l = ZIN0(5);
	float a = unit->m_a;
	float b = unit->m_b;
	float d = unit->m_d;
	float c = unit->m_c;
	float l = unit->m_l;
	float *dlybuf = unit->m_dlybuf;
	long iwrphase = unit->m_iwrphase;
	long mask = unit->m_mask;

	if (next_a != a || next_b != b || next_d != d || next_c != c || next_l != l) {
		float lslope=CALCSLOPE(next_l, l);
		float aslope=CALCSLOPE(next_a, a);
		float bslope=CALCSLOPE(next_b, b);
		float dslope=CALCSLOPE(next_d, d);
		float cslope=CALCSLOPE(next_c, c);
		LOOP(inNumSamples,
			l += lslope;
			a += aslope;
			b += bslope;
			d += dslope;
			c += cslope;
			float delay1 = dlybuf[(iwrphase - 1) & mask];
			float delay2 = dlybuf[(iwrphase - 2) & mask];
			long l1 = (long)l;
			float frac = l-l1;
			long irphase = iwrphase-l1;
			float delayL1 = dlybuf[irphase & mask];
			float delayL2 = dlybuf[(irphase - 1) & mask];
			float delayL = lininterp(frac, delayL1, delayL2);
			float input = ZXP(in);
			float output = input + a*delay1 + b*delay2 + d*delayL*delayL - c;
			output = output - (output * output * output * 0.166666667);
			output = Mirroring(-1.0, 1.0, output);
			if ((irphase-1) < 0) {
				dlybuf[iwrphase & mask] = input;
				ZXP(out)=0.f;
			} else {
				ZXP(out) = dlybuf[iwrphase & mask] = output;
			}
			iwrphase++;
		)
	} else {
		LOOP(inNumSamples,
			float delay1 = dlybuf[(iwrphase - 1) & mask];
			float delay2 = dlybuf[(iwrphase - 2) & mask];
			long l1=(long)l;
			float frac=l-l1;
			long irphase = iwrphase-l1;
			float delayL1 = dlybuf[irphase & mask];
			float delayL2 = dlybuf[(irphase - 1) & mask];
			float delayL = lininterp(frac, delayL1, delayL2);
			float input = ZXP(in);
			float output = input + a*delay1 + b*delay2 + d*delayL*delayL - c;
			output = output - (output * output * output * 0.166666667);
			output = Mirroring(-1.0, 1.0, output);
			if ((irphase-1) < 0) {
				dlybuf[iwrphase & mask] = input;
				ZXP(out)=0.f;
			} else {
				ZXP(out) = dlybuf[iwrphase & mask] = output;
			}
			iwrphase++;
		)
	}

	unit->m_iwrphase=iwrphase;
	unit->m_a = a;
	unit->m_b = b;
	unit->m_d = d;
	unit->m_c = c;
	unit->m_l = l;

	unit->m_numoutput += inNumSamples;
	if (unit->m_numoutput >= unit->m_idelaylen) {
		SETCALC(NLFiltL_next);
	}
}

void NLFiltC_Ctor(NLFiltC *unit)
{
	unit->m_maxdelaytime=0.05;
	FilterDelay_Reset(unit);
	SETCALC(NLFiltC_next_z);
	unit->m_a=ZIN0(1);
	unit->m_b=ZIN0(2);
	unit->m_d=ZIN0(3);
	unit->m_c=ZIN0(4);
	unit->m_l=ZIN0(5);
	ZOUT0(0) = 0.f;
}

void NLFiltC_next(NLFiltC *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float next_a = ZIN0(1);
	float next_b = ZIN0(2);
	float next_d = ZIN0(3);
	float next_c = ZIN0(4);
	float next_l = ZIN0(5);
	float a = unit->m_a;
	float b = unit->m_b;
	float d = unit->m_d;
	float c = unit->m_c;
	float l = unit->m_l;
	float *dlybuf = unit->m_dlybuf;
	long iwrphase = unit->m_iwrphase;
	long mask = unit->m_mask;

	if (next_a != a || next_b != b || next_d != d || next_c != c || next_l != l) {
		float lslope=CALCSLOPE(next_l, l);
		float aslope=CALCSLOPE(next_a, a);
		float bslope=CALCSLOPE(next_b, b);
		float dslope=CALCSLOPE(next_d, d);
		float cslope=CALCSLOPE(next_c, c);
		LOOP(inNumSamples,
			l += lslope;
			a += aslope;
			b += bslope;
			d += dslope;
			c += cslope;
			float delay1 = dlybuf[(iwrphase - 1) & mask];
			float delay2 = dlybuf[(iwrphase - 2) & mask];
			long l1 = (long)l;
			float frac = l-l1;
			long irdphase1 = iwrphase - l1;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;
			float d0 = dlybuf[irdphase0 & mask];
			float d1 = dlybuf[irdphase1 & mask];
			float d2 = dlybuf[irdphase2 & mask];
			float d3 = dlybuf[irdphase3 & mask];
			float delayL = cubicinterp(frac, d0, d1, d2, d3);

			float output = ZXP(in) + a*delay1 + b*delay2 + d*delayL*delayL - c;
			output = output - (output * output * output * 0.166666667);
			output = Mirroring(-1.0, 1.0, output);
			ZXP(out) = dlybuf[iwrphase & mask] = output;
			iwrphase++;
		)
	} else {
		LOOP(inNumSamples,
			float delay1 = dlybuf[(iwrphase - 1) & mask];
			float delay2 = dlybuf[(iwrphase - 2) & mask];
			long l1 = (long)l;
			float frac = l-l1;
			long irdphase1 = iwrphase - l1;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;
			float d0 = dlybuf[irdphase0 & mask];
			float d1 = dlybuf[irdphase1 & mask];
			float d2 = dlybuf[irdphase2 & mask];
			float d3 = dlybuf[irdphase3 & mask];
			float delayL = cubicinterp(frac, d0, d1, d2, d3);

			float output = ZXP(in) + a*delay1 + b*delay2 + d*delayL*delayL - c;
			output = output - (output * output * output * 0.166666667);
			output = Mirroring(-1.0, 1.0, output);
			ZXP(out) = dlybuf[iwrphase & mask] = output;
			iwrphase++;
		)
	}

	unit->m_iwrphase=iwrphase;
	unit->m_a = a;
	unit->m_b = b;
	unit->m_d = d;
	unit->m_c = c;
	unit->m_l = l;
}

void NLFiltC_next_z(NLFiltC *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float next_a = ZIN0(1);
	float next_b = ZIN0(2);
	float next_d = ZIN0(3);
	float next_c = ZIN0(4);
	float next_l = ZIN0(5);
	float a = unit->m_a;
	float b = unit->m_b;
	float d = unit->m_d;
	float c = unit->m_c;
	float l = unit->m_l;
	float *dlybuf = unit->m_dlybuf;
	long iwrphase = unit->m_iwrphase;
	long mask = unit->m_mask;

	if (next_a != a || next_b != b || next_d != d || next_c != c || next_l != l) {
		float lslope=CALCSLOPE(next_l, l);
		float aslope=CALCSLOPE(next_a, a);
		float bslope=CALCSLOPE(next_b, b);
		float dslope=CALCSLOPE(next_d, d);
		float cslope=CALCSLOPE(next_c, c);
		LOOP(inNumSamples,
			l += lslope;
			a += aslope;
			b += bslope;
			d += dslope;
			c += cslope;
			float delay1 = dlybuf[(iwrphase - 1) & mask];
			float delay2 = dlybuf[(iwrphase - 2) & mask];
			long l1 = (long)l;
			float frac = l-l1;
			long irdphase1 = iwrphase - l1;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;
			float d0 = dlybuf[irdphase0 & mask];
			float d1 = dlybuf[irdphase1 & mask];
			float d2 = dlybuf[irdphase2 & mask];
			float d3 = dlybuf[irdphase3 & mask];
			float delayL = cubicinterp(frac, d0, d1, d2, d3);
			float input = ZXP(in);
			float output = input + a*delay1 + b*delay2 + d*delayL*delayL - c;
			output = output - (output * output * output * 0.166666667);
			output = Mirroring(-1.0, 1.0, output);
			if (irdphase3 < 0) {
				dlybuf[iwrphase & mask] = input;
				ZXP(out)=0.f;
			} else {
				ZXP(out) = dlybuf[iwrphase & mask] = output;
			}
			iwrphase++;
		)
	} else {
		LOOP(inNumSamples,
				float delay1 = dlybuf[(iwrphase - 1) & mask];
			float delay2 = dlybuf[(iwrphase - 2) & mask];
			long l1 = (long)l;
			float frac = l-l1;
			long irdphase1 = iwrphase - l1;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;
			float d0 = dlybuf[irdphase0 & mask];
			float d1 = dlybuf[irdphase1 & mask];
			float d2 = dlybuf[irdphase2 & mask];
			float d3 = dlybuf[irdphase3 & mask];
			float delayL = cubicinterp(frac, d0, d1, d2, d3);
			float input = ZXP(in);
			float output = input + a*delay1 + b*delay2 + d*delayL*delayL - c;
			output = output - (output * output * output * 0.166666667);
			output = Mirroring(-1.0, 1.0, output);
			if (irdphase3 < 0) {
				dlybuf[iwrphase & mask] = input;
				ZXP(out)=0.f;
			} else {
				ZXP(out) = dlybuf[iwrphase & mask] = output;
			}
			iwrphase++;
		)
	}

	unit->m_iwrphase=iwrphase;
	unit->m_a = a;
	unit->m_b = b;
	unit->m_d = d;
	unit->m_c = c;
	unit->m_l = l;

	unit->m_numoutput += inNumSamples;
	if (unit->m_numoutput > unit->m_idelaylen) {
		SETCALC(NLFiltC_next);
	}
}

void Streson_Ctor(Streson *unit)
{
	unit->m_maxdelaytime=0.05;
	FilterDelay_Reset(unit);
	float delaytime = ZIN0(1);
	float tdelay = delaytime * SAMPLERATE;
	unit->m_dsamp = (long)(tdelay - 0.5);
	float fracdelay = tdelay - (unit->m_dsamp + 0.5);
	unit->m_a = (1-fracdelay)/(1+fracdelay);
	unit->m_lp=unit->m_ap=0.f;
	unit->m_delaytime = delaytime;
	if (INRATE(1) == calc_FullRate) SETCALC(Streson_next_a_z);
	else SETCALC(Streson_next_k_z);
	ZOUT0(0) = 0.f;
}

void Streson_next_k(Streson *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float delaytime = unit->m_delaytime;
	float next_delaytime = ZIN0(1);
	float dsamp = unit->m_dsamp;
	float a = unit->m_a;
	float *dlybuf = unit->m_dlybuf;
	long iwrphase = unit->m_iwrphase;
	long mask = unit->m_mask;
	float ap = unit->m_ap;
	float lp = unit->m_lp;

	if (next_delaytime != delaytime) {
		float tdelay = next_delaytime * SAMPLERATE;
		float dsamp = (long)(tdelay - 0.5);
		float fracdelay = tdelay - (dsamp + 0.5);
		float a = (1-fracdelay)/(1+fracdelay);
		LOOP(inNumSamples,
			long irdphase = iwrphase - (long)dsamp;
			float input = ZXP(in) + dlybuf[irdphase & mask];
			float s = lp * 0.5 + input * 0.5;
			lp = input;
			float sample = ap + s*a;
			ZXP(out) = sample;
			ap = s - (sample * a);
			dlybuf[iwrphase & mask] = sample * ZIN0(2);
			++iwrphase;
		);
	} else {
		LOOP(inNumSamples,
			long irdphase = iwrphase - (long)dsamp;
			float input = ZXP(in) + dlybuf[irdphase & mask];
			float s = lp * 0.5 + input * 0.5;
			lp = input;
			float sample = ap + s*a;
			ZXP(out) = sample;
			ap = s - (sample * a);
			dlybuf[iwrphase & mask] = sample * ZIN0(2);
			++iwrphase;
		);
	}

	unit->m_iwrphase = iwrphase;
	unit->m_ap = zapgremlins(ap);
	unit->m_lp = zapgremlins(lp);
	unit->m_dsamp = dsamp;
	unit->m_a = a;
}

void Streson_next_k_z(Streson *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float delaytime = unit->m_delaytime;
	float next_delaytime = ZIN0(1);
	float dsamp = unit->m_dsamp;
	float a = unit->m_a;
	float *dlybuf = unit->m_dlybuf;
	long iwrphase = unit->m_iwrphase;
	long mask = unit->m_mask;
	float ap = unit->m_ap;
	float lp = unit->m_lp;

	if (next_delaytime != delaytime) {
		float tdelay = next_delaytime * SAMPLERATE;
		float dsamp = (long)(tdelay - 0.5);
		float fracdelay = tdelay - (dsamp + 0.5);
		float a = (1-fracdelay)/(1+fracdelay);
		LOOP(inNumSamples,
			long irdphase = iwrphase - (long)dsamp;
			float aud = ZXP(in);
			float input = aud + dlybuf[irdphase & mask];
			float s = lp * 0.5 + input * 0.5;
			lp = input;
			float sample = ap + s*a;
			ap = s - (sample * a);
			if (irdphase < 0) {
				dlybuf[iwrphase & mask] = aud;
				ZXP(out) = 0.f;
			} else {
				ZXP(out) = sample;
				dlybuf[iwrphase & mask] = sample * ZIN0(2);
			}
			++iwrphase;
		);
	} else {
		LOOP(inNumSamples,
			long irdphase = iwrphase - (long)dsamp;
			float aud = ZXP(in);
			float input = aud + dlybuf[irdphase & mask];
			float s = lp * 0.5 + input * 0.5;
			lp = input;
			float sample = ap + s*a;
			ap = s - (sample * a);
			if (irdphase < 0) {
				dlybuf[iwrphase & mask] = aud;
				ZXP(out) = 0.f;
			} else {
				ZXP(out) = sample;
				dlybuf[iwrphase & mask] = sample * ZIN0(2);
			}
			++iwrphase;
		);
	}

	unit->m_iwrphase = iwrphase;
	unit->m_ap = zapgremlins(ap);
	unit->m_lp = zapgremlins(lp);
	unit->m_dsamp = dsamp;
	unit->m_a = a;

	unit->m_numoutput += inNumSamples;
	if (unit->m_numoutput >= unit->m_idelaylen) {
		SETCALC(Streson_next_k);
	}
}

void Streson_next_a(Streson *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float *delaytime = ZIN(1);
	float dsamp = unit->m_dsamp;
	float a = unit->m_a;
	float *dlybuf = unit->m_dlybuf;
	long iwrphase = unit->m_iwrphase;
	long mask = unit->m_mask;
	float ap = unit->m_ap;
	float lp = unit->m_lp;

	LOOP(inNumSamples,
		float tdelay = ZXP(delaytime) * SAMPLERATE;
		float dsamp = (long)(tdelay - 0.5);
		float fracdelay = tdelay - (dsamp + 0.5);
		float a = (1-fracdelay)/(1+fracdelay);
		long irdphase = iwrphase - (long)dsamp;
		float input = ZXP(in) + dlybuf[irdphase & mask];
		float s = lp * 0.5 + input * 0.5;
		lp = input;
		float sample = ap + s*a;
		ZXP(out) = sample;
		ap = s - (sample * a);
		dlybuf[iwrphase & mask] = sample * ZIN0(2);
		++iwrphase;
	);

	unit->m_iwrphase = iwrphase;
	unit->m_ap = zapgremlins(ap);
	unit->m_lp = zapgremlins(lp);
	unit->m_dsamp = dsamp;
	unit->m_a = a;
}

void Streson_next_a_z(Streson *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float *delaytime = ZIN(1);
	float dsamp = unit->m_dsamp;
	float a = unit->m_a;
	float *dlybuf = unit->m_dlybuf;
	long iwrphase = unit->m_iwrphase;
	long mask = unit->m_mask;
	float ap = unit->m_ap;
	float lp = unit->m_lp;

	LOOP(inNumSamples,
		float tdelay = ZXP(delaytime) * SAMPLERATE;
		float dsamp = (long)(tdelay - 0.5);
		float fracdelay = tdelay - (dsamp + 0.5);
		float a = (1-fracdelay)/(1+fracdelay);
		long irdphase = iwrphase - (long)dsamp;
		float aud = ZXP(in);
		float input = aud + dlybuf[irdphase & mask];
		float s = lp * 0.5 + input * 0.5;
		lp = input;
		float sample = ap + s*a;
		ap = s - (sample * a);
		if (irdphase < 0) {
			dlybuf[iwrphase & mask] = aud;
			ZXP(out) = 0.f;
		} else {
			ZXP(out) = sample;
			dlybuf[iwrphase & mask] = sample * ZIN0(2);
		}
		++iwrphase;
	);

	unit->m_iwrphase = iwrphase;
	unit->m_ap = zapgremlins(ap);
	unit->m_lp = zapgremlins(lp);
	unit->m_dsamp = dsamp;
	unit->m_a = a;

	unit->m_numoutput += inNumSamples;
	if (unit->m_numoutput >= unit->m_idelaylen) {
		SETCALC(Streson_next_a);
	}
}


void MoogLadder_Ctor(MoogLadder* unit) {
    if (INRATE(1) == calc_FullRate) {
	if (INRATE(2) == calc_FullRate) {
		SETCALC(MoogLadder_next_aa);
		} else {
		SETCALC(MoogLadder_next_ak);
		}
	    } else {
	if (INRATE(2) == calc_FullRate) {
		SETCALC(MoogLadder_next_ka);
		} else {
		SETCALC(MoogLadder_next_kk);
		}
	    }
	unit->m_fco = IN0(1);
	float fc = unit->m_fco * SAMPLEDUR;
	float f = unit->m_fco * SAMPLEDUR * 0.5;
	float fcr = 1.8730*fc*fc*fc + 0.4955*fc*fc - 0.6490*fc + 0.9988;
	unit->m_k2v = 1.25*(1-exp(-twopi*fcr*f));
	unit->m_res = IN0(2);
	unit->m_xnm1 = unit->m_y1nm1 = unit->m_y2nm1 = unit->m_y3nm1 = unit->m_y4nm1 = 0.0f;
	unit->m_y1n = unit->m_y2n = unit->m_y3n = unit->m_y4n = unit->m_y1n1 = 0.0f;
	ClearUnitOutputs(unit, 1);
}

void MoogLadder_next_kk(MoogLadder *unit, int inNumSamples){
    float* in = ZIN(0);
    float* out = ZOUT(0);
    float nextfco = IN0(1);
    float nextres = IN0(2);
    float fco = unit->m_fco;
    float res = unit->m_res;

    float xnm1 = unit->m_xnm1;
    float y1nm1 = unit->m_y1nm1;
    float y2nm1 = unit->m_y2nm1;
    float y3nm1 = unit->m_y3nm1;
	float y4nm1 = unit->m_y4nm1;
	float y1n1 = unit->m_y1n1;
	float y2n1, y3n1;
	float k2v = unit->m_k2v;
    float y1n = unit->m_y1n;
    float y2n = unit->m_y2n;
    float y3n = unit->m_y3n;
    float y4n = unit->m_y4n;
	// float i2v = 0.6103515625;
	float i2v = 0.70466;

    float fc, f, fcr, xn;
	float k2vslope=0;
	float resslope=0;

	if (nextfco != fco) {
		fc = nextfco * SAMPLEDUR;
		f = nextfco * SAMPLEDUR * 0.5;
		fcr = 1.8730*((fc * fc * fc) + 0.4955*(fc * fc)) - 0.6490*fc + 0.9988;
		float nextk2v = 1.25 * (1-exp(-twopi*fcr*f));
		k2vslope=CALCSLOPE(nextk2v, k2v);
	}

	if (nextres != res) {
		resslope=CALCSLOPE(nextres, res);
	};

   for (int i = 0; i < inNumSamples; i++) {
		float input = ZXP(in);
		xn = input - 8*res*y4n;
		float xn1 = xn * i2v;

		float y2nm11 = y2nm1 * i2v;
		float y3nm11 = y3nm1 * i2v;
		float y4nm11 = y4nm1 * i2v;

		y1n   = xnm1 + k2v * (xn1/(1 + fabs(xn1)) - y1n1/(1 + fabs(y1n1)));
		y1n1 = y1n * i2v;
		y2n   = y2nm1 + k2v * (y1n1/(1 + fabs(y1n1)) - y2nm11/(1 + fabs(y2nm11)));
		y2n1 = y2n * i2v;
		y3n   = y3nm1 + k2v * (y2n1/(1 + fabs(y2n1)) - y3nm11/(1 + fabs(y3nm11)));
		y3n1 = y3n * i2v;
		y4n   = y4nm1 + k2v * (y3n1/(1 + fabs(y3n1)) - y4nm11/(1 + fabs(y4nm11)));
		y4n = (y4n+y4nm1) * 0.5;

		xnm1  = xn;
		y1nm1 = y1n;
		y2nm1 = y2n;
		y3nm1 = y3n;
		y4nm1 = y4n;

		xn = input - 8*res*y4n;

		xn1 = xn * i2v;
		y2nm11 = y2nm1 * i2v;
		y3nm11 = y3nm1 * i2v;
		y4nm11 = y4nm1 * i2v;

		y1n   = xnm1 + k2v * (xn1/(1 + fabs(xn1)) - y1n1/(1 + fabs(y1n1)));
		y1n1 = y1n * i2v;
		y2n   = y2nm1 + k2v * (y1n1/(1 + fabs(y1n1)) - y2nm11/(1 + fabs(y2nm11)));
		y2n1 = y2n * i2v;
		y3n   = y3nm1 + k2v * (y2n1/(1 + fabs(y2n1)) - y3nm11/(1 + fabs(y3nm11)));
		y3n1 = y3n * i2v;
		y4n   = y4nm1 + k2v * (y3n1/(1 + fabs(y3n1)) - y4nm11/(1 + fabs(y4nm11)));
		y4n = (y4n+y4nm1) * 0.5;

		xnm1  = xn;
		y1nm1 = y1n;
		y2nm1 = y2n;
		y3nm1 = y3n;
		y4nm1 = y4n;

		if (k2vslope != 0) k2v += k2vslope;
		if (resslope != 0) res += resslope;
		ZXP(out) = y4n;
    }

    unit->m_fco = nextfco;
    unit->m_res = res;
	unit->m_k2v = k2v;
    unit->m_xnm1 = zapgremlins(xnm1);
    unit->m_y1nm1 = zapgremlins(y1nm1);
    unit->m_y2nm1 = zapgremlins(y2nm1);
    unit->m_y3nm1 = zapgremlins(y3nm1);
	unit->m_y4nm1 = zapgremlins(y4nm1);
	unit->m_y1n1 = zapgremlins(y1n1);
    unit->m_y1n = zapgremlins(y1n);
    unit->m_y2n = zapgremlins(y2n);
    unit->m_y3n = zapgremlins(y3n);
    unit->m_y4n = zapgremlins(y4n);
}

void MoogLadder_next_ka(MoogLadder *unit, int inNumSamples){
	float* in = ZIN(0);
    float* out = ZOUT(0);
    float nextfco = IN0(1);
    float fco = unit->m_fco;
    float* res = IN(2);

    float xnm1 = unit->m_xnm1;
    float y1nm1 = unit->m_y1nm1;
    float y2nm1 = unit->m_y2nm1;
    float y3nm1 = unit->m_y3nm1;
	float y4nm1 = unit->m_y4nm1;
	float y1n1 = unit->m_y1n1;
	float y2n1, y3n1;
	float k2v = unit->m_k2v;
    float y1n = unit->m_y1n;
    float y2n = unit->m_y2n;
    float y3n = unit->m_y3n;
    float y4n = unit->m_y4n;
	float i2v = 0.70466;

    float fc, f, fcr, xn;
	float k2vslope=0;

   if (nextfco != fco) {
		fc = nextfco * SAMPLEDUR;
		f = nextfco * SAMPLEDUR * 0.5;
		fcr = 1.8730*((fc * fc * fc) + 0.4955*(fc * fc)) - 0.6490*fc + 0.9988;
		float nextk2v = 1.25*(1-exp(-twopi*fcr*f));
		k2vslope=CALCSLOPE(nextk2v, k2v);
	}

    for (int i = 0; i < inNumSamples; i++) {
		float input = ZXP(in);
		xn = input - 8*res[i]*y4n;
		float xn1 = xn * i2v;

		float y2nm11 = y2nm1 * i2v;
		float y3nm11 = y3nm1 * i2v;
		float y4nm11 = y4nm1 * i2v;

		y1n   = xnm1 + k2v * (xn1/(1 + fabs(xn1)) - y1n1/(1 + fabs(y1n1)));
		y1n1 = y1n * i2v;
		y2n   = y2nm1 + k2v * (y1n1/(1 + fabs(y1n1)) - y2nm11/(1 + fabs(y2nm11)));
		y2n1 = y2n * i2v;
		y3n   = y3nm1 + k2v * (y2n1/(1 + fabs(y2n1)) - y3nm11/(1 + fabs(y3nm11)));
		y3n1 = y3n * i2v;
		y4n   = y4nm1 + k2v * (y3n1/(1 + fabs(y3n1)) - y4nm11/(1 + fabs(y4nm11)));
		y4n = (y4n+y4nm1) * 0.5;

		xnm1  = xn;
		y1nm1 = y1n;
		y2nm1 = y2n;
		y3nm1 = y3n;
		y4nm1 = y4n;

		xn = input - 8*res[i]*y4n;

		xn1 = xn * i2v;
		y2nm11 = y2nm1 * i2v;
		y3nm11 = y3nm1 * i2v;
		y4nm11 = y4nm1 * i2v;

		y1n   = xnm1 + k2v * (xn1/(1 + fabs(xn1)) - y1n1/(1 + fabs(y1n1)));
		y1n1 = y1n * i2v;
		y2n   = y2nm1 + k2v * (y1n1/(1 + fabs(y1n1)) - y2nm11/(1 + fabs(y2nm11)));
		y2n1 = y2n * i2v;
		y3n   = y3nm1 + k2v * (y2n1/(1 + fabs(y2n1)) - y3nm11/(1 + fabs(y3nm11)));
		y3n1 = y3n * i2v;
		y4n   = y4nm1 + k2v * (y3n1/(1 + fabs(y3n1)) - y4nm11/(1 + fabs(y4nm11)));
		y4n = (y4n+y4nm1) * 0.5;

		xnm1  = xn;
		y1nm1 = y1n;
		y2nm1 = y2n;
		y3nm1 = y3n;
		y4nm1 = y4n;

		if (k2vslope != 0) k2v += k2vslope;
		ZXP(out) = y4n;
    }

    unit->m_fco = nextfco;
	unit->m_k2v = k2v;
    unit->m_xnm1 = zapgremlins(xnm1);
    unit->m_y1nm1 = zapgremlins(y1nm1);
    unit->m_y2nm1 = zapgremlins(y2nm1);
    unit->m_y3nm1 = zapgremlins(y3nm1);
	unit->m_y4nm1 = zapgremlins(y4nm1);
	unit->m_y1n1 = zapgremlins(y1n1);
    unit->m_y1n = zapgremlins(y1n);
    unit->m_y2n = zapgremlins(y2n);
    unit->m_y3n = zapgremlins(y3n);
    unit->m_y4n = zapgremlins(y4n);
}

void MoogLadder_next_ak(MoogLadder *unit, int inNumSamples){
	float* in = ZIN(0);
    float* out = ZOUT(0);
    float* nextfco = IN(1);
    float nextres = IN0(2);
    float fco = unit->m_fco;
    float res = unit->m_res;

    float xnm1 = unit->m_xnm1;
    float y1nm1 = unit->m_y1nm1;
    float y2nm1 = unit->m_y2nm1;
    float y3nm1 = unit->m_y3nm1;
	float y4nm1 = unit->m_y4nm1;
	float y1n1 = unit->m_y1n1;
	float y2n1, y3n1;
	float k2v = unit->m_k2v;
    float y1n = unit->m_y1n;
    float y2n = unit->m_y2n;
    float y3n = unit->m_y3n;
    float y4n = unit->m_y4n;
	float i2v = 0.70466;

    float fc, f, fcr, xn;
	float resslope=0;

	if (nextres != res) {
		resslope=CALCSLOPE(nextres, res);
	}

    for (int i = 0; i < inNumSamples; i++) {
		if (nextfco[i] != fco) {
			fc = nextfco[i] * SAMPLEDUR;
			f = nextfco[i] * SAMPLEDUR * 0.5;
			fcr = 1.8730*((fc * fc * fc) + 0.4955*(fc * fc)) - 0.6490*fc + 0.9988;
			k2v = 1.25*(1-exp(-twopi*fcr*f));
			fco=nextfco[i];
		}
		float input = ZXP(in);
		xn = input - 8*res*y4n;
		float xn1 = xn * i2v;

		float y2nm11 = y2nm1 * i2v;
		float y3nm11 = y3nm1 * i2v;
		float y4nm11 = y4nm1 * i2v;

		y1n   = xnm1 + k2v * (xn1/(1 + fabs(xn1)) - y1n1/(1 + fabs(y1n1)));
		y1n1 = y1n * i2v;
		y2n   = y2nm1 + k2v * (y1n1/(1 + fabs(y1n1)) - y2nm11/(1 + fabs(y2nm11)));
		y2n1 = y2n * i2v;
		y3n   = y3nm1 + k2v * (y2n1/(1 + fabs(y2n1)) - y3nm11/(1 + fabs(y3nm11)));
		y3n1 = y3n * i2v;
		y4n   = y4nm1 + k2v * (y3n1/(1 + fabs(y3n1)) - y4nm11/(1 + fabs(y4nm11)));
		y4n = (y4n+y4nm1) * 0.5;

		xnm1  = xn;
		y1nm1 = y1n;
		y2nm1 = y2n;
		y3nm1 = y3n;
		y4nm1 = y4n;

		xn = input - 8*res*y4n;

		xn1 = xn * i2v;
		y2nm11 = y2nm1 * i2v;
		y3nm11 = y3nm1 * i2v;
		y4nm11 = y4nm1 * i2v;

		y1n   = xnm1 + k2v * (xn1/(1 + fabs(xn1)) - y1n1/(1 + fabs(y1n1)));
		y1n1 = y1n * i2v;
		y2n   = y2nm1 + k2v * (y1n1/(1 + fabs(y1n1)) - y2nm11/(1 + fabs(y2nm11)));
		y2n1 = y2n * i2v;
		y3n   = y3nm1 + k2v * (y2n1/(1 + fabs(y2n1)) - y3nm11/(1 + fabs(y3nm11)));
		y3n1 = y3n * i2v;
		y4n   = y4nm1 + k2v * (y3n1/(1 + fabs(y3n1)) - y4nm11/(1 + fabs(y4nm11)));
		y4n = (y4n+y4nm1) * 0.5;

		xnm1  = xn;
		y1nm1 = y1n;
		y2nm1 = y2n;
		y3nm1 = y3n;
		y4nm1 = y4n;

		if (resslope != 0) res += resslope;
		ZXP(out) = y4n;
    }

    unit->m_fco = fco;
    unit->m_res = res;
	unit->m_k2v = k2v;
    unit->m_xnm1 = zapgremlins(xnm1);
    unit->m_y1nm1 = zapgremlins(y1nm1);
    unit->m_y2nm1 = zapgremlins(y2nm1);
    unit->m_y3nm1 = zapgremlins(y3nm1);
	unit->m_y4nm1 = zapgremlins(y4nm1);
	unit->m_y1n1 = zapgremlins(y1n1);
    unit->m_y1n = zapgremlins(y1n);
    unit->m_y2n = zapgremlins(y2n);
    unit->m_y3n = zapgremlins(y3n);
    unit->m_y4n = zapgremlins(y4n);
}

void MoogLadder_next_aa(MoogLadder *unit, int inNumSamples){
    float* in = ZIN(0);
    float* out = ZOUT(0);
    float* nextfco = IN(1);
    float fco = unit->m_fco;
    float* res = IN(2);

    float xnm1 = unit->m_xnm1;
    float y1nm1 = unit->m_y1nm1;
    float y2nm1 = unit->m_y2nm1;
    float y3nm1 = unit->m_y3nm1;
	float y4nm1 = unit->m_y4nm1;
	float y1n1 = unit->m_y1n1;
	float y2n1, y3n1;
	float k2v = unit->m_k2v;
    float y1n = unit->m_y1n;
    float y2n = unit->m_y2n;
    float y3n = unit->m_y3n;
    float y4n = unit->m_y4n;
	float i2v = 0.70466;

    float fc, f, fcr, xn;

    for (int i = 0; i < inNumSamples; i++) {
		if (nextfco[i] != fco) {
			fc = nextfco[i] * SAMPLEDUR;
			f = nextfco[i] * SAMPLEDUR * 0.5;
			fcr = 1.8730*((fc * fc * fc) + 0.4955*(fc * fc)) - 0.6490*fc + 0.9988;
			k2v = 1.220703125*(1-exp(-twopi*fcr*f));
			fco=nextfco[i];
		}

		float input = ZXP(in);
		xn = input - 8*res[i]*y4n;
		float xn1 = xn * i2v;

		float y2nm11 = y2nm1 * i2v;
		float y3nm11 = y3nm1 * i2v;
		float y4nm11 = y4nm1 * i2v;

		y1n   = xnm1 + k2v * (xn1/(1 + fabs(xn1)) - y1n1/(1 + fabs(y1n1)));
		y1n1 = y1n * i2v;
		y2n   = y2nm1 + k2v * (y1n1/(1 + fabs(y1n1)) - y2nm11/(1 + fabs(y2nm11)));
		y2n1 = y2n * i2v;
		y3n   = y3nm1 + k2v * (y2n1/(1 + fabs(y2n1)) - y3nm11/(1 + fabs(y3nm11)));
		y3n1 = y3n * i2v;
		y4n   = y4nm1 + k2v * (y3n1/(1 + fabs(y3n1)) - y4nm11/(1 + fabs(y4nm11)));
		y4n = (y4n+y4nm1) * 0.5;

		xnm1  = xn;
		y1nm1 = y1n;
		y2nm1 = y2n;
		y3nm1 = y3n;
		y4nm1 = y4n;

		xn = input - 8*res[i]*y4n;

		xn1 = xn * i2v;
		y2nm11 = y2nm1 * i2v;
		y3nm11 = y3nm1 * i2v;
		y4nm11 = y4nm1 * i2v;

		y1n   = xnm1 + k2v * (xn1/(1 + fabs(xn1)) - y1n1/(1 + fabs(y1n1)));
		y1n1 = y1n * i2v;
		y2n   = y2nm1 + k2v * (y1n1/(1 + fabs(y1n1)) - y2nm11/(1 + fabs(y2nm11)));
		y2n1 = y2n * i2v;
		y3n   = y3nm1 + k2v * (y2n1/(1 + fabs(y2n1)) - y3nm11/(1 + fabs(y3nm11)));
		y3n1 = y3n * i2v;
		y4n   = y4nm1 + k2v * (y3n1/(1 + fabs(y3n1)) - y4nm11/(1 + fabs(y4nm11)));
		y4n = (y4n+y4nm1) * 0.5;

		xnm1  = xn;
		y1nm1 = y1n;
		y2nm1 = y2n;
		y3nm1 = y3n;
		y4nm1 = y4n;

		ZXP(out) = y4n;
    }

    unit->m_k2v = k2v;
    unit->m_xnm1 = zapgremlins(xnm1);
    unit->m_y1nm1 = zapgremlins(y1nm1);
    unit->m_y2nm1 = zapgremlins(y2nm1);
    unit->m_y3nm1 = zapgremlins(y3nm1);
	unit->m_y4nm1 = zapgremlins(y4nm1);
	unit->m_y1n1 = zapgremlins(y1n1);
    unit->m_y1n = zapgremlins(y1n);
    unit->m_y2n = zapgremlins(y2n);
    unit->m_y3n = zapgremlins(y3n);
    unit->m_y4n = zapgremlins(y4n);
}

void MultiFilt_Ctor(MultiFilt* unit) {
	if (INRATE(1) == calc_FullRate) {
	if (INRATE(2) == calc_FullRate) {
		SETCALC(MultiFilt_next_aa);
		} else {
		SETCALC(MultiFilt_next_ak);
		}
	} else {
	if (INRATE(2) == calc_FullRate) {
		SETCALC(MultiFilt_next_ka);
		} else {
		SETCALC(MultiFilt_next_kk);
		}
	}
	float cf = ZIN0(1);
	unit->m_cf = cf;
	unit->m_rq = ZIN0(2);
	unit->m_f = 2.f*sin(pi*cf*SAMPLEDUR*0.25);
	unit->m_hp = unit->m_lp = unit->m_bp = 0.0f;
	unit->m_halfsamp = 20000;
	ClearUnitOutputs(unit, 1);
}

void MultiFilt_next_kk(MultiFilt *unit, int inNumSamples) {
	float* in = ZIN(0);
    float* out[3];
	out[0] = ZOUT(0);
	out[1] = ZOUT(1);
	out[2] = ZOUT(2);
    float nextcf = ZIN0(1);
    float nextrq = ZIN0(2);
	float rq = unit->m_rq;
	float f = unit->m_f;
	float hp = unit->m_hp;
	float lp = unit->m_lp;
	float bp = unit->m_bp;

	if (nextcf != unit->m_cf) {
		float nextf = 2.f * sin(pi * nextcf * SAMPLEDUR * 0.25);
		float fslope = CALCSLOPE(nextf, f);

		if (nextrq != rq) {
			if (nextrq * unit->m_halfsamp < nextcf) nextrq = nextcf/unit->m_halfsamp;
			float rqslope = CALCSLOPE(nextrq, rq);
			LOOP(inNumSamples,
				float sig = ZXP(in);
				hp=sig - rq * bp - lp;
				lp=bp * f + lp;
				bp=hp * f + bp;

				hp=sig - rq * bp - lp;
				lp=bp * f + lp;
				bp=hp * f + bp;

				hp=sig - rq * bp - lp;
				lp=bp * f + lp;
				bp=hp * f + bp;

				hp=sig - rq * bp - lp;
				lp=bp * f + lp;
				bp=hp * f + bp;

				f += fslope;
				rq += rqslope;

				ZXP(out[0]) = lp;
				ZXP(out[1]) = hp;
				ZXP(out[2]) = bp;
			);
		} else {
			if (rq * unit->m_halfsamp < nextcf) rq = nextcf/unit->m_halfsamp;
			LOOP(inNumSamples,
				float sig = ZXP(in);
				hp=sig - rq * bp - lp;
				lp=bp * f + lp;
				bp=hp * f + bp;

				hp=sig - rq * bp - lp;
				lp=bp * f + lp;
				bp=hp * f + bp;

				hp=sig - rq * bp - lp;
				lp=bp * f + lp;
				bp=hp * f + bp;

				hp=sig - rq * bp - lp;
				lp=bp * f + lp;
				bp=hp * f + bp;

				f += fslope;

				ZXP(out[0]) = lp;
				ZXP(out[1]) = hp;
				ZXP(out[2]) = bp;
			);
		}
	} else {
		if (nextrq != rq) {
			if (nextrq * unit->m_halfsamp < nextcf) nextrq = nextcf/unit->m_halfsamp;
			float rqslope = CALCSLOPE(nextrq, rq);
			LOOP(inNumSamples,
				float sig = ZXP(in);
				hp=sig - rq * bp - lp;
				lp=bp * f + lp;
				bp=hp * f + bp;

				hp=sig - rq * bp - lp;
				lp=bp * f + lp;
				bp=hp * f + bp;

				hp=sig - rq * bp - lp;
				lp=bp * f + lp;
				bp=hp * f + bp;

				hp=sig - rq * bp - lp;
				lp=bp * f + lp;
				bp=hp * f + bp;

				rq += rqslope;

				ZXP(out[0]) = lp;
				ZXP(out[1]) = hp;
				ZXP(out[2]) = bp;
			);
		} else {
			if (rq * unit->m_halfsamp < nextcf) rq = nextcf/unit->m_halfsamp;
			LOOP(inNumSamples,
				float sig = ZXP(in);
				hp=sig - rq * bp - lp;
				lp=bp * f + lp;
				bp=hp * f + bp;

				hp=sig - rq * bp - lp;
				lp=bp * f + lp;
				bp=hp * f + bp;

				hp=sig - rq * bp - lp;
				lp=bp * f + lp;
				bp=hp * f + bp;

				hp=sig - rq * bp - lp;
				lp=bp * f + lp;
				bp=hp * f + bp;

				ZXP(out[0]) = lp;
				ZXP(out[1]) = hp;
				ZXP(out[2]) = bp;
			);
		}
	}

	unit->m_hp=zapgremlins(hp);
	unit->m_lp=zapgremlins(lp);
	unit->m_bp=zapgremlins(bp);
	unit->m_f=f;
	unit->m_cf=nextcf;
	unit->m_rq=nextrq;
}

void MultiFilt_next_ak(MultiFilt *unit, int inNumSamples) {
	float* in = ZIN(0);
    float* out[3];
	out[0] = ZOUT(0);
	out[1] = ZOUT(1);
	out[2] = ZOUT(2);
    float* nextcf = ZIN(1);
    float nextrq = ZIN0(2);
	float rq = unit->m_rq;
	float f = unit->m_f;
	float hp = unit->m_hp;
	float lp = unit->m_lp;
	float bp = unit->m_bp;
	float lastcf = unit->m_cf;
	float cf;

	if (nextrq != rq) {
		float rqslope = CALCSLOPE(nextrq, rq);
		LOOP(inNumSamples,
			cf = ZXP(nextcf);
			if (cf != lastcf) {
				f = 2.f * sin(pi * cf * SAMPLEDUR * 0.25);
				lastcf = cf;
			}

			if (rq * unit->m_halfsamp < cf) rq = cf/unit->m_halfsamp;

			float sig = ZXP(in);
			hp=sig - rq * bp - lp;
			lp=bp * f + lp;
			bp=hp * f + bp;

			hp=sig - rq * bp - lp;
			lp=bp * f + lp;
			bp=hp * f + bp;

			hp=sig - rq * bp - lp;
			lp=bp * f + lp;
			bp=hp * f + bp;

			hp=sig - rq * bp - lp;
			lp=bp * f + lp;
			bp=hp * f + bp;

			rq += rqslope;

			ZXP(out[0]) = lp;
			ZXP(out[1]) = hp;
			ZXP(out[2]) = bp;
		);
	} else {
		LOOP(inNumSamples,
			cf = ZXP(nextcf);
			if (cf != lastcf) {
				f = 2.f * sin(pi * cf * SAMPLEDUR * 0.25);
				lastcf = cf;
			}

			if (rq * unit->m_halfsamp < cf) rq = cf/unit->m_halfsamp;
			float sig = ZXP(in);
			hp=sig - rq * bp - lp;
			lp=bp * f + lp;
			bp=hp * f + bp;

			hp=sig - rq * bp - lp;
			lp=bp * f + lp;
			bp=hp * f + bp;

			hp=sig - rq * bp - lp;
			lp=bp * f + lp;
			bp=hp * f + bp;

			hp=sig - rq * bp - lp;
			lp=bp * f + lp;
			bp=hp * f + bp;

			ZXP(out[0]) = lp;
			ZXP(out[1]) = hp;
			ZXP(out[2]) = bp;
		);
	}

	unit->m_hp=zapgremlins(hp);
	unit->m_lp=zapgremlins(lp);
	unit->m_bp=zapgremlins(bp);
	unit->m_f=f;
	unit->m_cf=cf;
	unit->m_rq=nextrq;
}

void MultiFilt_next_ka(MultiFilt *unit, int inNumSamples) {
	float* in = ZIN(0);
    float* out[3];
	out[0] = ZOUT(0);
	out[1] = ZOUT(1);
	out[2] = ZOUT(2);
    float nextcf = ZIN0(1);
    float* nextrq = ZIN(2);
	float rq;
	float f = unit->m_f;
	float hp = unit->m_hp;
	float lp = unit->m_lp;
	float bp = unit->m_bp;

	if (nextcf != unit->m_cf) {
		float nextf = 2.f * sin(pi * nextcf * SAMPLEDUR * 0.25);
		float fslope = CALCSLOPE(nextf, f);

		LOOP(inNumSamples,
			rq = ZXP(nextrq);
			if (rq * unit->m_halfsamp < nextcf) rq = nextcf/unit->m_halfsamp;
			float sig = ZXP(in);
			hp=sig - rq * bp - lp;
			lp=bp * f + lp;
			bp=hp * f + bp;

			hp=sig - rq * bp - lp;
			lp=bp * f + lp;
			bp=hp * f + bp;

			hp=sig - rq * bp - lp;
			lp=bp * f + lp;
			bp=hp * f + bp;

			hp=sig - rq * bp - lp;
			lp=bp * f + lp;
			bp=hp * f + bp;

			f += fslope;

			ZXP(out[0]) = lp;
			ZXP(out[1]) = hp;
			ZXP(out[2]) = bp;
		);
	} else {
		LOOP(inNumSamples,
			rq = ZXP(nextrq);
			if (rq * unit->m_halfsamp < nextcf) rq = nextcf/unit->m_halfsamp;
			float sig = ZXP(in);
			hp=sig - rq * bp - lp;
			lp=bp * f + lp;
			bp=hp * f + bp;

			hp=sig - rq * bp - lp;
			lp=bp * f + lp;
			bp=hp * f + bp;

			hp=sig - rq * bp - lp;
			lp=bp * f + lp;
			bp=hp * f + bp;

			hp=sig - rq * bp - lp;
			lp=bp * f + lp;
			bp=hp * f + bp;

			ZXP(out[0]) = lp;
			ZXP(out[1]) = hp;
			ZXP(out[2]) = bp;
		);
	}

	unit->m_hp=zapgremlins(hp);
	unit->m_lp=zapgremlins(lp);
	unit->m_bp=zapgremlins(bp);
	unit->m_f=f;
	unit->m_cf=nextcf;
	unit->m_rq=rq;
}

void MultiFilt_next_aa(MultiFilt *unit, int inNumSamples) {
	float* in = ZIN(0);
    float* out[3];
	out[0] = ZOUT(0);
	out[1] = ZOUT(1);
	out[2] = ZOUT(2);
    float* nextcf = ZIN(1);
	float* nextrq = ZIN(2);
	float f = unit->m_f;
	float hp = unit->m_hp;
	float lp = unit->m_lp;
	float bp = unit->m_bp;
	float lastcf = unit->m_cf;
	float cf, rq;

	LOOP(inNumSamples,
		cf = ZXP(nextcf);
		rq = ZXP(nextrq);
		if (cf != lastcf) {
			f = 2.f * sin(pi * cf * SAMPLEDUR * 0.25);
			lastcf = cf;
		}

		if (rq * unit->m_halfsamp < cf) rq = cf/unit->m_halfsamp;

		float sig = ZXP(in);
		hp=sig - rq * bp - lp;
		lp=bp * f + lp;
		bp=hp * f + bp;

		hp=sig - rq * bp - lp;
		lp=bp * f + lp;
		bp=hp * f + bp;

		hp=sig - rq * bp - lp;
		lp=bp * f + lp;
		bp=hp * f + bp;

		hp=sig - rq * bp - lp;
		lp=bp * f + lp;
		bp=hp * f + bp;

		ZXP(out[0]) = lp;
		ZXP(out[1]) = hp;
		ZXP(out[2]) = bp;
	);

	unit->m_hp=zapgremlins(hp);
	unit->m_lp=zapgremlins(lp);
	unit->m_bp=zapgremlins(bp);
	unit->m_f=f;
	unit->m_cf=cf;
	unit->m_rq=rq;
}

void RLPFD_Ctor(RLPFD* unit) {
    if (INRATE(1) == calc_FullRate) {
	if (INRATE(2) == calc_FullRate) {
		SETCALC(RLPFD_next_aa); // a-rate fco and res
		} else {
		SETCALC(RLPFD_next_ak); // a-rate fco, k-rate res
		}
	    } else {
	if (INRATE(2) == calc_FullRate) {
		SETCALC(RLPFD_next_ka); // k-rate fco, a-rate res
		} else {
		SETCALC(RLPFD_next_kk);
		} // k-rate fco and res
	    }
	float fco = IN0(1);
	float fcn = 2.f * fco * SAMPLEDUR;
	unit->m_ap = ((-2.7441 * fcn + 3.1433) * fcn + 1.74) * fcn - 0.9984;
	float ap1 = unit->m_ap + 1;
	unit->m_scl = ((2.7079 * ap1 + 10.963) * ap1 - 14.934) * ap1 + 8.4974;
	unit->m_fco = fco;
	unit->m_res = IN0(2);
	unit->m_x1 = unit->m_y1 = unit->m_y2 = unit->m_y3 = 0.0f;
	unit->m_x1nm1 = unit->m_y1nm1 = unit->m_y2nm1 = 0.0f;
	ClearUnitOutputs(unit, 1);
}

void RLPFD_next_kk(RLPFD *unit, int inNumSamples){
    float* in = IN(0);
    float* out = OUT(0);
    float nextfco = IN0(1);
    float nextres = IN0(2);
	float dist = IN0(3) * 0.001;
    float fco = unit->m_fco;
    float res = unit->m_res;
	float fcn = 2.f * nextfco * SAMPLEDUR;

    float x1nm1 = unit->m_x1nm1;
    float y1nm1 = unit->m_y1nm1;
	float y2nm1 = unit->m_y2nm1;
	float ap = unit->m_ap;
	float ap1 = ap + 1.f;
	float scl = unit->m_scl;
    float y1 = unit->m_y1;
    float y2 = unit->m_y2;
    float y3 = unit->m_y3;
    float x1 = unit->m_x1;

	float apslope=0;
	float resslope=0;
	float sclslope=0;

	if (nextfco != fco) {
		float nextap = ((-2.7441 * fcn + 3.1433) * fcn + 1.74) * fcn - 0.9984;
		ap1 = nextap + 1;
		float nextscl = ((2.7079 * ap1 + 10.963) * ap1 - 14.934) * ap1 + 8.4974;
		apslope=CALCSLOPE(nextap, ap);
		sclslope=CALCSLOPE(nextscl, scl);
	}

	if (nextres != res) {
		resslope=CALCSLOPE(nextres, res);
	}

	for (int i = 0; i < inNumSamples; i++) {
		float kres = res * scl;
		ap1 = ap + 1.f;
		x1 = in[i] - kres * y3/sqrt(1 + y3 * y3);
		y1 = ((x1 + x1nm1) * ap1) * 0.5 - ap * y1;
		y2 = ((y1 + y1nm1) * ap1) * 0.5 - ap * y2;
		y3 = ((y2 + y2nm1) * ap1) * 0.5 - ap * y3;
		if (dist > 0.f) y3 = y3 * (1.f + (dist * (1.5 + 2.f * kres * (1.f - fcn))));
		y3 = y3/sqrt(1 + y3 * y3);

		x1nm1 = x1;
		y1nm1 = y1;
		y2nm1 = y2;

		out[i] = y3;

		if (apslope != 0.f) {
			ap += apslope;
			scl += sclslope;
		}
		if (resslope != 0.f) res += resslope;
	}

	unit->m_ap=ap;
	unit->m_scl=scl;
	unit->m_fco=nextfco;
	unit->m_res=res;
	unit->m_x1=zapgremlins(x1);
	unit->m_y1=zapgremlins(y1);
	unit->m_y2=zapgremlins(y2);
	unit->m_y3=zapgremlins(y3);
	unit->m_x1nm1=zapgremlins(x1nm1);
	unit->m_y1nm1=zapgremlins(y1nm1);
	unit->m_y2nm1=zapgremlins(y2nm1);
}

void RLPFD_next_ka(RLPFD *unit, int inNumSamples){
     float* in = IN(0);
    float* out = OUT(0);
    float nextfco = IN0(1);
	float dist = IN0(3) * 0.001;
    float fco = unit->m_fco;
    float* res = IN(2);
	float fcn = 2.f * nextfco * SAMPLEDUR;

    float x1nm1 = unit->m_x1nm1;
    float y1nm1 = unit->m_y1nm1;
	float y2nm1 = unit->m_y2nm1;
	float ap = unit->m_ap;
	float ap1 = ap + 1.f;
	float scl = unit->m_scl;
    float y1 = unit->m_y1;
    float y2 = unit->m_y2;
    float y3 = unit->m_y3;
    float x1 = unit->m_x1;

	float apslope=0;
	float sclslope=0;

	if (nextfco != fco) {
		float nextap = ((-2.7441 * fcn + 3.1433) * fcn + 1.74) * fcn - 0.9984;
		ap1 = nextap + 1;
		float nextscl = ((2.7079 * ap1 + 10.963) * ap1 - 14.934) * ap1 + 8.4974;
		apslope=CALCSLOPE(nextap, ap);
		sclslope=CALCSLOPE(nextscl, scl);
	}

	for (int i = 0; i < inNumSamples; i++) {
		float kres = res[i] * scl;
		ap1 = ap + 1.f;
		x1 = in[i] - kres * y3/sqrt(1 + y3 * y3);
		y1 = ((x1 + x1nm1) * ap1) * 0.5 - ap * y1;
		y2 = ((y1 + y1nm1) * ap1) * 0.5 - ap * y2;
		y3 = ((y2 + y2nm1) * ap1) * 0.5 - ap * y3;
		if (dist > 0.f) y3 = y3 * (1.f + (dist * (1.5 + 2.f * kres * (1.f - fcn))));
		y3 = y3/sqrt(1 + y3 * y3);

		x1nm1 = x1;
		y1nm1 = y1;
		y2nm1 = y2;

		out[i] = y3;

		if (apslope != 0.f) {
			ap += apslope;
			scl += sclslope;
		}
	}

	unit->m_ap=ap;
	unit->m_scl=scl;
	unit->m_fco=nextfco;
	unit->m_x1=zapgremlins(x1);
	unit->m_y1=zapgremlins(y1);
	unit->m_y2=zapgremlins(y2);
	unit->m_y3=zapgremlins(y3);
	unit->m_x1nm1=zapgremlins(x1nm1);
	unit->m_y1nm1=zapgremlins(y1nm1);
	unit->m_y2nm1=zapgremlins(y2nm1);

}

void RLPFD_next_ak(RLPFD *unit, int inNumSamples){
    float* in = IN(0);
    float* out = OUT(0);
    float nextres = IN0(2);
	float dist = IN0(3) * 0.001;
    float* nextfco = IN(1);
	float fco = unit->m_fco;
    float res = unit->m_res;

    float x1nm1 = unit->m_x1nm1;
    float y1nm1 = unit->m_y1nm1;
	float y2nm1 = unit->m_y2nm1;
	float ap = unit->m_ap;
	float ap1 = ap + 1.f;
	float scl = unit->m_scl;
    float y1 = unit->m_y1;
    float y2 = unit->m_y2;
    float y3 = unit->m_y3;
    float x1 = unit->m_x1;

	float resslope=0;

	if (nextres != res) {
		resslope=CALCSLOPE(nextres, res);
	}

	for (int i = 0; i < inNumSamples; i++) {
		float fcn = 2.f * nextfco[i] * SAMPLEDUR;
		if (nextfco[i] != fco) {
			ap = ((-2.7441 * fcn + 3.1433) * fcn + 1.74) * fcn - 0.9984;
			ap1 = ap + 1;
			scl = ((2.7079 * ap1 + 10.963) * ap1 - 14.934) * ap1 + 8.4974;
			fco=nextfco[i];
		}
		float kres = res * scl;
		x1 = in[i] - kres * y3/sqrt(1 + y3 * y3);
		y1 = ((x1 + x1nm1) * ap1) * 0.5 - ap * y1;
		y2 = ((y1 + y1nm1) * ap1) * 0.5 - ap * y2;
		y3 = ((y2 + y2nm1) * ap1) * 0.5 - ap * y3;
		if (dist > 0.f) y3 = y3 * (1.f + (dist * (1.5 + 2.f * kres * (1.f - fcn))));
		y3 = y3/sqrt(1 + y3 * y3);

		x1nm1 = x1;
		y1nm1 = y1;
		y2nm1 = y2;

		out[i] = y3;
		if (resslope != 0.f) res += resslope;
	}

	unit->m_res=res;
	unit->m_fco=fco;
	unit->m_ap=ap;
	unit->m_scl=scl;
	unit->m_x1=zapgremlins(x1);
	unit->m_y1=zapgremlins(y1);
	unit->m_y2=zapgremlins(y2);
	unit->m_y3=zapgremlins(y3);
	unit->m_x1nm1=zapgremlins(x1nm1);
	unit->m_y1nm1=zapgremlins(y1nm1);
	unit->m_y2nm1=zapgremlins(y2nm1);
}

void RLPFD_next_aa(RLPFD *unit, int inNumSamples){
	float* in = IN(0);
    float* out = OUT(0);
	float dist = IN0(3) * 0.001;
    float* nextfco = IN(1);
	float fco = unit->m_fco;
    float* res = IN(2);

    float x1nm1 = unit->m_x1nm1;
    float y1nm1 = unit->m_y1nm1;
	float y2nm1 = unit->m_y2nm1;
	float ap = unit->m_ap;
	float ap1 = ap + 1.f;
	float scl = unit->m_scl;
    float y1 = unit->m_y1;
    float y2 = unit->m_y2;
    float y3 = unit->m_y3;
    float x1 = unit->m_x1;

	for (int i = 0; i < inNumSamples; i++) {
		float fcn = 2.f * nextfco[i] * SAMPLEDUR;
		if (nextfco[i] != fco) {
			ap = ((-2.7441 * fcn + 3.1433) * fcn + 1.74) * fcn - 0.9984;
			ap1 = ap + 1;
			scl = ((2.7079 * ap1 + 10.963) * ap1 - 14.934) * ap1 + 8.4974;
			fco=nextfco[i];
		}
		float kres = res[i] * scl;
		x1 = in[i] - kres * y3/sqrt(1 + y3 * y3);
		y1 = ((x1 + x1nm1) * ap1) * 0.5 - ap * y1;
		y2 = ((y1 + y1nm1) * ap1) * 0.5 - ap * y2;
		y3 = ((y2 + y2nm1) * ap1) * 0.5 - ap * y3;
		if (dist > 0.f) y3 = y3 * (1.f + (dist * (1.5 + 2.f * kres * (1.f - fcn))));
		y3 = y3/sqrt(1 + y3 * y3);

		x1nm1 = x1;
		y1nm1 = y1;
		y2nm1 = y2;

		out[i] = y3;
	}

	unit->m_fco=fco;
	unit->m_ap=ap;
	unit->m_scl=scl;
	unit->m_x1=zapgremlins(x1);
	unit->m_y1=zapgremlins(y1);
	unit->m_y2=zapgremlins(y2);
	unit->m_y3=zapgremlins(y3);
	unit->m_x1nm1=zapgremlins(x1nm1);
	unit->m_y1nm1=zapgremlins(y1nm1);
	unit->m_y2nm1=zapgremlins(y2nm1);
}

void NestedAllpassN_Ctor(NestedAllpassN* unit ) {
	SET2BUF
	unit->m_phase=0;
	unit->m_dsamp1= -(long)(unit->m_dt1 * unit->mRate->mSampleRate);
	unit->m_dsamp2= -(long)(unit->m_dt2 * unit->mRate->mSampleRate);
	SETCALC(NestedAllpassN_next_z);
	ZOUT0(0)=0.f;
}

void NestedAllpassN_Dtor(NestedAllpassN *unit ) {
	RTFree(unit->mWorld, unit->m_buf1);
	RTFree(unit->mWorld, unit->m_buf2);
}

void NestedAllpassN_next_z(NestedAllpassN *unit, int inNumSamples) {
	float *in=ZIN(0);
	float *out=ZOUT(0);
	float *buf1=unit->m_buf1;
	float *buf2=unit->m_buf2;
	float del1;
	float dt1=unit->m_dt1;
	float dt2=unit->m_dt2;
	float next_dt1=ZIN0(2);
	float next_dt2=ZIN0(5);
	long dsamp1=unit->m_dsamp1;
	long dsamp2=unit->m_dsamp2;
	float gain1=ZIN0(3);
	float gain2=ZIN0(6);
	long phase=unit->m_phase;
	long mask1=unit->m_maxdel1;
	long mask2=unit->m_maxdel2;
	float samp, sum, output;

	if (next_dt1 == dt1 && next_dt2 == dt2) {
		LOOP (inNumSamples,
			phase++;
			dsamp1++;
			dsamp2++;
			samp=ZXP(in);
			if (dsamp1 < 0 || dsamp2 < 0) {
				buf1[phase & mask1]=samp;
				buf2[phase & mask2]=samp;
				ZXP(out)=0.f;
			} else {
				del1=buf1[dsamp1 & mask1];
				sum=buf2[dsamp2 & mask2]-gain2*del1;
				output=sum-gain1*samp;
				buf1[phase & mask1]=samp+gain1*output;
				buf2[phase & mask2]=del1+gain2*sum;
				ZXP(out)=output;
			};
		);
	} else {
		float dslope1=CALCSLOPE(next_dt1, dt1);
		float dslope2=CALCSLOPE(next_dt2, dt2);
		LOOP (inNumSamples,
			phase++;
			dt1 += dslope1;
			dt2 += dslope2;
			dsamp1=phase-(long)(dt1 * unit->mRate->mSampleRate);
			dsamp2=phase-(long)(dt2 * unit->mRate->mSampleRate);
			samp=ZXP(in);
			buf1[phase & mask1]=samp+gain1*output;
			buf2[phase & mask2]=del1+gain2*sum;
			if (dsamp1 < 0 || dsamp2 < 0) {
				buf1[phase & mask1]=samp;
				buf2[phase & mask2]=samp;
				ZXP(out)=0.f;
			} else {
				del1=buf1[dsamp1 & mask1];
				sum=buf2[dsamp2 & mask2]-gain2*del1;
				output=sum-gain1*samp;
				buf1[phase & mask1]=samp+gain1*output;
				buf2[phase & mask2]=del1+gain2*sum;
				ZXP(out)=output;
			};
		);
	}

	unit->m_phase=phase;
	unit->m_dsamp1=dsamp1;
	unit->m_dsamp2=dsamp2;
	unit->m_dt1=dt1;
	unit->m_dt2=dt2;

	unit->m_numoutput += inNumSamples;
	if (unit->m_numoutput >= unit->m_maxdel) {
		SETCALC(NestedAllpassN_next);
	}
}

void NestedAllpassN_next(NestedAllpassN *unit, int inNumSamples) {
	float *in=ZIN(0);
	float *out=ZOUT(0);
	float *buf1=unit->m_buf1;
	float *buf2=unit->m_buf2;
	float del1;
	float dt1=unit->m_dt1;
	float dt2=unit->m_dt2;
	float next_dt1=ZIN0(2);
	float next_dt2=ZIN0(5);
	long dsamp1=unit->m_dsamp1;
	long dsamp2=unit->m_dsamp2;
	float gain1=ZIN0(3);
	float gain2=ZIN0(6);
	long phase=unit->m_phase;
	long mask1=unit->m_maxdel1;
	long mask2=unit->m_maxdel2;
	float samp, sum, output;

	if (next_dt1 == dt1 && next_dt2 == dt2) {
		LOOP (inNumSamples,
			phase++;
			dsamp1++;
			dsamp2++;
			samp=ZXP(in);
			del1=buf1[dsamp1 & mask1];
			sum=buf2[dsamp2 & mask2]-gain2*del1;
			output=sum-gain1*samp;
			buf1[phase & mask1]=samp+gain1*output;
			buf2[phase & mask2]=del1+gain2*sum;
			ZXP(out)=output;
		);
	} else {
		float dslope1=CALCSLOPE(next_dt1, dt1);
		float dslope2=CALCSLOPE(next_dt2, dt2);
		LOOP (inNumSamples,
			phase++;
			dt1 += dslope1;
			dt2 += dslope2;
			dsamp1=phase-(long)(dt1 * unit->mRate->mSampleRate);
			dsamp2=phase-(long)(dt2 * unit->mRate->mSampleRate);
			samp=ZXP(in);
			del1=buf1[dsamp1 & mask1];
			sum=buf2[dsamp2 & mask2]-gain2*del1;
			output=sum-gain1*samp;
			buf1[phase & mask1]=samp+gain1*output;
			buf2[phase & mask2]=del1+gain2*sum;
			ZXP(out)=output;
		);
	}

	unit->m_phase=phase;
	unit->m_dsamp1=dsamp1;
	unit->m_dsamp2=dsamp2;
	unit->m_dt1=dt1;
	unit->m_dt2=dt2;
}

void NestedAllpassL_Ctor(NestedAllpassL* unit ) {
	SET2BUF
	unit->m_phase=0;
	unit->m_dsamp1= -(unit->m_dt1 * unit->mRate->mSampleRate);
	unit->m_dsamp2= -(unit->m_dt2 * unit->mRate->mSampleRate);
	SETCALC(NestedAllpassL_next_z);
	ZOUT0(0)=0.f;
}

void NestedAllpassL_Dtor(NestedAllpassL *unit ) {
	RTFree(unit->mWorld, unit->m_buf1);
	RTFree(unit->mWorld, unit->m_buf2);
}

void NestedAllpassL_next(NestedAllpassL *unit, int inNumSamples) {
	float *in=ZIN(0);
	float *out=ZOUT(0);
	float *buf1=unit->m_buf1;
	float *buf2=unit->m_buf2;
	float del1;
	float dt1=unit->m_dt1;
	float dt2=unit->m_dt2;
	float next_dt1=ZIN0(2);
	float next_dt2=ZIN0(5);
	float dsamp1=unit->m_dsamp1;
	float dsamp2=unit->m_dsamp2;
	float gain1=ZIN0(3);
	float gain2=ZIN0(6);
	long phase=unit->m_phase;
	long mask1=unit->m_maxdel1;
	long mask2=unit->m_maxdel2;
	float samp, sum, output, frac1, frac2, del11, del21;

	if (next_dt1 == dt1 && next_dt2 == dt2) {
		int idsamp1;
		int idsamp2;
		LOOP (inNumSamples,
			phase++;
			dsamp1++;
			dsamp2++;
			idsamp1=(int)dsamp1;
			idsamp2=(int)dsamp2;
			frac1=dsamp1-idsamp1;
			frac2=dsamp2-idsamp2;
			samp=ZXP(in);
			del11=buf1[idsamp1 & mask1];
			del21=buf2[idsamp2 & mask2];
			float del12=buf1[(idsamp1-1) & mask1];
			float del22=buf2[(idsamp2-1) & mask2];
			del1=lininterp(frac1, del11, del12);
			sum=lininterp(frac2, del21, del22) - gain2*del1;
			output=sum-gain1*samp;
			buf1[phase & mask1]=samp+gain1*output;
			buf2[phase & mask2]=del1+gain2*sum;
			ZXP(out)=output;
		);
	} else {
		float dslope1=CALCSLOPE(next_dt1, dt1);
		float dslope2=CALCSLOPE(next_dt2, dt2);
		LOOP (inNumSamples,
			phase++;
			dt1 += dslope1;
			dt2 += dslope2;
			dsamp1=phase-dt1 * unit->mRate->mSampleRate;
			dsamp2=phase-dt2 * unit->mRate->mSampleRate;
			int idsamp1=(int)dsamp1;
			int idsamp2=(int)dsamp2;
			frac1=dsamp1-idsamp1;
			frac2=dsamp2-idsamp2;
			samp=ZXP(in);
			del1=lininterp(frac1, buf1[idsamp1 & mask1], buf1[(idsamp1-1) & mask1]);
			sum=lininterp(frac2, buf2[idsamp2 & mask2], buf2[(idsamp2-1) & mask2]) - gain2*del1;
			output=sum-gain1*samp;
			buf1[phase & mask1]=samp+gain1*output;
			buf2[phase & mask2]=del1+gain2*sum;
			ZXP(out)=output;
		);
	}

	unit->m_phase=phase;
	unit->m_dsamp1=dsamp1;
	unit->m_dsamp2=dsamp2;
	unit->m_dt1=dt1;
	unit->m_dt2=dt2;
}

void NestedAllpassL_next_z(NestedAllpassL *unit, int inNumSamples) {
	float *in=ZIN(0);
	float *out=ZOUT(0);
	float *buf1=unit->m_buf1;
	float *buf2=unit->m_buf2;
	float del1;
	float dt1=unit->m_dt1;
	float dt2=unit->m_dt2;
	float next_dt1=ZIN0(2);
	float next_dt2=ZIN0(5);
	float dsamp1=unit->m_dsamp1;
	float dsamp2=unit->m_dsamp2;
	float gain1=ZIN0(3);
	float gain2=ZIN0(6);
	long phase=unit->m_phase;
	long mask1=unit->m_maxdel1;
	long mask2=unit->m_maxdel2;
	float samp, sum, output, frac1, frac2, del11, del21;

	if (next_dt1 == dt1 && next_dt2 == dt2) {
		int idsamp1;
		int idsamp2;
		LOOP (inNumSamples,
			phase++;
			dsamp1++;
			dsamp2++;
			idsamp1=(int)dsamp1;
			idsamp2=(int)dsamp2;
			frac1=dsamp1-idsamp1;
			frac2=dsamp2-idsamp2;
			samp=ZXP(in);
			del11=buf1[idsamp1 & mask1];
			del21=buf2[idsamp2 & mask2];
			float del12=buf1[(idsamp1-1) & mask1];
			float del22=buf2[(idsamp2-1) & mask2];
			del1=lininterp(frac1, del11, del12);
			sum=lininterp(frac2, del21, del22) - gain2*del1;
			output=sum-gain1*samp;
			if ((idsamp1-1) < 0 || (idsamp2-1) < 0) {
				buf1[phase & mask1]=samp;
				buf2[phase & mask2]=samp;
				ZXP(out)=0.f;
			} else {
				buf1[phase & mask1]=samp+gain1*output;
				buf2[phase & mask2]=del1+gain2*sum;
				ZXP(out)=output;
			}
		);
	} else {
		float dslope1=CALCSLOPE(next_dt1, dt1);
		float dslope2=CALCSLOPE(next_dt2, dt2);
		LOOP (inNumSamples,
			phase++;
			dt1 += dslope1;
			dt2 += dslope2;
			dsamp1=phase-dt1 * unit->mRate->mSampleRate;
			dsamp2=phase-dt2 * unit->mRate->mSampleRate;
			int idsamp1=(int)dsamp1;
			int idsamp2=(int)dsamp2;
			frac1=dsamp1-idsamp1;
			frac2=dsamp2-idsamp2;
			samp=ZXP(in);
			del11=buf1[idsamp1 & mask1];
			del21=buf2[idsamp2 & mask2];
			del1=lininterp(frac1, del11, buf1[(idsamp1-1) & mask1]);
			sum=lininterp(frac2, del21, buf2[(idsamp2-1) & mask2]) - gain2*del1;
			output=sum-gain1*samp;
			if ((idsamp1-1) < 0 || (idsamp2-1) < 0) {
				buf1[phase & mask1]=samp;
				buf2[phase & mask2]=samp;
				ZXP(out)=0.f;
			} else {
				buf1[phase & mask1]=samp+gain1*output;
				buf2[phase & mask2]=del1+gain2*sum;
				ZXP(out)=output;
			}
		);
	}

	unit->m_phase=phase;
	unit->m_dsamp1=dsamp1;
	unit->m_dsamp2=dsamp2;
	unit->m_dt1=dt1;
	unit->m_dt2=dt2;

	unit->m_numoutput += inNumSamples;
	if (unit->m_numoutput >= unit->m_maxdel) {
		SETCALC(NestedAllpassL_next);
	}
}


void NestedAllpassC_Ctor(NestedAllpassC* unit ) {
	SET2BUF
	unit->m_phase=0;
	unit->m_dsamp1= -(unit->m_dt1 * unit->mRate->mSampleRate);
	unit->m_dsamp2= -(unit->m_dt2 * unit->mRate->mSampleRate);
	SETCALC(NestedAllpassC_next_z);
	ZOUT0(0)=0.f;
}

void NestedAllpassC_Dtor(NestedAllpassC *unit ) {
	RTFree(unit->mWorld, unit->m_buf1);
	RTFree(unit->mWorld, unit->m_buf2);
}

void NestedAllpassC_next(NestedAllpassC *unit, int inNumSamples) {
	float *in=ZIN(0);
	float *out=ZOUT(0);
	float *buf1=unit->m_buf1;
	float *buf2=unit->m_buf2;
	float del1, del2;
	float dt1=unit->m_dt1;
	float dt2=unit->m_dt2;
	float next_dt1=ZIN0(2);
	float next_dt2=ZIN0(5);
	float dsamp1=unit->m_dsamp1;
	float dsamp2=unit->m_dsamp2;
	float gain1=ZIN0(3);
	float gain2=ZIN0(6);
	long phase=unit->m_phase;
	long mask1=unit->m_maxdel1;
	long mask2=unit->m_maxdel2;
	float samp, sum, output, frac1, frac2;

	if (next_dt1 == dt1 && next_dt2 == dt2) {
		int idsamp1;
		int idsamp2;
		LOOP (inNumSamples,
			phase++;
			dsamp1++;
			dsamp2++;
			idsamp1=(int)dsamp1;
			idsamp2=(int)dsamp2;
			frac1=dsamp1-idsamp1;
			frac2=dsamp2-idsamp2;
			samp=ZXP(in);
			float del10=buf1[(idsamp1 + 1) & mask1];
			float del11=buf1[idsamp1 & mask1];
			float del12=buf1[(idsamp1 - 1) & mask1];
			float del13=buf1[(idsamp1 - 2) & mask1];
			del1=cubicinterp(frac1, del10, del11, del12, del13);
			float del20=buf2[(idsamp2 + 1) & mask2];
			float del21=buf2[idsamp2 & mask2];
			float del22=buf2[(idsamp2 - 1) & mask2];
			float del23=buf2[(idsamp2 - 2) & mask2];
			del2=cubicinterp(frac1, del20, del21, del22, del23);
			sum=del2 - gain2*del1;
			output=sum-gain1*samp;
			buf1[phase & mask1]=samp+gain1*output;
			buf2[phase & mask2]=del1+gain2*sum;
			ZXP(out)=output;
		);
	} else {
		float dslope1=CALCSLOPE(next_dt1, dt1);
		float dslope2=CALCSLOPE(next_dt2, dt2);
		LOOP (inNumSamples,
			phase++;
			dt1 += dslope1;
			dt2 += dslope2;
			dsamp1=phase-dt1 * unit->mRate->mSampleRate;
			dsamp2=phase-dt2 * unit->mRate->mSampleRate;
			int idsamp1=(int)dsamp1;
			int idsamp2=(int)dsamp2;
			frac1=dsamp1-idsamp1;
			frac2=dsamp2-idsamp2;
			samp=ZXP(in);
			float del11=buf1[idsamp1 & mask1];
			float del12=buf1[(idsamp1-1) & mask1];
			float del13=buf1[(idsamp1-2) & mask1];
			float del10=buf1[(idsamp1 + 1) & mask1];
			del1=cubicinterp(frac1, del10, del11, del12, del13);
			float del21=buf2[idsamp2 & mask2];
			float del22=buf2[(idsamp2-1) & mask2];
			float del23=buf2[(idsamp2-2) & mask2];
			float del20=buf2[(idsamp2 + 1) & mask2];
			del2=cubicinterp(frac1, del20, del21, del22, del23);
			sum=del2 - gain2*del1;
			output=sum-gain1*samp;
			buf1[phase & mask1]=samp+gain1*output;
			buf2[phase & mask2]=del1+gain2*sum;
			ZXP(out)=output;
		);
	}

	unit->m_phase=phase;
	unit->m_dsamp1=dsamp1;
	unit->m_dsamp2=dsamp2;
	unit->m_dt1=dt1;
	unit->m_dt2=dt2;
}

void NestedAllpassC_next_z(NestedAllpassC *unit, int inNumSamples) {
	float *in=ZIN(0);
	float *out=ZOUT(0);
	float *buf1=unit->m_buf1;
	float *buf2=unit->m_buf2;
	float del1, del2;
	float dt1=unit->m_dt1;
	float dt2=unit->m_dt2;
	float next_dt1=ZIN0(2);
	float next_dt2=ZIN0(5);
	float dsamp1=unit->m_dsamp1;
	float dsamp2=unit->m_dsamp2;
	float gain1=ZIN0(3);
	float gain2=ZIN0(6);
	long phase=unit->m_phase;
	long mask1=unit->m_maxdel1;
	long mask2=unit->m_maxdel2;
	float samp, sum, output, frac1, frac2;

	if (next_dt1 == dt1 && next_dt2 == dt2) {
		int idsamp1;
		int idsamp2;
		LOOP (inNumSamples,
			phase++;
			dsamp1++;
			dsamp2++;
			idsamp1=(int)dsamp1;
			idsamp2=(int)dsamp2;
			frac1=dsamp1-idsamp1;
			frac2=dsamp2-idsamp2;
			samp=ZXP(in);
			float del10=buf1[(idsamp1 + 1) & mask1];
			float del11=buf1[idsamp1 & mask1];
			float del12=buf1[(idsamp1 - 1) & mask1];
			float del13=buf1[(idsamp1 - 2) & mask1];
			del1=cubicinterp(frac1, del10, del11, del12, del13);
			float del20=buf2[(idsamp2 + 1) & mask2];
			float del21=buf2[idsamp2 & mask2];
			float del22=buf2[(idsamp2 - 1) & mask2];
			float del23=buf2[(idsamp2 - 2) & mask2];
			del2=cubicinterp(frac1, del20, del21, del22, del23);
			sum=del2 - gain2*del1;
			output=sum-gain1*samp;
			if (idsamp1 < 0 || idsamp2 < 0) {
				buf1[phase & mask1]=samp;
				buf2[phase & mask2]=samp;
				ZXP(out)=0.f;
			} else {
				buf1[phase & mask1]=samp+gain1*output;
				buf2[phase & mask2]=del1+gain2*sum;
				ZXP(out)=output;
			}
		);
	} else {
		float dslope1=CALCSLOPE(next_dt1, dt1);
		float dslope2=CALCSLOPE(next_dt2, dt2);
		LOOP (inNumSamples,
			phase++;
			dt1 += dslope1;
			dt2 += dslope2;
			dsamp1=phase-dt1 * unit->mRate->mSampleRate;
			dsamp2=phase-dt2 * unit->mRate->mSampleRate;
			int idsamp1=(int)dsamp1;
			int idsamp2=(int)dsamp2;
			frac1=dsamp1-idsamp1;
			frac2=dsamp2-idsamp2;
			samp=ZXP(in);
			float del11=buf1[idsamp1 & mask1];
			float del12=buf1[(idsamp1-1) & mask1];
			float del13=buf1[(idsamp1-2) & mask1];
			float del10=buf1[(idsamp1 + 1) & mask1];
			del1=cubicinterp(frac1, del10, del11, del12, del13);
			float del21=buf2[idsamp2 & mask2];
			float del22=buf2[(idsamp2-1) & mask2];
			float del23=buf2[(idsamp2-2) & mask2];
			float del20=buf2[(idsamp2 + 1) & mask2];
			del2=cubicinterp(frac1, del20, del21, del22, del23);
			sum=del2 - gain2*del1;
			output=sum-gain1*samp;
			if (idsamp1 < 0 || idsamp2 < 0) {
				buf1[phase & mask1]=samp;
				buf2[phase & mask2]=samp;
				ZXP(out)=0.f;
			} else {
				buf1[phase & mask1]=samp+gain1*output;
				buf2[phase & mask2]=del1+gain2*sum;
				ZXP(out)=output;
			}
		);
	}

	unit->m_phase=phase;
	unit->m_dsamp1=dsamp1;
	unit->m_dsamp2=dsamp2;
	unit->m_dt1=dt1;
	unit->m_dt2=dt2;

	unit->m_numoutput += inNumSamples;
	if (unit->m_numoutput >= unit->m_maxdel) {
		SETCALC(NestedAllpassC_next);
	}
}

void DoubleNestedAllpassN_Ctor(DoubleNestedAllpassN* unit ) {
	SET3BUF
	unit->m_phase=0;
	unit->m_dsamp1= -(long)(unit->m_dt1 * unit->mRate->mSampleRate);
	unit->m_dsamp2= -(long)(unit->m_dt2 * unit->mRate->mSampleRate);
	unit->m_dsamp3= -(long)(unit->m_dt3 * unit->mRate->mSampleRate);
	SETCALC(DoubleNestedAllpassN_next_z);
	ZOUT0(0)=0.f;
}

void DoubleNestedAllpassN_Dtor(DoubleNestedAllpassN *unit ) {
	RTFree(unit->mWorld, unit->m_buf1);
	RTFree(unit->mWorld, unit->m_buf2);
	RTFree(unit->mWorld, unit->m_buf3);
}

void DoubleNestedAllpassN_next(DoubleNestedAllpassN *unit, int inNumSamples) {
	float *in=ZIN(0);
	float *out=ZOUT(0);
	float *buf1=unit->m_buf1;
	float *buf2=unit->m_buf2;
	float *buf3=unit->m_buf3;
	float del1;
	float dt1=unit->m_dt1;
	float dt2=unit->m_dt2;
	float dt3=unit->m_dt3;
	float next_dt1=ZIN0(2);
	float next_dt2=ZIN0(5);
	float next_dt3=ZIN0(8);
	long dsamp1=unit->m_dsamp1;
	long dsamp2=unit->m_dsamp2;
	long dsamp3=unit->m_dsamp3;
	float gain1=ZIN0(3);
	float gain2=ZIN0(6);
	float gain3=ZIN0(9);
	long phase=unit->m_phase;
	long mask1=unit->m_maxdel1;
	long mask2=unit->m_maxdel2;
	long mask3=unit->m_maxdel3;
	float samp, sum1, sum2, output;

	if (next_dt1 == dt1 && next_dt2 == dt2 && next_dt3 == dt3) {
		LOOP (inNumSamples,
			phase++;
			dsamp1++;
			dsamp2++;
			dsamp3++;
			samp=ZXP(in);
			del1=buf1[dsamp1 & mask1];
			sum1=buf2[dsamp2 & mask2]-gain2*del1;
			sum2=buf3[dsamp3 & mask3]-gain3*sum1;
			output=sum2-gain1*samp;
			buf1[phase & mask1]=samp+gain1*output;
			buf2[phase & mask2]=del1+gain2*sum1;
			buf3[phase & mask3]=sum1+gain3*sum2;
			ZXP(out)=output;
		);
	} else {
		float dslope1=CALCSLOPE(next_dt1, dt1);
		float dslope2=CALCSLOPE(next_dt2, dt2);
		float dslope3=CALCSLOPE(next_dt3, dt3);
		LOOP (inNumSamples,
			phase++;
			dt1 += dslope1;
			dt2 += dslope2;
			dt3 += dslope3;
			dsamp1=phase-(long)(dt1 * unit->mRate->mSampleRate);
			dsamp2=phase-(long)(dt2 * unit->mRate->mSampleRate);
			dsamp3=phase-(long)(dt3 * unit->mRate->mSampleRate);
			samp=ZXP(in);
			del1=buf1[dsamp1 & mask1];
			sum1=buf2[dsamp2 & mask2]-gain2*del1;
			sum2=buf3[dsamp3 & mask3]-gain3*sum1;
			output=sum2-gain1*samp;
			buf1[phase & mask1]=samp+gain1*output;
			buf2[phase & mask2]=del1+gain2*sum1;
			buf3[phase & mask3]=sum1+gain3*sum2;
			ZXP(out)=output;
		);
	}

	unit->m_phase=phase;
	unit->m_dsamp1=dsamp1;
	unit->m_dsamp2=dsamp2;
	unit->m_dsamp3=dsamp3;
	unit->m_dt1=dt1;
	unit->m_dt2=dt2;
	unit->m_dt3=dt3;
}

void DoubleNestedAllpassN_next_z(DoubleNestedAllpassN *unit, int inNumSamples) {
	float *in=ZIN(0);
	float *out=ZOUT(0);
	float *buf1=unit->m_buf1;
	float *buf2=unit->m_buf2;
	float *buf3=unit->m_buf3;
	float del1;
	float dt1=unit->m_dt1;
	float dt2=unit->m_dt2;
	float dt3=unit->m_dt3;
	float next_dt1=ZIN0(2);
	float next_dt2=ZIN0(5);
	float next_dt3=ZIN0(8);
	long dsamp1=unit->m_dsamp1;
	long dsamp2=unit->m_dsamp2;
	long dsamp3=unit->m_dsamp3;
	float gain1=ZIN0(3);
	float gain2=ZIN0(6);
	float gain3=ZIN0(9);
	long phase=unit->m_phase;
	long mask1=unit->m_maxdel1;
	long mask2=unit->m_maxdel2;
	long mask3=unit->m_maxdel3;
	float samp, sum1, sum2, output;

	if (next_dt1 == dt1 && next_dt2 == dt2 && next_dt3 == dt3) {
		LOOP (inNumSamples,
			phase++;
			dsamp1++;
			dsamp2++;
			dsamp3++;
			samp=ZXP(in);
			del1=buf1[dsamp1 & mask1];
			sum1=buf2[dsamp2 & mask2]-gain2*del1;
			sum2=buf3[dsamp3 & mask3]-gain3*sum1;
			output=sum2-gain1*samp;
			if (dsamp1 < 0 || dsamp2 < 0 || dsamp3 < 0) {
				buf1[phase & mask1]=samp;
				buf2[phase & mask2]=samp;
				buf3[phase & mask3]=samp;
				ZXP(out)=0.f;
			} else {
				buf1[phase & mask1]=samp+gain1*output;
				buf2[phase & mask2]=del1+gain2*sum1;
				buf3[phase & mask3]=sum1+gain3*sum2;
				ZXP(out)=output;
			}
		);
	} else {
		float dslope1=CALCSLOPE(next_dt1, dt1);
		float dslope2=CALCSLOPE(next_dt2, dt2);
		float dslope3=CALCSLOPE(next_dt3, dt3);
		LOOP (inNumSamples,
			phase++;
			dt1 += dslope1;
			dt2 += dslope2;
			dt3 += dslope3;
			dsamp1=phase-(long)(dt1 * unit->mRate->mSampleRate);
			dsamp2=phase-(long)(dt2 * unit->mRate->mSampleRate);
			dsamp3=phase-(long)(dt3 * unit->mRate->mSampleRate);
			samp=ZXP(in);
			del1=buf1[dsamp1 & mask1];
			sum1=buf2[dsamp2 & mask2]-gain2*del1;
			sum2=buf3[dsamp3 & mask3]-gain3*sum1;
			output=sum2-gain1*samp;
			if (dsamp1 < 0 || dsamp2 < 0 || dsamp3 < 0) {
				buf1[phase & mask1]=samp;
				buf2[phase & mask2]=samp;
				buf3[phase & mask3]=samp;
				ZXP(out)=0.f;
			} else {
				buf1[phase & mask1]=samp+gain1*output;
				buf2[phase & mask2]=del1+gain2*sum1;
				buf3[phase & mask3]=sum1+gain3*sum2;
				ZXP(out)=output;
			}
		);
	}

	unit->m_phase=phase;
	unit->m_dsamp1=dsamp1;
	unit->m_dsamp2=dsamp2;
	unit->m_dsamp3=dsamp3;
	unit->m_dt1=dt1;
	unit->m_dt2=dt2;
	unit->m_dt3=dt3;

	unit->m_numoutput += inNumSamples;
	if (unit->m_numoutput >= unit->m_maxdel) {
		SETCALC(DoubleNestedAllpassN_next);
	}
}

void DoubleNestedAllpassL_Ctor(DoubleNestedAllpassL* unit ) {
	SET3BUF
	unit->m_phase=0;
	unit->m_dsamp1= -unit->m_dt1 * unit->mRate->mSampleRate;
	unit->m_dsamp2= -unit->m_dt2 * unit->mRate->mSampleRate;
	unit->m_dsamp3= -unit->m_dt3 * unit->mRate->mSampleRate;
	SETCALC(DoubleNestedAllpassL_next_z);
	ZOUT0(0)=0.f;
}

void DoubleNestedAllpassL_Dtor(DoubleNestedAllpassL *unit ) {
	RTFree(unit->mWorld, unit->m_buf1);
	RTFree(unit->mWorld, unit->m_buf2);
	RTFree(unit->mWorld, unit->m_buf3);
}

void DoubleNestedAllpassL_next(DoubleNestedAllpassL *unit, int inNumSamples) {
	float *in=ZIN(0);
	float *out=ZOUT(0);
	float *buf1=unit->m_buf1;
	float *buf2=unit->m_buf2;
	float *buf3=unit->m_buf3;
	float del1;
	float dt1=unit->m_dt1;
	float dt2=unit->m_dt2;
	float dt3=unit->m_dt3;
	float next_dt1=ZIN0(2);
	float next_dt2=ZIN0(5);
	float next_dt3=ZIN0(8);
	float dsamp1=unit->m_dsamp1;
	float dsamp2=unit->m_dsamp2;
	float dsamp3=unit->m_dsamp3;
	float gain1=ZIN0(3);
	float gain2=ZIN0(6);
	float gain3=ZIN0(9);
	long phase=unit->m_phase;
	long mask1=unit->m_maxdel1;
	long mask2=unit->m_maxdel2;
	long mask3=unit->m_maxdel3;
	float samp, sum1, sum2, output;

	if (next_dt1 == dt1 && next_dt2 == dt2 && next_dt3 == dt3) {
		int idsamp1;
		int idsamp2;
		int idsamp3;
		LOOP (inNumSamples,
			phase++;
			dsamp1++;
			dsamp2++;
			dsamp3++;
			idsamp1=(int)dsamp1;
			float frac1=dsamp1-idsamp1;
			idsamp2=(int)dsamp2;
			float frac2=dsamp2-idsamp2;
			idsamp3=(int)dsamp3;
			float frac3=dsamp3-idsamp3;
			samp=ZXP(in);
			float del10=buf1[idsamp1 & mask1];
			float del20=buf2[idsamp2 & mask2];
			float del30=buf3[idsamp3 & mask3];
			float del11=buf1[(idsamp1-1) & mask1];
			float del21=buf2[(idsamp2-1) & mask2];
			float del31=buf3[(idsamp3-1) & mask3];
			del1=lininterp(frac1, del10, del11);
			sum1=lininterp(frac2, del20, del21)-gain2*del1;
			sum2=lininterp(frac3, del30, del31)-gain3*sum1;
			output=sum2-gain1*samp;
			buf1[phase & mask1]=samp+gain1*output;
			buf2[phase & mask2]=del1+gain2*sum1;
			buf3[phase & mask3]=sum1+gain3*sum2;
			ZXP(out)=output;
		);
	} else {
		float dslope1=CALCSLOPE(next_dt1, dt1);
		float dslope2=CALCSLOPE(next_dt2, dt2);
		float dslope3=CALCSLOPE(next_dt3, dt3);
		LOOP (inNumSamples,
			phase++;
			dt1 += dslope1;
			dt2 += dslope2;
			dt3 += dslope3;
			dsamp1=phase-(long)(dt1 * unit->mRate->mSampleRate);
			dsamp2=phase-(long)(dt2 * unit->mRate->mSampleRate);
			dsamp3=phase-(long)(dt3 * unit->mRate->mSampleRate);
			int idsamp1=(int)dsamp1;
			float frac1=dsamp1-idsamp1;
			int idsamp2=(int)dsamp2;
			float frac2=dsamp2-idsamp2;
			int idsamp3=(int)dsamp3;
			float frac3=dsamp3-idsamp3;
			samp=ZXP(in);
			del1=lininterp(frac1, buf1[idsamp1 & mask1], buf1[(idsamp1-1) & mask1]);
			sum1=lininterp(frac2, buf2[idsamp2 & mask2], buf2[(idsamp2-1) & mask2])-gain2*del1;
			sum2=lininterp(frac3, buf3[idsamp3 & mask3], buf3[(idsamp3-1) & mask3])-gain3*sum1;
			output=sum2-gain1*samp;
			buf1[phase & mask1]=samp+gain1*output;
			buf2[phase & mask2]=del1+gain2*sum1;
			buf3[phase & mask3]=sum1+gain3*sum2;
			ZXP(out)=output;
		);
	}

	unit->m_phase=phase;
	unit->m_dsamp1=dsamp1;
	unit->m_dsamp2=dsamp2;
	unit->m_dsamp3=dsamp3;
	unit->m_dt1=dt1;
	unit->m_dt2=dt2;
	unit->m_dt3=dt3;
}

void DoubleNestedAllpassL_next_z(DoubleNestedAllpassL *unit, int inNumSamples) {
	float *in=ZIN(0);
	float *out=ZOUT(0);
	float *buf1=unit->m_buf1;
	float *buf2=unit->m_buf2;
	float *buf3=unit->m_buf3;
	float del1;
	float dt1=unit->m_dt1;
	float dt2=unit->m_dt2;
	float dt3=unit->m_dt3;
	float next_dt1=ZIN0(2);
	float next_dt2=ZIN0(5);
	float next_dt3=ZIN0(8);
	float dsamp1=unit->m_dsamp1;
	float dsamp2=unit->m_dsamp2;
	float dsamp3=unit->m_dsamp3;
	float gain1=ZIN0(3);
	float gain2=ZIN0(6);
	float gain3=ZIN0(9);
	long phase=unit->m_phase;
	long mask1=unit->m_maxdel1;
	long mask2=unit->m_maxdel2;
	long mask3=unit->m_maxdel3;
	float samp, sum1, sum2, output;

	if (next_dt1 == dt1 && next_dt2 == dt2 && next_dt3 == dt3) {
		int idsamp1;
		int idsamp2;
		int idsamp3;
		LOOP (inNumSamples,
			phase++;
			dsamp1++;
			dsamp2++;
			dsamp3++;
			idsamp1=(int)dsamp1;
			float frac1=dsamp1-idsamp1;
			idsamp2=(int)dsamp2;
			float frac2=dsamp2-idsamp2;
			idsamp3=(int)dsamp3;
			float frac3=dsamp3-idsamp3;
			int idsamp11=idsamp1-1;
			int idsamp21=idsamp2-1;
			int idsamp31=idsamp3-1;
			samp=ZXP(in);
			float del10=buf1[idsamp1 & mask1];
			float del20=buf2[idsamp2 & mask2];
			float del30=buf3[idsamp3 & mask3];
			float del11=buf1[idsamp11 & mask1];
			float del21=buf2[idsamp21 & mask2];
			float del31=buf3[idsamp31 & mask3];
			del1=lininterp(frac1, del10, del11);
			sum1=lininterp(frac2, del20, del21)-gain2*del1;
			sum2=lininterp(frac3, del30, del31)-gain3*sum1;
			output=sum2-gain1*samp;
			if (idsamp11 < 0 || idsamp21 < 0 || idsamp31 < 0) {
				buf1[phase & mask1]=samp;
				buf2[phase & mask2]=samp;
				buf3[phase & mask3]=samp;
				ZXP(out)=0.f;
			} else {
				buf1[phase & mask1]=samp+gain1*output;
				buf2[phase & mask2]=del1+gain2*sum1;
				buf3[phase & mask3]=sum1+gain3*sum2;
				ZXP(out)=output;
			}
		);
	} else {
		float dslope1=CALCSLOPE(next_dt1, dt1);
		float dslope2=CALCSLOPE(next_dt2, dt2);
		float dslope3=CALCSLOPE(next_dt3, dt3);
		LOOP (inNumSamples,
			phase++;
			dt1 += dslope1;
			dt2 += dslope2;
			dt3 += dslope3;
			dsamp1=phase-(long)(dt1 * unit->mRate->mSampleRate);
			dsamp2=phase-(long)(dt2 * unit->mRate->mSampleRate);
			dsamp3=phase-(long)(dt3 * unit->mRate->mSampleRate);
			int idsamp1=(int)dsamp1;
			float frac1=dsamp1-idsamp1;
			int idsamp2=(int)dsamp2;
			float frac2=dsamp2-idsamp2;
			int idsamp3=(int)dsamp3;
			float frac3=dsamp3-idsamp3;
			int idsamp11=idsamp1-1;
			int idsamp21=idsamp2-1;
			int idsamp31=idsamp3-1;
			samp=ZXP(in);
			del1=lininterp(frac1, buf1[idsamp1 & mask1], buf1[idsamp11 & mask1]);
			sum1=lininterp(frac2, buf2[idsamp2 & mask2], buf2[idsamp21 & mask2])-gain2*del1;
			sum2=lininterp(frac3, buf3[idsamp3 & mask3], buf3[idsamp31 & mask3])-gain3*sum1;
			output=sum2-gain1*samp;
			if (idsamp11 < 0 || idsamp21 < 0 || idsamp31 < 0) {
				buf1[phase & mask1]=samp;
				buf2[phase & mask2]=samp;
				buf3[phase & mask3]=samp;
				ZXP(out)=0.f;
			} else {
				buf1[phase & mask1]=samp+gain1*output;
				buf2[phase & mask2]=del1+gain2*sum1;
				buf3[phase & mask3]=sum1+gain3*sum2;
				ZXP(out)=output;
			}
		);
	}

	unit->m_phase=phase;
	unit->m_dsamp1=dsamp1;
	unit->m_dsamp2=dsamp2;
	unit->m_dsamp3=dsamp3;
	unit->m_dt1=dt1;
	unit->m_dt2=dt2;
	unit->m_dt3=dt3;

	unit->m_numoutput += inNumSamples;
	if (unit->m_numoutput >= unit->m_maxdel) {
		SETCALC(DoubleNestedAllpassL_next);
	}
}

void DoubleNestedAllpassC_Ctor(DoubleNestedAllpassC* unit ) {
	SET3BUF
	unit->m_phase=0;
	unit->m_dsamp1= -unit->m_dt1 * unit->mRate->mSampleRate;
	unit->m_dsamp2= -unit->m_dt2 * unit->mRate->mSampleRate;
	unit->m_dsamp3= -unit->m_dt3 * unit->mRate->mSampleRate;
	SETCALC(DoubleNestedAllpassC_next_z);
	ZOUT0(0)=0.f;
}

void DoubleNestedAllpassC_Dtor(DoubleNestedAllpassC *unit ) {
	RTFree(unit->mWorld, unit->m_buf1);
	RTFree(unit->mWorld, unit->m_buf2);
	RTFree(unit->mWorld, unit->m_buf3);
}

void DoubleNestedAllpassC_next(DoubleNestedAllpassC *unit, int inNumSamples) {
	float *in=ZIN(0);
	float *out=ZOUT(0);
	float *buf1=unit->m_buf1;
	float *buf2=unit->m_buf2;
	float *buf3=unit->m_buf3;
	float del1;
	float dt1=unit->m_dt1;
	float dt2=unit->m_dt2;
	float dt3=unit->m_dt3;
	float next_dt1=ZIN0(2);
	float next_dt2=ZIN0(5);
	float next_dt3=ZIN0(8);
	float dsamp1=unit->m_dsamp1;
	float dsamp2=unit->m_dsamp2;
	float dsamp3=unit->m_dsamp3;
	float gain1=ZIN0(3);
	float gain2=ZIN0(6);
	float gain3=ZIN0(9);
	long phase=unit->m_phase;
	long mask1=unit->m_maxdel1;
	long mask2=unit->m_maxdel2;
	long mask3=unit->m_maxdel3;
	float samp, sum1, sum2, output;

	if (next_dt1 == dt1 && next_dt2 == dt2 && next_dt3 == dt3) {
		int idsamp1;
		int idsamp2;
		int idsamp3;
		LOOP (inNumSamples,
			phase++;
			dsamp1++;
			dsamp2++;
			dsamp3++;
			idsamp1=(int)dsamp1;
			float frac1=dsamp1-idsamp1;
			idsamp2=(int)dsamp2;
			float frac2=dsamp2-idsamp2;
			idsamp3=(int)dsamp3;
			float frac3=dsamp3-idsamp3;
			samp=ZXP(in);
			float del10=buf1[(idsamp1+1) & mask1];
			float del20=buf2[(idsamp2+1) & mask2];
			float del30=buf3[(idsamp3+1) & mask3];
			float del11=buf1[(idsamp1) & mask1];
			float del21=buf2[(idsamp2) & mask2];
			float del31=buf3[(idsamp3) & mask3];
			float del12=buf1[(idsamp1-1) & mask1];
			float del22=buf2[(idsamp2-1) & mask2];
			float del32=buf3[(idsamp3-1) & mask3];
			float del13=buf1[(idsamp1-2) & mask1];
			float del23=buf2[(idsamp2-2) & mask2];
			float del33=buf3[(idsamp3-2) & mask3];
			del1=cubicinterp(frac1, del10, del11, del12, del13);
			sum1=cubicinterp(frac2, del20, del21, del22, del23)-gain2*del1;
			sum2=cubicinterp(frac3, del30, del31, del32, del33)-gain3*sum1;
			output=sum2-gain1*samp;
			buf1[phase & mask1]=samp+gain1*output;
			buf2[phase & mask2]=del1+gain2*sum1;
			buf3[phase & mask3]=sum1+gain3*sum2;
			ZXP(out)=output;
		);
	} else {
		float dslope1=CALCSLOPE(next_dt1, dt1);
		float dslope2=CALCSLOPE(next_dt2, dt2);
		float dslope3=CALCSLOPE(next_dt3, dt3);
		LOOP (inNumSamples,
			phase++;
			dt1 += dslope1;
			dt2 += dslope2;
			dt3 += dslope3;
			dsamp1=phase-(long)(dt1 * unit->mRate->mSampleRate);
			dsamp2=phase-(long)(dt2 * unit->mRate->mSampleRate);
			dsamp3=phase-(long)(dt3 * unit->mRate->mSampleRate);
			int idsamp1=(int)dsamp1;
			float frac1=dsamp1-idsamp1;
			int idsamp2=(int)dsamp2;
			float frac2=dsamp2-idsamp2;
			int idsamp3=(int)dsamp3;
			float frac3=dsamp3-idsamp3;
			samp=ZXP(in);
			del1=cubicinterp(frac1, buf1[(idsamp1 + 1) & mask1], buf1[idsamp1 & mask1], buf1[(idsamp1-1) & mask1], buf1[(idsamp1-2) & mask1]);
			sum1=cubicinterp(frac2, buf2[(idsamp2 + 1) & mask2], buf2[idsamp2 & mask2], buf2[(idsamp2-1) & mask2], buf2[(idsamp2-2) & mask2])-gain2*del1;
			sum2=cubicinterp(frac3, buf3[(idsamp3 + 1) & mask3], buf3[idsamp3 & mask3], buf3[(idsamp3-1) & mask3], buf3[(idsamp3-2) & mask3])-gain3*sum1;
			output=sum2-gain1*samp;
			buf1[phase & mask1]=samp+gain1*output;
			buf2[phase & mask2]=del1+gain2*sum1;
			buf3[phase & mask3]=sum1+gain3*sum2;
			ZXP(out)=output;
		);
	}

	unit->m_phase=phase;
	unit->m_dsamp1=dsamp1;
	unit->m_dsamp2=dsamp2;
	unit->m_dsamp3=dsamp3;
	unit->m_dt1=dt1;
	unit->m_dt2=dt2;
	unit->m_dt3=dt3;
}

void DoubleNestedAllpassC_next_z(DoubleNestedAllpassC *unit, int inNumSamples) {
	float *in=ZIN(0);
	float *out=ZOUT(0);
	float *buf1=unit->m_buf1;
	float *buf2=unit->m_buf2;
	float *buf3=unit->m_buf3;
	float del1;
	float dt1=unit->m_dt1;
	float dt2=unit->m_dt2;
	float dt3=unit->m_dt3;
	float next_dt1=ZIN0(2);
	float next_dt2=ZIN0(5);
	float next_dt3=ZIN0(8);
	float dsamp1=unit->m_dsamp1;
	float dsamp2=unit->m_dsamp2;
	float dsamp3=unit->m_dsamp3;
	float gain1=ZIN0(3);
	float gain2=ZIN0(6);
	float gain3=ZIN0(9);
	long phase=unit->m_phase;
	long mask1=unit->m_maxdel1;
	long mask2=unit->m_maxdel2;
	long mask3=unit->m_maxdel3;
	float samp, sum1, sum2, output;

	if (next_dt1 == dt1 && next_dt2 == dt2 && next_dt3 == dt3) {
		int idsamp1;
		int idsamp2;
		int idsamp3;
		LOOP (inNumSamples,
			phase++;
			dsamp1++;
			dsamp2++;
			dsamp3++;
			idsamp1=(int)dsamp1;
			float frac1=dsamp1-idsamp1;
			idsamp2=(int)dsamp2;
			float frac2=dsamp2-idsamp2;
			idsamp3=(int)dsamp3;
			float frac3=dsamp3-idsamp3;
			int idsamp13=idsamp1-2;
			int idsamp23=idsamp2-2;
			int idsamp33=idsamp3-2;
			samp=ZXP(in);
			float del10=buf1[(idsamp1+1) & mask1];
			float del20=buf2[(idsamp2+1) & mask2];
			float del30=buf3[(idsamp3+1) & mask3];
			float del11=buf1[(idsamp1) & mask1];
			float del21=buf2[(idsamp2) & mask2];
			float del31=buf3[(idsamp3) & mask3];
			float del12=buf1[(idsamp1-1) & mask1];
			float del22=buf2[(idsamp2-1) & mask2];
			float del32=buf3[(idsamp3-1) & mask3];
			float del13=buf1[idsamp13 & mask1];
			float del23=buf2[idsamp23 & mask2];
			float del33=buf3[idsamp33 & mask3];
			del1=cubicinterp(frac1, del10, del11, del12, del13);
			sum1=cubicinterp(frac2, del20, del21, del22, del23)-gain2*del1;
			sum2=cubicinterp(frac3, del30, del31, del32, del33)-gain3*sum1;
			output=sum2-gain1*samp;
			if (idsamp13 < 0 || idsamp23 < 0 || idsamp33 < 0) {
				buf1[phase & mask1]=samp;
				buf2[phase & mask2]=samp;
				buf3[phase & mask3]=samp;
				ZXP(out)=0.f;
			} else {
				buf1[phase & mask1]=samp+gain1*output;
				buf2[phase & mask2]=del1+gain2*sum1;
				buf3[phase & mask3]=sum1+gain3*sum2;
				ZXP(out)=output;
			}
		);
	} else {
		float dslope1=CALCSLOPE(next_dt1, dt1);
		float dslope2=CALCSLOPE(next_dt2, dt2);
		float dslope3=CALCSLOPE(next_dt3, dt3);
		LOOP (inNumSamples,
			phase++;
			dt1 += dslope1;
			dt2 += dslope2;
			dt3 += dslope3;
			dsamp1=phase-(long)(dt1 * unit->mRate->mSampleRate);
			dsamp2=phase-(long)(dt2 * unit->mRate->mSampleRate);
			dsamp3=phase-(long)(dt3 * unit->mRate->mSampleRate);
			int idsamp1=(int)dsamp1;
			float frac1=dsamp1-idsamp1;
			int idsamp2=(int)dsamp2;
			float frac2=dsamp2-idsamp2;
			int idsamp3=(int)dsamp3;
			float frac3=dsamp3-idsamp3;
			int idsamp13=idsamp1-2;
			int idsamp23=idsamp2-2;
			int idsamp33=idsamp3-2;
			samp=ZXP(in);
			del1=cubicinterp(frac1, buf1[(idsamp1 + 1) & mask1], buf1[idsamp1 & mask1], buf1[(idsamp1-1) & mask1], buf1[idsamp13 & mask1]);
			sum1=cubicinterp(frac2, buf2[(idsamp2 + 1) & mask2], buf2[idsamp2 & mask2], buf2[(idsamp2-1) & mask2], buf2[idsamp23 & mask2])-gain2*del1;
			sum2=cubicinterp(frac3, buf3[(idsamp3 + 1) & mask3], buf3[idsamp3 & mask3], buf3[(idsamp3-1) & mask3], buf3[idsamp33 & mask3])-gain3*sum1;
			output=sum2-gain1*samp;
			if (idsamp13 < 0 || idsamp23 < 0 || idsamp33 < 0) {
				buf1[phase & mask1]=samp;
				buf2[phase & mask2]=samp;
				buf3[phase & mask3]=samp;
				ZXP(out)=0.f;
			} else {
				buf1[phase & mask1]=samp+gain1*output;
				buf2[phase & mask2]=del1+gain2*sum1;
				buf3[phase & mask3]=sum1+gain3*sum2;
				ZXP(out)=output;
			}
		);
	}

	unit->m_phase=phase;
	unit->m_dsamp1=dsamp1;
	unit->m_dsamp2=dsamp2;
	unit->m_dsamp3=dsamp3;
	unit->m_dt1=dt1;
	unit->m_dt2=dt2;
	unit->m_dt3=dt3;

	unit->m_numoutput += inNumSamples;
	if (unit->m_numoutput >= unit->m_maxdel) {
		SETCALC(DoubleNestedAllpassC_next);
	}
}

#define DefineFilterDelay(name) \
	(*ft->fDefineUnit)(#name, sizeof(name), (UnitCtorFunc)&name##_Ctor, \
	(UnitDtorFunc)&FilterDelay_Dtor, 0);

PluginLoad(BhobFilt)
{
	ft = inTable;

	DefineSimpleUnit(MoogLadder);
	DefineSimpleUnit(RLPFD);
	DefineSimpleUnit(MultiFilt);
	DefineDtorUnit(NestedAllpassN);
	DefineDtorUnit(NestedAllpassL);
	DefineDtorUnit(NestedAllpassC);
	DefineDtorUnit(DoubleNestedAllpassN);
	DefineDtorUnit(DoubleNestedAllpassL);
	DefineDtorUnit(DoubleNestedAllpassC);
	DefineFilterDelay(Streson);
	DefineFilterDelay(NLFiltN);
	DefineFilterDelay(NLFiltL);
	DefineFilterDelay(NLFiltC);
}

////////////////////////////////////////////////////////////////////
