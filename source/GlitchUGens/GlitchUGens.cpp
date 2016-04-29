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

//after sc3.4 HPF/RHPF were changed to use double-precision floats (svn revision: 10300 - 8sep2010).  and later also BPF, BRF.
//GlitchHPF/GlitchRHPF/GlitchBPF/GlitchBRF can all be used as drop-in replacements to get the old behaviour back.

#include "SC_PlugIn.h"

static InterfaceTable *ft;

struct GlitchRHPF : public Unit
{
	float m_y1, m_y2, m_a0, m_b1, m_b2, m_freq, m_reson;
};

struct GlitchHPF : public Unit
{
	float m_y1, m_y2, m_a0, m_b1, m_b2, m_freq;
};

struct GlitchBPF : public Unit
{
	float m_y1, m_y2, m_a0, m_b1, m_b2, m_freq, m_bw;
};

struct GlitchBRF : public Unit
{
	float m_y1, m_y2, m_a0, m_a1, m_b2, m_freq, m_bw;
};

//////////////////////////////////////////////////////////////////////////////////////////////////


extern "C"
{
	void GlitchRHPF_next(GlitchRHPF *unit, int inNumSamples);
	void GlitchRHPF_next_1(GlitchRHPF *unit, int inNumSamples);
	void GlitchRHPF_Ctor(GlitchRHPF* unit);

	void GlitchHPF_next(GlitchHPF *unit, int inNumSamples);
	void GlitchHPF_next_1(GlitchHPF *unit, int inNumSamples);
	void GlitchHPF_Ctor(GlitchHPF* unit);

    void GlitchBPF_next(GlitchBPF *unit, int inNumSamples);
	void GlitchBPF_next_1(GlitchBPF *unit, int inNumSamples);
	void GlitchBPF_Ctor(GlitchBPF* unit);

	void GlitchBRF_next(GlitchBRF *unit, int inNumSamples);
	void GlitchBRF_next_1(GlitchBRF *unit, int inNumSamples);
	void GlitchBRF_Ctor(GlitchBRF* unit);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void GlitchRHPF_Ctor(GlitchRHPF* unit)
{
	//printf("GlitchRHPF_Reset\n");
	if (unit->mBufLength == 1) {
		SETCALC(GlitchRHPF_next_1);
	} else {
		SETCALC(GlitchRHPF_next);
	};
	unit->m_a0 = 0.f;
	unit->m_b1 = 0.f;
	unit->m_b2 = 0.f;
	unit->m_y1 = 0.f;
	unit->m_y2 = 0.f;
	unit->m_freq = 0.f;
	unit->m_reson = 0.f;
	ZOUT0(0) = 0.f;
}


void GlitchRHPF_next(GlitchRHPF* unit, int inNumSamples)
{
	//printf("GlitchRHPFs_next\n");

	float *out = ZOUT(0);
	float *in = ZIN(0);
	float freq = ZIN0(1);
	float reson = ZIN0(2);

	float y0;
	float y1 = unit->m_y1;
	float y2 = unit->m_y2;
	float a0 = unit->m_a0;
	float b1 = unit->m_b1;
	float b2 = unit->m_b2;

	if (freq != unit->m_freq || reson != unit->m_reson) {

		float qres = sc_max(0.001f, reson);
		float pfreq = freq * unit->mRate->mRadiansPerSample;

		float D = tan(pfreq * qres * 0.5f);
		float C = ((1.f-D)/(1.f+D));
		float cosf = cos(pfreq);

		float next_b1 = (1.f + C) * cosf;
		float next_b2 = -C;
		float next_a0 = (1.f + C + next_b1) * .25f;

		//post("%g %g %g   %g %g   %g %g %g   %g %g\n", *freq, pfreq, qres, D, C, cosf, next_b1, next_b2, next_a0, y1, y2);

		float a0_slope = (next_a0 - a0) * unit->mRate->mFilterSlope;
		float b1_slope = (next_b1 - b1) * unit->mRate->mFilterSlope;
		float b2_slope = (next_b2 - b2) * unit->mRate->mFilterSlope;
		LOOP(unit->mRate->mFilterLoops,
			 y0 = a0 * ZXP(in) + b1 * y1 + b2 * y2;
			 ZXP(out) = y0 - 2.f * y1 + y2;

			 y2 = a0 * ZXP(in) + b1 * y0 + b2 * y1;
			 ZXP(out) = y2 - 2.f * y0 + y1;

			 y1 = a0 * ZXP(in) + b1 * y2 + b2 * y0;
			 ZXP(out) = y1 - 2.f * y2 + y0;

			 a0 += a0_slope;
			 b1 += b1_slope;
			 b2 += b2_slope;
			 );
		LOOP(unit->mRate->mFilterRemain,
			 y0 = a0 * ZXP(in) + b1 * y1 + b2 * y2;
			 ZXP(out) = y0 - 2.f * y1 + y2;
			 y2 = y1;
			 y1 = y0;
			 );

		unit->m_freq = freq;
		unit->m_reson = reson;
		unit->m_a0 = a0;
		unit->m_b1 = b1;
		unit->m_b2 = b2;
	} else {
		LOOP(unit->mRate->mFilterLoops,
			 y0 = a0 * ZXP(in) + b1 * y1 + b2 * y2;
			 ZXP(out) = y0 - 2.f * y1 + y2;

			 y2 = a0 * ZXP(in) + b1 * y0 + b2 * y1;
			 ZXP(out) = y2 - 2.f * y0 + y1;

			 y1 = a0 * ZXP(in) + b1 * y2 + b2 * y0;
			 ZXP(out) = y1 - 2.f * y2 + y0;
			 );
		LOOP(unit->mRate->mFilterRemain,
			 y0 = a0 * ZXP(in) + b1 * y1 + b2 * y2;
			 ZXP(out) = y0 - 2.f * y1 + y2;
			 y2 = y1;
			 y1 = y0;
			 );
	}
	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}

void GlitchRHPF_next_1(GlitchRHPF* unit, int inNumSamples)
{
	//printf("GlitchRHPFs_next_1\n");

	float in = ZIN0(0);
	float freq = ZIN0(1);
	float reson = ZIN0(2);

	float y0;
	float y1 = unit->m_y1;
	float y2 = unit->m_y2;
	float a0 = unit->m_a0;
	float b1 = unit->m_b1;
	float b2 = unit->m_b2;

	if (freq != unit->m_freq || reson != unit->m_reson) {

		float qres = sc_max(0.001f, reson);
		float pfreq = freq * unit->mRate->mRadiansPerSample;

		float D = tan(pfreq * qres * 0.5f);
		float C = ((1.f-D)/(1.f+D));
		float cosf = cos(pfreq);

		b1 = (1.f + C) * cosf;
		b2 = -C;
		a0 = (1.f + C + b1) * .25f;

		y0 = a0 * in + b1 * y1 + b2 * y2;
		ZOUT0(0) = y0 - 2.f * y1 + y2;
		y2 = y1;
		y1 = y0;

		unit->m_freq = freq;
		unit->m_reson = reson;
		unit->m_a0 = a0;
		unit->m_b1 = b1;
		unit->m_b2 = b2;
	} else {
		y0 = a0 * in + b1 * y1 + b2 * y2;
		ZOUT0(0) = y0 - 2.f * y1 + y2;
		y2 = y1;
		y1 = y0;
	}
	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);

}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void GlitchHPF_Ctor(GlitchHPF* unit)
{
	//printf("GlitchHPF_Reset\n");
	if (unit->mBufLength == 1) {
		SETCALC(GlitchHPF_next_1);
	} else {
		SETCALC(GlitchHPF_next);
	};
	unit->m_a0 = 0.f;
	unit->m_b1 = 0.f;
	unit->m_b2 = 0.f;
	unit->m_y1 = 0.f;
	unit->m_y2 = 0.f;
	unit->m_freq = -1e6f;
	ZOUT0(0) = 0.f;
}


