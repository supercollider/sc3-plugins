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

//Spectral Modeling Synthesis plugin

//no use of chain since must create multiple internal FFTs
//cross synthesis could come later if can abstract out components of analysis into multiple UGens; too dangerous for now, test all in one place
//must accept defaults hard coded, ie 1024 fft size, 256 hop
//SMS.ar(maxpeaks=80, currentpeaks, tolerance=4, noisefloor= 0.2, freqmult=1.0, noiseprop=0.5)

//need to try debugger
//http://lists.create.ucsb.edu/pipermail/sc-dev/2003-October/005103.html
//Shark profiler?

//DEVELOPMENT NOTES:
//SOLVED- AHA! both deterministic synthesis routines simultaneous and sharing phase variable!
//TRIED, not any better: sine detection windowing from Kaiser Bessel/Blackman Harris window as most refined (see Harris windows paper)
//test scale factor on FFT + IFFT via Green's code: SCALE FACTOR is 1 for fft->ifft chain
//any scale factor for sinusoidal resynthesis? factor of N larger at FFT output (N/2 outside dc/nyquist); windowing reduces further (by -6 dB or so? windows necessarily remove power from signals)!
//cos function put through it, or just white noise
//for spectral interpolation would like zero padding!


//initial delay = framesize - hopsize
//newframedelay = hopsize
//amortise delay = hopsize
//total delay to output is framesize+hopsize



#include "NCAnalysis.h"
#include <stdio.h>

//#include "FFT_UGens.h"
//
//#include "SC_PlugIn.h"
//#include "SC_Constants.h"
//
//#include <string.h>
//#include <math.h>

//

//extern "C"
//{
//	//void load(InterfaceTable *inTable);
//
//	//Requires C linking
//	#include "fftlib.h"
//}

//
//InterfaceTable *ft;


//blackman92fft[]= {-2.990080, -0.000001, 36.167683, 0.000001, -125.002235, 0.000002, 183.679993, -0.000001, -125.002228, 0.000000, 36.167679, 0.000000, -2.990080 }
//these are results of cosine(10*w)*window and show real coefficients only (all imag are 0 due to even input)
float g_blackman92fft[7]= {-2.990080, 36.167683, -125.002235, 183.679993, -125.002228, 36.167679, -2.990080 };

float g_blackman92window[1024]; //created in loadSMS()
//BOTH of these assume 1024 window, no zero padding

//will calculate output as A/2 * exp(i phase) * coeff over real and imag bin components centred on frequency (freq1+freq2)*0.5 at blackman[3]





//SndBuf is too full a construct with fields I don't need. Just need an indication of whether rectangular or polar; but then, will know this anyway!
SCPolarBuf* ToPolarApx2(float* buf, int windowsize=1024)
{
	//assumes it already is complex format
	//if (buf->coord == coord_Complex) {
		SCComplexBuf* p = (SCComplexBuf*)buf;
		int numbins = (windowsize - 2) >> 1;
		for (int i=0; i<numbins; ++i) {
			p->bin[i].ToPolarApxInPlace();
		}
		//buf->coord = coord_Polar;
	//}
	return (SCPolarBuf*)buf; //->data;
}

SCComplexBuf* ToComplexApx2(float *buf, int windowsize=1024)
{
	//if (buf->coord == coord_Polar) {
		SCPolarBuf* p = (SCPolarBuf*)buf; //->data;
		int numbins = (windowsize - 2) >> 1;
		for (int i=0; i<numbins; ++i) {
			p->bin[i].ToComplexApxInPlace();
		}
		//buf->coord = coord_Complex;
	//}
	return (SCComplexBuf*)buf;
}

//
//void init_SCComplex(InterfaceTable *inTable);

//
////too much hassle, not compatible with runtime requirements (problem with new and delete, would want to make a global object and allocate on load)
////wrapper for platform specific fft implementation details, doesn't use existing buffer, internal UGen use only?
////class FFTAssist {
//
//FFTAssist(int n);
//~FFTAssist();
//
//fft(float *);
//ifft(float *);
//};


//const int g_maxpeaks = 80;

//not used currently
//const int g_maxsleep=3;

//phases not interpolated, just linear increment of running phase over guide's life
struct Guide {
	float phase1, phase2; //two phase variables required- in general, one for each resynthesis required!
	float amp1, amp2, freq1, freq2;
	//not using for now
	//int status; //alive 0 sleeping 1+ for number of steps asleep

};

//no phase stored now
struct Peak {
	float mag, freq, phase;
};



//overlap add for output requires storage of sample data for last x frames and resynthesis


//no zero padding to start with but option for later?
//will support FFT size of 1024 with hop size of 256


//static float g_GreenCosTable[257]; //cosTable for 1024 point FFT for Green FFT
//static float g_HannTable[1024]; //fixed von Hann window for now
static float g_fade[512]; //fixed size for now

//for sine based resynthesis
const int g_costablesize=1024; //large so truncation error not too bad (see Moore book)
float g_costable[g_costablesize+1]; //extra value for wraparound linear interpolation calculations


struct SMS : Unit {

	//final list of peaks is size at most numpeaks(n) + numpeaks(n+1). reasonably around max of the two.
	//as long as have birth and death resolved for each peak in the two lists, can synthesise up to curent frame. So output latency is one FFT frame

	int m_sr;
	int m_blocksize;

	float * buf; //for passing sines+noise data to a graphics object
	float * m_inputbuffer;
	int m_inputpos;

	//will be using Green transform which is in place and requires auxilliary cos table data
	//make cos table data when plugin loaded
	//will be using a variant of ToComplexApx functions, since don't want to store buf->coord and will know what state each buffer is in
	//////////FFT data
	//int m_fftsize; //no zero padding
	int m_windowsize;		//1024
	int m_hopsize;			//256
	int m_shuntsize;		//768
	int m_overlapfactor;	//4
	int m_nover2;			//512
	int m_nover4;			//256
	int m_log2n;			//10

	//SC FFT variables
	//float* m_transformbuf;
	scfft* m_scfftinput;
	scfft* m_scfftresynth;
	scfft* m_scifft;
	float* m_q;

	scfft* m_scifftresynth1;
	float * resynth1;

	int m_useifft;
	float m_ampmult;

	//ready for transform
	float * m_inplace;

	//store last magnitude spectrum calculated
	float * m_magspectrum;

	//512 samples in each for crossfade in and out via triangular envelopes
	float * m_outputold; //fades out
	float * m_outputnew; //fades in

	//independent buffers for sines and noise signal
	float * m_outputoldnoise; //fades out
	float * m_outputnewnoise; //fades in
	int m_outputpos;

	//1024 samples worth
	float * m_straightresynthesis;
	//512 samples worth
	float * m_deterministicresynthesis;
	int m_straightpos, m_deterministicpos;
	//synthesised noise can be added onto

	//frequency multiplication
	float m_freqmult, m_freqadd;
	Guide * m_tracks2; //adjusted tracks for formant preservation, only calculated as necessary
	int m_formantpreserve; //whether using alternative formant preserving track data or not
	////////


	//or for each partial to be rendered persistent data for rendering need phasek, angfreqk (omegak), alphak, betak as per (37)
	Guide * m_tracks; //space for double maxpeaks if all birth and die at once!
	int m_numtracks;

	int m_maxpeaks;

	//use buffer swapping of pointer as needed
	Peak * m_prevpeaks;
	Peak * m_newpeaks;
	int m_numprevpeaks;
	int m_numnewpeaks;

	//keep track of how many samples resynthesised in current run
	//int m_resynthesisposition;


	//not sure how many steps to take yet
	//float * m_noiseEnvelope;
	//easiest if just have hop of half N, overlapfactor of 2
	//number depends on overlapfactor
	//for overlapfactor4, just synthesise half of N in output
	//so only ever two active at once
	//float ** m_outputframe;
	//int outputframecounter;

};





