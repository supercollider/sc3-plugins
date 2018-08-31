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
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

//This file is part of The BBCut Library. Copyright (C) 2001  Nick M.Collins distributed under the terms of the GNU General Public License full notice in file BBCutLibrary.help

//last revisions pre release 1.0 Nick Collins 15 June 2004

#include "SC_PlugIn.h"
#include "SC_fftlib.h"
#include <stdio.h>

//helpful constants
#define PI 3.1415926535898f
#define TWOPI 6.28318530717952646f 

//FFT data 
#define N 1024  //FFT size
#define NOVER2 512  //FFT size
#define NOVER4 256  //FFT size
#define OVERLAP 512
#define OVERLAPINDEX 512
#define HOPSIZE 512  
#define FS 44100 //assumes fixed sampling rate
#define FRAMESR 86.1328
#define FRAMEPERIOD 0.01161 //seconds

//algorithm quantities NR tempo hypotheses, ND num phase hypotheses

//used to be TEMPOLOW 90 TEMPOHIGH 180 TEMPOSTEP 0.9 INTEGRATIONTIME 3.4 LOUDNESSFRAMESSTORED 293 
//used to be TEMPOLOW 80 TEMPOHIGH 200 TEMPOSTEP 1.2 INTEGRATIONTIME 4 LOUDNESSFRAMESSTORED 350
#define TEMPOLOW 90
//#define TEMPOHIGH 190
#define NR 100
#define ND 20
#define NDOVER2 10
#define NRD 2000
#define NDRECIP 0.05
#define TEMPOSTEP 1.0
#define INTEGRATIONTIME 3.4 
//#define NUMBEATS 4

//MINPERIOD 0.33 MAXPERIOD 0.66 MINDIFF 28 MAXDIFF 58
//MINPERIOD 0.3 MAXPERIOD 0.75 MINDIFF 25 MAXDIFF 65
#define MINPERIOD 0.315
#define MAXPERIOD 0.66
#define MINDIFF 27
#define MAXDIFF 58


//ceil(INTEGRATIONTIME*FRAMESR)
#define LOUDNESSFRAMESSTORED 293
//can calculate after frame 293 then

//these values assume TEMPOLOW 90 and sample rate 44100 with 1024 FFT
//#define FFTSTOREMEM 350
//#define FFTSTORESIZE 8400
//about 4 seconds of FFT powers stored between 0 and 1000 Hz (bin indices 1 to 24 for aforementioned FFT size)

//about once per second, now made quicker update

//old values SKIP 28 TIMEELAPSED 0.3250794
#define SKIP 24
//28/86.1328 
#define TIMEELAPSED 0.2786395
//just quicker than one beat at 180bpm 1.9969164 //0.9984582 

#define tempoconsistency 2
#define phaseconsistency 2
//#define tempotestvalue 0.25
//#define phasetestvalue 0.4

#define MAXONSETS 50

//const int toptempi=5;

const float subbeats[8]= {0.0,0.5,1.0,1.5,2.0,2.5,3.0,3.5};
//const float subbeats[2][16]= {{0.0,0.25,0.5,0.75,1.0,1.25,1.5,1.75,2.0,2.25,2.5,2.75,3.0,3.25,3.5,3.75},{0.0,0.32,0.5,0.82,1.0,1.32,1.5,1.82,2.0,2.32,2.5,2.82,3.0,3.32,3.5,3.82}};
//const float subbeats[16]={0.0,0.25,0.5,0.75,1.0,1.25,1.5,1.75,2.0,2.25,2.5,2.75,3.0,3.25,3.5,3.75};
const float beats[4]= {0.0,1.0,2.0,3.0};

//weights
const float sbtemplate[8]= {1.0,0.5,1.0,0.5,1.0,0.5,1.0,0.5};

//could be different templates based on a parameter
//const float sbtemplate[16]= {1.0,0.1,0.8,0.2,1.0,0.1,0.8,0.2,1.0,0.1,0.8,0.2,1.0,0.1,0.8,0.2};

//phase matching conditions
//float required[4]={1.0, 0.5, 0.3, 0.5};


//use integrationtime for maxwindow
//#define MAXWINDOW    //(numbeats+1)*lowperiod; 


//for hanning window, constructed in plugin initialisation
float hanning[N]; 


int eqlbandbins[43]= {1,2,3,4,5,6,7,8,9,11,13,15,17,19,22,25,28,32,36,41,46,52,58,65,73,82,92,103,116,129,144,161,180,201,225,251,280,312,348,388,433,483,513}; 
int eqlbandsizes[42]= {1,1,1,1,1,1,1,1,2,2,2,2,2,3,3,3,4,4,5,5,6,6,7,8,9,10,11,13,13,15,17,19,21,24,26,29,32,36,40,45,50,30}; 

//this loudness conversion was removed since it worked better without transient detection but just using current energy
//float contours[42][11]= {{ 47.88, 59.68, 68.55, 75.48, 81.71, 87.54, 93.24, 98.84,104.44,109.94,115.31},{ 29.04, 41.78, 51.98, 60.18, 67.51, 74.54, 81.34, 87.97, 94.61,101.21,107.74},{ 20.72, 32.83, 43.44, 52.18, 60.24, 67.89, 75.34, 82.70, 89.97, 97.23,104.49},{ 15.87, 27.14, 37.84, 46.94, 55.44, 63.57, 71.51, 79.34, 87.14, 94.97,102.37},{ 12.64, 23.24, 33.91, 43.27, 52.07, 60.57, 68.87, 77.10, 85.24, 93.44,100.90},{ 10.31, 20.43, 31.03, 40.54, 49.59, 58.33, 66.89, 75.43, 83.89, 92.34,100.80},{  8.51, 18.23, 28.83, 38.41, 47.65, 56.59, 65.42, 74.16, 82.89, 91.61,100.33},{  7.14, 16.55, 27.11, 36.79, 46.16, 55.27, 64.29, 73.24, 82.15, 91.06, 99.97},{  5.52, 14.58, 25.07, 34.88, 44.40, 53.73, 62.95, 72.18, 81.31, 90.44, 99.57},{  3.98, 12.69, 23.10, 32.99, 42.69, 52.27, 61.66, 71.15, 80.54, 89.93, 99.31},{  2.99, 11.43, 21.76, 31.73, 41.49, 51.22, 60.88, 70.51, 80.11, 89.70, 99.30},{  2.35, 10.58, 20.83, 30.86, 40.68, 50.51, 60.33, 70.08, 79.83, 89.58, 99.32},{  2.05, 10.12, 20.27, 30.35, 40.22, 50.10, 59.97, 69.82, 79.67, 89.52, 99.38},{  2.00,  9.93, 20.00, 30.07, 40.00, 49.93, 59.87, 69.80, 79.73, 89.67, 99.60},{  2.19, 10.00, 20.00, 30.00, 40.00, 50.00, 59.99, 69.99, 79.98, 89.98, 99.97},{  2.71, 10.56, 20.61, 30.71, 40.76, 50.81, 60.86, 70.96, 81.01, 91.06,101.17},{  3.11, 11.05, 21.19, 31.41, 41.53, 51.64, 61.75, 71.95, 82.05, 92.15,102.33},{  2.39, 10.69, 21.14, 31.52, 41.73, 51.95, 62.11, 72.31, 82.46, 92.56,102.59},{  1.50, 10.11, 20.82, 31.32, 41.62, 51.92, 62.12, 72.32, 82.52, 92.63,102.56},{ -0.17,  8.50, 19.27, 29.77, 40.07, 50.37, 60.57, 70.77, 80.97, 91.13,101.23},{ -1.80,  6.96, 17.77, 28.29, 38.61, 48.91, 59.13, 69.33, 79.53, 89.71, 99.86},{ -3.42,  5.49, 16.36, 26.94, 37.31, 47.61, 57.88, 68.08, 78.28, 88.41, 98.39},{ -4.73,  4.38, 15.34, 25.99, 36.39, 46.71, 57.01, 67.21, 77.41, 87.51, 97.41},{ -5.73,  3.63, 14.74, 25.48, 35.88, 46.26, 56.56, 66.76, 76.96, 87.06, 96.96},{ -6.24,  3.33, 14.59, 25.39, 35.84, 46.22, 56.52, 66.72, 76.92, 87.04, 97.00},{ -6.09,  3.62, 15.03, 25.83, 36.37, 46.70, 57.00, 67.20, 77.40, 87.57, 97.68},{ -5.32,  4.44, 15.90, 26.70, 37.28, 47.60, 57.90, 68.10, 78.30, 88.52, 98.78},{ -3.49,  6.17, 17.52, 28.32, 38.85, 49.22, 59.52, 69.72, 79.92, 90.20,100.61},{ -0.81,  8.58, 19.73, 30.44, 40.90, 51.24, 61.52, 71.69, 81.87, 92.15,102.63},{  2.91, 11.82, 22.64, 33.17, 43.53, 53.73, 63.96, 74.09, 84.22, 94.45,104.89},{  6.68, 15.19, 25.71, 36.03, 46.25, 56.31, 66.45, 76.49, 86.54, 96.72,107.15},{ 10.43, 18.65, 28.94, 39.02, 49.01, 58.98, 68.93, 78.78, 88.69, 98.83,109.36},{ 13.56, 21.65, 31.78, 41.68, 51.45, 61.31, 71.07, 80.73, 90.48,100.51,111.01},{ 14.36, 22.91, 33.19, 43.09, 52.71, 62.37, 71.92, 81.38, 90.88,100.56,110.56},{ 15.06, 23.90, 34.23, 44.05, 53.48, 62.90, 72.21, 81.43, 90.65, 99.93,109.34},{ 15.36, 23.90, 33.89, 43.31, 52.40, 61.42, 70.29, 79.18, 88.00, 96.69,105.17},{ 15.60, 23.90, 33.60, 42.70, 51.50, 60.20, 68.70, 77.30, 85.80, 94.00,101.70},{ 15.60, 23.90, 33.60, 42.70, 51.50, 60.20, 68.70, 77.30, 85.80, 94.00,101.70},{ 15.60, 23.90, 33.60, 42.70, 51.50, 60.20, 68.70, 77.30, 85.80, 94.00,101.70},{ 15.60, 23.90, 33.60, 42.70, 51.50, 60.20, 68.70, 77.30, 85.80, 94.00,101.70},{ 15.60, 23.90, 33.60, 42.70, 51.50, 60.20, 68.70, 77.30, 85.80, 94.00,101.70},{ 15.60, 23.90, 33.60, 42.70, 51.50, 60.20, 68.70, 77.30, 85.80, 94.00,101.70}};
//double phons[11]={2,10,20,30,40,50,60,70,80,90,100};


