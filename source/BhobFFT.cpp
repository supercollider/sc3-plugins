/* various FFT manipulations by bhob rainey http://www.bhobrainey.net
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

#include "SC_fftlib.h"
#include "FFT_UGens.h"

struct PV_MagScale : public PV_Unit
{
	uint32 m_scalebufnum;
	SndBuf *m_scalebuf;
};

#define MAKE_TEMP_BUF2 \
	if (!unit->m_tempbuf) { \
		unit->m_tempbuf = (float*)RTAlloc(unit->mWorld, buf1->samples * sizeof(float)); \
		unit->m_numbins = numbins; \
	} else if (numbins != unit->m_numbins) return;


extern "C"
{
	void PV_CommonMag_Ctor(PV_Unit *unit);
	void PV_CommonMag_next(PV_Unit *unit, int inNumSamples);

	void PV_CommonMul_Ctor(PV_Unit *unit);
	void PV_CommonMul_next(PV_Unit *unit, int inNumSamples);

	void PV_MagMinus_Ctor(PV_Unit *unit);
	void PV_MagMinus_next(PV_Unit *unit, int inNumSamples);

	void PV_MagGate_Ctor(PV_Unit *unit);
	void PV_MagGate_next(PV_Unit *unit, int inNumSamples);

	void PV_Compander_Ctor(PV_Unit *unit);
	void PV_Compander_next(PV_Unit *unit, int inNumSamples);

	void PV_MagScale_Ctor(PV_MagScale *unit);
	void PV_MagScale_next(PV_MagScale* unit, int inNumSamples);

	void PV_Morph_Ctor(PV_Unit *unit);
	void PV_Morph_next(PV_Unit *unit, int inNumSamples);

	void PV_XFade_Ctor(PV_Unit *unit);
	void PV_XFade_next(PV_Unit *unit, int inNumSamples);

	void PV_SoftWipe_Ctor(PV_Unit *unit);
	void PV_SoftWipe_next(PV_Unit *unit, int inNumSamples);

	void PV_Cutoff_Ctor(PV_Unit *unit);
	void PV_Cutoff_next(PV_Unit *unit, int inNumSamples);
}

// these are already defined in PV_ThrirdParty.cpp. so build doest work on pbuilder - br
#if gccversion >= 4

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

#endif // gccversion >= 4


void PV_CommonMag_next(PV_Unit *unit, int inNumSamples)
{
	PV_GET_BUF2

	SCPolarBuf *p = ToPolarApx(buf1);
	SCPolarBuf *q = ToPolarApx(buf2);
	float tolerance=ZIN0(2);
	float remove=ZIN0(3);

	p->dc = q->dc;
	p->nyq = q->nyq;
	for (int i=0; i<numbins; ++i) {
		float mag1 = p->bin[i].mag;
		float mag2 = q->bin[i].mag;
		if (((mag2 - tolerance) >= mag1) || ((mag2 + tolerance) <= mag1)) p->bin[i].mag = mag1 * remove;
	}
}

void PV_CommonMag_Ctor(PV_Unit *unit)
{
	SETCALC(PV_CommonMag_next);
	ZOUT0(0) = ZIN0(0);
}

void PV_CommonMul_next(PV_Unit *unit, int inNumSamples)
{
	PV_GET_BUF2

	SCPolarBuf *p = ToPolarApx(buf1);
	SCPolarBuf *q = ToPolarApx(buf2);
	float tolerance=ZIN0(2);
	float remove=ZIN0(3);

	p->dc *= q->dc;
	p->nyq *= q->nyq;
	for (int i=0; i<numbins; ++i) {
		float mag1 = p->bin[i].mag;
		float mag2 = q->bin[i].mag;
		if (((mag2 - tolerance) >= mag1) || ((mag2 + tolerance) <= mag1)) {
			p->bin[i].mag = mag1 * remove;
		} else {
			p->bin[i].mag *= mag2;
		}
	}
}

void PV_CommonMul_Ctor(PV_Unit *unit)
{
	SETCALC(PV_CommonMul_next);
	ZOUT0(0) = ZIN0(0);
}

void PV_MagMinus_next(PV_Unit *unit, int inNumSamples)
{
	PV_GET_BUF2

	SCPolarBuf *p = ToPolarApx(buf1);
	SCPolarBuf *q = ToPolarApx(buf2);
	float remove=ZIN0(2);

	for (int i=0; i<numbins; ++i) {
		p->bin[i].mag = sc_max(p->bin[i].mag - q->bin[i].mag*remove, 0.f);
	}
}

void PV_MagMinus_Ctor(PV_Unit *unit)
{
	SETCALC(PV_MagMinus_next);
	ZOUT0(0) = ZIN0(0);
}

void PV_MagGate_next(PV_Unit *unit, int inNumSamples)
{
	PV_GET_BUF

	SCPolarBuf *p = ToPolarApx(buf);
	float thresh=ZIN0(1);
	float reduce=ZIN0(2);

	for (int i=0; i<numbins; ++i) {
		float mag = p->bin[i].mag;
		if (thresh < 0.f) {
			if (mag > fabs(thresh)) p->bin[i].mag=mag * reduce;
		} else if (mag < thresh) p->bin[i].mag=mag * reduce;
	}
}

void PV_MagGate_Ctor(PV_Unit *unit)
{
	SETCALC(PV_MagGate_next);
	ZOUT0(0) = ZIN0(0);
}

void PV_Compander_next(PV_Unit *unit, int inNumSamples)
{
	PV_GET_BUF

	SCPolarBuf *p = ToPolarApx(buf);
	float thresh=ZIN0(1);
	float slope_below=ZIN0(2);
	float slope_above=ZIN0(3);

	for (int i=0; i<numbins; ++i) {
		float mag = p->bin[i].mag;
		if (mag < thresh) {
			if (slope_below != 1.f) {
				float gain = pow(mag / thresh, slope_below - 1.f);
				float32 absx = fabs(gain);
				gain =  (absx > (float32)1e-15 && absx < (float32)1e15) ? gain : (float32)1.;
				p->bin[i].mag=mag * gain;
			}
		} else {
			if (slope_above != 1.f) {
				p->bin[i].mag=mag * (pow(mag / thresh, slope_above - 1.f));
			}
		}
	}
}

void PV_Compander_Ctor(PV_Unit *unit)
{
	SETCALC(PV_Compander_next);
	ZOUT0(0) = ZIN0(0);
}

void PV_MagScale_next(PV_MagScale *unit, int inNumSamples)
{
	PV_GET_BUF
	float bufin = ZIN0(1);
	uint32 scalebuf = (uint32)bufin;
	if (scalebuf != unit->m_scalebufnum) {
			if (scalebuf + 1 >= world->mNumSndBufs) scalebuf = 0;
			unit->m_scalebufnum = scalebuf;
	}
	SndBuf *scales = world->mSndBufs + scalebuf;
	float frac = bufin - (int)bufin;
	if (frac == 0.f) {
		float *scaleData1 = scales[0].data;
		SCPolarBuf *p = ToPolarApx(buf);
		for (int i=0; i<numbins; ++i) {
			p->bin[i].mag = p->bin[i].mag * scaleData1[i];
		}
	} else {
		float *scaleData1 = scales[0].data;
		float *scaleData2 = scales[1].data;
		SCPolarBuf *p = ToPolarApx(buf);
		for (int i=0; i<numbins; ++i) {
			p->bin[i].mag = p->bin[i].mag * (scaleData1[i] * (1-frac) + scaleData2[i] * frac);
		}
	}
}

void PV_MagScale_Ctor(PV_MagScale *unit)
{
	SETCALC(PV_MagScale_next);
	ZOUT0(0) = ZIN0(0);
}

void PV_XFade_next(PV_Unit *unit, int inNumSamples)
{
	PV_GET_BUF2
	float fade=ZIN0(2);

	SCComplexBuf *p = ToComplexApx(buf1);
	SCComplexBuf *q = ToComplexApx(buf2);

	p->dc = q->dc;
	p->nyq = q->nyq;

	for (int i=0; i<numbins; ++i) {
		p->bin[i].real=(1-fade) * p->bin[i].real + fade * q->bin[i].real;
		p->bin[i].imag=(1-fade) * p->bin[i].imag + fade * q->bin[i].imag;
	}
}

void PV_XFade_Ctor(PV_Unit *unit)
{
	SETCALC(PV_XFade_next);
	ZOUT0(0) = ZIN0(0);
}

void PV_Morph_next(PV_Unit *unit, int inNumSamples)
{
	PV_GET_BUF2
	float morph=ZIN0(2);

	SCPolarBuf *p = ToPolarApx(buf1);
	SCPolarBuf *q = ToPolarApx(buf2);

	p->dc = q->dc;
	p->nyq = q->nyq;

	for (int i=0; i<numbins; ++i) {
		p->bin[i].mag=(1-morph) * p->bin[i].mag + morph * q->bin[i].mag;
		p->bin[i].phase=(1-morph) * p->bin[i].phase + morph * q->bin[i].phase;
	}
}

void PV_Morph_Ctor(PV_Unit *unit)
{
	SETCALC(PV_Morph_next);
	ZOUT0(0) = ZIN0(0);
}

void PV_SoftWipe_next(PV_Unit *unit, int inNumSamples)
{
	PV_GET_BUF2

	SCComplexBuf *p = (SCComplexBuf*)buf1->data;
	SCComplexBuf *q = (SCComplexBuf*)buf2->data;

	float wipe = ZIN0(2) * numbins;
	int iwipe = (int)wipe;
	float frac = fabs(wipe - iwipe);
	if (iwipe > 0) {
		iwipe = sc_min(iwipe, numbins);
		for (int i=0; i < iwipe-1; ++i) {
			p->bin[i]=q->bin[i];
		}
		p->bin[iwipe]=(1-frac) * p->bin[iwipe] + frac * q->bin[iwipe];
	} else if (iwipe < 0) {
		iwipe = sc_max(iwipe, -numbins);
		int lastbin=numbins+iwipe;
		for (int i=lastbin+1; i < numbins; ++i) {
			p->bin[i]=q->bin[i];
		}
		p->bin[lastbin]=(1-frac) * p->bin[lastbin] + frac * q->bin[lastbin];
	}
}

void PV_SoftWipe_Ctor(PV_Unit *unit)
{
	SETCALC(PV_SoftWipe_next);
	ZOUT0(0) = ZIN0(0);
}

void PV_Cutoff_next(PV_Unit *unit, int inNumSamples)
{
	PV_GET_BUF

	SCComplexBuf *p = (SCComplexBuf*)buf->data;

	float wipe = ZIN0(1) * numbins;
	int iwipe = (int)wipe;
	float frac = fabs(wipe - iwipe);
	if (iwipe > 0) {
		iwipe = sc_min(iwipe, numbins);
		for (int i=0; i < iwipe-1; ++i) {
			p->bin[i] = 0.;
		}
		p->bin[iwipe]=(1-frac) * p->bin[iwipe];
	} else if (wipe < 0) {
		iwipe = sc_max(iwipe, -numbins);
		int lastbin=numbins+iwipe;
		for (int i=lastbin+1; i < numbins; ++i) {
			p->bin[i] = 0.;
		}
		p->bin[lastbin]=(1-frac) * p->bin[lastbin];
	}
}

void PV_Cutoff_Ctor(PV_Unit *unit)
{
	SETCALC(PV_Cutoff_next);
	ZOUT0(0) = ZIN0(0);
}

#define DefinePVUnit(name) \
	(*ft->fDefineUnit)(#name, sizeof(PV_Unit), (UnitCtorFunc)&name##_Ctor, 0, 0);


extern "C"
{
    void load(InterfaceTable *inTable);
}

InterfaceTable *ft;

PluginLoad(BhobPV)
{
	ft = inTable;

	DefinePVUnit(PV_MagMinus);
	DefinePVUnit(PV_CommonMag);
	DefinePVUnit(PV_CommonMul);
	DefinePVUnit(PV_MagGate);
	DefinePVUnit(PV_MagScale);
	DefinePVUnit(PV_Morph);
	DefinePVUnit(PV_XFade);
	DefinePVUnit(PV_SoftWipe);
	DefinePVUnit(PV_Cutoff);
	DefinePVUnit(PV_Compander);
}