void GlitchHPF_next(GlitchHPF* unit, int inNumSamples)
{
	//printf("GlitchHPF_next\n");

	float *out = ZOUT(0);
	float *in = ZIN(0);
	float freq = ZIN0(1);

	float y0;
	float y1 = unit->m_y1;
	float y2 = unit->m_y2;
	float a0 = unit->m_a0;
	float b1 = unit->m_b1;
	float b2 = unit->m_b2;

	if (freq != unit->m_freq) {

		float pfreq = freq * unit->mRate->mRadiansPerSample * 0.5;

		float C = tan(pfreq);
		float C2 = C * C;
		float sqrt2C = C * sqrt2_f;
		float next_a0 = 1.f / (1.f + sqrt2C + C2);
		float next_b1 = 2.f * (1.f - C2) * next_a0 ;
		float next_b2 = -(1.f - sqrt2C + C2) * next_a0;

		//post("%g %g %g   %g %g   %g %g %g   %g %g\n", *freq, pfreq, qres, D, C, cosf, next_b1, next_b2, next_a0, y1, y2);

		float a0_slope = (next_a0 - a0) * unit->mRate->mFilterSlope;
		float b1_slope = (next_b1 - b1) * unit->mRate->mFilterSlope;
		float b2_slope = (next_b2 - b2) * unit->mRate->mFilterSlope;
		LOOP(unit->mRate->mFilterLoops,
			 y0 = ZXP(in) + b1 * y1 + b2 * y2;
			 ZXP(out) = a0 * (y0 - 2.f * y1 + y2);

			 y2 = ZXP(in) + b1 * y0 + b2 * y1;
			 ZXP(out) = a0 * (y2 - 2.f * y0 + y1);

			 y1 = ZXP(in) + b1 * y2 + b2 * y0;
			 ZXP(out) = a0 * (y1 - 2.f * y2 + y0);

			 a0 += a0_slope;
			 b1 += b1_slope;
			 b2 += b2_slope;
			 );
		LOOP(unit->mRate->mFilterRemain,
			 y0 = ZXP(in) + b1 * y1 + b2 * y2;
			 ZXP(out) = a0 * (y0 - 2.f * y1 + y2);
			 y2 = y1;
			 y1 = y0;
			 );

		unit->m_freq = freq;
		unit->m_a0 = a0;
		unit->m_b1 = b1;
		unit->m_b2 = b2;
	} else {
		LOOP(unit->mRate->mFilterLoops,
			 y0 = ZXP(in) + b1 * y1 + b2 * y2;
			 ZXP(out) = a0 * (y0 - 2.f * y1 + y2);

			 y2 = ZXP(in) + b1 * y0 + b2 * y1;
			 ZXP(out) = a0 * (y2 - 2.f * y0 + y1);

			 y1 = ZXP(in) + b1 * y2 + b2 * y0;
			 ZXP(out) = a0 * (y1 - 2.f * y2 + y0);
			 );
		LOOP(unit->mRate->mFilterRemain,
			 y0 = ZXP(in) + b1 * y1 + b2 * y2;
			 ZXP(out) = a0 * (y0 - 2.f * y1 + y2);
			 y2 = y1;
			 y1 = y0;
			 );
	}
	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}