extern "C"
{
}

InterfaceTable *ft;



struct DrumTrack : Unit {
	
	//FFT data
	int m_bufWritePos;
	float * m_prepareFFTBuf;
	float * m_FFTBuf;

	scfft *m_scfft;
	
	//time positions
	long m_frame;
	
	//loudness measure
	float m_loudness[LOUDNESSFRAMESSTORED];
	int m_loudnesscounter;
	float m_loudbands[40]; 
	
	//for bass onset detections
	float m_bassonsets[LOUDNESSFRAMESSTORED];
	
	//tempo
	
	float m_currtempo;
	float m_lasttempo; 
	float m_tempodiff[tempoconsistency]; //initialise to ones
	int m_tdindex;
	
	//phase
	
	float m_currphase;
	float m_lastphasewinner;
	float m_phasediffs[phaseconsistency]; //initialise to ones
	int m_pdindex;
	
	//phasor, trigger beat and modulo when >1.0
	float m_phase, m_phaseperblock;
	
	//phasor output separate so can have it lock and keep going when don't want to track
	float m_outputphase, m_outputtempo, m_outputphaseperblock;
	
	int halftrig;
    int q1trig;
	int q2trig;
	
	//can make twice as many including groove, or just test eighth note positions for now
	
	//dynamic programming 
	float m_lastscore[40];
	float m_lastcandidatetempo[10]; //indices
	float m_lastcandidatephase[40];
	
	//candidate scores
	//float m_scores[NRD]; //2000 possibles
	float m_bestscores[10];
	float m_besttemposcores[10]; //store tempi
	float m_bestphasescores[40]; //store phases four for each, always best two and two antiphase 
	float m_phasescores[40]; //store the actual scores for use in the the dyn prog cost function
	
	//amortization
	int m_amortisationstate;
	int m_storeloudnesscounter;
	float m_scorenorm; //normalise all scores in a sweep by the maximum
	
	//chord detection- Goto histogramming requires storing fft bins 1 to 24 over last 4 seconds (see workbook 15 dec 04)
//	float * m_fftstore;
//	int m_fftstorepos;
//	int m_fftstoreposhold;
//	float m_chordhistogram[24];
//	float m_chordhistogramprev[24];
//	


	//bass/chord normalisation
	float m_bassmax; 
	
	//,m_chordmax;
	
	//final consistency requirement
	float m_prevphase,m_prevtempo;
	float m_consistency;
	
	
	//goto onset spotting
	float * m_powerbuf[3];
	int m_powerbufcount;
	
	float m_maxsnaresig;
	long m_lastsnaredetect;
	
	float m_maxkicksig;
	long m_lastkickdetect;
	
	//0 for nothing, 1 for snare, 2 for kick, same counter as m_loudnesscounter 
	int m_onsets[LOUDNESSFRAMESSTORED];
	//int onsetposition[50];
	int m_onsetposition[MAXONSETS]; //no more than this
	float m_patternphase; //if 
	float m_patternscore;
	float m_patterntempo;
	
	float * m_prior;
	
	int m_debugmode;
	
};



extern "C"
{
	//required interface functions
	void DrumTrack_next(DrumTrack *unit, int wrongNumSamples);
	void DrumTrack_Ctor(DrumTrack *unit);
	void DrumTrack_Dtor(DrumTrack *unit);
}

//other functions
void preparefft(DrumTrack *unit, float* in, int n);
void dofft(DrumTrack *unit);
void calculateloudness(DrumTrack *unit);
void updatetempophase(DrumTrack *unit, float tempowinner,float phasewinner);
float phasematchesbassonsets(DrumTrack *unit, float phase, float bps);
//float comparechordetect(DrumTrack *unit, float phase, float bps);

void snaredetection(DrumTrack *unit);
void kickdetection(DrumTrack *unit);
int findarchetype(DrumTrack *unit, float tempowinner);
void testpattern(DrumTrack * unit, int &bestindex, float &bestscore, int &besttempo, int first, int diff, int firstval);

//amortisation
void do100thscoring(DrumTrack *unit,int j);
float calcphasediff(float prevphase, float phase, float prevtempo, float tempo);
void do40thpath(DrumTrack *unit,int i);
void finaldecision(DrumTrack *unit);

//old
void tempoassess(DrumTrack *unit, float *, float *);

void DrumTrack_Ctor(DrumTrack* unit) {
	int i,j;
	
	unit->m_debugmode=(int)ZIN0(10);
	
	////prior//////
	
	unit->m_prior=NULL;
	
	World *world = unit->mWorld;
    
	int bufnum = (int)ZIN0(7);
	
	//if (bufnum >= world->mNumSndBufs) bufnum = 0;
	
	//printf("bufnum%d\n",bufnum);
	
	if (bufnum>=0) {
	SndBuf *buf = world->mSndBufs + bufnum; 
	
	int priorbufsize = buf->samples;

	if(priorbufsize!=100) {printf("buffer for tempo weightings prior not size 100  bufnum%d\n",bufnum);}
	else {
	unit->m_prior= buf->data; 
	
	//for (int h=0; h<100; ++h) {
//	printf("%f\n",unit->m_prior[h]);
	//}
	}
	}
	
	
	////////FFT data///////////
	
	unit->m_prepareFFTBuf = (float*)RTAlloc(unit->mWorld, N * sizeof(float));
	unit->m_FFTBuf = (float*)RTAlloc(unit->mWorld, N * sizeof(float));
	unit->m_bufWritePos = 0;	

	//N=1024
	SCWorld_Allocator alloc(ft, unit->mWorld);
	//no overlap
	unit->m_scfft = scfft_create(N, N, kHannWindow, unit->m_FFTBuf, unit->m_FFTBuf, kForward, alloc);
	
	////////time positions//////////
	unit->m_frame=0;
	
	/////////loudness measure////////
	unit->m_loudnesscounter=LOUDNESSFRAMESSTORED-1;
	//zero loudness store 
	for(j=0;j<LOUDNESSFRAMESSTORED;++j) {
		unit->m_loudness[j]=0.0;
		unit->m_bassonsets[j]=0.0;
	}
	
	//zero previous specific loudness in Bark bands
	for(j=0;j<40;++j) {
		unit->m_loudbands[j]=0.0;
	}
	
	/////////tempo assess///////////
	unit->m_currtempo=2;
	unit->m_lasttempo=2; 
	
	for(j=0;j<tempoconsistency;++j)
		unit->m_tempodiff[j]=1.0;		//if 0.0 biases taking first tempo estimate it can! 
	
	unit->m_tdindex=0;
	
	
	////////phase assess///////////
	
	unit->m_currphase=0.0;
	
	unit->m_lastphasewinner=0.0;
	
	for(j=0;j<phaseconsistency;++j)
		unit->m_phasediffs[j]=1.0; //take first phase you can? No, wait
	
	unit->m_pdindex=0;
	
	unit->m_phase=0.0;
	
	//default of 2bps
	unit->m_phaseperblock= (unit->mWorld->mFullRate.mBufLength*2)/(FS);
	
	unit->m_outputphase= unit->m_phase;
	unit->m_outputtempo= unit->m_currtempo;
	unit->m_outputphaseperblock= unit->m_phaseperblock;
	
	unit->mCalcFunc = (UnitCalcFunc)&DrumTrack_next;
	
	unit->halftrig=0;
    unit->q1trig=0;
	unit->q2trig=0;
	
	//amortisation
	unit->m_amortisationstate=142; //off
	
	//dyn prog initialise playing field
	float * lastscore= unit->m_lastscore;
	float * lastcandtempo= unit->m_lastcandidatetempo;
	float * lastcandphase= unit->m_lastcandidatephase;
	
	for(j=0;j<10;++j) {
		
		lastcandtempo[j]=1.5+j*0.15; //even spread
		
		for (i=0;i<4;++i) {
			lastcandphase[4*j+i]=i*0.25; //even spread
			lastscore[4*j+i]=0;
			
		}
	}
	
	
	
	unit->m_scorenorm= 1.0;
	
	//int storesize= FFTSTORESIZE; //24*FFTSTOREMEM;
//	
//	unit->m_fftstore = (float*)RTAlloc(unit->mWorld, storesize * sizeof(float));
//	
//	float * fftstore= unit->m_fftstore;
//	
//	//zero initialise
//	for(j=0;j<storesize;++j) fftstore[j]=0;
//	
//	unit->m_fftstorepos=FFTSTOREMEM-1;
//	
	unit->m_bassmax=1.0;
	//unit->m_chordmax=1.0;
	
	unit->m_prevphase=0.0;
	unit->m_prevtempo=2;
	unit->m_consistency=0.0;
	
	
	//snare detection
	for (i=0;i<3;++i)
		unit->m_powerbuf[i] = (float*)RTAlloc(unit->mWorld, NOVER4 * sizeof(float)); 
	
	//zero buffers
	for (i=0;i<3;++i)
		for (j=0;j<NOVER4;++j)
			unit->m_powerbuf[i][j]=0.0;
	
	unit->m_powerbufcount=0;
	
	unit->m_maxsnaresig=ZIN0(9); //*15.0; //15.0; //40.0; //from testing, taken as the fair value
	unit->m_lastsnaredetect=0;
	
	unit->m_maxkicksig=ZIN0(8)*20000.0; //1.0
	
	//blank out onsets
	for(j=0;j<LOUDNESSFRAMESSTORED;++j) {
		unit->m_onsets[j]=0;
	}
	
}