extern "C"
{
	//required interface functions
	void SMS_next(SMS *unit, int wrongNumSamples);
	void SMS_Ctor(SMS *unit);
	void SMS_Dtor(SMS *unit);
}




//void calculatefeatures(SMS *unit, int ibufnum);

//calculate by summing
//void oscillatorbankresynthesis(SMS *unit, int numsamples);
//void peakmatch(SMS *unit);
//void newframe(SMS* unit, int ibufnum);
void peakmatching(SMS * unit);
void peakdetection(SMS * unit, float * magspectrum, SCPolarBuf *p);
void newinputframe(SMS * unit, float * inputbuffer);
//void synthesisedeterministic(SMS * unit, float * output, int number,int& pos, int total, float mult, int which);
void synthesisedeterministic(SMS * unit, float * output, int number,int& pos, int total, Guide * tracks);
void synthesisestochastic(SMS * unit);
void formantpreserve(SMS * unit, float freqmult);
void ifftsines(SMS * unit, float * output, int number,int& pos, int total, Guide * tracks);
void graphicsbuffer(SMS * unit, Guide * tracks);

void SMS_Ctor(SMS* unit) {

	int j; //i,j;


	//CHECK SAMPLING RATE AND BUFFER SIZE
	unit->m_blocksize = unit->mWorld->mFullRate.mBufLength;

	if(unit->m_blocksize!=64) {
		printf("SMS complains: block size not 64, you have %d\n", unit->m_blocksize);
		SETCALC(*ClearUnitOutputs);
		unit->mDone = true;
		return;
	}

	unit->m_sr = unit->mWorld->mSampleRate;

	if(unit->m_sr!=44100) {
	printf("SMS complains: sample rate not 44100, you have %d\n", unit->m_sr);
	SETCALC(*ClearUnitOutputs);
	unit->mDone = true;
	return;
	}


	unit->m_windowsize=1024;
	unit->m_hopsize=256;
	unit->m_shuntsize=768;
	unit->m_overlapfactor=4;
	unit->m_nover2=512;
	unit->m_nover4=256;
	unit->m_log2n = 10; //LOG2CEIL(unit->m_windowsize);

	//no choices allowed on FFt size for the moment
	//assumption for now is that FFT size and hop rate match the temporal window size and hop rate
	//unit->m_windowsize=(int)(ZIN0(1)); //defaults for now, may have to set as options later
	//unit->m_hopsize=(int)(ZIN0(2));
	//unit->m_nover2=unit->m_windowsize/2;



	//unit->tcache=  (float*)RTAlloc(unit->mWorld, unit->m_hopsize * sizeof(float));

	//printf("another check: windowsize %d hopsize %d \n", unit->m_windowsize, unit->m_hopsize);

	unit->m_inputbuffer = (float*)RTAlloc(unit->mWorld, unit->m_windowsize * sizeof(float));
	unit->m_inputpos = 0;
	unit->m_inplace = (float*)RTAlloc(unit->mWorld, unit->m_windowsize * sizeof(float));

	//ignoring dc and nyquist I assume? No, need them to carry through for differencing
	unit->m_magspectrum =  (float*)RTAlloc(unit->mWorld, (unit->m_nover2+1) * sizeof(float));

	unit->m_outputold = (float*)RTAlloc(unit->mWorld, (unit->m_nover2) * sizeof(float));
	unit->m_outputnew= (float*)RTAlloc(unit->mWorld, (unit->m_nover2) * sizeof(float));
	unit->m_outputpos=0;

	unit->m_outputoldnoise = (float*)RTAlloc(unit->mWorld, (unit->m_nover2) * sizeof(float));
	unit->m_outputnewnoise= (float*)RTAlloc(unit->mWorld, (unit->m_nover2) * sizeof(float));

    for(int i=0; i<unit->m_nover2; ++i) {
        unit->m_outputold[i] = 0.f;
        unit->m_outputnew[i] = 0.f;
        unit->m_outputoldnoise[i] = 0.f;
        unit->m_outputnewnoise[i] = 0.f;
    }
    
    
	//1024 samples worth
	unit->m_straightresynthesis=(float*)RTAlloc(unit->mWorld, unit->m_windowsize * sizeof(float));
	//double hopsize samples worth
	unit->m_deterministicresynthesis=(float*)RTAlloc(unit->mWorld, (unit->m_nover2) * sizeof(float));

	//two forwards and one inverse FFT

	//rffts(resynthesis, unit->m_log2n, 1, g_GreenCosTable);
	//inputbuffer is unit->m_inplace
	//rffts(inputbuffer, unit->m_log2n, 1, g_GreenCosTable);
	//riffts(q, unit->m_log2n, 1, g_GreenCosTable);

	//unit->m_transformbuf = (float*)RTAlloc(unit->mWorld, scfft_trbufsize(unit->m_windowsize));

    SCWorld_Allocator alloc(ft, unit->mWorld);

    unit->m_scfftinput = scfft_create(unit->m_windowsize, unit->m_windowsize, kHannWindow, unit->m_inplace, unit->m_inplace, kForward, alloc);

    unit->m_scfftresynth = scfft_create(unit->m_windowsize, unit->m_windowsize, kHannWindow, unit->m_straightresynthesis, unit->m_straightresynthesis, kForward, alloc);
    
    unit->m_scifft = scfft_create(unit->m_windowsize, unit->m_windowsize, kRectWindow, unit->m_straightresynthesis, unit->m_inplace, kBackward, alloc);
 	    
//	unit->m_scfftinput = (scfft*)RTAlloc(unit->mWorld, sizeof(scfft));
//	unit->m_scfftresynth = (scfft*)RTAlloc(unit->mWorld, sizeof(scfft));
//	unit->m_scifft = (scfft*)RTAlloc(unit->mWorld, sizeof(scfft));
//
//	//scfft, input size, window size, window type, in, out, transfer, forwards flag
//	scfft_create(unit->m_scfftinput, unit->m_windowsize, unit->m_windowsize, WINDOW_HANN, unit->m_inplace, unit->m_inplace, unit->m_transformbuf, true);
//	//should be WINDOW_RECT or WINDOW_HANN? WINDOW_HANN because comparing FFT of resynthesis to original magnitude spectrum, so both must have same windowing, see Fig 1 in CMJ 14(4): p.14
//	//scfft_create(unit->m_scfftresynth, unit->m_windowsize, unit->m_windowsize, WINDOW_RECT, unit->m_straightresynthesis, unit->m_straightresynthesis, unit->m_transformbuf, true);
//	scfft_create(unit->m_scfftresynth, unit->m_windowsize, unit->m_windowsize, WINDOW_HANN, unit->m_straightresynthesis, unit->m_straightresynthesis, unit->m_transformbuf, true);
//	//scfft_create(unit->m_scifft, unit->m_windowsize, unit->m_windowsize, WINDOW_RECT, unit->m_q, unit->m_q, unit->m_transformbuf, false);
//	scfft_create(unit->m_scifft, unit->m_windowsize, unit->m_windowsize, WINDOW_RECT, unit->m_straightresynthesis, unit->m_inplace, unit->m_transformbuf, false);

    unit->resynth1= (float*)RTAlloc(unit->mWorld, (unit->m_windowsize) * sizeof(float));
    unit->m_scifftresynth1= scfft_create(unit->m_windowsize, unit->m_windowsize, kRectWindow, unit->resynth1, unit->resynth1, kBackward, alloc);

    
//	scfft_create(unit->m_scifftresynth1, unit->m_windowsize, unit->m_windowsize, WINDOW_RECT, unit->resynth1, unit->resynth1, unit->m_transformbuf, false);

	float * ifftsum= unit->resynth1;
	for (j=0; j<unit->m_windowsize; ++j)
		ifftsum[j]=0.0;

	unit->m_useifft = (int)ZIN0(8);

	unit->m_straightpos=0;
	unit->m_deterministicpos=0;

	float * resynth= unit->m_straightresynthesis;

	//zero these buffers
	for (j=0; j<unit->m_windowsize; ++j)
	resynth[j]=0.0;

	resynth= unit->m_deterministicresynthesis;

	//2 times hopsize
	for (j=0; j<unit->m_nover2; ++j)
	resynth[j]=0.0;

	//for (j=0; j<numSamples; ++j) {
//		inputbuffer[pos++] = 0.0; //in[j]; //post increment returns previous?
//	}


	unit->m_ampmult= 2.0*ZIN0(9)/((float)unit->m_windowsize); //compensatio for half components (should be except at dc/nyquist)

	unit->m_maxpeaks=(int)ZIN0(1);
	//int m_maxlistsize; //double m_maxpeaks

    //no LocalBuf support for graphics buffer since communicating back to language
    
	float fbufnum = ZIN0(10);
	//if (bufnum >= world->mNumSndBufs) bufnum = 0;
	World * world = unit->mWorld;

	//printf("fbufnum %f int %d \n",fbufnum, (int)fbufnum);
    
	if(fbufnum>=0.f){

		int bufnum= (int)fbufnum;

	if(0<=bufnum<= world->mNumSndBufs) {
		SndBuf * buf;

		buf = world->mSndBufs + bufnum;
		//unit->mBufNum=bufnum;
		//printf("%d \n",bufnum);
		//unit->mBufSize = buf->samples;
		unit->buf= buf->data; //(float*)RTAlloc(unit->mWorld, unit->mBufSize * sizeof(float));
							  //
							  //		} else {
							  //			if(world->mVerbosity > -1){ Print("SLUGens buffer number error: invalid buffer number: %i.\n", bufnum); }
							  //			SETCALC(*ClearUnitOutputs);
							  //			unit->mDone = true;
							  //			return NULL;
							  //		}

		//must be enough room for current peak count integer, up to max number of sine peaks*2, and a mag spectrum for the noise
		if(buf->samples<(unit->m_maxpeaks*10 + unit->m_nover2+1+1)) {
			Print("buffer not large enough %i.\n", buf->samples);
		SETCALC(*ClearUnitOutputs);
		unit->mDone = true;
		//return NULL;
		}

buf->data[0]= 0; //start with no trails to paint

	} else {
		unit->buf=NULL;
	}

} else
{
		unit->buf=NULL;
}






	unit->m_tracks= (Guide*)RTAlloc(unit->mWorld, 2*unit->m_maxpeaks * sizeof(Guide));

	//use buffer swapping of pointer as needed
	unit->m_prevpeaks = (Peak*)RTAlloc(unit->mWorld, unit->m_maxpeaks * sizeof(Peak));
	unit->m_newpeaks=(Peak*)RTAlloc(unit->mWorld, unit->m_maxpeaks * sizeof(Peak));

	//no need to initialise these arrays since filled as needed

	unit->m_numprevpeaks =0;
	unit->m_numnewpeaks =0;
	unit->m_numtracks=0;
	//unit->m_resynthesisposition=0;

	//formant preservation test
	unit->m_freqmult=1.0;
	unit->m_freqadd=0.0;
	unit->m_formantpreserve=0;
	unit->m_tracks2= (Guide*)RTAlloc(unit->mWorld, 2*unit->m_maxpeaks * sizeof(Guide));


	//SETCALC(*ClearUnitOutputs);
	unit->mCalcFunc = (UnitCalcFunc)&SMS_next;

	//printf("Made it to here at least! %d\n", unit->m_sr);

}



