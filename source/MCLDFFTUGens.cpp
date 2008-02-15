/*

FFT analysis and phase vocoder UGens for SuperCollider, by Dan Stowell.
(c) Dan Stowell 2006-2008.

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
#include "SCComplex.h"
#include "FFT_UGens.h"

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

struct FFTPercentile_Unit : FFTAnalyser_OutOfPlace 
{
	bool m_interpolate;
};

struct FFTFlux_Unit : FFTAnalyser_OutOfPlace 
{
	float m_yesternorm;
	float m_yesterdc;
	float m_yesternyq;
	bool m_normalise;
};

struct FFTFlatnessSplitPercentile_Unit : FFTPercentile_Unit
{
	float outval2;
};

struct FFTPower : FFTAnalyser_Unit
{
	float m_normfactor;
};

struct FFTSubbandPower : FFTAnalyser_Unit
{
	int m_numbands;
			int *m_cutoffs; // Will hold bin indices corresponding to frequencies
	float *m_outvals;
	bool m_cutoff_inited;
	bool m_inc_dc;
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

struct FFTRumble : FFTAnalyser_Unit
{
	int m_binpos; // The bin index corresponding to the supplied pitch. We'll round DOWN so as to avoid the fundamental contaminating.
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


// for operation on one buffer
// just like PV_GET_BUF except it outputs unit->outval rather than -1 when FFT not triggered
#define FFTAnalyser_GET_BUF \
	float fbufnum = ZIN0(0); \
	if (fbufnum < 0.f) { ZOUT0(0) = unit->outval; return; } \
	ZOUT0(0) = fbufnum; \
	uint32 ibufnum = (uint32)fbufnum; \
	World *world = unit->mWorld; \
	if (ibufnum >= world->mNumSndBufs) ibufnum = 0; \
	SndBuf *buf = world->mSndBufs + ibufnum; \
	int numbins = buf->samples - 2 >> 1;

// Same as above; but with "output2" as well as "output"
#define FFTAnalyser_GET_BUF_TWOOUTS \
	float fbufnum = ZIN0(0); \
	if (fbufnum < 0.f) { ZOUT0(0) = unit->outval; ZOUT0(1) = unit->outval2; return; } \
	ZOUT0(0) = fbufnum; \
	uint32 ibufnum = (uint32)fbufnum; \
	World *world = unit->mWorld; \
	if (ibufnum >= world->mNumSndBufs) ibufnum = 0; \
	SndBuf *buf = world->mSndBufs + ibufnum; \
	int numbins = buf->samples - 2 >> 1;

// As above; but for operation on two input buffers
#define FFTAnalyser_GET_BUF2 \
	float fbufnum1 = ZIN0(0); \
	float fbufnum2 = ZIN0(1); \
	if (fbufnum1 < 0.f || fbufnum2 < 0.f) { ZOUT0(0) = unit->outval; return; } \
	uint32 ibufnum1 = (int)fbufnum1; \
	uint32 ibufnum2 = (int)fbufnum2; \
	World *world = unit->mWorld; \
	if (ibufnum1 >= world->mNumSndBufs) ibufnum1 = 0; \
	if (ibufnum2 >= world->mNumSndBufs) ibufnum2 = 0; \
	SndBuf *buf1 = world->mSndBufs + ibufnum1; \
	SndBuf *buf2 = world->mSndBufs + ibufnum2; \
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

	void FFTFlatness_Ctor(FFTAnalyser_Unit *unit);
	void FFTFlatness_next(FFTAnalyser_Unit *unit, int inNumSamples);

	void FFTPercentile_Ctor(FFTPercentile_Unit *unit);
	void FFTPercentile_next(FFTPercentile_Unit *unit, int inNumSamples);
	void FFTPercentile_Dtor(FFTPercentile_Unit *unit);

	void FFTFlux_Ctor(FFTFlux_Unit *unit);
	void FFTFlux_next(FFTFlux_Unit *unit, int inNumSamples);
	void FFTFlux_Dtor(FFTFlux_Unit *unit);
	void FFTFluxPos_Ctor(FFTFlux_Unit *unit);
	void FFTFluxPos_next(FFTFlux_Unit *unit, int inNumSamples);
	void FFTFluxPos_Dtor(FFTFlux_Unit *unit);

	void FFTFlatnessSplitPercentile_Ctor(FFTFlatnessSplitPercentile_Unit *unit);
	void FFTFlatnessSplitPercentile_next(FFTFlatnessSplitPercentile_Unit *unit, int inNumSamples);
	void FFTFlatnessSplitPercentile_Dtor(FFTFlatnessSplitPercentile_Unit *unit);

	void FFTDiffMags_Ctor(FFTAnalyser_Unit *unit);
	void FFTDiffMags_next(FFTAnalyser_Unit *unit, int inNumSamples);

//	void PV_DiffAndToDC_Ctor(PV_Unit *unit);
//	void PV_DiffAndToDC_next(PV_Unit *unit, int inNumSamples);

	void PV_DiffMags_Ctor(PV_Unit *unit);
	void PV_DiffMags_next(PV_Unit *unit, int inNumSamples);

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
	
	void FFTRumble_Ctor(FFTRumble *unit);
	void FFTRumble_next(FFTRumble *unit, int inNumSamples);
	
	void FFTSubbandFlatness_Ctor(FFTSubbandFlatness *unit);
	void FFTSubbandFlatness_next(FFTSubbandFlatness *unit, int inNumSamples);
	void FFTSubbandFlatness_Dtor(FFTSubbandFlatness *unit);
	
	void FFTCrest_Ctor(FFTCrest *unit);
	void FFTCrest_next(FFTCrest *unit, int inNumSamples);
	
}

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

InterfaceTable *ft;

void init_SCComplex(InterfaceTable *inTable);

//////////////////////////////////////////////////////////////////////////////////////////////////

void FFTPower_next(FFTPower *unit, int inNumSamples)
{
	FFTAnalyser_GET_BUF

	float normfactor = unit->m_normfactor;
	if(normfactor == 0.f){
		unit->m_normfactor = normfactor = 1.f / (numbins + 2.f);
	}


//	SCComplexBuf *p = ToComplexApx(buf);
	SCPolarBuf *p = ToPolarApx(buf);
	
	float total = sc_abs(p->dc) + sc_abs(p->nyq);
	
//	for (int i=0; i<numbins; ++i) {
//		float rabs = (p->bin[i].real);
//		float iabs = (p->bin[i].imag);
//		total += sqrt((rabs*rabs) + (iabs*iabs));
//	}
	for (int i=0; i<numbins; ++i) {
		total += sc_abs(p->bin[i].mag);
	}

	// Store the val for output in future calls
	unit->outval = total * normfactor;

	ZOUT0(0) = unit->outval;
}

void FFTPower_Ctor(FFTPower *unit)
{
	SETCALC(FFTPower_next);
	ZOUT0(0) = unit->outval = 0.;
	
	unit->m_normfactor = 0.f;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

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
	if (ibufnum >= world->mNumSndBufs) ibufnum = 0;
	SndBuf *buf = world->mSndBufs + ibufnum;
	int numbins = buf->samples - 2 >> 1;
	// End: Multi-output equiv of FFTAnalyser_GET_BUF
	
	// Now we create the integer lookup list, if it doesn't already exist
	int * cutoffs = unit->m_cutoffs;
	if(!unit->m_cutoff_inited){
		
		float srate = world->mFullRate.mSampleRate;
		for(int i=0; i < numcutoffs; i++) {
			cutoffs[i] = (int)(buf->samples * ZIN0(3 + i) / srate);
			//Print("Allocated bin cutoff #%d, at bin %d\n", i, cutoffs[i]);
		}
		
		unit->m_cutoff_inited = true;
	}
	
	SCComplexBuf *p = ToComplexApx(buf);

	// Now we can actually calculate the bandwise subtotals
	float total = unit->m_inc_dc ? sc_abs(p->dc) : 0;
	int binaddcount = unit->m_inc_dc ? 1 : 0; // Counts how many bins contributed to the current band (1 because of the DC value)
	int curband = 0;
	float * outvals = unit->m_outvals;
	
	for (int i=0; i<numbins; ++i) {
		if(i == cutoffs[curband]){
			outvals[curband] = total / binaddcount;
			curband++;
			total = 0.f;
			binaddcount = 0;
		}
		
		float rabs = (p->bin[i].real);
		float iabs = (p->bin[i].imag);
		total += sqrt((rabs*rabs) + (iabs*iabs));
		binaddcount++;
	}

	// Remember to output the very last (highest) band
	total += sc_abs(p->nyq);
	outvals[curband] = total / (binaddcount + 1); // Plus one because of the nyq value

	// Now we can output the vals
	for(int i=0; i<numbands; i++) {
		ZOUT0(i) = outvals[i];
	}
}

void FFTSubbandPower_Ctor(FFTSubbandPower *unit)
{
	SETCALC(FFTSubbandPower_next);
	
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

	unit->m_inc_dc = ZIN0(2) > 0.f;
	
	unit->m_numbands = numbands;
	ZOUT0(0) = unit->outval = 0.;
}

void FFTSubbandPower_Dtor(FFTSubbandPower *unit)
{
	RTFree(unit->mWorld, unit->m_cutoffs);
	RTFree(unit->mWorld, unit->m_outvals);
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void FFTFlatness_next(FFTAnalyser_Unit *unit, int inNumSamples)
{
	FFTAnalyser_GET_BUF

	SCComplexBuf *p = ToComplexApx(buf);
	
	// Spectral Flatness Measure is geometric mean divided by arithmetic mean.
	//
	// In order to calculate geom mean without hitting the precision limit, 
	//  we use the trick of converting to log, taking the average, then converting back from log.
	double geommean = log(sc_abs(p->dc)) + log(sc_abs(p->nyq));
	double mean     = sc_abs(p->dc)      + sc_abs(p->nyq);
	
	for (int i=0; i<numbins; ++i) {
		float rabs = (p->bin[i].real);
		float iabs = (p->bin[i].imag);
		float amp = sqrt((rabs*rabs) + (iabs*iabs));
		geommean += log(amp);
		mean += amp;
	}

	double oneovern = 1/(numbins + 2.);
	geommean = exp(geommean * oneovern); // Average and then convert back to linear
	mean *= oneovern;

	// Store the val for output in future calls
	unit->outval = geommean / mean;

	ZOUT0(0) = unit->outval;
}

void FFTFlatness_Ctor(FFTAnalyser_Unit *unit)
{
	SETCALC(FFTFlatness_next);
	ZOUT0(0) = unit->outval = 0.;
}

////////////////////////////////////////////////////////////////////////////////////


void FFTPercentile_next(FFTPercentile_Unit *unit, int inNumSamples)
{
	FFTAnalyser_GET_BUF
	MAKE_TEMP_BUF

	// Percentile value as a fraction. eg: 0.5 == 50-percentile (median).
	float fraction = ZIN0(1);
	bool interpolate = unit->m_interpolate;

	// The magnitudes in *p will be converted to cumulative sum values and stored in *q temporarily
	SCComplexBuf *p = ToComplexApx(buf);
	SCComplexBuf *q = (SCComplexBuf*)unit->m_tempbuf;
	
	float cumul = sc_abs(p->dc);
	
	for (int i=0; i<numbins; ++i) {
		float real = p->bin[i].real;
		float imag = p->bin[i].imag;
		cumul += sqrt(real*real + imag*imag);
		
		// A convenient place to store the mag values...
		q->bin[i].real = cumul;
	}
	
	cumul += sc_abs(p->nyq);
	
	float target = cumul * fraction; // The target cumul value, stored in the "real" slots

	float bestposition = 0; // May be linear-interpolated between bins, but not implemented yet
	           // NB If nothing beats the target (e.g. if fraction is -1) zero Hz is returned
	float nyqfreq = ((float)unit->mWorld->mSampleRate) * 0.5f;
	float binpos;
	for(int i=0; i<numbins; i++) {
		//Print("Testing %g, at position %i", q->bin[i].real, i);
		if(q->bin[i].real >= target){
			if(interpolate && i!=0) {
				binpos = ((float)i) + 1.f - (q->bin[i].real - target) / (q->bin[i].real - q->bin[i-1].real);
			} else {
				binpos = ((float)i) + 1.f;
			}
			bestposition = (nyqfreq * binpos) / (float)(numbins+2);
			//Print("Target %g beaten by %g (at position %i), equating to freq %g\n", 
			//				target, p->bin[i].real, i, bestposition);
			break;
		}
	}
/* THIS COUNTDOWN METHOD DEPRECATED IN FAVOUR OF COUNT-UP, for various reasons.
	for(int i=numbins-1; i>-1; i--) {
		//Print("Testing %g, at position %i", q->bin[i].real, i);
		if(q->bin[i].real <= target){
			bestposition = (nyqfreq * (float)i) / (float)numbins;
			//Print("Target %g beaten by %g (at position %i), equating to freq %g\n", 
			//				target, p->bin[i].real, i, bestposition);
			break;
		}
	}
*/

	// Store the val for output in future calls
	unit->outval = bestposition;

	ZOUT0(0) = unit->outval;
}