void DrumTrack_Dtor(DrumTrack *unit)
{
	
	RTFree(unit->mWorld, unit->m_prepareFFTBuf);
	RTFree(unit->mWorld, unit->m_FFTBuf);
	//RTFree(unit->mWorld, unit->m_fftstore);
		
	for (int i=0;i<3;++i)
		RTFree(unit->mWorld, unit->m_powerbuf[i]); 
	
	if(unit->m_scfft) {
		SCWorld_Allocator alloc(ft, unit->mWorld);
		scfft_destroy(unit->m_scfft, alloc);
	}
}


void DrumTrack_next(DrumTrack *unit, int wrongNumSamples)
{
	//would normally be float,will be cast to int for Tristan's optimisation
	float *in = IN(0);
	
	//updated at control rate
	unit->m_maxsnaresig=ZIN0(9); //*15.0; //15.0; //40.0; //from testing, taken as the fair value
	unit->m_maxkicksig=ZIN0(8)*20000.0; //1.0
	
	//printf("%d \n",wrongNumSamples);
	
	int numSamples = unit->mWorld->mFullRate.mBufLength;
	
	//conditions in reverse order to avoid immediate spillover
	
	//printf("state %d \n",unit->m_amortisationstate);	
	
	//final update phase decision
	if(unit->m_amortisationstate==140) {
		
		//printf("finaldecision \n");	
		
		unit->m_amortisationstate=unit->m_amortisationstate+1; //141 is off, nothing to do
		
		finaldecision(unit);
	}
	
	//then path assessments, 40*40= 1600, do 40 each time for conceptual clarity
	if((unit->m_amortisationstate>99) && (unit->m_amortisationstate<140)) {
		
		//printf("do40thpath %d \n",unit->m_amortisationstate);	
		
		if(unit->m_amortisationstate==100) {
			
			//maybe 
			//	if((1.0/unit->m_bestscores[0])>0.00001) {
			if((1.0/unit->m_bestscores[0])<unit->m_scorenorm) {
				unit->m_scorenorm= 1.0/unit->m_bestscores[0];
				
				//printf("%f \n",unit->m_scorenorm);
				 } 
			//else
			//	unit->m_scorenorm= 1.0;
			
			//set weakest candidate(s) to be the patten phase, tempo
			
			//dangerous
			//unit->m_bestscores[9]=unit->m_patternscore;
			unit->m_bestscores[9]=unit->m_bestscores[0];

		unit->m_besttemposcores[9]= unit->m_patterntempo;
		unit->m_bestphasescores[36]= unit->m_patternphase;
			
			
		}
		
		do40thpath(unit, unit->m_amortisationstate-100);
		unit->m_amortisationstate=unit->m_amortisationstate+1;
		
	}
	
	//calculation spread out in time
	if(unit->m_amortisationstate<100) {
		
		//printf("do100thscore %d \n",unit->m_amortisationstate);	
		
		do100thscoring(unit,unit->m_amortisationstate);
		
		unit->m_amortisationstate=unit->m_amortisationstate+1;
		
	}
	
	preparefft(unit, in, numSamples);
	
	//test if impulse to output
	
	unit->m_phase+=unit->m_phaseperblock;
	
	//if not locked, update output phase from model phase, else keep a separate output phase
	
	float lock= ZIN0(1);
	
	//printf("lock %f \n",lock);
	
	if(lock<0.5) {
	
	unit->m_outputphase= unit->m_phase;
	unit->m_outputtempo= unit->m_currtempo;
	unit->m_outputphaseperblock= unit->m_phaseperblock;
	} else {
	unit->m_outputphase+=unit->m_outputphaseperblock;
	}
	
	if (unit->m_phase >= 1.f) {unit->m_phase-= 1.f;}

	//0 is beat, 1 is quaver, 2 is semiquaver, 3 is actual current tempo in bps
	//so no audio accuracy with beats, just asap, may as well be control rate
	ZOUT0(0)=0.0;
	ZOUT0(1)=0.0;
	ZOUT0(2)=0.0;
	ZOUT0(3)=unit->m_outputtempo; //*0.016666667;
	
	//output beat
	if (unit->m_outputphase >= 1.f) {
		
		//printf("beat \n");
		
		unit->m_outputphase -= 1.f;
		ZOUT0(0)=1.0;
		ZOUT0(1)=1.0;
		ZOUT0(2)=1.0;
		unit->halftrig=0;
		unit->q1trig=0;
		unit->q2trig=0;
	}
	
	if (unit->m_outputphase>=0.5 && unit->halftrig==0) {
		ZOUT0(1)=1.0;
		ZOUT0(2)=1.0;
		unit->halftrig=1;
	}
	
	if (unit->m_outputphase>=0.25 && unit->q1trig==0) {
		ZOUT0(2)=1.0;
		unit->q1trig=1;
	}
	
	if (unit->m_outputphase>=0.75 && unit->q2trig==0) {
		ZOUT0(2)=1.0;
		unit->q2trig=1;
	}
	
	//for (int i=0; i<numSamples; ++i) {
	//		*++output = 0.0;
	//	}
	//	
	
	
}


//Tristan recommends copying ints rather than floats- I say negligible compared to other algorithm costs for the moment
// TO TREAT, check, update, probably replace entirely with pre allocated buffer based scheme? 
void preparefft(DrumTrack *unit, float* in, int n) {
	
	int i, index = 0, cpt = n, maxindex;
	
	int bufpos= unit->m_bufWritePos;
	
	float * preparefftbuf=unit->m_prepareFFTBuf;
	float * fftbuf= unit->m_FFTBuf;
	
	// Copy input samples into prepare buffer	
	while ((bufpos < N) && (cpt > 0)) {
		preparefftbuf[bufpos] = in[index];
		bufpos++;
		index++;
		cpt--;
	}
	
	// When Buffer is full...
	if (bufpos >= N) {
		
		// Make a copy of prepared buffer into FFT buffer for computation
		for (i=0; i<N; i++) 
			fftbuf[i] = preparefftbuf[i];
		
		// Save overlapping samples back into buffer- no danger since no indices overwritten
		for (i=0; i<OVERLAP; i++) 
			preparefftbuf[i] = preparefftbuf[OVERLAPINDEX+i];
		
		maxindex = n - index + OVERLAPINDEX;
		
		//blockSize less than N-OVERLAPINDEX so no problem
		// Copy the rest of incoming samples into prepareFFTBuffer
		for (i=OVERLAPINDEX; i<maxindex; i++) {
			preparefftbuf[i] = in[index];
			index++;
		}
		
		bufpos = maxindex;
		
		//FFT buffer ready- calculate away!
		unit->m_frame= unit->m_frame+1;
		dofft(unit);
	}
	
	
	unit->m_bufWritePos= bufpos;
	//printf("%d \n",bufpos);
	
}