void SMS_Dtor(SMS *unit)
{

    
    RTFree(unit->mWorld, unit->m_tracks2);
    
	RTFree(unit->mWorld, unit->m_tracks);
	RTFree(unit->mWorld, unit->m_prevpeaks);
	RTFree(unit->mWorld, unit->m_newpeaks);
    
	//RTFree(unit->mWorld, unit->tcache);
	RTFree(unit->mWorld, unit->m_inputbuffer);
	RTFree(unit->mWorld, unit->m_inplace);
	RTFree(unit->mWorld, unit->m_magspectrum);
    
	RTFree(unit->mWorld, unit->m_outputold);
	RTFree(unit->mWorld, unit->m_outputnew);
	RTFree(unit->mWorld, unit->m_outputoldnoise);
	RTFree(unit->mWorld, unit->m_outputnewnoise);
    
	RTFree(unit->mWorld, unit->m_straightresynthesis);
	RTFree(unit->mWorld, unit->m_deterministicresynthesis);
    
	//RTFree(unit->mWorld, unit->m_transformbuf);
    
	RTFree(unit->mWorld, unit->resynth1);

    SCWorld_Allocator alloc(ft, unit->mWorld);
    
    if(unit->m_scfftinput) {
     
        scfft_destroy(unit->m_scfftinput, alloc);
        scfft_destroy(unit->m_scfftresynth, alloc);
        scfft_destroy(unit->m_scifft, alloc);
 
	}
    
    if(unit->m_scifftresynth1){
        scfft_destroy(unit->m_scifftresynth1, alloc);
        //scfft_destroy(unit->m_scifftresynth1);
        //RTFree(unit->mWorld, unit->m_scifftresynth1);
	
	}

//	if(unit->m_scfftinput){
//		scfft_destroy(unit->m_scfftinput);
//		RTFree(unit->mWorld, unit->m_scfftinput);
//	}
//
//	if(unit->m_scfftresynth){
//		scfft_destroy(unit->m_scfftresynth);
//		RTFree(unit->mWorld, unit->m_scfftresynth);
//	}
//
//	if(unit->m_scifft){
//		scfft_destroy(unit->m_scifft);
//		RTFree(unit->mWorld, unit->m_scifft);
//	}

	//



}



//HARD CODE 4*overlap? OR use 2*overlap?

//each next:
//collect block of samples for new fft
//calculate block of samples for straight resynthesis (need 4*hopsize worth for 4*overlap)
//calculate block of samples for deterministic transformation (only need 2*hopsize worth)

//on fft
//FFT(input sample frame)
//peak detection
//FFT of previous straight resynthesis
//magnitude difference, noise modelling, noise transformation, IFFT (size M of window can be less than N)
//now ready to begin outputting previous frame as transformed frame (apply window beforehand so just need to add blocks of samples)

//sum up overlap signals



