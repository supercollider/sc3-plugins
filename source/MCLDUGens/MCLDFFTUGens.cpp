/*

FFT analysis and phase vocoder UGens for SuperCollider, by Dan Stowell.
(c) Dan Stowell 2006-2010.

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
#include "SC_PlugIn.h"
#include "FFT_UGens.h"
#include <stdio.h>

// Used by PV_MagLog
#define SMALLEST_NUM_FOR_LOG 2e-42

#define PI 3.1415926535898f
#define MPI -3.1415926535898f
#define TWOPI 6.28318530717952646f
#define THREEPI 9.4247779607694f

/* Rewrap phase into +-pi domain: essentially mod(phase+pi,-2pi)+pi */
#define PHASE_REWRAP(phase)  ((phase) + TWOPI * (1.f + floorf(-((phase)+PI)/TWOPI)))

//////////////////////////////////////////////////////////////////////////////////////////////////


struct FFTAnalyser_Unit : Unit
{
	float outval;

	// Not always used: multipliers which convert from bin indices to freq vals, and vice versa.
	// See also the macros for deriving these.
	float m_bintofreq, m_freqtobin;
};

struct FFTAnalyser_OutOfPlace : FFTAnalyser_Unit
{
	int m_numbins;
	float *m_tempbuf;
};

struct PV_MagSubtract : Unit {
};

struct FFTFlux_Unit : FFTAnalyser_OutOfPlace
{
	float m_yesternorm;
	float m_yesterdc;
	float m_yesternyq;
	bool m_normalise;
};

struct FFTPower : FFTAnalyser_Unit
{
	float m_normfactor;
	bool m_square;
};

struct FFTSubbandPower : FFTAnalyser_Unit
{
	float m_normfactor;
	bool m_square;

	int m_numbands;
	int *m_cutoffs; // Will hold bin indices corresponding to frequencies
	float *m_outvals;
	bool m_cutoff_inited;

	int m_scalemode;
};

struct FFTPhaseDev : FFTAnalyser_OutOfPlace
{
	bool m_weight; // Whether or not to weight the phase deviations according to the magnitudes
};

struct FFTComplexDev : FFTAnalyser_OutOfPlace
{
	bool m_rectify; // Whether or not to ignore bins whose power is decreasing
};

struct FFTMKL : FFTAnalyser_OutOfPlace
{
};

struct PV_Whiten : Unit {
	float m_relaxcoef, m_floor, m_smear;
	int m_bindownsample;
};


struct FFTSubbandFlatness : FFTAnalyser_Unit
{
	int m_numbands;
			int *m_cutoffs; // Will hold bin indices corresponding to frequencies
	float *m_outvals;
	bool m_cutoff_inited;
};
struct FFTCrest : FFTAnalyser_Unit
{
	int m_frombin; // Will hold bin index
	int m_tobinp1; // Will hold bin index
	bool m_cutoffneedsinit;
};
struct FFTSpread : FFTAnalyser_Unit
{
};
struct FFTSlope : FFTAnalyser_Unit
{
};
struct FFTPeak : FFTAnalyser_Unit
{
	float outval2, maxfreq, minfreq;
	int maxbin, minbin;
};
struct PV_MagSmooth : Unit {
	float *m_memory;
};
struct PV_ExtractRepeat : Unit {
	float* m_logmags;
	int m_cursor;
	float m_fbufnum;
	SndBuf* m_buf;
};


// for operation on one buffer
// just like PV_GET_BUF except it outputs unit->outval rather than -1 when FFT not triggered
#define FFTAnalyser_GET_BUF \
	float fbufnum = ZIN0(0); \
	if (fbufnum < 0.f) { ZOUT0(0) = unit->outval; return; } \
	ZOUT0(0) = fbufnum; \
	uint32 ibufnum = (uint32)fbufnum; \
	World *world = unit->mWorld; \
	SndBuf *buf; \
	if (ibufnum >= world->mNumSndBufs) { \
		int localBufNum = ibufnum - world->mNumSndBufs; \
		Graph *parent = unit->mParent; \
		if(localBufNum <= parent->localBufNum) { \
			buf = parent->mLocalSndBufs + localBufNum; \
		} else { \
			buf = world->mSndBufs; \
		} \
	} else { \
		buf = world->mSndBufs + ibufnum; \
	} \
	int numbins = buf->samples - 2 >> 1;

// Same as above; but with "output2" as well as "output"
#define FFTAnalyser_GET_BUF_TWOOUTS \
	float fbufnum = ZIN0(0); \
	if (fbufnum < 0.f) { ZOUT0(0) = unit->outval; ZOUT0(1) = unit->outval2; return; } \
	ZOUT0(0) = fbufnum; \
	uint32 ibufnum = (uint32)fbufnum; \
	World *world = unit->mWorld; \
	SndBuf *buf; \
	if (ibufnum >= world->mNumSndBufs) { \
		int localBufNum = ibufnum - world->mNumSndBufs; \
		Graph *parent = unit->mParent; \
		if(localBufNum <= parent->localBufNum) { \
			buf = parent->mLocalSndBufs + localBufNum; \
		} else { \
			buf = world->mSndBufs; \
		} \
	} else { \
		buf = world->mSndBufs + ibufnum; \
	} \
	int numbins = buf->samples - 2 >> 1;

// As above; but for operation on two input buffers
#define FFTAnalyser_GET_BUF2 \
	float fbufnum1 = ZIN0(0); \
	float fbufnum2 = ZIN0(1); \
	if (fbufnum1 < 0.f || fbufnum2 < 0.f) { ZOUT0(0) = unit->outval; return; } \
	uint32 ibufnum1 = (int)fbufnum1; \
	uint32 ibufnum2 = (int)fbufnum2; \
	World *world = unit->mWorld; \
	SndBuf *buf1; \
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
	SndBuf *buf2; \
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

// Copied from FFT_UGens.cpp
#define MAKE_TEMP_BUF \
	if (!unit->m_tempbuf) { \
		unit->m_tempbuf = (float*)RTAlloc(unit->mWorld, buf->samples * sizeof(float)); \
		unit->m_numbins = numbins; \
	} else if (numbins != unit->m_numbins) return;

#define GET_BINTOFREQ \
	if(unit->m_bintofreq==0.f){ \
		unit->m_bintofreq = world->mFullRate.mSampleRate / buf->samples; \
	} \
	float bintofreq = unit->m_bintofreq;
#define GET_FREQTOBIN \
	if(unit->m_freqtobin==0.f){ \
		unit->m_freqtobin = buf->samples / world->mFullRate.mSampleRate; \
	} \
	float freqtobin = unit->m_freqtobin;

//////////////////////////////////////////////////////////////////////////////////////////////////