void FFTPercentile_Ctor(FFTPercentile_Unit *unit)
{
	SETCALC(FFTPercentile_next);

//	unit->m_subtotals = (float*)RTAlloc(unit->mWorld, N * sizeof(float));
	unit->m_interpolate = ZIN0(2) > 0.f;

	ZOUT0(0) = unit->outval = 0.;
	unit->m_tempbuf = 0;
}

void FFTPercentile_Dtor(FFTPercentile_Unit *unit)
{
	RTFree(unit->mWorld, unit->m_tempbuf);
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


void FFTFlatnessSplitPercentile_next(FFTFlatnessSplitPercentile_Unit *unit, int inNumSamples)
{
	FFTAnalyser_GET_BUF
	MAKE_TEMP_BUF

	// Percentile value as a fraction. eg: 0.5 == 50-percentile (median).
	float fraction = ZIN0(1);

	// The magnitudes in *p will be converted to cumulative sum values and stored in *q temporarily
	SCComplexBuf *p = ToComplexApx(buf);
	SCComplexBuf *q = (SCComplexBuf*)unit->m_tempbuf;
	
	// Spectral Flatness Measure is geometric mean divided by arithmetic mean.
	//
	// In order to calculate geom mean without hitting the precision limit, 
	//  we use the trick of converting to log, taking the average, then converting back from log.
	double geommeanupper = log(sc_abs(p->nyq));
	double meanupper     = sc_abs(p->nyq);
	double geommeanlower = log(sc_abs(p->dc));
	double meanlower     = sc_abs(p->dc);

	float cumul = sc_abs(p->dc);
	for (int i=0; i<numbins; ++i) {
		float real = p->bin[i].real;
		float imag = p->bin[i].imag;
		float amp = sqrt(real*real + imag*imag);
		cumul += amp;
		
		// A convenient place to store the mag values...
		// NOTE: The values stored here are NOT real and imag pairs.
		q->bin[i].real = cumul;
		q->bin[i].imag = amp;
	}
	cumul += sc_abs(p->nyq);
	
	float target = cumul * fraction; // The target cumul value, stored in the "real" slots

	int numupper = -1;
	int numlower = -1;
	for(int i=numbins-1; i>-1; i--) {
	
		float amp = q->bin[i].imag; // This is where I stored the amp earlier.
		
		if(numupper == -1) {
			//Print("Testing %g, at position %i", q->bin[i].real, i);
			if(q->bin[i].real <= target){ // We are transitioning from upper to lower region
				//bestposition = (nyqfreq * (float)i) / (float)numbins;
				//Print("Target %g beaten by %g (at position %i), equating to freq %g\n", 
				//				target, p->bin[i].real, i, bestposition);
				geommeanlower += log(amp);
				meanlower += amp;
				numupper = numbins - i; // inc nyq, therefore skip the "minus one"
				numlower = i + 2; // inc DC, therefore "plus two" rather than "plus one"
			} else { // We're still in the upper portion of the spectrum
				geommeanupper += log(amp);
				meanupper += amp;
			}
		} else { // We're in the lower portion of the spectrum
			geommeanlower += log(amp);
			meanlower += amp;
		}
	} // End of iteration backwards over the bins
	
	if(numupper == -1) { // Should be very unlikely, but may happen (e.g. if fraction==-1)
		numupper = numbins + 1; // All, plus nyquist
		numlower = 1; // Just the DC
	}
	
	geommeanupper = exp(geommeanupper / numupper); // Average and then convert back to linear
	meanupper /= numupper;
	geommeanlower = exp(geommeanlower / numlower); // Average and then convert back to linear
	meanlower /= numlower;

	// Store the val for output in future calls
	unit->outval  = geommeanlower / meanlower;
	unit->outval2 = geommeanupper / meanupper;

	ZOUT0(0) = unit->outval;
	ZOUT0(1) = unit->outval2;
}

void FFTFlatnessSplitPercentile_Ctor(FFTFlatnessSplitPercentile_Unit *unit)
{
	SETCALC(FFTFlatnessSplitPercentile_next);

//	unit->m_subtotals = (float*)RTAlloc(unit->mWorld, N * sizeof(float));

	ZOUT0(0) = unit->outval = 0.;
	ZOUT0(1) = unit->outval2 = 0.;
	unit->m_tempbuf = 0;
}

void FFTFlatnessSplitPercentile_Dtor(FFTFlatnessSplitPercentile_Unit *unit)
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


void PV_DiffMags_next(PV_Unit *unit, int inNumSamples)
{
	PV_GET_BUF2
	
	SCPolarBuf *p = ToPolarApx(buf1);
	SCPolarBuf *q = ToPolarApx(buf2);
	
	// First the DC and nyquist
	p->dc  = p->dc - q->dc;
	p->nyq = p->nyq - q->nyq;

	for (int i=0; i<numbins; ++i) {
		p->bin[i].mag -= q->bin[i].mag;
	}
}

void PV_DiffMags_Ctor(PV_Unit *unit)
{
	SETCALC(PV_DiffMags_next);
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
			deviation = p->bin[i].phase - storedvals[tbpointer++] - storedvals[tbpointer++];
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
	if (ibufnum1 >= world->mNumSndBufs) ibufnum1 = 0;
	if (ibufnum2 >= world->mNumSndBufs) ibufnum2 = 0;
	SndBuf *buf1 = world->mSndBufs + ibufnum1;
	SndBuf *buf2 = world->mSndBufs + ibufnum2;
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

void FFTRumble_next(FFTRumble *unit, int inNumSamples)
{
	FFTAnalyser_GET_BUF

	SCPolarBuf *p = ToPolarApx(buf);
	
	GET_FREQTOBIN
	
	float pitch = ZIN0(1);
	bool sqrmode = ZIN0(2) == 1.f;
	bool normalise = ZIN0(3) > 0.f;
	
	int binpos = unit->m_binpos;
	if(binpos==0){
		binpos = unit->m_binpos = (int)floorf(pitch * freqtobin);
	}
	
	float total = 0.f;
	if(sqrmode){
		for (int i=0; i<binpos; ++i) {
			total += p->bin[i].mag * p->bin[i].mag;
		}
	}else{
		for (int i=0; i<binpos; ++i) {
			total += p->bin[i].mag;
		}
	}
	
	if(normalise){
		float denom = total;
		if(sqrmode){
			for (int i=binpos; i<numbins; ++i) {
				denom += p->bin[i].mag * p->bin[i].mag;
			}
		}else{
			for (int i=binpos; i<numbins; ++i) {
				denom += p->bin[i].mag;
			}
		}
		if(denom!=0.f){
			total /= denom;
		}
	}
	
	ZOUT0(0) = unit->outval = total;
}

void FFTRumble_Ctor(FFTRumble *unit)
{
	SETCALC(FFTRumble_next);
	ZOUT0(0) = unit->outval = 0.;
	
	unit->m_freqtobin = 0.f;
	unit->m_binpos = 0.f;
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
	if (ibufnum >= world->mNumSndBufs) ibufnum = 0;
	SndBuf *buf = world->mSndBufs + ibufnum;
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

//////////////////////////////////////////////////////////////////////////////////////////////////

void load(InterfaceTable *inTable)
{
	ft= inTable;

	init_SCComplex(inTable);

	(*ft->fDefineUnit)("FFTPower", sizeof(FFTPower), (UnitCtorFunc)&FFTPower_Ctor, 0, 0);
	(*ft->fDefineUnit)("FFTFlatness", sizeof(FFTAnalyser_Unit), (UnitCtorFunc)&FFTFlatness_Ctor, 0, 0);
	(*ft->fDefineUnit)("FFTPercentile", sizeof(FFTPercentile_Unit), (UnitCtorFunc)&FFTPercentile_Ctor, (UnitDtorFunc)&FFTPercentile_Dtor, 0);
	(*ft->fDefineUnit)("FFTFlux", sizeof(FFTFlux_Unit), (UnitCtorFunc)&FFTFlux_Ctor, (UnitDtorFunc)&FFTFlux_Dtor, 0);
	(*ft->fDefineUnit)("FFTFluxPos", sizeof(FFTFlux_Unit), (UnitCtorFunc)&FFTFluxPos_Ctor, (UnitDtorFunc)&FFTFluxPos_Dtor, 0);
	(*ft->fDefineUnit)("FFTFlatnessSplitPercentile", sizeof(FFTFlatnessSplitPercentile_Unit), (UnitCtorFunc)&FFTFlatnessSplitPercentile_Ctor, (UnitDtorFunc)&FFTFlatnessSplitPercentile_Dtor, 0);
	(*ft->fDefineUnit)("FFTDiffMags", sizeof(FFTAnalyser_Unit), (UnitCtorFunc)&FFTDiffMags_Ctor, 0, 0);
	(*ft->fDefineUnit)("PV_DiffMags", sizeof(PV_Unit), (UnitCtorFunc)&PV_DiffMags_Ctor, 0, 0);
	(*ft->fDefineUnit)("PV_MagLog", sizeof(PV_Unit), (UnitCtorFunc)&PV_MagLog_Ctor, 0, 0);
	(*ft->fDefineUnit)("PV_MagExp", sizeof(PV_Unit), (UnitCtorFunc)&PV_MagExp_Ctor, 0, 0);
	DefineDtorUnit(FFTSubbandPower);

	DefineDtorUnit(FFTPhaseDev);
	DefineDtorUnit(FFTComplexDev);
	DefineDtorUnit(FFTMKL);

	DefineSimpleUnit(PV_Whiten);
	
	DefineSimpleUnit(FFTRumble);
	DefineSimpleUnit(FFTCrest);
	DefineDtorUnit(FFTSubbandFlatness);
}