//int numSamples = unit->mWorld->mFullRate.mBufLength;
void SMS_next(SMS *unit, int numSamples)
{
	int i,j;

	float* in = IN(0);
	float* out = OUT(0);
	float* out2 = OUT(1);

	float * inputbuffer= unit->m_inputbuffer;
	int pos = unit->m_inputpos;

	//fill up input buffer with next numSamples
	for (j=0; j<numSamples; ++j) {
		inputbuffer[pos++] = in[j]; //post increment returns previous?
	}


	//ASSUMPTIONS RIFE HERE FOR WHAT BLOCKSIZE IS
	//printf("before str %d det %d \n",unit->m_straightpos, unit->m_deterministicpos);


	if(unit->m_useifft) {
		//experimental inverse FFT resynthesis
		ifftsines(unit, unit->m_straightresynthesis, numSamples*unit->m_overlapfactor, unit->m_straightpos, unit->m_windowsize, unit->m_tracks);
		ifftsines(unit, unit->m_deterministicresynthesis, numSamples*2, unit->m_deterministicpos, unit->m_hopsize*2, unit->m_tracks2);

	} else
	{
		//calculate blocksize*overlapfactor into straight resynthesis buffer
		//would be more efficient if separate function
		//NOT THIS ONE synthesisedeterministic(unit, unit->m_straightresynthesis, numSamples*unit->m_overlapfactor, unit->m_straightpos, unit->m_windowsize, 1.0,0);

		synthesisedeterministic(unit, unit->m_straightresynthesis, numSamples*unit->m_overlapfactor, unit->m_straightpos, unit->m_windowsize, unit->m_tracks);

		//POTENTIAL ERROR HERE SINCE freqmult can change during resynthesis of set of tracks
		//precalculates twice as much as needed so ready to do crossfade for next time without preserving multiple guide lists?
		//assumes overlap is 4 or 2????
		//calculate blocksize*2 into deterministic transformation buffer
		//which flag set to 1 here to differentiate which phase variable to use in Guides
		//ZIN0(5)
		//no need to separate as flag 0 or 1 anymore since independent guides list

		//CURRENT
		synthesisedeterministic(unit, unit->m_deterministicresynthesis, numSamples*2, unit->m_deterministicpos, unit->m_hopsize*2,  unit->m_tracks2);

		//OLD
		//synthesisedeterministic(unit, unit->m_deterministicresynthesis, numSamples*2, unit->m_deterministicpos, unit->m_hopsize*2, unit->m_freqmult, 1);
	}


	//printf("after str %d det %d \n",unit->m_straightpos, unit->m_deterministicpos);

	//resynthpos

	//resynthesisposition += numSamples;
	//unit->m_resynthesisposition=resynthesisposition;

	if (pos>=unit->m_windowsize) {

		//prepare new output buffers
		//buffer swap
		float * temp= unit->m_outputold; //has finished fade out
		unit->m_outputold = unit->m_outputnew; //will now start to fade out
		unit->m_outputnew = temp;	//will now be overwritten with new data

		//for noise output too!
		temp= unit->m_outputoldnoise; //has finished fade out
		unit->m_outputoldnoise = unit->m_outputnewnoise; //will now start to fade out
		unit->m_outputnewnoise = temp;	//will now be overwritten with new data

		float * outputnew= unit-> m_outputnew;
		float * deterministic= unit->m_deterministicresynthesis;
		int nover2 = unit->m_nover2;

		for (i=0; i<nover2; ++i) {
		outputnew[i]= deterministic[i];
		}

		unit->m_outputpos=0;

		//printf("now synthesise stochastic\n");

		//deal with last frame update first
		synthesisestochastic(unit);
		//APPLY WINDOWING HERE TO AVOID DOING ELSEWHERE

		//printf("post stochastic\n");

		//update inputbuffer (shunt procedure)
		float * inplace= unit->m_inplace;
		memcpy(inplace, inputbuffer, unit->m_windowsize * sizeof(float));
		//shunt operation should work, after testing
		memcpy(inputbuffer, inputbuffer+(unit->m_hopsize), unit->m_shuntsize * sizeof(float));
		pos= unit->m_shuntsize; //ready to continue next time


		//printf("shunt done, now new input frame\n");

		unit->m_ampmult= 2.0*ZIN0(9)/(float)unit->m_windowsize;
		//must update before starting new cycle since determines amp coefficients in peak detection
		unit->m_useifft = (int)ZIN0(8);
		//fft, phase vocode, peak pick and peak match
		newinputframe(unit, inplace);

		//printf("after input frame\n");

		//ready for new resynthesis round
		//unit->m_resynthesisposition=0;
		unit->m_straightpos=0;
		unit->m_deterministicpos=0;

		//get new freq mult at this point?
		//create guide copies with adjusted amplitudes
		unit->m_freqmult= ZIN0(5);
		unit->m_freqadd= ZIN0(6);
		unit->m_formantpreserve= ZIN0(7);
		//adjust guide amplitudes based on matching frequencies to original magspectrum
		//if(unit->m_formantpreserve)


		//IF always running, probably don't need two phase variables any more?
		//always run this, for freqmult
		formantpreserve(unit, unit->m_freqmult);

		//HERE! create formantpreserve function
		//then potentially rejig synthesisedeterminstic into two versions, avoiding ifs and freqmult step


		float * resynth= unit->m_straightresynthesis;

		//zero these buffers
		for (j=0; j<unit->m_windowsize; ++j)
		resynth[j]=0.0;

		resynth= unit->m_deterministicresynthesis;

		//2 times hopsize
		for (j=0; j<unit->m_nover2; ++j)
		resynth[j]=0.0;


	}

	unit->m_inputpos=pos; //store for next time

	int outputpos= unit->m_outputpos;

	float * fadeout= unit->m_outputold+unit->m_hopsize;
	float * fadein= unit->m_outputnew;

	float * fadeout2= unit->m_outputoldnoise+unit->m_hopsize;
	float * fadein2= unit->m_outputnewnoise;

	//float rhop= 1.0f/unit->m_hopsize;
	float temp;
	//crossfade of previous frame data and newer frame
	for (j=0; j<numSamples; ++j) {

		//float prop= outputpos*rhop;
		//crossfade already precalculated
		temp = fadeout[outputpos]+fadein[outputpos]; //*(1.0-prop) + fadein[outputpos]*prop;
		out[j]=temp;
		temp = fadeout2[outputpos]+fadein2[outputpos];
		out2[j]=temp;
		++outputpos;
	}

    //safety (especially for first time round when waiting to collect full input window) 
    if(outputpos>=unit->m_nover4) 
        outputpos = 0; 
    
	unit->m_outputpos = outputpos;

}



//can get magspectrum of noise and sine peak data
void graphicsbuffer(SMS * unit, Guide * tracks, SCPolarBuf * p) {

	int i;

	float * buf = unit->buf;

	int numtracks = unit->m_numtracks;

	int pos= 0; //numtracks;

	buf[0]= (float)numtracks;

	//printf("numtracks %d ",numtracks);

	for (i=0; i<numtracks; ++i) {

		pos= i*5+1; //numtracks;

		Guide *  pointer= &tracks[i];

		buf[pos]= pointer->freq1;
		buf[pos+1]= pointer->freq2;
		buf[pos+2]= pointer->amp1;
		buf[pos+3]= pointer->amp2;
		buf[pos+4]= pointer->phase1;
	}

	pos= numtracks*5+1;

	int nover2= unit->m_nover2;

	for (i=0; i<(nover2-1); ++i) {

		//will be copy or actual one?
		SCPolar& polar = p->bin[i];

		buf[pos+i] = polar.mag;
	}

}




