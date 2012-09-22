#include "SC_PlugIn.h"

// realtime Matching Pursuit ugen
// (c) Dan Stowell 2012. All rights reserved.

static InterfaceTable *ft;

struct MatchingP : public Unit
{
	// These member names are fixed by the GET_BUF macro:
	SndBuf* m_buf;
	float m_fbufnum;

	int m_dictsize, m_hopspls, m_shuntspls, m_audiowritepos, m_audioplaybackpos;
	// Arrays allocated in the ctor:
	float *m_audiobuf, *m_activations, **m_activouts;
};

static void MatchingP_next(MatchingP *unit, int inNumSamples);
static void MatchingP_Ctor(MatchingP* unit);
static void MatchingP_Dtor(MatchingP* unit);

//////////////////////////////////////////////////////////////////
// The "generic" 1D matching pursuit code (in principle could be applied to fft mags or time-domain)

static void doMatchingPursuit(float* input, float* activations, const float* dict, const int natoms, const int nsamples, const int niters);
static double innerproductWithDictAtom(const float* input, const float* dict, const int whichatom, const int natoms, const int nsamples);

// NB! The 'input' arg points to an array which WILL BE MODIFIED: on return, it will contain the residual.
// "activations" will contain the activations.
static void doMatchingPursuit(float* input, float* activations, const float* dict, const int natoms, const int nsamples, const int niters)
{
	Clear(natoms, activations); // initialise activations (correlations) to zero
	for(int whichiter=0; whichiter<niters; ++whichiter){
		int chosenatom=-1;
		double chosencorr=0.f, chosenabscorr=0.f;
		for(int whichatom=0; whichatom<natoms; ++whichatom){
			float corr = innerproductWithDictAtom(input, dict, whichatom, natoms, nsamples);
			float abscorr = sc_abs(corr);
			if(abscorr>chosenabscorr){
				chosencorr = corr;
				chosenabscorr = abscorr;
				chosenatom = whichatom;
			}
		}
		if(chosenatom == -1){
			//printf("MP (iter %i): no atoms selected, finished early.\n", whichiter);
		}else{
			//printf("MP (iter %i): selected atom %i (corr %g)\n", whichiter, chosenatom, chosencorr);
			// Now we have our winner for this iter, time to update: subtract the projection from the residual, and add chosencorr to the resultsarray
			//float projectionfac = -1.f / chosencorr;
			float projectionfac = -chosencorr;
			//printf("projectionfac %g\n", projectionfac);
			const float* readpos = dict + chosenatom;
			for(int i=0; i<nsamples; ++i){
				input[i] += projectionfac * (*readpos);
				readpos += natoms;
			}
			activations[chosenatom] += chosencorr;
		}
	}
}

static double innerproductWithDictAtom(const float* input, const float* dict, const int whichatom, const int natoms, const int nsamples){
	// TODO optimise eg by using vDSP_dotpr() on mac, and/or nova-simd calls.
	double sum = 0.f;
	const float* readpos = dict + whichatom;
	for(int i=0; i<nsamples; ++i){
		sum += input[i] * (*readpos);
		readpos += natoms;
	}
	return sum;
}

//////////////////////////////////////////////////////////////////
// macro copied from DelayUGens.cpp
#define CTOR_GET_BUF \
        float fbufnum  = ZIN0(0); \
        fbufnum = sc_max(0.f, fbufnum); \
        uint32 bufnum = (int)fbufnum; \
        World *world = unit->mWorld; \
        SndBuf *buf; \
        if (bufnum >= world->mNumSndBufs) { \
                int localBufNum = bufnum - world->mNumSndBufs; \
                Graph *parent = unit->mParent; \
                if(localBufNum <= parent->localBufNum) { \
                        buf = parent->mLocalSndBufs + localBufNum; \
                } else { \
                        bufnum = 0; \
                        buf = world->mSndBufs + bufnum; \
                } \
        } else { \
                buf = world->mSndBufs + bufnum; \
        }


