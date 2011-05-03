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

//PV_UGens by Batuhan Bozkurt http://www.batuhanbozkurt.com

//#include "SC_PlugIn.h"
//#include "SCComplex.h"
#include "FFT_UGens.h"

#define PV_GET_BUF2_FCOMPARE \
	float fbufnum1 = ZIN0(0); \
	float fbufnum2 = ZIN0(1); \
	if (fbufnum1 < 0.f || fbufnum2 < 0.f) { ZOUT0(0) = unit->outVal; return; } \
	ZOUT0(0) = unit->outVal; \
	uint32 ibufnum1 = (int)fbufnum1; \
	uint32 ibufnum2 = (int)fbufnum2; \
	World *world = unit->mWorld; \
	SndBuf *buf1; \
	SndBuf *buf2; \
	if (ibufnum1 >= world->mNumSndBufs) { \
		int localBufNum = ibufnum1 - world->mNumSndBufs; \
		Graph *parent = unit->mParent; \
		if(localBufNum <= parent->localBufNum) { \
			buf1 = parent->mLocalSndBufs + localBufNum; \
		} else { \
			buf1 = world->mSndBufs; \
		} \
	} else { \
		buf1 = world->mSndBufs + ibufnum1; \
	} \
	if (ibufnum2 >= world->mNumSndBufs) { \
		int localBufNum = ibufnum2 - world->mNumSndBufs; \
		Graph *parent = unit->mParent; \
		if(localBufNum <= parent->localBufNum) { \
			buf2 = parent->mLocalSndBufs + localBufNum; \
		} else { \
			buf2 = world->mSndBufs; \
		} \
	} else { \
		buf2 = world->mSndBufs + ibufnum2; \
	} \
	if (buf1->samples != buf2->samples) return; \
	int numbins = buf1->samples - 2 >> 1;

InterfaceTable *ft;

struct FrameCompare : PV_Unit
{
	float magSum;
	int numFrames;
	float outVal;
};

extern "C"
{
	void load(InterfaceTable *inTable);

	void FrameCompare_Ctor(FrameCompare* unit);
	void FrameCompare_Dtor(FrameCompare* unit);
	void FrameCompare_next(FrameCompare* unit, int inNumSamples);
}

//SCPolarBuf* ToPolarApx(SndBuf *buf)
//{
//	if (buf->coord == coord_Complex) {
//		SCComplexBuf* p = (SCComplexBuf*)buf->data;
//		int numbins = buf->samples - 2 >> 1;
//		for (int i=0; i<numbins; ++i) {
//			p->bin[i].ToPolarApxInPlace();
//		}
//		buf->coord = coord_Polar;
//	}
//	return (SCPolarBuf*)buf->data;
//}
//
//SCComplexBuf* ToComplexApx(SndBuf *buf)
//{
//	if (buf->coord == coord_Polar) {
//		SCPolarBuf* p = (SCPolarBuf*)buf->data;
//		int numbins = buf->samples - 2 >> 1;
//		for (int i=0; i<numbins; ++i) {
//			p->bin[i].ToComplexApxInPlace();
//		}
//		buf->coord = coord_Complex;
//	}
//	return (SCComplexBuf*)buf->data;
//}

//FrameCompare
//Gives a similarity rating for two (short duration) sound sources.
//Smaller the output, more similar the source and target.
//Compares magnitudes only.
//Distance metrics formula taken from an article by Ricardo A. Garcia:
//(Growing Sound Synthesizers Using Evolutionary Methods)
//http://galileo.cincom.unical.it/Music/workshop/articoli/garcia.pdf

void FrameCompare_Ctor(FrameCompare* unit)
{

	SETCALC(FrameCompare_next);
	unit->magSum = 0.f, unit->numFrames = 0;
	ZOUT0(0) = unit->outVal = ZIN0(0);
}

void FrameCompare_next(FrameCompare *unit, int inNumSamples)
{
	PV_GET_BUF2_FCOMPARE

	float minTargetMag = 900000.f, maxTargetMag = 0.f;
	float magdiff, magweight, lmtemp, minmaxtemp, p1mag, p2mag, scaleFactor;
	float wOffset = ZIN0(2);

	SCPolarBuf *p1 = ToPolarApx(buf1); //rendered
	SCPolarBuf *p2 = ToPolarApx(buf2); //target

	scaleFactor =  (numbins + 1) * 0.5; //hanning window only
	//Print("%f\n", p2->bin[100].mag / scaleFactor);

	for(int i = 0; i < numbins; ++i)
	{
		p2mag = p2->bin[i].mag / scaleFactor;

		minmaxtemp = (p2mag < 2e-42) ? log(2e-42) : log(fabs(p2mag));

		minTargetMag = (minmaxtemp <= minTargetMag) ? minmaxtemp : minTargetMag;
		maxTargetMag = (minmaxtemp >= maxTargetMag) ? minmaxtemp : maxTargetMag;
	}

	for(int i = 0; i < numbins; ++i)
	{
		p1mag = p1->bin[i].mag / scaleFactor;
		p2mag = p2->bin[i].mag / scaleFactor;

		magdiff = fabs(p1mag) - fabs(p2mag);
		lmtemp = p2mag < 2e-42 ? log(2e-42) : log(fabs(p2mag));
		magweight = (1 - wOffset) + (wOffset * ((lmtemp - minTargetMag) / fabs(minTargetMag - maxTargetMag)));
		unit->magSum = unit->magSum + (magdiff * magdiff * magweight);
	}

	unit->numFrames = unit->numFrames + 1;
	ZOUT0(0) = unit->outVal = unit->magSum/unit->numFrames;
}

void FrameCompare_Dtor(FrameCompare* unit)
{
}

void init_SCComplex(InterfaceTable *inTable);

PluginLoad(BatPV)
{
	ft = inTable;

	init_SCComplex(inTable);
	DefineDtorUnit(FrameCompare);
}