//float phase1, phase2; //two phase variables required- in general, one for each resynthesis required!
//float amp1, amp2, freq1, freq2;
void formantpreserve(SMS * unit, float freqmult) {

	//copy between
	//m_tracks2
	//m_tracks

	Guide * tracks = unit->m_tracks;
	Guide * tracks2 = unit->m_tracks2;
	int numtracks = unit->m_numtracks;

	float freqshift = 2*pi* unit->m_freqadd/unit->m_sr;  //2pi* f/R

	int i;

	Guide * pointer;
	Guide * pointer2;

	if(unit->m_formantpreserve) {

		float * magspectrum= unit->m_magspectrum;
		float angmultr= unit->m_nover2/pi;
		float ampmult= unit->m_ampmult; //(1.0/unit->m_windowsize); //NOT NEEDED: remember blackman window function already has scale factor of N implcit unit->m_useifft? 1.0 : (1.0/unit->m_windowsize);

		int test= (unit->m_nover2-1); //top bin value

		int low; //, high;
		float freqbin; //, frac;

		for (i=0; i<numtracks; ++i) {

			pointer= &tracks[i];
			pointer2= &tracks2[i];

			pointer2->phase1= pointer->phase1;
			//pointer2->phase2= pointer->phase2;
			pointer2->freq1= pointer->freq1 * freqmult + freqshift;
			pointer2->freq2= pointer->freq2 * freqmult + freqshift;

			//bin number to freq (i-1)*angmult float angmult= pi/nover2;

			freqbin= pointer2->freq1 * angmultr;

			//don't do linear interpolation, rough calc only
			low= (int)freqbin;

			if(low>=test) low= low%test;
			if(low<0) low= (-low)%test;

//			frac= freqbin-low;
//			high= low+1;

			//must check for birth and deaths and preserve those
			pointer2->amp1=(pointer->amp1<0.000001)? (pointer->amp1): (magspectrum[low]*ampmult);

			freqbin= pointer2->freq2 * angmultr;
			low= (int)freqbin;

			if(low>=test) low= low%test;
			if(low<0) low= (-low)%test;

			pointer2->amp2=(pointer->amp2<0.000001)? (pointer->amp2): (magspectrum[low]*ampmult);

		}

	}
	else
	{

		for (i=0; i<numtracks; ++i) {

			pointer= &tracks[i];
			pointer2= &tracks2[i];

			pointer2->phase1= pointer->phase1;
			//pointer2->phase2= pointer->phase2;
			pointer2->amp1= pointer->amp1;
			pointer2->amp2= pointer->amp2;

			//printf("amp1 %f amp2 %f \n",pointer->amp1, pointer->amp2);


			pointer2->freq1= pointer->freq1 * freqmult + freqshift;
			pointer2->freq2= pointer->freq2 * freqmult + freqshift;

		}

	}


}


//, int number,int& pos, int total,
void ifftsines(SMS * unit, float * output, int number,int& pos, int total, Guide * tracks) {

	if(pos<total) {

	pos=total;

	int i,j;

	float * ifftsum= unit->resynth1;
	for (j=0; j<unit->m_windowsize; ++j)
		ifftsum[j]=0.0;

	int numtracks = unit->m_numtracks;

	Guide * pointer;

	//printf("numtracks %d \n", numtracks);

	float phase;

	float angmultr= unit->m_nover2/pi;

	SCComplex what(1.0,0.0);

	int highestbinallowed= unit->m_nover2-4;

	for (i=0; i<numtracks; ++i) {

		pointer = &(tracks[i]);

	//	float amp1= pointer->amp1;
		//float amp2=pointer->amp2;
		//((i*40.0)/22050.0)*pi;
		//float freq1= pointer->freq1;
		//float freq2= pointer->freq2;

		//take averages
		float amp= ((pointer->amp1 + pointer->amp2)*0.5); //*0.5; //additional half recommended by Laroche
		float freq= (pointer->freq1 + pointer->freq2)*0.5;
		int freqbin= ((angmultr*freq)+0.5);

		//need sc_fold if outside of 0 to Nyquist bins?
		phase= pointer->phase1;
		what= SCPolar(1.0,phase).ToComplexApx();

		//safety; don't synthesise if off the scale
		if((freqbin>=4) && (freqbin<highestbinallowed)) {
		//ignore phase for now, later would work in via e(i*phase) term to spread over real and imag parts
		//if (freqbin>=4) {

			for (j=0; j<7; ++j)
			{
				int now = 2*(freqbin-3+j);
				float val = amp*g_blackman92fft[j];
				//ifftsum[now] += val;

				ifftsum[now] += what.real*(val);
				ifftsum[now+1]+= what.imag*(val);
			}

		//}
		//must take account of negative frequencies entering positive region- times by i
//		else {
//				//need clauses to cope with neg frequencies etc; just skip it for now
//			for (j=0; j<7; ++j)
//			{
//				int now = 2*(freqbin-3+j);
//				float val = amp*g_blackman92fft[j];
//				//ifftsum[now] += val;
//
//				ifftsum[now] += what.real*(val);
//				ifftsum[now+1]+= what.imag*(val);
//			}
//
//		}
		}
		//if(which)
		//else
		//phase= pointer->phase2;
	}

	//do IFFT
	scfft_doifft(unit->m_scifftresynth1);

	//divide out window as well   /g_blackman92window
	//total = unit->m_windowsize or half window size in one case
	for (j=0; j<total; ++j)
		output[j]= 	ifftsum[j]*g_blackman92window[j];

	}

}


//void synthesisedeterministic(SMS * unit, float * output, int number,int& pos, int total, float mult, int which=0)
void synthesisedeterministic(SMS * unit, float * output, int number,int& pos, int total, Guide * tracks) {

	int i,j;

	//Guide * tracks = unit->m_tracks;
	int numtracks = unit->m_numtracks;

	//int resynthesisposition = unit->m_resynthesisposition;

	//float T = unit->m_hopsize;
	//float rT= 1.0/T;

	Guide * pointer;

	//printf("numtracks %d \n", numtracks);

	//test avoids pile-up, particulaly at start
	//printf("problem pos %d \n",pos);
	if(pos<total) {

	int top= pos+number;
	float rtotal= 1.0/total;
	float constantsize= rtwopi*g_costablesize;

	float phase;

	for (i=0; i<numtracks; ++i) {

		pointer = &(tracks[i]);

		float amp1=pointer->amp1;
		float amp2=pointer->amp2;
		//((i*40.0)/22050.0)*pi;
		float freq1= pointer->freq1;
		float freq2= pointer->freq2;

		//if(which)
		phase= pointer->phase1;
		//else
		//phase= pointer->phase2;

		for (j=pos; j<top; ++j) {

			float tpos= (float)j*rtotal;

			//linear interpolation of amplitude
			float amp= amp1 + (tpos*(amp2- amp1));
			//amp= amp*0.00390625; //compensation for approx 256.0 increase in size of magnitude values for Hanning window observing a sine peak on bin?

			//printf("amp %f temp3 %f amp2 %f number %f \n",amp,temp3, tracks[i].amp2, ((float)t/T));

			float freq= freq1 + (tpos*(freq2- freq1));

			//SPEED UP could remove this line if had precalculated frequency multiplication effects, ie separate guides array with freq1 and freq2 already adjusted
			//update by instantaneous phase
			phase +=  freq; //WAS freq*mult; //*rT; //per sample? with frequency multiplier in here! rT

			float phasetemp= phase*constantsize; //rtwopi*g_costablesize;

			//if(i==10)
			//printf("freq check f1 %f f2 %f instantaneous freq %f phase %f \n",freq1, freq2, freq, phase);

			//linear interpolation into costable
			//could use fmod if add very big multiple of pi so modulo works properly; ie, no negative phases allowed BUT fmod is really inefficient!
			//float wrapval= sc_wrap(phasetemp,0.0f,1024.0f); //modulo or fold won't work correctly- i.e., we need -1 = 1023

			//phase now always positive so take modulo 1024.0?
			//int prev= ((int)phasetemp)%1024;	//cheaper on CPU?

			//int prev= (int)wrapval; //truncation alone is sufficient for larger wavetable
			//float interp= cos(freq1*j); //TEST
			//float interp= g_costable[((int)phasetemp)%1024];
			//can use efficient trick for any power of two when doing modulo
			float interp= g_costable[((int)phasetemp) & 0x03FF];




			//float prop=  wrapval-prev; //linear interpolation parameter
			//float interp= ((1.0-prop)*(g_costable[prev])) + (prop*(g_costable[prev+1]));

			//if(number==128)
			//printf("pos %d amp %f phase %f interp %f \n",j,amp, phase, interp);

			output[j] += amp*interp; //g_costable[((int)(phasetemp))%g_costablesize];

			//if(i==10)
			//printf("output j %d value %f amp %f interp %f \n",j, output[j], amp, interp);

		}


		//if(which)
		pointer->phase1= phase;
		//else
		//pointer->phase2= phase;

	}

	//for(j=0;j<128;++j)
	//	printf("test %f \n",output[j]);


	pos= pos+number; //reference so should update
	//resynthesisposition += numSamples;
	//unit->m_resynthesisposition=resynthesisposition;
	}


}