//calculation function once FFT data ready
void dofft(DrumTrack *unit) {
	
	int i;
	
	float * fftbuf= unit->m_FFTBuf;
	
	for (i=0; i<N; ++i)
		fftbuf[i] *= hanning[i];

	scfft_dofft(unit->m_scfft);
	
	float val1, val2;
	// Squared Absolute so get power
	for (i=2; i<N; i+=2) {
		val1 = fftbuf[i];
		val2 = fftbuf[i+1];
		//i>>1 is i/2
		fftbuf[i>>1] = (val1*val1)+(val2*val2);
	}
	
	//calculate loudness first, increments loudnesscounter needed in checkforonsets
	calculateloudness(unit);
	
	//for snare and kick detection
	///keeping a record of last three fftbufs
	//update last three powers in required bands
	unit->m_powerbufcount=(unit->m_powerbufcount+1)%3;
	
	float * storebuf= unit->m_powerbuf[unit->m_powerbufcount];
	
	//only stores lower bands under 11kHz
	for (i=0;i<NOVER4;++i)
		storebuf[i]=fftbuf[i];
	
	//zero out now, not two ago like before
	int onsetframe= (unit->m_loudnesscounter+LOUDNESSFRAMESSTORED)%LOUDNESSFRAMESSTORED;
	unit->m_onsets[onsetframe]=0;  //zero out old values		
	
	//high detections
	//gotodetectioninband(unit,unit->m_gotobands[1]);	
	
	//low detections second so these onsets always overpower the hgh onsets	
	//gotodetectioninband(unit,unit->m_gotobands[0]);
	
	//snare detections (not overwritable by kicks)
	snaredetection(unit);
	kickdetection(unit);
	
	//avoid snare overwrite, trust snare detector
	
	//	//check for any twos in last ten, checking ten either side
	//	int basepos= (unit->m_loudnesscounter+LOUDNESSFRAMESSTORED-2)%LOUDNESSFRAMESSTORED;
	//	
	//	//if any kicks near recent snares, zero the snares
	//	for (j=0;j<5;++j) {
	//	 int pos=(basepos+LOUDNESSFRAMESSTORED-j)%LOUDNESSFRAMESSTORED;
	//	 
	//	 if(unit->m_onsets[pos]==1) {
	//		for (i=0;i<5;++i) { //checking 5 previous frames from current position, will gradually test 5 either side
	//		 int pos2=(basepos+LOUDNESSFRAMESSTORED-i)%LOUDNESSFRAMESSTORED;
	//	
	//		if(unit->m_onsets[pos2]==2) {
	//		unit->m_onsets[pos]=0;
	//		//break //could break because know no 2 can come till 10 after a 2 onset, but safer to leave alone
	//		}
	//		
	//		}
	//	 
	//	 }
	//	 
	//	} 
	
	//int onsetval= unit->m_onsets[(unit->m_loudnesscounter+LOUDNESSFRAMESSTORED-2)%LOUDNESSFRAMESSTORED];
	//if(onsetval) printf("onset %d \n",onsetval);
	
	
	if (unit->m_frame%SKIP==0) {
		
		//printf("amortisation time \n");
		
		//amortisation- 8 control periods in a frame
		//have 2000 calcs to do, split over 100 control periods = 6400 samples, ie one tempo per control period
		
		unit->m_amortisationstate=0;
		
		//fix time reference for calculations, so it doesn't update during the amortisation
		unit->m_storeloudnesscounter= unit->m_loudnesscounter;  
		
		//unit->m_fftstoreposhold= unit->m_fftstorepos;
		
		unit->m_currphase=unit->m_phase;
		
		//reset scoring
		//float * scores=  unit->m_scores;
		float * bestscores= unit->m_bestscores;
		//float * besttemposcores=unit->m_besttemposcores;
		//	float * bestphasescores=unit->m_bestphasescores;
		//	
		for (i=0; i<10; ++i) { 
			
			//no need to zero, gets written into during this process
			//for (j=0; j<ND; ++j)
			//scores[i][j]; //2000 possibles
			
			bestscores[i]=-1000;
			//besttemposcores[i]=0.0; 
			//	bestphasescores[i*4]=0.0; 
			//	bestphasescores[i*4+1]=0.0;
			//	bestphasescores[i*4+2]=0.0;
			//	bestphasescores[i*4+3]=0.0;
			//	no need to reset these since will automatically get written over
		}
		
		//assess pattern matches for scoring 
		int match= findarchetype(unit,0.0); //don't know tempowinner yet 
	
		//if (match && (unit->m_patternscore>1.75)) {
//	
//		printf("pattern phase %f pattern tempo %f \n",unit->m_patternphase,unit->m_patterntempo);
//
//		}

		
		
		//float tempowinner,phasewinner;
		//	
		//	tempoassess(unit,&tempowinner,&phasewinner);
		//	
		//	//1.0-phasewinner because the phasenow should be (1.0-pw) to get to a beat at pw
		//	updatetempophase(unit, tempowinner,phasewinner);
		//	
	}
	
	//updatephase
	
}	



//global
float scoretemp[ND]; 		

void do100thscoring(DrumTrack *unit,int j) {
	int i,k, baseframe, rounded, testframe;
	float tempo, Ti, starttime;
	float phase, beatpos;
	float beat, beatsum; 
	float prior;
	
	
	//weight by prior if one exists
	prior= 1.0;
	
	if (unit->m_prior) {prior= unit->m_prior[j];}
	
	//printf("prior %f %d\n",prior,j);
	
	baseframe=unit->m_storeloudnesscounter+LOUDNESSFRAMESSTORED;
	float * loudness= unit->m_loudness;
	
	float best, secondbest;
	int bestindex, secondbestindex;
	//find best two phases 
	best=-1000;
	bestindex=0;
	secondbest=-1000;
	secondbestindex=0;
	
	//float * scores=  unit->m_scores;
	
	//int groovetype=0;
	
	//for(int groove=0;groove<1;++groove) {
	
	tempo=TEMPOLOW+(j*TEMPOSTEP);
	
	Ti= 60/tempo;   //could have precalculated these- OPTIMISE 
	
	starttime= 0.0-(5*Ti);
	
	
	//different phase options
	for (k=0; k<ND; ++k) {
		
		phase=k*NDRECIP;    
		
		beatpos=starttime+(Ti*phase); 
		
		beatsum=0.0;
		
		//8 subbeats
		for (i=0;i<8;++i) {
			
			beat=((subbeats[i])*Ti)+beatpos;
			
			//beat=((subbeats[groove][i])*Ti)+beatpos;
			
			//get nearest frame position  
			
			rounded= (int)floor(beat*FRAMESR+ .5f);
			
			//printf("error in rounding? beat %f rounded %d correction %d \n",beat, rounded,  (int)(beat*FRAMESR -.5f));
			
			testframe=(baseframe+rounded)%LOUDNESSFRAMESSTORED;
			
			beatsum+=(sbtemplate[i])*loudness[testframe];    
			
			//could do a local sum +-2 frames
			
		//	float localsum=0.0;
//			
//			for (int hh=0; hh<3; ++hh) {
//			
//			int locale= (testframe-1+hh+LOUDNESSFRAMESSTORED)%LOUDNESSFRAMESSTORED;
//			
//			localsum+=loudness[locale];    
//			}
			
			//beatsum+=(sbtemplate[i])*localsum*0.333;    
			
		}
		
		beatsum=beatsum*prior;
		scoretemp[k]=beatsum; 
		//what is best so far?
		
		//for normalising
		if (beatsum>best) {
			secondbest=best;
			secondbestindex=bestindex;
			best=beatsum;
			bestindex=k;
			//groovetype=groove;
		} else if (beatsum>secondbest) {
			secondbest=beatsum;
			secondbestindex=k;
		} 
		
		//not sure this needs to be stored at all
		//scores[j*ND+k]=beatsum; 
		
	}
	
	
	
	
	//what is minimum you need to do to get into the best scoring table? 
	float leastbest= unit->m_bestscores[9];
	
	//update topten		
	if (best>leastbest) {
		
		float * bestscores= unit->m_bestscores;
		float * besttemposcores=unit->m_besttemposcores;
		float * bestphasescores=unit->m_bestphasescores;
		float * phasescores= unit->m_phasescores;
		
		//where does it rate
		for (i=0; i<10; ++i) {
			if(best>bestscores[i]) break;
		}
		
		//copy all below down one
		for (k=9; k>i; --k) {
			bestscores[k]=bestscores[k-1];
			besttemposcores[k]=besttemposcores[k-1];
			
			for (int g=0; g<4; ++g) {
				bestphasescores[k*4+g]=bestphasescores[(k-1)*4+g];
				phasescores[k*4+g]=phasescores[(k-1)*4+g];	
			}
		}
		
		//insertion at i
		bestscores[i]=best;
		besttemposcores[i]=tempo*0.01666667; //j;
		
		//actually stores phases
		bestphasescores[i*4]=bestindex*NDRECIP;
		bestphasescores[i*4+1]=secondbestindex*NDRECIP;
		bestphasescores[i*4+2]=((bestindex+NDOVER2)%ND)*NDRECIP; //pi out of phase option, may be doubled up safely I assume
		bestphasescores[i*4+3]=((secondbestindex+NDOVER2)%ND)*NDRECIP;
		
		//this stores scores
		phasescores[i*4]= scoretemp[bestindex];
		phasescores[i*4+1]= scoretemp[secondbestindex];
		phasescores[i*4+2]= scoretemp[((bestindex+NDOVER2)%ND)];
		phasescores[i*4+3]= scoretemp[((secondbestindex+NDOVER2)%ND)];
		
	}
	
	}