void MatchingP_Ctor(MatchingP* unit)
{
	SETCALC(MatchingP_next);

	//  [trigger, residual, activ0, activ1,...] = MatchingP.ar(dict, in, dictsize, ntofind, hop=1, method=0)

	CTOR_GET_BUF

	// initialize the unit generator state variables.
	unit->m_dictsize = IN0(2);
	if(unit->m_dictsize != buf->channels){
		printf("ERROR: (unit->m_dictsize != bufChannels)\n");
		SETCALC(ClearUnitOutputs);
		return;
	}
	unit->m_hopspls  = static_cast<int>(sc_max(0.f, sc_min(1.f, IN0(4))) * buf->frames);
	unit->m_shuntspls = buf->frames - unit->m_hopspls;
	// UNUSED: unit->mMethod = IN0(5);

	unit->m_audiowritepos    = unit->m_hopspls;
	unit->m_audioplaybackpos = 0;
	// audiobuf size is bufFrames + hopspls -- playback happens in first bufFrames, input is written in last hopspls, analysis is in last bufFrames
	unit->m_audiobuf    = (float* )RTAlloc(unit->mWorld, sizeof(float)  * (buf->frames + unit->m_hopspls));
	Clear(buf->frames + unit->m_hopspls, unit->m_audiobuf);
	unit->m_activations = (float* )RTAlloc(unit->mWorld, sizeof(float)  * buf->channels);
	unit->m_activouts   = (float**)RTAlloc(unit->mWorld, sizeof(float*) * buf->channels);

	// calculate one sample of output.
	unit->m_fbufnum = -9.9e9; // set it to something that will force the buffer info to be updated when _next runs
	MatchingP_next(unit, 1);
}

void MatchingP_Dtor(MatchingP* unit)
{
	if (unit->m_audiobuf   ) RTFree(unit->mWorld, unit->m_audiobuf   );
	if (unit->m_activations) RTFree(unit->mWorld, unit->m_activations);
	if (unit->m_activouts  ) RTFree(unit->mWorld, unit->m_activouts  );
}

void MatchingP_next(MatchingP *unit, int inNumSamples)
{
	//  [trigger, residual, activ0, activ1,...] = MatchingP.ar(dict, in, dictsize, ntofind, hop=1, method=0)
	float *input = IN(1);
	const int niters = (const int)IN0(3);

	GET_BUF

	float *out_trig = OUT(0);
	float *out_resid = OUT(1);
	int audiowritepos    = unit->m_audiowritepos;
	int audioplaybackpos = unit->m_audioplaybackpos;
	float* audiobuf      = unit->m_audiobuf;
	float* activations   = unit->m_activations;
	int hopspls          = unit->m_hopspls;
	int shuntspls        = unit->m_shuntspls;

	for (int i=0; i < inNumSamples; ++i)
	{
		if (audiowritepos == (bufFrames+hopspls)){
			doMatchingPursuit(audiobuf + hopspls, activations, bufData, bufChannels, bufFrames, niters);
			// At this point, the first "hopspls" of audiobuf contains old stuff we output in the past,
			//  while the range [hopspls .. bufFrames+hopspls] now contains residual

			// shunt the residual down AND ZERO the remainder
			memmove(audiobuf, audiobuf + hopspls, bufFrames * sizeof(float)); // memmove is safe for overlap (unlike memcpy or Copy)
			Clear(hopspls, audiobuf + bufFrames);
			// At this point, the first "bufFrames" of audiobuf contains the residual,
			//  while the range [bufFrames .. bufFrames+hopspls] is empty and will accumulate the input

			audioplaybackpos = 0;
			audiowritepos = bufFrames;
			out_trig[i] = 1.f;
		}else{
			out_trig[i] = 0.f;
		}
		// Now do the outputs and inputs
		out_resid[i] = audiobuf[audioplaybackpos];
		audiobuf[audiowritepos] += input[i];
		for(int o=0; o < bufChannels; ++o){
			OUT(o+2)[i] = activations[o];
		}
		++audioplaybackpos;
		++audiowritepos;
	}
	// store state back to the struct
	unit->m_audiowritepos    = audiowritepos;
	unit->m_audioplaybackpos = audioplaybackpos;
}

//////////////////////////////////////////////////////////////////
PluginLoad(MCLDSparseUGens)
{
	ft = inTable;
	DefineDtorCantAliasUnit(MatchingP);
}