void newinputframe(SMS * unit, float * inputbuffer) {

int i;

//windowing
//int n= unit->m_windowsize;

//for (i=0; i<n; ++i)
//	inputbuffer[i] *= g_HannTable[i];
//
//
////FFT in place via Green FFT , 10 is log2(1024)
////real forward fft carried out in place
//rffts(inputbuffer, unit->m_log2n, 1, g_GreenCosTable);

//will act on inputbuffer since same as m_inplace
scfft_dofft(unit->m_scfftinput);


//phase vocoder

//input buffer now in standard format but need magnitudes and phases viewpoint- actually, don't need phases, but efficient McCartney procedure to get magnitude goes via angle, which pretty much solves phase anyway!
SCPolarBuf *p = ToPolarApx2(inputbuffer); //assumes inputbuffer in SCComplex format, which it is!

//save magnitude spectrum
//copy magnitude spectrum for differencing calculation

//this will overwrite previous; must have done differencing from previous frame by now?
float * magspectrum= unit->m_magspectrum;

int nover2 = unit->m_nover2;

for (i=0; i<(nover2-1); ++i) {
magspectrum[i]= p->bin[i].mag;
}

//store dc and nyquist for later
magspectrum[nover2-1] = p->dc;
magspectrum[nover2] = p->nyq;

//peak detection for new fft frame
peakdetection(unit, magspectrum, p);

//peak matching
peakmatching(unit);

}



//peak detection searching through magspectrum, could limit up to 10000 Hz
void peakdetection(SMS * unit, float * magspectrum, SCPolarBuf *p) {

	int i; //,j;

int nover2 = unit->m_nover2;

Peak * prevpeaks= unit->m_prevpeaks;
Peak * newpeaks= unit->m_newpeaks;
int numprevpeaks= unit->m_numprevpeaks;
int numnewpeaks= unit->m_numnewpeaks;

//printf("prev pointer %p new pointer %p \n",prevpeaks, newpeaks);

//ditch old
numprevpeaks= numnewpeaks;
numnewpeaks=0;

//swap pointers ready to write new peaks
Peak * temp= prevpeaks;
prevpeaks=newpeaks;
newpeaks=temp;

//printf("prev pointer %p new pointer %p temp %p \n",prevpeaks, newpeaks, temp);

float prevmag, mag, nextmag; //phase,

//bin 1 can't be pick since no interpolation possible! dc should be ignored
//test each if peak candidate; if so, add to list and add to peaks total

prevmag= magspectrum[0]; //this is at analysis frequency, not dc
mag= magspectrum[1];

int numpeaksrequested= (int)ZIN0(2); //(int)(ZIN0(4)+0.0001);
int maxpeaks= unit->m_maxpeaks;

maxpeaks = sc_min(maxpeaks,numpeaksrequested);

//angular frequency is pi*(i/nover2)

float angmult= pi/nover2; //MIGHT NEED TO VARY BASED ON RESYNTHESIS SPEED? NAH
//if IFFT resynthesis, get 1/N from the IFFT call; otherwise, time domain resynthesis requires scalefactor to be built in
float ampmult= unit->m_ampmult; //(1.0/unit->m_windowsize); //unit->m_useifft? 1.0 : (1.0/unit->m_windowsize); //*(1.0/unit->m_maxpeaks);

	//defined here since needed in backdating phase for track births (and potentially for track deaths too)
//T = number of samples per interpolaion frame, so equals hopsize
//float T = unit->m_hopsize;

float ampcheck= ZIN0(4); //0.001

    float temp1, position, refinement; 
    
//could restrict not to go above nover4!
for (i=2; i<(nover2-1); ++i) {

	//phase= p->bin[i].phase;
	nextmag= magspectrum[i];

	if ((prevmag<mag) && (nextmag<mag) && (mag>ampcheck) && (numnewpeaks<maxpeaks)) {
		//found a peak

		//should I be preserving phase?

        //quadratic interpolation formula 
        
        //temp1 should be greater than zero since mag is a local peak
        temp1= prevmag+nextmag-(2*mag);
        
        if(temp1>0.00001) {
            position=(prevmag-nextmag)/(2*temp1);
            refinement = (0.5*temp1*(position*position)) + (0.5*(nextmag-prevmag)*position) + mag;
        } {
        
            position= 0.f; 
            refinement = mag; 
            
        
        }
            
		//parabolic interpolation to refine peak location used here
		newpeaks[numnewpeaks].mag = refinement * ampmult; //was mag*ampmult must divide by fftsize before resynthesis!
		newpeaks[numnewpeaks].freq =(i-1+position)*angmult; //was (i-1)*angmult *freqmult; //if should be angular frequency per sample, divide by T
		newpeaks[numnewpeaks].phase =p->bin[i-1].phase;

        //old
//        newpeaks[numnewpeaks].mag = mag * ampmult; //was mag*ampmult must divide by fftsize before resynthesis!
//		newpeaks[numnewpeaks].freq =(i-1)*angmult; //was (i-1)*angmult *freqmult; //if should be angular frequency per sample, divide by T
//		newpeaks[numnewpeaks].phase =p->bin[i-1].phase;
//        
        
		++numnewpeaks;
	}

	prevmag=mag;
	mag=nextmag;

}

unit->m_prevpeaks = prevpeaks;
unit->m_newpeaks = newpeaks;
unit->m_numprevpeaks = numprevpeaks;
unit->m_numnewpeaks = numnewpeaks;

}