void do40thpath(DrumTrack *unit,int i) {
	
	int j;
	
	//will make them constants, variables?
	float dynleak=ZIN0(2); //0.7; //0.5;
	float tempowt=ZIN0(3); //1.0; //1.0; //dyn tempo cost
	float phasewt=ZIN0(4); //2.0; //dyn phase cost
						//	float gamma=0.0; //1.0; //1.0 //consistent tempo cost
						//	float epsilon=0.0; //2.0; //2.0; //consistent phase cost
	float basswt=ZIN0(5); //1.0;
	float patternwt=ZIN0(6); //1.0 used to be chordwt;
	
	
	//printf("do40th test1 %d %d \n",unit->m_amortisationstate,i);	
	
	float * lastscore= unit->m_lastscore;
	
	//maybe store float as cheaper on CPU?
	float * lastcandtempo= unit->m_lastcandidatetempo;
	float * lastcandphase= unit->m_lastcandidatephase;
	
	//float * bestscores= unit->m_bestscores;
	
	//or should have stored actual values?
	float tempocand= unit->m_besttemposcores[i/4];
	float phasecand=unit->m_bestphasescores[i];
	
	
	//test phasewinner against bass onsets
	float bassonsetson= phasematchesbassonsets(unit,phasecand,tempocand);
	//compare to antiphase
	
	//printf("phase winner %f antiphase %f\n",phasewinner,fmod(phasewinner+0.5,1.0));
	
	float antiphase= fmod(phasecand+0.5,1.0);
	
	float bassonsetsoff= phasematchesbassonsets(unit,antiphase,tempocand);
	
	if (bassonsetsoff<1) bassonsetsoff=1;
	
	float bassratio= bassonsetson/bassonsetsoff;
	
	//float chordratio= comparechordetect(unit,phasecand,tempocand);
	
	float bassmax= unit->m_bassmax;
	
	//gradual decay over time
	bassmax= 0.999*bassmax; 
	
	if (bassratio>bassmax) bassmax=bassratio;
	
	unit->m_bassmax=bassmax;
	
	//float chordmax= unit->m_chordmax;
	//
	////gradual decay over time
	//chordmax= 0.999*chordmax; 
	//
	//if (chordratio>chordmax) chordmax=chordratio;
	//
	//unit->m_chordmax=chordmax;
	
	//bigger the bass/chord ratio, less the cost
	float basscost= (basswt)*(1.0-(bassratio/bassmax));
	
	//maximal cost if no evidence
	float patterncost= 1.0; //(chordwt)*(1.0-(chordratio/chordmax));

	if (unit->m_patternscore>1.75) {
		//be more lenient because kick/snare detection is approximate
		float patttempodiff=fabs(tempocand-unit->m_patterntempo);
		if (patttempodiff>0.105) patttempodiff=1; else patttempodiff=0.0;
	
		//phase cost of this phase replacing current phase of oscillator
		float pattphaseerror= fabs(calcphasediff(unit->m_patternphase,phasecand,unit->m_patterntempo,tempocand));
	
		//printf("pattern phase cost %f pattern tempo cost %f \n",pattphaseerror,patttempodiff);
		
		//patterncost= patternwt*((0.0*patttempodiff) + (3*pattphaseerror)); 
		patterncost= patternwt*((0.5*patttempodiff) + (0.5*3*pattphaseerror)); 
	}
	
	
	//values now
	float tempo=unit->m_currtempo;
	float phase=unit->m_currphase; //captured at beginning of amortisation, can't used unit->m_phase without correction as some time has passed due to amortisation
	
	float prevtempo,prevphase;
	
	//find lowest score
	
	//score= previousscore*dynleak + score+ tempotransitioncost*alpha + epsilon*tempoconfrimationcost (from currtempo to this)  
	//beta*phasetransitioncost (path cost) + gamma*phaseconfirmationcost (from last phase to this) 
	
	float cost,mincost;
	
	mincost=1000000;
	
	//these two transitions are dyn prog independent so calculated outside this loop
	//tempo transition costs- fixed if tempo transition above some constant (assume JND for tempo = 8% so 0.105bps at 1.5bps 
	
	float tempodiff=fabs(tempocand-tempo);
	if (tempodiff>0.105) tempodiff=1; else tempodiff=0.0;
	
	float tempowincost= 0.0; //gamma*tempodiff;
	
	//phase cost of this phase replacing current phase of oscillator
	float phaseerror= fabs(calcphasediff(phase,phasecand,tempo,tempocand));
	
	float phasewincost = 0.0; //epsilon*phaseerror;
	
	float tempotransitioncost, phasetransitioncost, oldscorecost, newscorecost;
	
	//aim is to minimse cost, normed to 0-1 then inverted
	//could have *0.01);//fixed norm to avoid bias from calc to calc//
	newscorecost= 1.0-(unit->m_phasescores[i]*(unit->m_scorenorm)); //score normalised by best result //bestscores[j];
	
	//through 40 previous candidates to this one
	for(j=0;j<40;++j) {
		
		prevtempo=lastcandtempo[j/4];
		prevphase=lastcandphase[j];
		
		oldscorecost= dynleak*lastscore[j]; //could pre multiply to avoid redoing in each function call, one optimisation too much maybe
		
		//cost over changing hypotheses (no cost if consistent)
		tempodiff=fabs(prevtempo-tempocand);
		if(tempodiff>0.105) tempodiff=1; else tempodiff=0.0;
		
		tempotransitioncost=tempowt*tempodiff;
		
		//DrumTrack phase transition costs (my correction)
		phaseerror= fabs(calcphasediff(prevphase,phasecand,prevtempo,tempocand));
		
		phasetransitioncost=phasewt*3*phaseerror; //random 3!
		
		cost = oldscorecost+newscorecost+phasetransitioncost+tempotransitioncost+phasewincost+tempowincost+basscost+patterncost;
		
		//if(j==0)
		//printf(" oldscorecost %f newscorecost %f phasetransitioncost %f tempotransitioncost %f phasewincost %f tempowincost %f cost %f \n", oldscorecost,newscorecost,phasetransitioncost,tempotransitioncost,phasewincost,tempowincost,cost);
		
		//cost 
		if (cost<mincost)
			mincost=cost;
		
	}
 	
	//will store mincost in current score until finalisation
	//bestscores[j]=mincost;
	
	//will also be the min passed through as lastscore, works OK
	unit->m_phasescores[i]=mincost; 
	
	//save cost to unit->m_cost[j]=cost for unit->m_phasescores;
	
}

//find next beat times with respect to old phase, tempo and new phase, tempo at this point

//favours low tempi since phase representation is time independent! 
float calcphasediff(float prevphase, float phase, float prevtempo, float tempo) {
	
	float a,b,c,result;
	
	float phasediff= TIMEELAPSED*prevtempo- prevphase; //used to be timediff= TIMEELAPSED-(prevphase/prevtempo); phasediff=timediff*prevtempo
	
	//phase from old beat to next beat, see 12 OCT diagram
	float phasetobeat= floor(phasediff)+1.0-phasediff;
	
	//corrected for old tempo (currtempo) then rewritten in terms of the new tempowinner
	//float requiredphase= (phasetobeat*tempo)/prevtempo;
	
	float oldtimetobeat= phasetobeat/prevtempo; 
	float newtimetobeat= phase/tempo;	
	
	a=oldtimetobeat;
	b=newtimetobeat;
	
	//wlog
	if(b<a) {c=b; b=a; a=c;  c=1/prevtempo;} else {c= 1/tempo;}
	
	//closest between the two beat predictions
	result= sc_min(b-a, a-(b-c));
//	
//	printf("phase error %f \n",result);
//	
//if correct within 50mS
	//if (result<0.03) result=0.0;

	return result;//sc_min(b-a, a-(b-c));
}

/*
//favours low tempi since phase representation is time independent! 
float calcphasediff(float prevphase, float phase, float prevtempo, float tempo) {
	
	float a,b;
	
	float phasediff= TIMEELAPSED*prevtempo- prevphase; //used to be timediff= TIMEELAPSED-(prevphase/prevtempo); phasediff=timediff*prevtempo
	
	//phase from old beat to next beat, see 12 OCT diagram
	float phasetobeat= floor(phasediff)+1.0-phasediff;
	
	//corrected for old tempo (currtempo) then rewritten in terms of the new tempowinner
	float requiredphase= (phasetobeat*tempo)/prevtempo;
	
	a=requiredphase;
	b=phase;
	
	if(b<a) b=b+1.0;
	
	return sc_min(b-a, 1.0+a-b);
	
}
*/