void GlitchHPF_next_1(GlitchHPF* unit, int inNumSamples)
{
	//printf("GlitchHPF_next\n");

	float in = ZIN0(0);
	float freq = ZIN0(1);

	float y0;
	float y1 = unit->m_y1;
	float y2 = unit->m_y2;
	float a0 = unit->m_a0;
	float b1 = unit->m_b1;
	float b2 = unit->m_b2;

	if (freq != unit->m_freq) {

		float pfreq = freq * unit->mRate->mRadiansPerSample * 0.5;

		float C = tan(pfreq);
		float C2 = C * C;
		float sqrt2C = C * sqrt2_f;
		a0 = 1.f / (1.f + sqrt2C + C2);
		b1 = 2.f * (1.f - C2) * a0 ;
		b2 = -(1.f - sqrt2C + C2) * a0;

		y0 = in + b1 * y1 + b2 * y2;
		ZOUT0(0) = a0 * (y0 - 2.f * y1 + y2);
		y2 = y1;
		y1 = y0;

		unit->m_freq = freq;
		unit->m_a0 = a0;
		unit->m_b1 = b1;
		unit->m_b2 = b2;
	} else {
		y0 = in + b1 * y1 + b2 * y2;
		ZOUT0(0) = a0 * (y0 - 2.f * y1 + y2);
		y2 = y1;
		y1 = y0;

	}
	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void GlitchBPF_Ctor(GlitchBPF* unit)
{
	//printf("GlitchBPF_Reset\n");
	if (unit->mBufLength == 1) {
		SETCALC(GlitchBPF_next_1);
	} else {
		SETCALC(GlitchBPF_next);
	};
	unit->m_a0 = 0.f;
	unit->m_b1 = 0.f;
	unit->m_b2 = 0.f;
	unit->m_y1 = 0.f;
	unit->m_y2 = 0.f;
	unit->m_freq = 0.f;
	unit->m_bw = 0.f;
	ZOUT0(0) = 0.f;
}

void GlitchBPF_next(GlitchBPF* unit, int inNumSamples)
{
	//printf("GlitchBPF_next\n");

	float *out = ZOUT(0);
	float *in = ZIN(0);
	float freq = ZIN0(1);
	float bw = ZIN0(2);

	float y0;
	float y1 = unit->m_y1;
	float y2 = unit->m_y2;
	float a0 = unit->m_a0;
	float b1 = unit->m_b1;
	float b2 = unit->m_b2;

	if (freq != unit->m_freq || bw != unit->m_bw) {

		float pfreq = freq * unit->mRate->mRadiansPerSample;
		float pbw   = bw   * pfreq * 0.5f;

		float C = 1.f / tan(pbw);
		float D = 2.f * cos(pfreq);

		float next_a0 = 1.f / (1.f + C);
		float next_b1 = C * D * next_a0 ;
		float next_b2 = (1.f - C) * next_a0;

		float a0_slope = (next_a0 - a0) * unit->mRate->mFilterSlope;
		float b1_slope = (next_b1 - b1) * unit->mRate->mFilterSlope;
		float b2_slope = (next_b2 - b2) * unit->mRate->mFilterSlope;
		LOOP(unit->mRate->mFilterLoops,
             y0 = ZXP(in) + b1 * y1 + b2 * y2;
             ZXP(out) = a0 * (y0 - y2);

             y2 = ZXP(in) + b1 * y0 + b2 * y1;
             ZXP(out) = a0 * (y2 - y1);

             y1 = ZXP(in) + b1 * y2 + b2 * y0;
             ZXP(out) = a0 * (y1 - y0);

             a0 += a0_slope;
             b1 += b1_slope;
             b2 += b2_slope;
             );
		LOOP(unit->mRate->mFilterRemain,
             y0 = ZXP(in) + b1 * y1 + b2 * y2;
             ZXP(out) = a0 * (y0 - y2);
             y2 = y1;
             y1 = y0;
             );

		unit->m_freq = freq;
		unit->m_bw = bw;
		unit->m_a0 = a0;
		unit->m_b1 = b1;
		unit->m_b2 = b2;
	} else {
		LOOP(unit->mRate->mFilterLoops,
             y0 = ZXP(in) + b1 * y1 + b2 * y2;
             ZXP(out) = a0 * (y0 - y2);

             y2 = ZXP(in) + b1 * y0 + b2 * y1;
             ZXP(out) = a0 * (y2 - y1);

             y1 = ZXP(in) + b1 * y2 + b2 * y0;
             ZXP(out) = a0 * (y1 - y0);
             );
		LOOP(unit->mRate->mFilterRemain,
             y0 = ZXP(in) + b1 * y1 + b2 * y2;
             ZXP(out) = a0 * (y0 - y2);
             y2 = y1;
             y1 = y0;
             );
	}
	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}

void GlitchBPF_next_1(GlitchBPF* unit, int inNumSamples)
{
	//printf("GlitchBPF_next_1\n");

	float in = ZIN0(0);
	float freq = ZIN0(1);
	float bw = ZIN0(2);

	float y0;
	float y1 = unit->m_y1;
	float y2 = unit->m_y2;
	float a0 = unit->m_a0;
	float b1 = unit->m_b1;
	float b2 = unit->m_b2;

	if (freq != unit->m_freq || bw != unit->m_bw) {

		float pfreq = freq * unit->mRate->mRadiansPerSample;
		float pbw   = bw   * pfreq * 0.5;

		float C = 1.f / tan(pbw);
		float D = 2.f * cos(pfreq);

		float a0 = 1.f / (1.f + C);
		float b1 = C * D * a0 ;
		float b2 = (1.f - C) * a0;

		y0 = in + b1 * y1 + b2 * y2;
		ZOUT0(0) = a0 * (y0 - y2);
		y2 = y1;
		y1 = y0;

		unit->m_freq = freq;
		unit->m_bw = bw;
		unit->m_a0 = a0;
		unit->m_b1 = b1;
		unit->m_b2 = b2;
	} else {
		y0 = in + b1 * y1 + b2 * y2;
		ZOUT0(0) = a0 * (y0 - y2);
		y2 = y1;
		y1 = y0;
	}
	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////

void GlitchBRF_Ctor(GlitchBRF* unit)
{
	//printf("GlitchBRF_Reset\n");
	if (unit->mBufLength == 1) {
		SETCALC(GlitchBRF_next_1);
	} else {
		SETCALC(GlitchBRF_next);
	};
	unit->m_a0 = 0.f;
	unit->m_a1 = 0.f;
	unit->m_b2 = 0.f;
	unit->m_y1 = 0.f;
	unit->m_y2 = 0.f;
	unit->m_freq = 0.f;
	unit->m_bw = 0.f;
	ZOUT0(0) = 0.f;
}


void GlitchBRF_next(GlitchBRF* unit, int inNumSamples)
{
	//printf("GlitchBRF_next\n");

	float *out = ZOUT(0);
	float *in = ZIN(0);
	float freq = ZIN0(1);
	float bw = ZIN0(2);

	float ay;
	float y0;
	float y1 = unit->m_y1;
	float y2 = unit->m_y2;
	float a0 = unit->m_a0;
	float a1 = unit->m_a1;
	float b2 = unit->m_b2;

	if (freq != unit->m_freq || bw != unit->m_bw) {
		float pfreq = freq * unit->mRate->mRadiansPerSample;
		float pbw   = bw   * pfreq * 0.5f;

		float C = tan(pbw);
		float D = 2.f * cos(pfreq);

		float next_a0 = 1.f / (1.f + C);
		float next_a1 = -D * next_a0;
		float next_b2 = (1.f - C) * next_a0;

		float a0_slope = (next_a0 - a0) * unit->mRate->mFilterSlope;
		float a1_slope = (next_a1 - a1) * unit->mRate->mFilterSlope;
		float b2_slope = (next_b2 - b2) * unit->mRate->mFilterSlope;

		LOOP(unit->mRate->mFilterLoops,
             ay = a1 * y1;
             y0 = ZXP(in) - ay - b2 * y2;
             ZXP(out) = a0 * (y0 + y2) + ay;

             ay = a1 * y0;
             y2 = ZXP(in) - ay - b2 * y1;
             ZXP(out) = a0 * (y2 + y1) + ay;

             ay = a1 * y2;
             y1 = ZXP(in) - ay - b2 * y0;
             ZXP(out) = a0 * (y1 + y0) + ay;

             a0 += a0_slope;
             a1 += a1_slope;
             b2 += b2_slope;
             );
		LOOP(unit->mRate->mFilterRemain,
             ay = a1 * y1;
             y0 = ZXP(in) - ay - b2 * y2;
             ZXP(out) = a0 * (y0 + y2) + ay;
             y2 = y1;
             y1 = y0;
             );

		unit->m_freq = freq;
		unit->m_bw = bw;
		unit->m_a0 = a0;
		unit->m_a1 = a1;
		unit->m_b2 = b2;
	} else {
		LOOP(unit->mRate->mFilterLoops,
             ay = a1 * y1;
             y0 = ZXP(in) - ay - b2 * y2;
             ZXP(out) = a0 * (y0 + y2) + ay;

             ay = a1 * y0;
             y2 = ZXP(in) - ay - b2 * y1;
             ZXP(out) = a0 * (y2 + y1) + ay;

             ay = a1 * y2;
             y1 = ZXP(in) - ay - b2 * y0;
             ZXP(out) = a0 * (y1 + y0) + ay;
             );
		LOOP(unit->mRate->mFilterRemain,
             ay = a1 * y1;
             y0 = ZXP(in) - ay - b2 * y2;
             ZXP(out) = a0 * (y0 + y2) + ay;
             y2 = y1;
             y1 = y0;
             );
	}
	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);

}


void GlitchBRF_next_1(GlitchBRF* unit, int inNumSamples)
{
	//printf("GlitchBRF_next_1\n");

	float in = ZIN0(0);
	float freq = ZIN0(1);
	float bw = ZIN0(2);

	float ay;
	float y0;
	float y1 = unit->m_y1;
	float y2 = unit->m_y2;
	float a0 = unit->m_a0;
	float a1 = unit->m_a1;
	float b2 = unit->m_b2;

	if (freq != unit->m_freq || bw != unit->m_bw) {
		float pfreq = freq * unit->mRate->mRadiansPerSample;
		float pbw   = bw   * pfreq * 0.5f;

		float C = tan(pbw);
		float D = 2.f * cos(pfreq);

		float a0 = 1.f / (1.f + C);
		float a1 = -D * a0;
		float b2 = (1.f - C) * a0;

		ay = a1 * y1;
		y0 = in - ay - b2 * y2;
		ZOUT0(0) = a0 * (y0 + y2) + ay;
		y2 = y1;
		y1 = y0;

		unit->m_freq = freq;
		unit->m_bw = bw;
		unit->m_a0 = a0;
		unit->m_a1 = a1;
		unit->m_b2 = b2;
	} else {

		ay = a1 * y1;
		y0 = in - ay - b2 * y2;
		ZOUT0(0) = a0 * (y0 + y2) + ay;
		y2 = y1;
		y1 = y0;

	}
	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
PluginLoad(Filter)
{
	ft = inTable;

	DefineSimpleUnit(GlitchHPF);
	DefineSimpleUnit(GlitchRHPF);
    DefineSimpleUnit(GlitchBPF);
	DefineSimpleUnit(GlitchBRF);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