//TO TIDY AND MAKE SURE IT WORKS
//non-recursive procedure from TPV for now, no sleeping
void peakmatching(SMS * unit) {

int i,j;

int nover2 = unit->m_nover2;
float angmult= pi/nover2;

Peak * prevpeaks= unit->m_prevpeaks;
Peak * newpeaks= unit->m_newpeaks;
int numprevpeaks= unit->m_numprevpeaks;
int numnewpeaks= unit->m_numnewpeaks;

//now peak matching algorithm
//int leftsort=0;
int rightsort=0;
bool flag= true;
//float rightfreq= newpeaks[0].freq;

Guide * tracks = unit->m_tracks;
int numtracks = 0; //unit->m_numtracks;

//increase tolerance
float tolerance= ZIN0(3)*angmult;

float testfreq;

//	SEEMS OK
//	printf("numprevpeaks %d numnewpeaks %d \n",numprevpeaks, numnewpeaks);
//	//print all and look for junk data
	//for (i=0; i<numnewpeaks; ++i)
	//printf("new i %d amp %f freq %f \n",i,newpeaks[i].mag,newpeaks[i].freq);
//
//	for (i=0; i<numprevpeaks; ++i)
//	printf("prev i %d amp %f freq %f phase %f \n",i,prevpeaks[i].mag,prevpeaks[i].freq,prevpeaks[i].phase);
//
//

//RGen& rgen = *unit->mParent->mRGen;

//ASSUMES BOTH PEAKS LISTS ARE IN ORDER OF INCREASING FREQUENCY

//while right less than left-tolerance then birth on right

//if right within tolerance, find closest; if less than, match, else must check next on left whether better match. If not, match, else, check previous on right. If within tolerance, match, else death on right.

//step through prevpeaks
for (i=0; i<numprevpeaks; ++i) {

	float freqnow= prevpeaks[i].freq;

	flag=true;
	while(flag) {

		if(rightsort>=numnewpeaks) {flag=false;} else {
			testfreq= newpeaks[rightsort].freq;

			if((testfreq+tolerance)<freqnow) {
				//birth on right
				tracks[numtracks].freq1=newpeaks[rightsort].freq;
				tracks[numtracks].freq2=newpeaks[rightsort].freq; //match to itself
				tracks[numtracks].amp1=0.0;
				tracks[numtracks].amp2=newpeaks[rightsort].mag;
				tracks[numtracks].phase1=newpeaks[rightsort].phase;//rgen.frand()*twopi;
				tracks[numtracks].phase2=tracks[numtracks].phase1;
				++numtracks;
				++rightsort;

			} else {

				flag=false;

			}

		}

	}

	flag=false; //whether match process fails
	if(rightsort>=numnewpeaks) {flag=true;} else {
			//printf("testfreq %f freqnow %f tolerance %f \n ", testfreq, freqnow, tolerance);

		//assumption that testfreq already valid;
		if (testfreq>(freqnow+tolerance)) {flag=true;} else {

			//now have a candidate. search for closest amongst remaining; as soon as no closer, break
			//printf("candidate! \n ");

			float bestsofar= fabs(freqnow- testfreq);
			int bestindex= rightsort;

			for (j=(rightsort+1); j<numnewpeaks; ++j) {
				float newcandidate= newpeaks[j].freq;
				float newproximity= fabs(newcandidate-freqnow);

				//must keep getting closer, else no use
				if(newproximity<bestsofar) {bestindex= j; bestsofar= newproximity;}
				else break; //nothing better
			}

			//now have closest estimate. If less than freqnow have match
			float closest= newpeaks[bestindex].freq;
			bool havematch=false;

			//printf("closest! %f bestindex %d rightsort %d \n ", closest, bestindex, rightsort);

			if(closest<freqnow || (i==(numprevpeaks-1))) havematch=true;
			else { //test next i as available in this case

				float competitor = prevpeaks[i+1].freq;

				if (fabs(competitor-closest)<bestsofar) {

					//if no alternative
					if (bestindex==rightsort) flag= true; //failure to match anything
					else {bestindex= rightsort-1;
						havematch=true;
					}

				} else
				havematch=true;

			}

			if(havematch) {

				//int newrightsort= bestindex;
				//if() newrightsort=

				//TIDY UP ANY CANIDATES MISSED OUT BY THIS PROCESS

				for (j=rightsort; j<=(bestindex-1);++j) {
					//BIRTHS ON RIGHT

					tracks[numtracks].freq1=newpeaks[j].freq;
					tracks[numtracks].freq2=newpeaks[j].freq; //match to itself
					tracks[numtracks].amp1=0.0;
					tracks[numtracks].amp2=newpeaks[j].mag;
					tracks[numtracks].phase1=newpeaks[j].phase; //rgen.frand()*twopi;
					tracks[numtracks].phase2=tracks[numtracks].phase1;
					++numtracks;
					++rightsort;
				}

				//printf("match! \n ");

				//MATCH!
				tracks[numtracks].freq1=prevpeaks[i].freq;
				tracks[numtracks].freq2=newpeaks[rightsort].freq;
				//find currently existing Guide corresponding to this frequency and its phase
				tracks[numtracks].amp1=prevpeaks[i].mag;
				tracks[numtracks].amp2=newpeaks[rightsort].mag;
				tracks[numtracks].phase1=prevpeaks[i].phase; //rgen.frand()*twopi;
				tracks[numtracks].phase2=tracks[numtracks].phase1;
				//yes, OK
				//printf("amp check i %d amp1 %f amp2 %f source1 %f source2 %f\n",i,tracks[numtracks].amp1, tracks[numtracks].amp2, prevpeaks[i].mag, newpeaks[rightsort].mag);
				++numtracks;
				++rightsort;

				//rightsort=bestindex+1;

			}

			//if was flag==true, then none missed out, still on rightsort

		}

	}


	//match failed, death on left
	if (flag==true) {

		//DEATH ON LEFT

		//death on left
		tracks[numtracks].freq1=prevpeaks[i].freq;
		tracks[numtracks].freq2=prevpeaks[i].freq; //match to itself
		//find phase of corresponding Guide
		tracks[numtracks].amp1=prevpeaks[i].mag;
		tracks[numtracks].amp2=0.0;
		tracks[numtracks].phase1=prevpeaks[i].phase; //rgen.frand()*twopi;
		tracks[numtracks].phase2=tracks[numtracks].phase1;
		++numtracks;

		//ADDCODE
		//++leftsort;
	}

}

//rightsort should equal numnewpeaks!

//now iterate through PartialTracks, preparing them for synthesis
unit->m_numtracks = numtracks;

//printf("numtracks problem? %d \n",numtracks);

}






//IFFT
void synthesisestochastic(SMS * unit) {

int i;

float * resynthesis= unit->m_straightresynthesis;

//printf("in synthesis function \n");

//FFT of m_straightresynthesis
//float * inplace= unit->m_inplace;
//does it need to be copied? NO
//memcpy(inplace, unit->m_straightresynthesis, unit->m_windowsize*sizeof(float));

///MISSING HANN WINDOWING BEFOREHAND?

//FFT in place via Green FFT , 10 is log2(1024)
//real forward fft carried out in place
//rffts(resynthesis, unit->m_log2n, 1, g_GreenCosTable);
scfft_dofft(unit->m_scfftresynth);

//printf("past fft \n");

//phase vocoder

//input buffer now in standard format but need magnitudes and phases viewpoint- actually, don't need phases, but efficient McCartney procedure to get magnitude goes via angle, which pretty much solves phase anyway!
SCPolarBuf *p = ToPolarApx2(resynthesis); //assumes inputbuffer in SCComplex format, which it is!

//difference to last magnitudespectrum
//RENAME magspectrum as straight name

//this will overwrite previous; must have done differencing from previous frame by now?
float * magspectrum= unit->m_magspectrum;

//model with line segment approximation: LATER, not needed for now...

//in place disturbance of p for now

int nover2 = unit->m_nover2;

//leave dc and nyquist as they were since abandoned for sinusoidal modelling

//USE FABS for differences as per p21 Serra and Smith?

//just preserved since no peaks here
p->dc= fabs(magspectrum[nover2-1]- p->dc);
p->nyq= fabs(magspectrum[nover2] - p->nyq);

RGen& rgen = *unit->mParent->mRGen;

//printf("mag diff loop \n");


for (i=0; i<(nover2-1); ++i) {

//will be copy or actual one?
SCPolar& polar = p->bin[i];

polar.phase = rgen.frand2()*pi; //rgen.frand()*twopi- pi;  //random number from -pi to pi
//polar.mag = magspectrum[nover2] - polar.mag; //surely terrible mistake?
float newmag =magspectrum[i] - polar.mag;
polar.mag =  newmag>=0.0? newmag: -newmag; //make sure positive!
}


//must write to buffer at this point if going to do so, while have noise magnitude spectrum
if(unit->buf)
graphicsbuffer(unit, unit->m_tracks, p);


//returns SCComplexBuf*
 float* q = (float *) ToComplexApx2((float *)p); //assumes inputbuffer in SCPolar format, which it is!


//printf("Pre inverse FFT \n");

unit->m_q = q;
//resynthesise via IFFT of line segment magnitudes and random phase
//riffts(q, unit->m_log2n, 1, g_GreenCosTable);
scfft_doifft(unit->m_scifft);

float * ifftoutput= unit->m_inplace;

//printf("Post inverse FFT \n");


//in place so resynthesis buffer now contains noise
//add to already calculated deterministic resynthesis output

float * outputbuffer= unit->m_outputnewnoise;
float * outputbuffer2= unit->m_outputnew;

//need to make proper interpolation or independent attributes later (premultiply whole buffer by another multiplier)
//float sineamp= ZIN0(6);
//float noiseamp= ZIN0(7);

//only need to use first half
for (i=0; i<nover2; ++i) {
outputbuffer[i] = ifftoutput[i]; //(outputbuffer[i]*sineamp) + (q[i]*noiseamp);
}

//impose fade in and fade out envelope
for (i=0; i<nover2; ++i) {
outputbuffer[i] *= g_fade[i];
outputbuffer2[i] *= g_fade[i];
}

}