//choose lowest score as winner-
//only swap if sufficiently unambiguous lead?
void finaldecision(DrumTrack *unit) {
	int i,j;
	
	float * lastscore= unit->m_lastscore;
	
	//maybe store float as cheaper on CPU?
	float * lastcandtempo= unit->m_lastcandidatetempo;
	float * lastcandphase= unit->m_lastcandidatephase;
	
	float * bestscores= unit->m_phasescores; //actually stored in this array, was bestscores before but bestscores used for holding 10 best tempi scores
	
	//or should have stored actual values?
	float * tempocand= unit->m_besttemposcores;
	float * phasecand=unit->m_bestphasescores;
	
	float score,min, secondmin;
	int minindex,secondminindex;
	
	minindex=0;
	min=10000000;
	secondmin=10000000;
	secondminindex=0;
	
	for (j=0;j<40;++j){
		
		score=bestscores[j];
		
		if(score<min) {
			min=score;
			minindex=j;
		} else if((score<secondmin) && (score>min)) {
			secondmin=score;
			secondminindex=j;
		}
		
		
	}
	
	//minindex is winner, update paths
	float phasewinner=phasecand[minindex];
	float tempowinner=tempocand[minindex/4];
	
	//int match= findarchetype(unit,tempowinner); //don't use tempowinner yet 
//	
//	if (match && (unit->m_patternscore>1.75)) {
//	
//	printf("phasewinner %f pattern phase %f tempowinner %f pattern tempo %f \n",phasewinner,unit->m_patternphase, tempowinner,unit->m_patterntempo);
//	
//	phasewinner= unit->m_patternphase; 
//	tempowinner= unit->m_patterntempo;
//	
//	}
	
	//perhaps only allow a tempochange if these two are sufficiently different
	//printf("best %f secondbest %f ratio %f \n",min,secondmin, min/secondmin);
	
	//check consistency of estimate
	
	//float tempodiff=fabs(tempocand-unit->m_prevtempo);
	//if (tempodiff<0.105) tempodiff=1; else tempodiff=0.0;
	//	
	
	//phase cost of last posited phase becoming new phase of oscillator
	float phaseerror= fabs(calcphasediff(unit->m_prevphase,phasewinner,unit->m_prevtempo,tempowinner));
	
	//phaseerror is in time, if less than 0.075 seconds? 
	//used to be 0.1
	//if ((phaseerror<0.1) && ((min/secondmin)<0.995))
	if (phaseerror<0.135) //0.14
		++unit->m_consistency;
	else
		unit->m_consistency=0.0;
	
	unit->m_prevphase=phasewinner;
	unit->m_prevtempo=tempowinner;
	
	
	//printf("phasewinner %f tempowinner %f \n",phasewinner, tempowinner);
	//printf("minimum score %f consistency %f phaseerror %f\n",min,unit->m_consistency,phaseerror);
	
	
	//could make the test value a parameter, or do -2 on score 
	//only swap when sufficient consistent, don't keep swapping if consistent! 
	//if( (unit->m_consistency>2.5) && (unit->m_consistency<3.5)) {
	if( (unit->m_consistency>1.5) && (unit->m_consistency<2.5)) {
	//if( (unit->m_consistency>0.5) && (unit->m_consistency<1.5)) {
	
		//phase has moved on by some time since the calculation frame
		////correction for recording happened during this! 
		//int framespassed= (unit->m_loudnesscounter+LOUDNESSFRAMESSTORED- unit->m_storeloudnesscounter)%LOUDNESSFRAMESSTORED;
		//float phasepassed= framespassed*FRAMEPERIOD*tempowinner;
		
		float timepassed= 0.2046259; //141*64/44100; //fixed by control period, amortisation and sampling rate
		float phasepassed= timepassed*tempowinner; 
		
		//don't update phase if sufficiently similar
		updatetempophase(unit, tempowinner,fmod(phasewinner+phasepassed,1.0)); //phasewinner
		
		if(unit->m_debugmode==1)
		printf("update phasewinner %f pattern phase %f tempowinner %f pattern tempo %f \n",phasewinner,unit->m_patternphase, tempowinner,unit->m_patterntempo);

	}
	
	int index;
	
	//update previous scores with these
	for(j=0;j<10;++j) {
		
		lastcandtempo[j]=tempocand[j]; //even spread
		
		for (i=0;i<4;++i) {
			index=4*j+i;
			lastcandphase[index]=phasecand[index]; //even spread
			lastscore[index]=bestscores[index];
		}
	}
	
}



//used twice, once for phase, once for antiphase?
float phasematchesbassonsets(DrumTrack *unit, float phase, float bps) {
	int i,j,baseframe, rounded, testframe;
	float Ti, starttime, beatpos, beat;
	//just testing at beat frame positions
	
	baseframe=unit->m_storeloudnesscounter+LOUDNESSFRAMESSTORED;
	
	Ti= 1/bps;   //could have precalculated these- OPTIMISE 
	
	////correction for recording happened during this! 
	//int framespassed= (unit->m_loudnesscounter+LOUDNESSFRAMESSTORED- unit->m_storeloudnesscounter)%LOUDNESSFRAMESSTORED;
	//		
	//printf("gone by %d \n",framespassed);		
	//		
	starttime= 0.0-(5*Ti);
	
	beatpos=starttime+(Ti*phase); 
	
	float beatsum=0;
	//int found;
	float avg;
	
	float * bassonsets= unit->m_bassonsets;
	
	//4 beats
	for (i=0;i<4;++i) {
		
		beat=(i*Ti)+beatpos;
		
		rounded= (int)floor(beat*FRAMESR+ .5f);
		
		//found=0;
		avg=0.0;
	
	//	//test within 3 either side
//		for(j=0;j<9;++j) {
//			testframe=(baseframe+rounded+j-4)%LOUDNESSFRAMESSTORED;
//			
//			//could do weighted sum, peak in middle...
//			avg=avg+bassonsets[testframe]; //if(bassonsets[testframe]==1) found=1;
//		}
//		
//		//testframe=(baseframe+rounded)%LOUDNESSFRAMESSTORED;
//		
//		beatsum+=avg*0.11111; //bassonsets[testframe];  
//		
			
		//test within 3 either side
		for(j=0;j<7;++j) {
			testframe=(baseframe+rounded+j-3)%LOUDNESSFRAMESSTORED;
			
			//could do weighted sum, peak in middle...
			avg=avg+bassonsets[testframe]; //if(bassonsets[testframe]==1) found=1;
		}
		
		//testframe=(baseframe+rounded)%LOUDNESSFRAMESSTORED;
		
		beatsum+=avg*0.1428571; //bassonsets[testframe];  
			
											                
	}
	
	return beatsum;
}




//only updates if sufficiently different
void updatetempophase(DrumTrack *unit, float newtempo,float newphase) {
	
	float phase,phasenow,a,b,ahead;
	
	phase=unit->m_phase; 
	
	//test phase difference of new estimate from current phase of oscillator
	
	a=1.0-newphase;
	b=phase;
	
	if(b<a) b=b+1.0;
	
	b=sc_min(b-a, 1.0+a-b); //value from 0.0 to 0.5
	
	//only update if greater than 50mS phase difference (adjusted for tempo)	
	
	//if (b>(0.05*newtempo)) {
	//if (b>(0.025*newtempo)) { //improved CDR by 5%
	if (b>(0.01*newtempo)) { //by 10% now, but 0.0 doesn't help
		//pass in [] if no update
		//if(newphase>=(-0.1))
		phasenow=1.0-newphase;
		//printf("phasechange %f\n",phasenow);
		//printf("check b %f\n",b);
		
	}
	else
		phasenow=phase;
	
	a=phase;
	b=phasenow;
	
	if(b<a) b=1.0+b;
	
	//phasenow up to 1.0 ahead, but by how much? 
	ahead= b-a;
	
	if(ahead>0.5) //phasenow behind phase- drop phase back, ahead will be negative
		ahead= ahead-1.0;
	
	
	//both cases now adjust by ahead
	//correction by up to a quarter cycle? No, correct by a maximum of x seconds, tempo dependent
	
	//actually, correct by full amount as long as there is consistent evidence
	//0.2 seconds in beats at current tempo
	//maxcorrection=1.0; //min(0.1*temponow,1.0); %correct by no more than a beat 
	//ahead=sign(ahead)*(min(abs(ahead),maxcorrection));
	
	//can't correct back earlier than current phase else may beat twice!
	//must temporarily drop the rate
	//phase=mod(max(phase+ahead,floor(phase)),1.0);
	
	//no, just store last beat time and require greater than...
	unit->m_phase=fmod(phase+ahead+1.0,1.0);
	
	unit->m_currtempo=newtempo;
	
	unit->m_phaseperblock= (unit->mWorld->mFullRate.mBufLength*newtempo)/(FS);
	
}