extern "C"
{
	void load(InterfaceTable *inTable);

	void FFTPower_Ctor(FFTPower *unit);
	void FFTPower_next(FFTPower *unit, int inNumSamples);

	void FFTFlux_Ctor(FFTFlux_Unit *unit);
	void FFTFlux_next(FFTFlux_Unit *unit, int inNumSamples);
	void FFTFlux_Dtor(FFTFlux_Unit *unit);
	void FFTFluxPos_Ctor(FFTFlux_Unit *unit);
	void FFTFluxPos_next(FFTFlux_Unit *unit, int inNumSamples);
	void FFTFluxPos_Dtor(FFTFlux_Unit *unit);

	void FFTDiffMags_Ctor(FFTAnalyser_Unit *unit);
	void FFTDiffMags_next(FFTAnalyser_Unit *unit, int inNumSamples);

//	void PV_DiffAndToDC_Ctor(PV_Unit *unit);
//	void PV_DiffAndToDC_next(PV_Unit *unit, int inNumSamples);

	void PV_MagSubtract_Ctor(PV_Unit *unit);
	void PV_MagSubtract_next(PV_Unit *unit, int inNumSamples);

	void FFTSubbandPower_Ctor(FFTSubbandPower *unit);
	void FFTSubbandPower_next(FFTSubbandPower *unit, int inNumSamples);
	void FFTSubbandPower_Dtor(FFTSubbandPower *unit);

	void PV_MagLog_Ctor(PV_Unit *unit);
	void PV_MagLog_next(PV_Unit *unit, int inNumSamples);

	void PV_MagExp_Ctor(PV_Unit *unit);
	void PV_MagExp_next(PV_Unit *unit, int inNumSamples);

	void FFTPhaseDev_Ctor(FFTPhaseDev *unit);
	void FFTPhaseDev_Dtor(FFTPhaseDev *unit);
	void FFTPhaseDev_next(FFTPhaseDev *unit, int inNumSamples);

	void FFTComplexDev_Ctor(FFTComplexDev *unit);
	void FFTComplexDev_Dtor(FFTComplexDev *unit);
	void FFTComplexDev_next(FFTComplexDev *unit, int inNumSamples);

	void FFTMKL_Ctor(FFTMKL *unit);
	void FFTMKL_Dtor(FFTMKL *unit);
	void FFTMKL_next(FFTMKL *unit, int inNumSamples);

	void PV_Whiten_Ctor(PV_Whiten *unit);
	void PV_Whiten_next(PV_Whiten *unit, int inNumSamples);

	void FFTSubbandFlatness_Ctor(FFTSubbandFlatness *unit);
	void FFTSubbandFlatness_next(FFTSubbandFlatness *unit, int inNumSamples);
	void FFTSubbandFlatness_Dtor(FFTSubbandFlatness *unit);

	void FFTCrest_Ctor(FFTCrest *unit);
	void FFTCrest_next(FFTCrest *unit, int inNumSamples);

	void FFTSpread_Ctor(FFTSpread *unit);
	void FFTSpread_next(FFTSpread *unit, int inNumSamples);

	void FFTSlope_Ctor(FFTSlope *unit);
	void FFTSlope_next(FFTSlope *unit, int inNumSamples);

	void FFTPeak_Ctor(FFTPeak *unit);
	void FFTPeak_next(FFTPeak *unit, int inNumSamples);

	void PV_MagSmooth_Ctor(PV_MagSmooth *unit);
	void PV_MagSmooth_next(PV_MagSmooth *unit, int inNumSamples);
	void PV_MagSmooth_Dtor(PV_MagSmooth *unit);

	void PV_MagMulAdd_Ctor(PV_Unit *unit);
	void PV_MagMulAdd_next(PV_Unit *unit, int inNumSamples);

	void PV_ExtractRepeat_Ctor(PV_ExtractRepeat *unit);
	void PV_ExtractRepeat_next(PV_ExtractRepeat *unit, int inNumSamples);
	void PV_ExtractRepeat_Dtor(PV_ExtractRepeat *unit);

}

InterfaceTable *ft;

void init_SCComplex(InterfaceTable *inTable);

//////////////////////////////////////////////////////////////////////////////////////////////////

void FFTPower_Ctor(FFTPower *unit)
{
	SETCALC(FFTPower_next);
	ZOUT0(0) = unit->outval = 0.;

	unit->m_square = ZIN0(1) > 0.f;
	unit->m_normfactor = 0.f;
}