//void peakmatch(SMS *unit) {
//}


void loadSMS(InterfaceTable *inTable)
{
	int i;
	double w;

	ft= inTable;

	//init_SCComplex(inTable);

//	//Create g_GreenCosTable for Green FFT
//	int size = 1024; //1 << log2n;
//	int size2 = 257; //size / 4 + 1;
//	//float *win = (float*)malloc(size2 * sizeof(float));
//	double winc = twopi / size;
//	for (i=0; i<size2; ++i) {
//		w = i * winc;
//		g_GreenCosTable[i] = cos(w);
//	}
//
//	//create von Hann window
//
//	//int size = 1 << log2n;
//	winc = twopi / size;
//	for (i=0; i<size; ++i) {
//		w = i * winc;
//		g_HannTable[i] = 0.5 - 0.5 * cos(w);
//	}

//less useful than Hann in this situation since have a threshold on peak detection anyway! so not seeking to find low amplitude components in the presence of higher amplitude components
//	//Blackman Harris window, low sidelobes but wider main lobe than Hann
//	for (i=0; i<size; ++i) {
//		w = i * winc;
//		g_HannTable[i] = 0.40217-(0.49703*cos(w))+(0.09392*cos(w*2))-(0.00183*cos(w*3));
//		//g_HannTable[i] = 0.402 - 0.498*cos(winc) + 0.098*cos(2*winc)+ 0.001*cos(3*winc);
//	}
//
//

	//prepare cosine table for resynthesis
	for (i=0; i<=g_costablesize; ++i) {

	float temp= twopi*((float)i/g_costablesize);
	g_costable[i]= cos(temp); //or sin

	//printf("cos check %d %f",i,g_costable[i]);
	}

	for (i=0; i<256; ++i) {

	float prop= (float)i/256.0;
	g_fade[i]= prop;
	g_fade[i+256]= 1.0-prop;
	}

	int windowsize=1024;
	//g_blackman92window
	double winc = twopi / windowsize;
	for (i=0; i<windowsize; ++i) {
			w = i * winc;
			//92dB 4-term
		//reciprocal now since otherwise have to divide by window later, can multiply this way round
			g_blackman92window[i] = 1.0/(0.35875-(0.48829*cos(w))+(0.14128*cos(w*2))-(0.01168*cos(w*3)));
			//printf("i %d value %f \n", i,g_blackman92window[i]);	//doesn't go totally to zero, but dangerously low skirts
	}


	//printf("SMS LOADED CHECK  test1 %d %d   test2 %d %d  \n",1025 & 0x03FF, 1025%1024, 2050 & 0x03FF, 2050%1024);

	// & 0x07FF


	//DefineDtorUnit(SMS);
	//
	DefineDtorCantAliasUnit(SMS);

	//printf("SMS LOADED CHECK  test1 %d %d   test2 %d %d  \n",1025 & 0x0400, 1025%1024, 2050 & 0x0400, 2050);

	//testing amplitude multiplier and phase result for input sine at multiple of
//
//	//int windowsize=1024;
//	int fullsize= windowsize; //*8;
//	float * transformbuf = new float[scfft_trbufsize(fullsize)];
//	float * inplace = new float[fullsize];
//	scfft *scfftwindow = new scfft;
//
//	//scfft, input size, window size, window type, in, out, transfer, forwards flag
//	scfft_create(scfftwindow, fullsize, fullsize, WINDOW_RECT, inplace, inplace, transformbuf, true);
//
//	//make window
//	for (i=0; i<fullsize; ++i)
//	inplace[i]=sin(twopi* (1.0/fullsize) * i);  //no phase offset yet
//
//	scfft_dofft(scfftwindow);
//
//	for (i=0; i<512; ++i) {
//		printf("i %d value %f \n", i,sqrt((inplace[2*i]*inplace[2*i]) + (inplace[2*i+1]*inplace[2*i+1])) );
//	}
//
////	for (i=0; i<windowsize; ++i) {
////		printf("i %d value %f \n", i,inplace[i]);
////	}
////
//
//
//

	//
//
//	//testing IFFT resynthesis ; take FFT of zero-padded window shape
//
//	int windowsize=1024;
//	int fullsize= windowsize; //*8;
//	float * transformbuf = new float[scfft_trbufsize(fullsize)];
//	float * inplace = new float[fullsize];
//	scfft *scfftwindow = new scfft;
//
//	//scfft, input size, window size, window type, in, out, transfer, forwards flag
//	scfft_create(scfftwindow, fullsize, fullsize, WINDOW_RECT, inplace, inplace, transformbuf, true);
//
//	//make window
//	for (i=0; i<fullsize; ++i)
//	inplace[i]=0.0;
//
//	//Blackman-Harris?
//	double winc = twopi / windowsize;
//	for (i=0; i<windowsize; ++i) {
//		w = i * winc;
//
//		//92dB 4-term
//		//inplace[i] = 0.35875-(0.48829*cos(w))+(0.14128*cos(w*2))-(0.01168*cos(w*3));
//
//		//boost up to a multiple of analysis fundamental
//		inplace[i] = sin(w*10)*(0.35875-(0.48829*cos(w))+(0.14128*cos(w*2))-(0.01168*cos(w*3)));
//
//
//		//http://www.diracdelta.co.uk/science/source/b/l/blackman-harris%20window/source.html
//		//72dB
//		//inplace[i] = 0.40217-(0.49703*cos(w))+(0.09392*cos(w*2))-(0.00183*cos(w*3));
//	}
//
//	scfft_dofft(scfftwindow);
//
//	for (i=0; i<windowsize; ++i) {
//		printf("i %d value %f \n", i,inplace[i]);
//	}


//	for (i=0; i<windowsize; ++i) {
//		printf("i %d value %f \n", i,inplace[i]);
//	}

		//18 bins either side of bin 22
	//for (i=4; i<40; ++i) {
	//	printf("i %d value %f \n", i,inplace[i]);
	//}


	//testing scale factors for FFT library, particularly with resynthesis via IFFT
	//
//	float inputbuffer[1024];
//	float outputbuffer[1024];
//
//	for (i=0; i<1024; ++i) {
//
//	float temp= 14.0*twopi*((float)i/1024.0);
//	inputbuffer[i]= cos(temp)*g_HannTable[i]; //or sin
//
//	//printf("cos check %d %f",i,g_costable[i]);
//	outputbuffer[i]= inputbuffer[i];
//	}
//
//
//	rffts(outputbuffer, 10, 1, g_GreenCosTable);
//
//	//check scale factor of sinusoidal resynthesis from magnitudes
//	for (i=2; i<512; ++i) {
//		printf("i %d magnitude %f\n",i,pow((outputbuffer[2*i]*outputbuffer[2*i]) + (outputbuffer[2*i+1]*outputbuffer[2*i+1]),0.5));
//	}
//
//
//	riffts(outputbuffer, 10, 1, g_GreenCosTable);
//
//	for (i=0; i<1024; ++i) {
//		printf("i %d input %f output %f difference %f ratio %f\n",i,inputbuffer[i], outputbuffer[i], inputbuffer[i]-outputbuffer[i], outputbuffer[i]/inputbuffer[i]);
//	}
//

}