//also calc kick drum detector on bottom three bands
void calculateloudness(DrumTrack *unit) {
	
	int j,k;
	
	float * fftbuf= unit->m_FFTBuf;
	
	float dfsum=0.0; //,lsum=0.0;
	float bassonset=0;
	float basssum=0.0;
	
		//store powers for later chord detection step 
//	float * fftstore= unit->m_fftstore;
//	
//	unit->m_fftstorepos=(unit->m_fftstorepos+1)%FFTSTOREMEM;
//	
//	int basepos= 24*unit->m_fftstorepos;
//	for(j=0;j<24;++j) {
//		fftstore[basepos+j]=fftbuf[j+1];
//	}	
//	
	

	
	for (k=0; k<40; ++k){
		
		int bandstart=eqlbandbins[k];
		//int bandend=eqlbandbins[k+1];
		int bandsize= eqlbandsizes[k];
		
		float bsum=0.0;
		
		for (int h=0; h<bandsize;++h) {
			bsum= bsum+fftbuf[h+bandstart];
		}
		
		//store recips of bandsizes?
		bsum= bsum/bandsize;
		
		
			float diff=bsum; 
		dfsum=dfsum+diff; 
				if (k<5) basssum=basssum+diff; 
		
	}
	
		
	//increment first so this frame is unit->loudnesscounter
	unit->m_loudnesscounter=(unit->m_loudnesscounter+1)%LOUDNESSFRAMESSTORED;
	
		unit->m_loudness[unit->m_loudnesscounter]=dfsum; //dfsum*0.025; //divide by num of bands to get a dB answer
													 //printf("loudness %f %f \n",unit->loudness[unit->loudnesscounter], lsum);
		
bassonset= basssum; //90+(10*log10(basssum));

unit->m_bassonsets[unit->m_loudnesscounter]=bassonset; 

}


//snare limits= [34,177] (may try to reduce summands for CPU saving)
void snaredetection(DrumTrack *unit){
	
	float snaresig,highfreqave,lowfreqave,quantsum, power,temp;
	int count,num;
	
	//will also use these for indexing 
	int now=unit->m_powerbufcount;
	int last, lastlast;
	last=(2+now)%3; //avoid negative numbers in modulo- get back negatives! 
	lastlast=(1+now)%3;
	
	float * nowbuf,*lastbuf,*lastlastbuf;
	nowbuf=unit->m_powerbuf[now];
	lastbuf=unit->m_powerbuf[last];
	lastlastbuf=unit->m_powerbuf[lastlast];
	
    num=0;
    quantsum=0.0;
    count=0;
    
	snaresig=1.0;
	
    for(int f=33; f<177;++f) {
        
        power=lastbuf[f];
        
        highfreqave = 0.25*(lastbuf[f+2]+lastlastbuf[f+1]+lastbuf[f+1]+nowbuf[f+1]);  
        lowfreqave = 0.25*(lastbuf[f-2]+lastlastbuf[f-1]+lastbuf[f-1]+nowbuf[f-1]); 
        
		temp=sc_min(highfreqave,lowfreqave);
		
		//0.25
        if(temp > (0.5*power)) {
            quantsum=quantsum+power;
            num=num+1;
        }
        
		//printf("minave %f power %f quantsum %f \n",temp,power,quantsum);
		
        count=count+1;
        
        if(count==9) {
			//0.125*(quantsum*0.11111111)
			temp=0.0138889*quantsum;
			temp=sc_min(temp,0.54);
            snaresig*=(1+temp);
            quantsum=0.0;
            count=0;
		}
        
    }
    
    //snaresig(1,t)= prod(noisebands); %mean(noisebands); %num
	
	//printf("snaresig %f \n",snaresig);
	
	//can't allow max to get too high else detections suddenly cut out- 100 seems a fair value
	
	//if(snaresig>(unit->m_maxsnaresig)) {
//		//unit->m_maxsnaresig=snaresig;
//		
//		printf("max found! %f \n",snaresig); //unit->m_maxsnaresig);
//	}
	
	//max vals up to 1.54**16 ie about 1000
	
	snaresig=snaresig/(1000*unit->m_maxsnaresig);
	
	//printf("postsnaresig %f \n",snaresig);
	
	//should peak pick really!
	
	//snare detection- don't allow another for a while? m_lastsnaredetect
	if(snaresig>0.5) { //used to be 0.5, can add a user defined threshold
		
		//stop in a row detections
		if(unit->m_lastsnaredetect<(unit->m_frame-6)) {
			
			if(unit->m_debugmode==2)
			printf("snare found! %ld %ld \n", unit->m_lastsnaredetect, unit->m_frame);
			
			//occured one frame ago, but do -2 to avoid kick overwrite on next cycle!
			int onsetframe= (unit->m_loudnesscounter+LOUDNESSFRAMESSTORED-2)%LOUDNESSFRAMESSTORED;
			
			unit->m_onsets[onsetframe]=2; //(-1);  //snare
		}
		
		unit->m_lastsnaredetect=unit->m_frame;
		
	}
	
}



//uses Goto onset detection to with threshold to spot a kick
void kickdetection(DrumTrack *unit){
	
	float dsum;
	float kicksig;
	
	//will also use these for indexing 
	int now=unit->m_powerbufcount;
	int last, lastlast;
	last=(2+now)%3; //avoid negative numbers in modulo- get back negatives! 
	lastlast=(1+now)%3;
	
	float * nowbuf,*lastbuf,*lastlastbuf;
	nowbuf=unit->m_powerbuf[now];
	lastbuf=unit->m_powerbuf[last];
	lastlastbuf=unit->m_powerbuf[lastlast];
	
	dsum=0.0;
	
	for(int f=1; f<=3; ++f) {
		
		float updownmax= sc_max(lastlastbuf[f-1],lastlastbuf[f+1]);
		float prevpow= sc_max(lastlastbuf[f],updownmax);
		
		float tempmin=sc_min(lastbuf[f],nowbuf[f]);
		
		if(tempmin>prevpow) {
			
			float temp=sc_max(lastbuf[f],nowbuf[f]);
			dsum=dsum+temp;    
		}
	}			
	
	
	kicksig=dsum/3;
	//running max must be kept- just will do best so far for now...could become best within last x seconds
	//updatemaxforband
	
	
//	if(kicksig>(unit->m_maxkicksig)) {
//		unit->m_maxkicksig=kicksig;
//		
//		printf("max found! %f \n",unit->m_maxkicksig);
//	}
	
	kicksig=kicksig/(unit->m_maxkicksig);
	
	//kick detection- don't allow another for a while? m_lastsnaredetect
	if(kicksig>0.6) { //used to be 0.5, can add a user defined threshold
		
		//stop in a row detections
		if(unit->m_lastkickdetect<(unit->m_frame-6)) {
			
			if(unit->m_debugmode==2)
			printf("kick found! %ld %ld \n", unit->m_lastkickdetect, unit->m_frame);
			
			
			//occured one frame ago, but do -2 to avoid kick overwrite on next cycle!
			int onsetframe= (unit->m_loudnesscounter+LOUDNESSFRAMESSTORED-2)%LOUDNESSFRAMESSTORED;
			
			unit->m_onsets[onsetframe]=1;  //kick- might do 1 kick, -1 snare
		}
		
		unit->m_lastkickdetect=unit->m_frame;
		
	}
	
}



//one matching the tempowinner will score most (can over write even if an equal score)

//impose tempowinner t choose selection spread? o/w could be missing second beat but have third and fourth

//look for archetypal pattern in m_onsets
//phase not important, want to just look at a fixed area in the past
int findarchetype(DrumTrack *unit, float tempowinner) {
	int i,j,k,baseframe;
	float starttime, lasttime;
	int startframe, lastframe, nextonset; 
	//just testing at beat frame positions
	
	baseframe=unit->m_storeloudnesscounter+LOUDNESSFRAMESSTORED;
	int * onsets= unit->m_onsets;
	
	//don't know periods, just going to find from search
	starttime= 0.0-(5*MAXPERIOD); //0.0-(5*0.667); //one more eighth note than usual to calc pre beat
	
	startframe=((int)(starttime*FRAMESR+ .5f) + baseframe)%LOUDNESSFRAMESSTORED; 
	
	//must have at least room for a fast 0.333*4 bar else no point checking
	lasttime= -4*MINPERIOD; //-4*0.33; 
	
	//this is last start frame, last end frame is baseframe
	lastframe= ((int)(lasttime*FRAMESR+ .5f) + baseframe)%LOUDNESSFRAMESSTORED; 
	
	nextonset=(startframe+LOUDNESSFRAMESSTORED-1)%LOUDNESSFRAMESSTORED;
	
	//already made a list of onsets? 
	int numtolast=0;
	
	if (lastframe<startframe) 
		numtolast= lastframe+LOUDNESSFRAMESSTORED-startframe;
	else 
		numtolast= lastframe-startframe;
	
	int numtonow=0;
	
	if (baseframe<startframe) 
		numtonow= baseframe+LOUDNESSFRAMESSTORED-startframe;
	else 
		numtonow= baseframe-startframe;
	
	int pos;
	
	int numonsets=0;
	int numstarts=0;
	
	
	//store them in an array (preallocate 50, maximum size)
	//permanent array for this
	//int onsetposition[50];
	int * onsetposition= unit->m_onsetposition;
	
	for (i=0;i<numtonow;++i) {
		
		pos= (startframe+i+LOUDNESSFRAMESSTORED)%LOUDNESSFRAMESSTORED;
		
		if(onsets[pos]!=0) {
			if(i<numtolast) ++numstarts;
			
			onsetposition[numonsets]=pos;
			
			  ++numonsets;
			
			if (numonsets==MAXONSETS) {printf("disaster? %d \n",numonsets); break;}
		}
	}
	
	//if (numstarts>MAXONSETS) numstarts= MAXONSETS;
	//printf("numonsets %d numstarts %d \n", numonsets,numstarts);
		
	
	int bestindex=-1;
	float bestscore=0.0;
	int besttempo=0;
	
	//float score;
	//int tempo;
	

	
	for (i=0; i<numstarts; ++i) {
		
		int first=onsetposition[i];
		int firstval= onsets[first];
		
		for (j=i+1;j<numonsets; ++j) {
			int second=onsetposition[j];
			
			int diff, realdiff;
			
			diff= (second+LOUDNESSFRAMESSTORED- first)%LOUDNESSFRAMESSTORED; //works for both cases second<first and first<=second
			int test= (baseframe+LOUDNESSFRAMESSTORED- first)%LOUDNESSFRAMESSTORED; 
			
			//second IS second beat
			if(test>(3*diff)) {
			
			testpattern(unit,bestindex,bestscore,besttempo,first,diff,firstval);
	
			}
			
			////second is actually third beat
		
			if(test>((3/2)*diff)) {
			
			realdiff= (diff/2);
			
			testpattern(unit,bestindex,bestscore,besttempo,first,realdiff,firstval);
	
			}
			
			//second is fourth, already all beats will fit within
			realdiff= (diff/3);
			
			testpattern(unit,bestindex,bestscore,besttempo,first,realdiff,firstval);
			
		}
		
	}  
	
	//tempo= FRAMESR/(float)tempo;
	
	float actualtempo;
	
	if (besttempo>0) {
		actualtempo = FRAMESR/(float)besttempo;
		
		int dist= (baseframe-bestindex+LOUDNESSFRAMESSTORED)%LOUDNESSFRAMESSTORED;
		
		float phase= (dist%besttempo)/(float)besttempo;
		
		unit->m_patternphase= 1.0-phase;  
		unit->m_patternscore=bestscore;
		unit->m_patterntempo=actualtempo;
		
		if(unit->m_debugmode==1)
		printf("best match phase %f score %f tempo %f \n", unit->m_patternphase, unit->m_patternscore, unit->m_patterntempo);
		
		return 1;
	}
	
	return 0; //no matches found
}