void FFTPower_next(FFTPower *unit, int inNumSamples)
{
	FFTAnalyser_GET_BUF

	float normfactor = unit->m_normfactor;
	bool square = unit->m_square;
	if(normfactor == 0.f){
		if(square)
			unit->m_normfactor = normfactor = 1.f / powf(numbins + 2.f, 1.5f);
		else
			unit->m_normfactor = normfactor = 1.f / (numbins + 2.f);
	}


	SCComplexBuf *p = ToComplexApx(buf);
//	SCPolarBuf *p = ToPolarApx(buf);

	float total;
	if(square){
		total = sc_abs(p->dc) * sc_abs(p->dc) + sc_abs(p->nyq) *  sc_abs(p->nyq);

		for (int i=0; i<numbins; ++i) {
			float rabs = (p->bin[i].real);
			float iabs = (p->bin[i].imag);
			total += (rabs*rabs) + (iabs*iabs);
		}
	}else{
		total = sc_abs(p->dc) + sc_abs(p->nyq);

		for (int i=0; i<numbins; ++i) {
			float rabs = (p->bin[i].real);
			float iabs = (p->bin[i].imag);
			total += sqrt((rabs*rabs) + (iabs*iabs));
		}
	//	for (int i=0; i<numbins; ++i) {
	//		total += sc_abs(p->bin[i].mag);
	//	}
	}

	// Store the val for output in future calls
	unit->outval = total * normfactor;

	ZOUT0(0) = unit->outval;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void FFTSubbandPower_Ctor(FFTSubbandPower *unit)
{
	SETCALC(FFTSubbandPower_next);
	ZOUT0(0) = unit->outval = 0.;

	unit->m_square = ZIN0(2) > 0.f;
	unit->m_normfactor = 0.f;

	// ZIN0(1) tells us how many cutoffs we're looking for
	int numcutoffs = (int)ZIN0(1);
	int numbands = numcutoffs+1;

	unit->m_scalemode = (int)ZIN0(3);

	float * outvals = (float*)RTAlloc(unit->mWorld, numbands * sizeof(float));
	for(int i=0; i<numbands; i++) {
		outvals[i] = 0.f;
	}
	unit->m_outvals = outvals;

	unit->m_cutoffs = (int*)RTAlloc(unit->mWorld, numcutoffs * sizeof(int));
	unit->m_cutoff_inited = false;

	unit->m_numbands = numbands;
}

void FFTSubbandPower_next(FFTSubbandPower *unit, int inNumSamples)
{
	int numbands = unit->m_numbands;
	int numcutoffs = numbands - 1;

	// Multi-output equiv of FFTAnalyser_GET_BUF
	float fbufnum = ZIN0(0);
	if (fbufnum < 0.f) {
		for(int i=0; i<numbands; i++){
			ZOUT0(i) = unit->m_outvals[i];
		}
		return;
	}
	uint32 ibufnum = (uint32)fbufnum;
	World *world = unit->mWorld;
	SndBuf *buf;
	if (ibufnum >= world->mNumSndBufs) {
		int localBufNum = ibufnum - world->mNumSndBufs;
		Graph *parent = unit->mParent;
		if(localBufNum <= parent->localBufNum) {
			buf = parent->mLocalSndBufs + localBufNum;
		} else {
			buf = world->mSndBufs;
		}
	} else {
		buf = world->mSndBufs + ibufnum;
	}
	int numbins = buf->samples - 2 >> 1;
	// End: Multi-output equiv of FFTAnalyser_GET_BUF

	int scalemode = unit->m_scalemode;

	float normfactor = unit->m_normfactor;
	bool square = unit->m_square;
	if(normfactor == 0.f){
		if(square)
			unit->m_normfactor = normfactor = 1.f / powf(numbins + 2.f, 1.5f);
		else
			unit->m_normfactor = normfactor = 1.f / (numbins + 2.f);
	}

	// Now we create the integer lookup list, if it doesn't already exist
	int * cutoffs = unit->m_cutoffs;
	if(!unit->m_cutoff_inited){

		float srate = world->mFullRate.mSampleRate;
		for(int i=0; i < numcutoffs; ++i) {
			cutoffs[i] = (int)(buf->samples * ZIN0(4 + i) / srate);
			//Print("Allocated bin cutoff #%d, at bin %d\n", i, cutoffs[i]);
		}

		unit->m_cutoff_inited = true;
	}

	SCComplexBuf *p = ToComplexApx(buf);

	// Now we can actually calculate the bandwise subtotals
	float total = sc_abs(p->dc);
	if(square){
		total *= total; // square the DC val
	}
	int binaddcount = 1; // Counts how many bins contributed to the current band (1 because of the DC value)
	int curband = 0;
	float * outvals = unit->m_outvals;
	float magsq;
	for (int i=0; i<numbins; ++i) {
		if((curband != numbands) && (i >= cutoffs[curband])){
			if(scalemode==1){
				outvals[curband] = total * normfactor;
			}else{
				if(square)
					outvals[curband] = total / powf((float)binaddcount, 1.5f);
				else
					outvals[curband] = total / binaddcount;
			}
			//Print("Finished off band %i while in bin %i\n", curband, i);
			++curband;
			total = 0.f;
			binaddcount = 0;
		}

		float rabs = (p->bin[i].real);
		float iabs = (p->bin[i].imag);
		magsq = ((rabs*rabs) + (iabs*iabs));
		if(square)
			total += magsq;
		else
			total += std::sqrt(magsq);
		++binaddcount;
	}
	// Remember to output the very last (highest) band
	if(square)
		total += p->nyq * p->nyq;
	else
		total += sc_abs(p->nyq);
	// Pop the last one onto the end of the lovely list
	if(scalemode==1){
		outvals[curband] = total * normfactor;
	}else{
		if(square)
			outvals[curband] = total / powf((float)binaddcount + 1.f, 1.5f); // Plus one because of the nyq value
		else
			outvals[curband] = total / (binaddcount + 1); // Plus one because of the nyq value
	}

	// Now we can output the vals
	for(int i=0; i<numbands; i++) {
		ZOUT0(i) = outvals[i];
	}
}

void FFTSubbandPower_Dtor(FFTSubbandPower *unit)
{
	if(unit->m_cutoffs) RTFree(unit->mWorld, unit->m_cutoffs);
	if(unit->m_outvals) RTFree(unit->mWorld, unit->m_outvals);
}

////////////////////////////////////////////////////////////////////////////////////

void FFTFlux_Ctor(FFTFlux_Unit *unit)
{
	SETCALC(FFTFlux_next);

	unit->m_tempbuf = 0;

	unit->m_yesternorm = 1.0f;
	unit->m_yesterdc = 0.0f;
	unit->m_yesternyq = 0.0f;

	unit->m_normalise = ZIN0(1) > 0.f; // Whether we want to normalise or not

	unit->outval = 0.f;
	ZOUT0(0) = 0.f;
}

void FFTFlux_next(FFTFlux_Unit *unit, int inNumSamples)
{
	FFTAnalyser_GET_BUF

	// Modified form of MAKE TEMP BUF, here used for storing last frame's magnitudes:
	if (!unit->m_tempbuf) {
		unit->m_tempbuf = (float*)RTAlloc(unit->mWorld, numbins * sizeof(float));
		unit->m_numbins = numbins;

		// Must also ensure the yester is zero'ed
		// because it will be compared before being filled in FFTFlux calculation
		memset(unit->m_tempbuf, 0, numbins * sizeof(float));
	} else if (numbins != unit->m_numbins) return;

	SCPolarBuf *p = ToPolarApx(buf); // Current frame
	float* yestermags = unit->m_tempbuf; // This is an array storing the yester magnitudes

	float yesternorm = unit->m_yesternorm; // Should have been calculated on prev cycle

	float currnorm;
	if(unit->m_normalise){
		// First iteration is to find the sum of magnitudes (to find the normalisation factor):
		currnorm = (p->dc * p->dc) + (p->nyq * p->nyq);
		for (int i=0; i<numbins; ++i) {
			currnorm += p->bin[i].mag * p->bin[i].mag;
		}
		// The normalisation factor is 1-over-sum
		if(currnorm != 0.0f) {
			currnorm = 1.0f / currnorm;
		}
	} else {
		currnorm = 1.f;
	}

	// This iteration is the meat of the algorithm. Compare current (normed) bins against prev.
	float onebindiff  = sc_abs(p->dc  * currnorm) - sc_abs(unit->m_yesterdc  * yesternorm);
	float fluxsquared = (onebindiff * onebindiff);
	onebindiff        = sc_abs(p->nyq * currnorm) - sc_abs(unit->m_yesternyq * yesternorm);
	fluxsquared      += (onebindiff * onebindiff);
	// Now the bins
	for (int i=0; i<numbins; ++i) {
		// Sum the squared difference of normalised mags onto the cumulative value
		onebindiff = (p->bin[i].mag * currnorm) - (yestermags[i] * yesternorm);
		fluxsquared += (onebindiff * onebindiff);
		// Overwrite yestermag values with current values, so they're available next time
		yestermags[i] = p->bin[i].mag;
	}

	// Store the just-calc'ed norm as yesternorm
	unit->m_yesternorm = currnorm;
	unit->m_yesterdc = p->dc;
	unit->m_yesternyq = p->nyq;

	// Store the val for output in future calls
	unit->outval = sqrt(fluxsquared);

	ZOUT0(0) = unit->outval;

}
void FFTFlux_Dtor(FFTFlux_Unit *unit)
{
	RTFree(unit->mWorld, unit->m_tempbuf);
}


////////////////////////////////////////////////////////////////////////////////////

void FFTFluxPos_Ctor(FFTFlux_Unit *unit)
{
	SETCALC(FFTFluxPos_next);

	unit->m_tempbuf = 0;

	unit->m_yesternorm = 1.0f;
	unit->m_yesterdc = 0.0f;
	unit->m_yesternyq = 0.0f;

	unit->outval = 0.f;
	ZOUT0(0) = 0.f;
}

void FFTFluxPos_next(FFTFlux_Unit *unit, int inNumSamples)
{
	FFTAnalyser_GET_BUF

	// Modified form of MAKE TEMP BUF, here used for storing last frame's magnitudes:
	if (!unit->m_tempbuf) {
		unit->m_tempbuf = (float*)RTAlloc(unit->mWorld, numbins * sizeof(float));
		unit->m_numbins = numbins;

		// Must also ensure the yester is zero'ed
		// because it will be compared before being filled in FFTFlux calculation
		memset(unit->m_tempbuf, 0, numbins * sizeof(float));
	} else if (numbins != unit->m_numbins) return;

	SCPolarBuf *p = ToPolarApx(buf); // Current frame
	float* yestermags = unit->m_tempbuf; // This is an array storing the yester magnitudes

	float yesternorm = unit->m_yesternorm; // Should have been calculated on prev cycle

	float currnorm;
	if(unit->m_normalise){
		// First iteration is to find the sum of magnitudes (to find the normalisation factor):
		currnorm = (p->dc * p->dc) + (p->nyq * p->nyq);
		for (int i=0; i<numbins; ++i) {
			currnorm += p->bin[i].mag * p->bin[i].mag;
		}
		// The normalisation factor is 1-over-sum
		if(currnorm != 0.0f) {
			currnorm = 1.0f / currnorm;
		}
	} else {
		currnorm = 1.f;
	}


	// This iteration is the meat of the algorithm. Compare current (normed) bins against prev.
	float onebindiff  = sc_abs(p->dc  * currnorm) - sc_abs(unit->m_yesterdc  * yesternorm);
	float fluxsquared = 0.f;
	if(onebindiff > 0.f)
		fluxsquared += (onebindiff * onebindiff);
	onebindiff        = sc_abs(p->nyq * currnorm) - sc_abs(unit->m_yesternyq * yesternorm);
	if(onebindiff > 0.f)
		fluxsquared += (onebindiff * onebindiff);
	// Now the bins
	for (int i=0; i<numbins; ++i) {
		// Sum the squared difference of normalised mags onto the cumulative value
		onebindiff = (p->bin[i].mag * currnorm) - (yestermags[i] * yesternorm);

		////////// THIS IS WHERE FFTFluxPos DIFFERS FROM FFTFlux - THE SIMPLE ADDITION OF AN "IF": //////////
		if(onebindiff > 0.f) // The IF only applies to the next line - formatting is a bit weird to keep in line with the other func

		fluxsquared += (onebindiff * onebindiff);
		// Overwrite yestermag values with current values, so they're available next time
		yestermags[i] = p->bin[i].mag;
	}

	// Store the just-calc'ed norm as yesternorm
	unit->m_yesternorm = currnorm;
	unit->m_yesterdc = p->dc;
	unit->m_yesternyq = p->nyq;

	// Store the val for output in future calls
	unit->outval = sqrt(fluxsquared);

	ZOUT0(0) = unit->outval;

}
void FFTFluxPos_Dtor(FFTFlux_Unit *unit)
{
	RTFree(unit->mWorld, unit->m_tempbuf);
}

////////////////////////////////////////////////////////////////////////////////////

void FFTDiffMags_next(FFTAnalyser_Unit *unit, int inNumSamples)
{

	FFTAnalyser_GET_BUF2

	SCComplexBuf *p = ToComplexApx(buf1);
	SCComplexBuf *q = ToComplexApx(buf2);

	// First the DC and nyquist.
	float diffsum = sc_abs(p->dc - q->dc) + sc_abs(p->nyq - q->nyq);

	for (int i=0; i<numbins; ++i) {
		float rdiff = p->bin[i].real - q->bin[i].real;
		float idiff = p->bin[i].imag - q->bin[i].imag;

		diffsum += sqrt(rdiff*rdiff + idiff*idiff);
	}

	// Store the val for output in future calls
	unit->outval = diffsum / (numbins + 2);

    //Print("FFTDiffMags_next: output is %g\n", unit->outval);

	ZOUT0(0) = unit->outval;
}

void FFTDiffMags_Ctor(FFTAnalyser_Unit *unit)
{
	SETCALC(FFTDiffMags_next);
	ZOUT0(0) = ZIN0(0);
}

////////////////////////////////////////////////////////////////////////////////////

void PV_MagSubtract_next(PV_Unit *unit, int inNumSamples)
{
	PV_GET_BUF2

	SCPolarBuf *p = ToPolarApx(buf1);
	SCPolarBuf *q = ToPolarApx(buf2);

	bool zerolimit = ZIN0(2) > 0.f;

	if(zerolimit){
		// First the DC and nyquist
		if(p->dc > q->dc)
			p->dc  = p->dc - q->dc;
		else
			p->dc = 0.f;

		if(p->nyq > q->nyq)
			p->nyq = p->nyq - q->nyq;
		else
			p->nyq = 0.f;

		for (int i=0; i<numbins; ++i) {
			if(p->bin[i].mag > q->bin[i].mag)
				p->bin[i].mag -= q->bin[i].mag;
			else
				p->bin[i].mag = 0.f;
		}
	}else{
		// First the DC and nyquist
		p->dc  = p->dc - q->dc;
		p->nyq = p->nyq - q->nyq;
		for (int i=0; i<numbins; ++i) {
			p->bin[i].mag -= q->bin[i].mag;
		}
	}
}

void PV_MagSubtract_Ctor(PV_Unit *unit)
{
	SETCALC(PV_MagSubtract_next);
	ZOUT0(0) = ZIN0(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void PV_MagLog_Ctor(PV_Unit *unit)
{
	SETCALC(PV_MagLog_next);
	ZOUT0(0) = ZIN0(0);
}

void PV_MagLog_next(PV_Unit *unit, int inNumSamples)
{
	PV_GET_BUF

	SCPolarBuf *p = ToPolarApx(buf);

	for (int i=0; i<numbins; ++i) {
		float mag = p->bin[i].mag;
		p->bin[i].mag = log(mag > SMALLEST_NUM_FOR_LOG ? mag : SMALLEST_NUM_FOR_LOG);
	}
}

////

void PV_MagExp_Ctor(PV_Unit *unit)
{
	SETCALC(PV_MagExp_next);
	ZOUT0(0) = ZIN0(0);
}

void PV_MagExp_next(PV_Unit *unit, int inNumSamples)
{
	PV_GET_BUF

	SCPolarBuf *p = ToPolarApx(buf);

	for (int i=0; i<numbins; ++i) {
		float mag = p->bin[i].mag;
		p->bin[i].mag = exp(mag);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void FFTPhaseDev_next(FFTPhaseDev *unit, int inNumSamples)
{
	FFTAnalyser_GET_BUF

	// Get the current frame, as polar. NB We don't care about DC or nyquist in this UGen.
	SCPolarBuf *p = ToPolarApx(buf);
	int tbpointer;

	float powthresh = ZIN0(2);

	// MAKE_TEMP_BUF but modified:
	if (!unit->m_tempbuf) {
		unit->m_tempbuf = (float*)RTAlloc(unit->mWorld, numbins * 2 * sizeof(float));
		memset(unit->m_tempbuf, 0, numbins * 2 * sizeof(float)); // Ensure it's zeroed
		// Ensure the initial values don't cause some weird jump in the output - set them to vals which will produce deviation of zero
		tbpointer = 0;
		for (int i=0; i<numbins; ++i) {
			unit->m_tempbuf[tbpointer++] = p->bin[i].phase;
			unit->m_tempbuf[tbpointer++] = 0.f;
		}
		unit->m_numbins = numbins;
	} else if (numbins != unit->m_numbins) return;

	// Retrieve state
	bool useweighting = unit->m_weight;
	float *storedvals = unit->m_tempbuf;

	// Note: temp buf is stored in this format: phase[0],d_phase[0],phase[1],d_phase[1], ...

//	Print("\nbin[10] phase: %g\nbin[10] yesterphase: %g\nbin[10] yesterdiff: %g\n",
//		/*PHASE_REWRAP(*/p->bin[10].phase/*)*/, storedvals[20], storedvals[21]);

	//Print("\npowthresh is %g", powthresh);

	// Iterate through, calculating the deviation from expected value.
	double totdev = 0.0;
	tbpointer = 0;
	float deviation;
	for (int i=0; i<numbins; ++i) {
		// Thresholding as Brossier did - discard bin's phase deviation if bin's power is minimal
		if(p->bin[i].mag > powthresh) {

			// Deviation is the *second difference* of the phase, which is calc'ed as curval - yesterval - yesterfirstdiff
			deviation = p->bin[i].phase - storedvals[tbpointer] - storedvals[tbpointer+1];
			tbpointer += 2;
			// Wrap onto +-PI range
			deviation = PHASE_REWRAP(deviation);

			if(useweighting){
				totdev += fabs(deviation * p->bin[i].mag);
			} else {
				totdev += fabs(deviation);
			}
		}
	}

	// totdev will be the output, but first we need to fill tempbuf with today's values, ready for tomorrow.
	tbpointer = 0;
	float diff;
	for (int i=0; i<numbins; ++i) {
		diff = p->bin[i].phase - storedvals[tbpointer]; // Retrieving yesterphase from buf
		storedvals[tbpointer++] = p->bin[i].phase; // Storing phase
		// Wrap onto +-PI range
		diff = PHASE_REWRAP(diff);

		storedvals[tbpointer++] = diff; // Storing first diff to buf

	}

	// Store the val for output in future calls
	unit->outval = (float)totdev;

	ZOUT0(0) = unit->outval;
}

void FFTPhaseDev_Ctor(FFTPhaseDev *unit)
{
	SETCALC(FFTPhaseDev_next);

	unit->m_weight = (ZIN0(1) > 0.f) ? true : false;

	ZOUT0(0) = unit->outval = 0.;
	unit->m_tempbuf = 0;
}

void FFTPhaseDev_Dtor(FFTPhaseDev *unit)
{
	RTFree(unit->mWorld, unit->m_tempbuf);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void FFTComplexDev_next(FFTComplexDev *unit, int inNumSamples)
{
	FFTAnalyser_GET_BUF

	// Get the current frame, as polar. NB We don't care about DC or nyquist in this UGen.
	SCPolarBuf *p = ToPolarApx(buf);
	int tbpointer;

	float powthresh = ZIN0(2);

	// MAKE_TEMP_BUF but modified:
	if (!unit->m_tempbuf) {
		unit->m_tempbuf = (float*)RTAlloc(unit->mWorld, numbins * 3 * sizeof(float));
		memset(unit->m_tempbuf, 0,  numbins * 3 * sizeof(float)); // Ensure it's zeroed
		// Ensure the initial values don't cause some weird jump in the output - set them to vals which will produce deviation of zero
		tbpointer = 0;
		for (int i=0; i<numbins; ++i) {
			unit->m_tempbuf[tbpointer++] = p->bin[i].phase;
			unit->m_tempbuf[tbpointer++] = 0.f;
		}
		unit->m_numbins = numbins;
	} else if (numbins != unit->m_numbins) return;

	// Retrieve state
	float *storedvals = unit->m_tempbuf;
	bool rectify = unit->m_rectify;

	// Note: temp buf is stored in this format: mag[0],phase[0],d_phase[0],mag[1],phase[1],d_phase[1], ...


	//Print("\npowthresh is %g", powthresh);

	// Iterate through, calculating the deviation from expected value.
	double totdev = 0.0;
	tbpointer = 0;
	float curmag, predmag, predphase, yesterphase, yesterphasediff;
	float deviation;
	for (int i=0; i<numbins; ++i) {
		curmag = p->bin[i].mag;

		// Predict mag as yestermag
		predmag = storedvals[tbpointer++];
		yesterphase = storedvals[tbpointer++];
		yesterphasediff = storedvals[tbpointer++];

		// Thresholding as Brossier did - discard bin's deviation if bin's power is minimal
		if(curmag > powthresh) {
			// If rectifying, ignore decreasing bins
			if((!rectify) || (curmag >= predmag)){

				// Predict phase as yesterval + yesterfirstdiff
				predphase = yesterphase + yesterphasediff;

				// Deviation is Euclidean distance between predicted and actual.
				// In polar coords: sqrt(r1^2 +  r2^2 - r1r2 cos (theta1 - theta2))
				deviation = sqrt(predmag * predmag + curmag * curmag
								  - predmag * predmag * cos(PHASE_REWRAP(predphase - p->bin[i].phase))
								);

				totdev += deviation;
			}
		}
	}

	// totdev will be the output, but first we need to fill tempbuf with today's values, ready for tomorrow.
	tbpointer = 0;
	float diff;
	for (int i=0; i<numbins; ++i) {
		storedvals[tbpointer++] = p->bin[i].mag; // Storing mag
		diff = p->bin[i].phase - storedvals[tbpointer]; // Retrieving yesterphase from buf
		storedvals[tbpointer++] = p->bin[i].phase; // Storing phase
		// Wrap onto +-PI range
		diff = PHASE_REWRAP(diff);

		storedvals[tbpointer++] = diff; // Storing first diff to buf

	}

	// Store the val for output in future calls
	unit->outval = (float)totdev;

	ZOUT0(0) = unit->outval;
}

void FFTComplexDev_Ctor(FFTComplexDev *unit)
{
	SETCALC(FFTComplexDev_next);

	unit->m_rectify = (ZIN0(1) > 0.f) ? true : false;

	ZOUT0(0) = unit->outval = 0.;
	unit->m_tempbuf = 0;
}

void FFTComplexDev_Dtor(FFTComplexDev *unit)
{
	RTFree(unit->mWorld, unit->m_tempbuf);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////

void FFTMKL_next(FFTMKL *unit, int inNumSamples)
{
	FFTAnalyser_GET_BUF

	// Get the current frame, as polar. NB We don't care about DC or nyquist in this UGen.
	SCPolarBuf *p = ToPolarApx(buf);
	int tbpointer;
	float eta = ZIN0(1);

	// MAKE_TEMP_BUF but modified:
	if (!unit->m_tempbuf) {
		unit->m_tempbuf = (float*)RTAlloc(unit->mWorld, numbins * 1 * sizeof(float));
		memset(unit->m_tempbuf, 0, numbins * 1 * sizeof(float)); // Ensure it's zeroed
		// Ensure the initial values don't cause some weird jump in the output - set them to vals which will produce deviation of zero
		tbpointer = 0;
		for (int i=0; i<numbins; ++i) {
			unit->m_tempbuf[tbpointer++] = p->bin[i].mag;
		}
		unit->m_numbins = numbins;
	} else if (numbins != unit->m_numbins) return;

	// Retrieve state
	float *storedvals = unit->m_tempbuf;

	// Note: for this UGen, temp buf is just mag[0],mag[1],... - we ain't interested in phase etc

	// Iterate through, calculating the Modified Kullback-Liebler distance
	double totdev = 0.0;
	tbpointer = 0;
	float curmag, yestermag;
	float deviation;
	for (int i=0; i<numbins; ++i) {
		curmag = p->bin[i].mag;
		yestermag = storedvals[tbpointer];

		// Here's the main implementation of Brossier's MKL eq'n (eqn 2.9 from his thesis):
		deviation = sc_abs(curmag) / (sc_abs(yestermag) + eta);
		totdev += log(1.f + deviation);

		// Store the mag as yestermag
		storedvals[tbpointer++] = curmag;
	}

	// Store the val for output in future calls
	unit->outval = (float)totdev;

	ZOUT0(0) = unit->outval;
}

void FFTMKL_Ctor(FFTMKL *unit)
{
	SETCALC(FFTMKL_next);

	ZOUT0(0) = unit->outval = 0.;
	unit->m_tempbuf = 0;
}

void FFTMKL_Dtor(FFTMKL *unit)
{
	RTFree(unit->mWorld, unit->m_tempbuf);
}


////////////////////////////////////////////////////////////////////////////////

void PV_Whiten_Ctor(PV_Whiten *unit){

	SETCALC(PV_Whiten_next);

	ZOUT0(0) = ZIN0(0);
}


void PV_Whiten_next(PV_Whiten *unit, int inNumSamples){

	float fbufnum1 = ZIN0(0);
	float fbufnum2 = ZIN0(1);
	if (fbufnum1 < 0.f || fbufnum2 < 0.f) { ZOUT0(0) = -1.f; return; }
//	Print("\nfbufnum1: %g; fbufnum2: %g", fbufnum1, fbufnum2);
	uint32 ibufnum1 = (int)fbufnum1;
	uint32 ibufnum2 = (int)fbufnum2;
	World *world = unit->mWorld;
	SndBuf *buf1;
	if (ibufnum1 >= world->mNumSndBufs) {
		int localBufNum = ibufnum1 - world->mNumSndBufs;
		Graph *parent = unit->mParent;
		if(localBufNum <= parent->localBufNum) {
			buf1 = parent->mLocalSndBufs + localBufNum;
		} else {
			buf1 = world->mSndBufs;
		}
	} else {
		buf1 = world->mSndBufs + ibufnum1;
	}
	SndBuf *buf2;
	if (ibufnum2 >= world->mNumSndBufs) {
		int localBufNum = ibufnum2 - world->mNumSndBufs;
		Graph *parent = unit->mParent;
		if(localBufNum <= parent->localBufNum) {
			buf2 = parent->mLocalSndBufs + localBufNum;
		} else {
			buf2 = world->mSndBufs;
		}
	} else {
		buf2 = world->mSndBufs + ibufnum2;
	}
	int numbins = buf1->samples - 2 >> 1;
//	Print("\nibufnum1: %d; ibufnum2: %d", ibufnum1, ibufnum2);
//	if (buf1->samples != buf2->samples) return;

//	Print("\nnumbins: %d", numbins);

//	memcpy(buf2->data, buf1->data, buf1->samples * sizeof(float));

	SCPolarBuf *indata = ToPolarApx(buf1);

	// This buffer stores numbins+2 amplitude tracks, in "logical" order (DC, bin1, ... nyquist), not in the order produced by the FFT
	float *pkdata = buf2->data;

	// Update the parameters
	float relax = ZIN0(2);
	float relaxcoef = (relax == 0.0f) ? 0.0f : exp(log1/(relax * SAMPLERATE));
	float floor = ZIN0(3);
	float smear = ZIN0(4);
//	unsigned int bindownsample = (int)ZIN0(5);

	float val,oldval;
	////////////////////// Now for each bin, update the record of the peak value /////////////////////

	val = fabs(indata->dc);	// Grab current magnitude
	oldval = pkdata[0];
	// If it beats the amplitude stored then that's our new amplitude; otherwise our new amplitude is a decayed version of the old one
	if(val < oldval) {
		val = val + (oldval - val) * relaxcoef;
	}
	pkdata[0] = val; // Store the "amplitude trace" back

	val = fabs(indata->nyq);
	oldval = pkdata[numbins+1];
	if(val < oldval) {
		val = val + (oldval - val) * relaxcoef;
	}
	pkdata[numbins+1] = val;

//	Print("-----------Peaks-------\n");
	for(int i=0; i<numbins; ++i){
		val = fabs(indata->bin[i].mag);
		oldval = pkdata[i+1];
		if(val < oldval) {
			val = val + (oldval - val) * relaxcoef;
		}
		pkdata[i+1] = val;
		//Print("%g, ", val);
	}
//	Print("\n");

	// Perform smearing now
	if(smear != 0.f){
		float oldval, newval;
		oldval = pkdata[0];
		// What we want is to keep the largest of curval, prevval*smear, nextval*smear.
		// We do this in two steps, by keeping the biggest of prevval and nextval, then keeping the largest of (biggest*smear, curval)
		for(int i=1; i<=numbins; i++){
			oldval = sc_max(oldval, pkdata[i+1]);
			newval = sc_max(oldval * smear, pkdata[i]);

			oldval = pkdata[i]; // For next iter
			pkdata[i] = newval;
		}
	}

	//////////////////////////// Now for each bin, rescale the current magnitude ////////////////////////////
	indata->dc  /= sc_max(floor, pkdata[0]);
	indata->nyq /= sc_max(floor, pkdata[numbins+1]);
	for(int i=0; i<numbins; ++i){
		indata->bin[i].mag /= sc_max(floor, pkdata[i+1]);
	}

	ZOUT0(0) = fbufnum1;

}

//////////////////////////////////////////////////////////////////////////////////////////////////

void FFTSubbandFlatness_next(FFTSubbandFlatness *unit, int inNumSamples)
{
	int numbands = unit->m_numbands;
	int numcutoffs = numbands - 1;

	// Multi-output equiv of FFTAnalyser_GET_BUF
	float fbufnum = ZIN0(0);
	if (fbufnum < 0.f) {
		for(int i=0; i<numbands; i++){
			ZOUT0(i) = unit->m_outvals[i];
		}
		return;
	}
	uint32 ibufnum = (uint32)fbufnum;
	World *world = unit->mWorld;
	SndBuf *buf;
	if (ibufnum >= world->mNumSndBufs) {
		int localBufNum = ibufnum - world->mNumSndBufs;
		Graph *parent = unit->mParent;
		if(localBufNum <= parent->localBufNum) {
			buf = parent->mLocalSndBufs + localBufNum;
		} else {
			buf = world->mSndBufs;
		}
	} else {
		buf = world->mSndBufs + ibufnum;
	}
	int numbins = buf->samples - 2 >> 1;
	// End: Multi-output equiv of FFTAnalyser_GET_BUF

	// Now we create the integer lookup list, if it doesn't already exist
	int * cutoffs = unit->m_cutoffs;
	if(!unit->m_cutoff_inited){

		float srate = world->mFullRate.mSampleRate;
		for(int i=0; i < numcutoffs; i++) {
			cutoffs[i] = (int)(buf->samples * ZIN0(2 + i) / srate);
			//Print("Allocated bin cutoff #%d, at bin %d\n", i, cutoffs[i]);
		}

		unit->m_cutoff_inited = true;
	}

	SCPolarBuf *p = ToPolarApx(buf);

	// Now we can actually calculate the bandwise stuff
	int binaddcount = 0; // Counts how many bins contributed to the current band
	int curband = 0;
	float * outvals = unit->m_outvals;

	double geommean = 0.0, arithmean = 0.0;

	for (int i=0; i<numbins; ++i) {
		if(i == cutoffs[curband]){
			// Finish off the mean calculations
			geommean = exp(geommean / binaddcount);
			arithmean /= binaddcount;
			outvals[curband] = (float)(geommean / arithmean);
			curband++;
			geommean = arithmean = 0.0;
			binaddcount = 0;
		}

		float mag = (p->bin[i].mag);
		geommean += log(mag);
		arithmean += mag;

		binaddcount++;
	}

	// Remember to output the very last (highest) band
	// Do the nyquist
	geommean += log(sc_abs(p->nyq));
	arithmean += sc_abs(p->nyq);
	binaddcount++;
	// Finish off the mean calculations
	geommean = exp(geommean / binaddcount);
	arithmean /= binaddcount;
	outvals[curband] = (float)(geommean / arithmean);

	// Now we can output the vals
	for(int i=0; i<numbands; i++) {
		ZOUT0(i) = outvals[i];
	}
}

void FFTSubbandFlatness_Ctor(FFTSubbandFlatness *unit)
{
	SETCALC(FFTSubbandFlatness_next);

	// ZIN0(1) tells us how many cutoffs we're looking for
	int numcutoffs = (int)ZIN0(1);
	int numbands = numcutoffs+1;

	float * outvals = (float*)RTAlloc(unit->mWorld, numbands * sizeof(float));
	for(int i=0; i<numbands; i++) {
		outvals[i] = 0.f;
	}
	unit->m_outvals = outvals;

	unit->m_cutoffs = (int*)RTAlloc(unit->mWorld,
			numcutoffs * sizeof(int)
		);

	unit->m_cutoff_inited = false;

	unit->m_numbands = numbands;
	ZOUT0(0) = unit->outval = 0.;
}

void FFTSubbandFlatness_Dtor(FFTSubbandFlatness *unit)
{
	RTFree(unit->mWorld, unit->m_cutoffs);
	RTFree(unit->mWorld, unit->m_outvals);
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void FFTCrest_Ctor(FFTCrest *unit)
{
	SETCALC(FFTCrest_next);

	unit->m_cutoffneedsinit = true;
	unit->m_freqtobin = 0.f;

	ZOUT0(0) = unit->outval = 1.f;
}

void FFTCrest_next(FFTCrest *unit, int inNumSamples)
{
	float freqlo = IN0(1);
	float freqhi = IN0(2);

	FFTAnalyser_GET_BUF

	//SCPolarBuf *p = ToPolarApx(buf); // Seems buggy...?
	SCComplexBuf *p = ToComplexApx(buf);

	GET_FREQTOBIN

	if(unit->m_cutoffneedsinit){
		// Get desired range, convert to bin index
		unit->m_frombin = (int)(freqtobin * freqlo);
		unit->m_tobinp1 = (int)(freqtobin * freqhi);
		if(unit->m_frombin < 0)
			unit->m_frombin = 0;
		if(unit->m_tobinp1 > numbins)
			unit->m_tobinp1 = numbins;

		unit->m_cutoffneedsinit = false;
	}
	int frombin = unit->m_frombin;
	int tobinp1 = unit->m_tobinp1;

	float total = 0.f, scf, sqrmag, peak=0.f;
	for (int i=frombin; i<tobinp1; ++i) {
		//sqrmag = p->bin[i].mag * p->bin[i].mag;
		sqrmag = (p->bin[i].real * p->bin[i].real) + (p->bin[i].imag * p->bin[i].imag);
		// (1) Check if it's the peak
		if(sqrmag >= peak){
			peak = sqrmag;
		}
		// (2) Add to subtotal
		total = total + sqrmag;
	}

	// SCF defined as peak val divided by mean val; in other words, peak * count / total
	if(total == 0.f)
		scf = 1.f; // If total==0, peak==0, so algo output is indeterminate; but 1 indicates a perfectly flat spectrum, so we use that
	else
		scf = peak * ((float)(tobinp1 - frombin - 1)) / total;

	// Store the val for output in future calls
	unit->outval = scf;

	ZOUT0(0) = unit->outval;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void FFTSpread_Ctor(FFTSpread *unit)
{
	SETCALC(FFTSpread_next);
	ZOUT0(0) = unit->outval = 0.;

	unit->m_bintofreq = 0.f;
}

void FFTSpread_next(FFTSpread *unit, int inNumSamples)
{
	FFTAnalyser_GET_BUF

	SCPolarBuf *p = ToPolarApx(buf);

	GET_BINTOFREQ

	float centroid = ZIN0(1);

	float  distance = ((numbins + 1) * bintofreq) - centroid;
	float  mag      = sc_abs(p->nyq);
	double num      = mag * distance * distance;
	double denom    = mag;
	for (int i=0; i<numbins; ++i) {
		distance = ((i+1) * bintofreq) - centroid;
		mag      = sc_abs(p->bin[i].mag);
		num     += mag * distance * distance;
		denom   += mag;
	}

	ZOUT0(0) = unit->outval = (denom==0.0 ? 0.f : (float) num/denom);

}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void FFTSlope_Ctor(FFTSlope *unit)
{
	SETCALC(FFTSlope_next);
	ZOUT0(0) = unit->outval = 0.;

	unit->m_bintofreq = 0.f;
}

void FFTSlope_next(FFTSlope *unit, int inNumSamples)
{
	FFTAnalyser_GET_BUF

	SCPolarBuf *p = ToPolarApx(buf);

	GET_BINTOFREQ

	// This is a straightforward linear regression slope on the magnitudes.

	// These vars accumulate as we iter the bins. We start by putting in their values from the DC & Nyquist oddities.
	double sumx  = (numbins+1) * bintofreq;
	double sumx2 = sumx * sumx;
	double sumxy = sumx * sc_abs(p->nyq);
	double sumy  = sc_abs(p->dc) + sc_abs(p->nyq);
	double mag, freq;

	for(int i=0; i<numbins; ++i){
	  mag = p->bin[i].mag;
	  freq = (i+1) * bintofreq;

	  sumxy += (freq * mag);
	  sumx  += freq;
	  sumy  += mag;
	  sumx2 += (freq*freq);
	};

	float slope = (float)((numbins * sumxy - sumx * sumy)
	   / (numbins * sumx2 - sumx * sumx));

	ZOUT0(0) = unit->outval = slope;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void FFTPeak_Ctor(FFTPeak *unit)
{
	SETCALC(FFTPeak_next);
	ZOUT0(0) = unit->outval = 0.;

	unit->m_bintofreq = 0.f;
	unit->m_freqtobin = 0.f;

	unit->minbin = -99; // flag for the _next func to initialise it
	unit->minfreq = ZIN0(1);
	if(unit->minfreq < 0.f) unit->minfreq = 0.f;
	unit->maxfreq = ZIN0(2);
	if(unit->maxfreq < 0.f) unit->maxfreq = 0.f;
}

void FFTPeak_next(FFTPeak *unit, int inNumSamples)
{
	FFTAnalyser_GET_BUF_TWOOUTS

	SCPolarBuf *p = ToPolarApx(buf);

	GET_BINTOFREQ
	GET_FREQTOBIN

	int minbin = unit->minbin;
	int maxbin = unit->maxbin;
	if(minbin == -99){
		// Initialise bin ranges from frequency ranges
		minbin = unit->minbin = ((int)(unit->minfreq * freqtobin)) - 1;
		if(minbin >= numbins)
			minbin = unit->minbin = numbins - 1;
		maxbin = unit->maxbin = ((int)(unit->maxfreq * freqtobin)) - 1;
		if(maxbin > numbins)
			maxbin = unit->maxbin = numbins;
	}

	// Start off assuming DC is the best...
	int peakbin=-1; // "-1" meaning DC here, since the DC is not included in the main list
	float peakmag;
	if(minbin == -1){
		peakmag = sc_abs(p->dc);
		minbin = 0;
	}else{
		peakmag = -9999.f; // Will ensure DC gets beaten if not in desired range
	}

	// ...then check all the others. We neglect nyquist for efficiency purposes. Sorry.
	float mag;
	for(int i = minbin; i < maxbin; ++i){
	  mag = p->bin[i].mag;

	  if(peakmag < mag){
		peakmag = mag;
		peakbin = i;
	  }
	};

	ZOUT0(0) = unit->outval = (peakbin+1) * bintofreq;
	ZOUT0(1) = unit->outval2 = peakmag;

}

////////////////////////////////////////////////////////////////////////////////

void PV_MagSmooth_Ctor(PV_MagSmooth *unit)
{
	SETCALC(PV_MagSmooth_next);
	ZOUT0(0) = ZIN0(0);
	unit->m_memory = NULL;
}

void PV_MagSmooth_next(PV_MagSmooth *unit, int inNumSamples)
{
	PV_GET_BUF

	SCPolarBuf *p = ToPolarApx(buf);

	float* memory = unit->m_memory;
	if(memory==NULL){
		memory = unit->m_memory = (float*)RTAlloc(unit->mWorld, (numbins+2) * sizeof(float));
		// Now copy the first frame into the memory
		for (int i=0; i<numbins; ++i) {
			memory[i] = p->bin[i].mag;
		}
		memory[numbins] = p->dc;
		memory[numbins+1] = p->nyq;
	}

	float factor = ZIN0(1);
	float onemfactor = 1.f - factor;

	// The actual smoothing calculation:
	for (int i=0; i<numbins; ++i) {
		memory[i]     = p->bin[i].mag = (memory[i        ] * factor) + (p->bin[i].mag  * onemfactor);
	}
	memory[numbins]   = p->dc         = (memory[numbins  ] * factor) + (p->dc          * onemfactor);
	memory[numbins+1] = p->nyq        = (memory[numbins+1] * factor) + (p->nyq         * onemfactor);
}

void PV_MagSmooth_Dtor(PV_MagSmooth *unit)
{
	if(unit->m_memory!=NULL){
		RTFree(unit->mWorld, unit->m_memory);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void PV_MagMulAdd_Ctor(PV_Unit *unit)
{
	SETCALC(PV_MagMulAdd_next);
	ZOUT0(0) = ZIN0(0);
}

void PV_MagMulAdd_next(PV_Unit *unit, int inNumSamples)
{
	PV_GET_BUF

	SCPolarBuf *p = ToPolarApx(buf);

	float m = ZIN0(1);
	float a = ZIN0(2);

	p->dc  = p->dc  * m + a;
	p->nyq = p->nyq * m + a;
	for (int i=0; i<numbins; ++i) {
		p->bin[i].mag = p->bin[i].mag * m + a;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

#define GET_BUF_AT1 \
	float fbufnum1  = ZIN0(1); \
	if (fbufnum1 < 0.f) { fbufnum1 = 0.f; } \
	if (fbufnum1 != unit->m_fbufnum) { \
		uint32 bufnum = (int)fbufnum1; \
		World *world = unit->mWorld; \
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
		unit->m_fbufnum = fbufnum1; \
	} \
	SndBuf* buf1 = unit->m_buf; \
	LOCK_SNDBUF(buf1); \
	float *bufData __attribute__((__unused__)) = buf1->data; \
	uint32 bufChannels __attribute__((__unused__)) = buf1->channels; \
	uint32 bufSamples __attribute__((__unused__)) = buf1->samples; \
	uint32 bufFrames = buf1->frames; \
	int mask __attribute__((__unused__)) = buf1->mask; \
	int guardFrame __attribute__((__unused__)) = bufFrames - 2;

void PV_ExtractRepeat_Ctor(PV_ExtractRepeat *unit)
{
	unit->m_logmags = NULL;
	unit->m_cursor = 0;
	unit->m_fbufnum = -1;
	SETCALC(PV_ExtractRepeat_next);
	ZOUT0(0) = ZIN0(0);
}

void PV_ExtractRepeat_next(PV_ExtractRepeat *unit, int inNumSamples)
{
	PV_GET_BUF
	SCPolarBuf *p = ToPolarApx(buf);
	GET_BUF_AT1

	if((numbins+2) != bufChannels){
		printf("PV_ExtractRepeat error: fft magnitude size != bufChannels, %i > %i\n", (numbins+2), bufChannels);
		return;
	}

	float loopdursecs    = ZIN0(2);
	float memorytimesecs = ZIN0(3);
	bool  which          = ZIN0(4) > 0.f;
	float ffthopsize     = ZIN0(5);
	float thresh         = ZIN0(6);

	int loopdurframes = loopdursecs * FULLRATE / ((numbins+numbins+2) * ffthopsize);
	//printf("PV_ExtractRepeat info: loopdurframes is %i\n", bufFrames);
	if(loopdurframes > bufFrames){
		printf("PV_ExtractRepeat warning: loopdurframes > bufFrames, %i > %i\n", loopdurframes, bufFrames);
		loopdurframes = bufFrames;
	}

	float* logmags = unit->m_logmags;
	if(logmags==NULL){
		logmags = unit->m_logmags = (float*)RTAlloc(unit->mWorld, (numbins+2) * sizeof(float));
		// Here we zero the buffer, not the logmags, but we do it here as first-run initialisation.
		memset(bufData, 0, bufChannels * bufFrames * sizeof(float));
	}

	float mag;
	for (int i=0; i<numbins; ++i) {
		mag = p->bin[i].mag;
		logmags[i]     = log(mag > SMALLEST_NUM_FOR_LOG ? mag : SMALLEST_NUM_FOR_LOG);
	}
	// Note, we store the dc and nyquist ON THE END
	mag = sc_abs(p->dc);
	logmags[numbins  ] = log(mag > SMALLEST_NUM_FOR_LOG ? mag : SMALLEST_NUM_FOR_LOG);
	mag = sc_abs(p->nyq);
	logmags[numbins+1] = log(mag > SMALLEST_NUM_FOR_LOG ? mag : SMALLEST_NUM_FOR_LOG);

	// Advance the recursive-buffer-cursor by one. Let's call the 'current' frame in the recursive buffer R.
	int cursor = unit->m_cursor + 1;
	if(cursor >= loopdurframes)
		cursor = 0;
	unit->m_cursor = cursor;
	float* curframe = bufData + (cursor * bufChannels);

	// find (X-R) (the original authors would have taken abs of this too, but that seems harmful to me).
	// find which bins are (X-R)<t  where t=1. Zero them out (in S).
	//    (OR do the reverse; user-settable flag, applied here using xors.)
	for (int i=0; i<numbins; ++i)
		if((logmags[i] - curframe[i] < thresh) xor which)
			p->bin[i].mag = 0;
	if((logmags[numbins  ] - curframe[numbins  ] < thresh) xor which)
		p->dc  = 0;
	if((logmags[numbins+1] - curframe[numbins+1] < thresh) xor which)
		p->nyq = 0;

	// Finally update the recursive buffer by writing X onto it crossfade-style.
	float writestrength = (memorytimesecs == 0.f) ? 0.f :
		exp(log001 / (memorytimesecs * FULLRATE / ((numbins+numbins+2) * ffthopsize)));
	float antiwritestrength = 1.f - writestrength;
	//printf("PV_ExtractRepeat info: memorytimesecs %g gives write strength %g\n", memorytimesecs, writestrength);
	for (int i=0; i<numbins+2; ++i) {
		curframe[i] = (curframe[i] * antiwritestrength) + (logmags[i] * writestrength);
	}
}

void PV_ExtractRepeat_Dtor(PV_ExtractRepeat *unit)
{
	if(unit->m_logmags) RTFree(unit->mWorld, unit->m_logmags);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

PluginLoad(MCLDFFT)
{
	ft= inTable;

	init_SCComplex(inTable);

	(*ft->fDefineUnit)("FFTPower", sizeof(FFTPower), (UnitCtorFunc)&FFTPower_Ctor, 0, 0);
	(*ft->fDefineUnit)("FFTFlux", sizeof(FFTFlux_Unit), (UnitCtorFunc)&FFTFlux_Ctor, (UnitDtorFunc)&FFTFlux_Dtor, 0);
	(*ft->fDefineUnit)("FFTFluxPos", sizeof(FFTFlux_Unit), (UnitCtorFunc)&FFTFluxPos_Ctor, (UnitDtorFunc)&FFTFluxPos_Dtor, 0);
	(*ft->fDefineUnit)("FFTDiffMags", sizeof(FFTAnalyser_Unit), (UnitCtorFunc)&FFTDiffMags_Ctor, 0, 0);
	DefineSimpleUnit(PV_MagSubtract);
	(*ft->fDefineUnit)("PV_MagLog", sizeof(PV_Unit), (UnitCtorFunc)&PV_MagLog_Ctor, 0, 0);
	(*ft->fDefineUnit)("PV_MagExp", sizeof(PV_Unit), (UnitCtorFunc)&PV_MagExp_Ctor, 0, 0);
	DefineDtorUnit(FFTSubbandPower);

	DefineDtorUnit(FFTPhaseDev);
	DefineDtorUnit(FFTComplexDev);
	DefineDtorUnit(FFTMKL);

	DefineSimpleUnit(PV_Whiten);

	DefineSimpleUnit(FFTCrest);
	DefineSimpleUnit(FFTSpread);
	DefineSimpleUnit(FFTSlope);

	DefineDtorUnit(FFTSubbandFlatness);

	DefineSimpleUnit(FFTPeak);

	DefineDtorUnit(PV_MagSmooth);

	(*ft->fDefineUnit)("PV_MagMulAdd", sizeof(PV_Unit), (UnitCtorFunc)&PV_MagMulAdd_Ctor, 0, 0);

	DefineDtorUnit(PV_ExtractRepeat);
}