void testpattern(DrumTrack * unit, int &bestindex, float &bestscore, int &besttempo, int first, int diff, int firstval) {
	
	int k;
	float score;
	
	//int * onsetposition= unit->m_onsetposition;	
	int * onsets= unit->m_onsets;
	
	int second= (first+diff)%LOUDNESSFRAMESSTORED;
	
	//first case- second is the second beat of the pattern
	//min one beat 0.33*FRAMESR max three beats 0.66*FRAMESR  86.1328
	int mindiff= MINDIFF; //28; //allowing for some error could be 25, usually 28
	int maxdiff= MAXDIFF; //58;
	
	//int test= (baseframe+LOUDNESSFRAMESSTORED- first)%LOUDNESSFRAMESSTORED; 
	
	//can exclude more cases knowing diff- 3*diff can't be greater than LOUDNESSFRAMESSTORED
	//if(3*diff<LOUDNESSFRAMESSTORED) {
	
	//spacing must be plausible
	if(diff>=mindiff && diff<=maxdiff) {
		
		int tempo= diff; //will convert later FRAMESR/(float)diff;
		int secondval= onsets[second];
		
		//spacing is diff for testing
		
		int third=(second+diff)%LOUDNESSFRAMESSTORED;
		int fourth=(third+diff)%LOUDNESSFRAMESSTORED;
		
		//two tests for each case, each case determines a unique position of bestindex
		
		int thirdval=0;
		int fourthval=0;
		
		int posindex,negindex;
		
		//allowing for an error of up to 5 either side, successively overwrites with best option if non zero
		for (k=4;k>=0;--k) {
			
			negindex=(third-k+LOUDNESSFRAMESSTORED)%LOUDNESSFRAMESSTORED;
			
			if(onsets[negindex])
				thirdval=onsets[negindex];
			
			posindex=(third+k)%LOUDNESSFRAMESSTORED;
			
			if(onsets[posindex])
				thirdval=onsets[posindex];
			
		}
		
		for (k=4;k>=0;--k) {
			
			negindex=(fourth-k+LOUDNESSFRAMESSTORED)%LOUDNESSFRAMESSTORED;
			
			if(onsets[negindex])
				fourthval=onsets[negindex];
			
			posindex=(fourth+k)%LOUDNESSFRAMESSTORED;
			
			if(onsets[posindex])
				fourthval=onsets[posindex];
			
		}
		
		
		if (firstval==2) //snare
		{
			
			//two possibilities
			
			score = 1.0;
			
			if(secondval==1) score+=0.5;
			if(thirdval==2) score+=1.0;
			if(fourthval==1) score+=1.0;
			
			if(score>bestscore) {
				
				bestindex=fourth; //first beat of bar! 
				bestscore=score;
				besttempo=tempo;
			}
			
			score = 1.0;
			
			if(secondval==1) score+=1.0;
			if(thirdval==2) score+=1.0;
			if(fourthval==1) score+=0.5;
			
			if(score>bestscore) {
				
				bestindex=second; //first beat of bar! 
				bestscore=score;
				besttempo=tempo;
			}
			
			
		} else { //kick
			
			score = 1.0;
			
			if(secondval==2) score+=1.0;
			if(thirdval==1) score+=0.5;
			if(fourthval==2) score+=1.0;
			
			if(score>bestscore) {
				
				bestindex=first; //first beat of bar! 
				bestscore=score;
				besttempo=tempo;
			}
			
			
			score = 0.5;
			
			if(secondval==2) score+=1.0;
			if(thirdval==1) score+=1.0;
			if(fourthval==2) score+=1.0;
			
			if(score>bestscore) {
				
				bestindex=third; //first beat of bar! 
				bestscore=score;
				besttempo=tempo;
			}
			
			
		}
		
		
		
		
		
	}
	
	
	
}











void prepareHanningWindow(){
	float ang;
	
	ang=(1.0/N)*TWOPI;
	
	for(int i=0;i<N;++i)
		hanning[i]=0.5 - 0.5*cos(ang*i);
	
}


PluginLoad(DrumTrack) {
	
	ft= inTable;
	
	prepareHanningWindow();
	
	DefineDtorCantAliasUnit(DrumTrack);
}



/*


//used twice, once for phase, once for antiphase?
float comparechordetect(DrumTrack *unit, float phase, float bps) {
	int i,j,k,baseframe, rounded, testframe;
	float Ti, starttime, beatpos, beat;
	//just testing at beat frame positions
	
	baseframe=unit->m_fftstoreposhold+FFTSTOREMEM;
	
	Ti= 1/bps;   //could have precalculated these- OPTIMISE 
	
	int framesize= (int)floor(0.5*Ti/FRAMEPERIOD); //eighth note area
	
	//%as goto ignore outer 1/5s, only take middle 3/5 
	int keep= (int)floor((0.6*framesize)+ .5f);
	int onefifth= (int)floor((0.2*framesize)+ .5f);
	
	starttime= 0.0-(5.5*Ti); //one more eighth note than usual to calc pre beat
	
	beatpos=starttime+(Ti*phase); 
	
	float * fftstore= unit->m_fftstore;
	
	//should allocate for each 
	float *chordhistogram= unit->m_chordhistogram; 
	float *chordhistogramprev= unit->m_chordhistogramprev;
	float onsum=0.0;
	float offsum=0.0;
	
	//8 beats
	for (i=0;i<9;++i) {
		
		beat=(i*(Ti*0.5))+beatpos;
		
		
		//ERROR if beat negative? Should be -.5f in rounding! No, floor goes moe negative
		rounded= (int)floor(beat*FRAMESR+ .5f);
		
		//printf("error in rounding? beat %f rounded %d correction %d \n",beat, rounded,  (int)(beat*FRAMESR -.5f));
		
		testframe=baseframe+rounded+onefifth;
		
		//calc new histogram
		for (j=0;j<24;++j) {
			
			float sum=0.0;
			
			for(k=0;k<keep;++k) { 
				
				sum+= fftstore[((testframe+k)%FFTSTOREMEM)*24+j];
			}
			
			
			chordhistogram[j]=sum;
		}
		
		//peak pick
		//for (j=1;j<23;++j) {
		//
		//if((chordhistogram[j-1]>chordhistogram[j]) || (chordhistogram[j]<chordhistogram[j+1]))
		//chordhistogram[j]=0;
		//
		//}
		
		//zero 0 and 23
		chordhistogram[0]=0;
		chordhistogram[23]=0;
		
		//do difference to prev, sum of positives only
		if(i>0) {
			
			for (j=0;j<24;++j) {	
				
				float diff; 
				
				diff=chordhistogram[j]-chordhistogramprev[j];
				
				//printf("%f \n",diff);
				
				if(diff<0) diff=0;	
				
				if(i%2==1)
					onsum+=diff; //bassonsets[testframe];                
				else
					offsum+=diff;
			}
			
		}
		
		//copy new into previous
		for (j=1;j<23;++j) 
			chordhistogramprev[j]=chordhistogram[j];
		
		
	}


if (offsum<1) offsum=1;

//printf("onsum %f offsum %f ratio %f\n",onsum, offsum, onsum/offsum);
//

return onsum/offsum;
}


*/

