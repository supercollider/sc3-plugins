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

//This file is part of MachineListening. Copyright (C) 2006  Nicholas M.Collins distributed under the terms of the GNU General Public License full notice in file MachineListening.license

//AutoTrack UGen implemented by Nick Collins (http://www.cus.cam.ac.uk/~nc272/ and http://www.informatics.sussex.ac.uk/users/nc81/)
//fftw version 13 Dec 2006

//conversion of Matthew Davies autocorrelation beat tracking model, adapted for real-time use
//my detection function frontend to begin with, currently using QMUL complex domain onset detection function model

//26/11/05 updated complexdf detection function 

//#include "SC_PlugIn.h"
//#include <vecLib/vecLib.h>
//#include <string.h>
//#include <math.h>
//#include <stdlib.h>
//#include <stdio.h>

#include "MLfftwUGens.h"


//for hanning window, use global hanning1024

//int eqlbandbins[43]= {1,2,3,4,5,6,7,8,9,11,13,15,17,19,22,25,28,32,36,41,46,52,58,65,73,82,92,103,116,129,144,161,180,201,225,251,280,312,348,388,433,483,513}; 
//int eqlbandsizes[42]= {1,1,1,1,1,1,1,1,2,2,2,2,2,3,3,3,4,4,5,5,6,6,7,8,9,10,11,13,13,15,17,19,21,24,26,29,32,36,40,45,50,30}; 

//this loudness conversion was removed since it worked better without transient detection but just using current energy
//float contours[42][11]= {{ 47.88, 59.68, 68.55, 75.48, 81.71, 87.54, 93.24, 98.84,104.44,109.94,115.31},{ 29.04, 41.78, 51.98, 60.18, 67.51, 74.54, 81.34, 87.97, 94.61,101.21,107.74},{ 20.72, 32.83, 43.44, 52.18, 60.24, 67.89, 75.34, 82.70, 89.97, 97.23,104.49},{ 15.87, 27.14, 37.84, 46.94, 55.44, 63.57, 71.51, 79.34, 87.14, 94.97,102.37},{ 12.64, 23.24, 33.91, 43.27, 52.07, 60.57, 68.87, 77.10, 85.24, 93.44,100.90},{ 10.31, 20.43, 31.03, 40.54, 49.59, 58.33, 66.89, 75.43, 83.89, 92.34,100.80},{  8.51, 18.23, 28.83, 38.41, 47.65, 56.59, 65.42, 74.16, 82.89, 91.61,100.33},{  7.14, 16.55, 27.11, 36.79, 46.16, 55.27, 64.29, 73.24, 82.15, 91.06, 99.97},{  5.52, 14.58, 25.07, 34.88, 44.40, 53.73, 62.95, 72.18, 81.31, 90.44, 99.57},{  3.98, 12.69, 23.10, 32.99, 42.69, 52.27, 61.66, 71.15, 80.54, 89.93, 99.31},{  2.99, 11.43, 21.76, 31.73, 41.49, 51.22, 60.88, 70.51, 80.11, 89.70, 99.30},{  2.35, 10.58, 20.83, 30.86, 40.68, 50.51, 60.33, 70.08, 79.83, 89.58, 99.32},{  2.05, 10.12, 20.27, 30.35, 40.22, 50.10, 59.97, 69.82, 79.67, 89.52, 99.38},{  2.00,  9.93, 20.00, 30.07, 40.00, 49.93, 59.87, 69.80, 79.73, 89.67, 99.60},{  2.19, 10.00, 20.00, 30.00, 40.00, 50.00, 59.99, 69.99, 79.98, 89.98, 99.97},{  2.71, 10.56, 20.61, 30.71, 40.76, 50.81, 60.86, 70.96, 81.01, 91.06,101.17},{  3.11, 11.05, 21.19, 31.41, 41.53, 51.64, 61.75, 71.95, 82.05, 92.15,102.33},{  2.39, 10.69, 21.14, 31.52, 41.73, 51.95, 62.11, 72.31, 82.46, 92.56,102.59},{  1.50, 10.11, 20.82, 31.32, 41.62, 51.92, 62.12, 72.32, 82.52, 92.63,102.56},{ -0.17,  8.50, 19.27, 29.77, 40.07, 50.37, 60.57, 70.77, 80.97, 91.13,101.23},{ -1.80,  6.96, 17.77, 28.29, 38.61, 48.91, 59.13, 69.33, 79.53, 89.71, 99.86},{ -3.42,  5.49, 16.36, 26.94, 37.31, 47.61, 57.88, 68.08, 78.28, 88.41, 98.39},{ -4.73,  4.38, 15.34, 25.99, 36.39, 46.71, 57.01, 67.21, 77.41, 87.51, 97.41},{ -5.73,  3.63, 14.74, 25.48, 35.88, 46.26, 56.56, 66.76, 76.96, 87.06, 96.96},{ -6.24,  3.33, 14.59, 25.39, 35.84, 46.22, 56.52, 66.72, 76.92, 87.04, 97.00},{ -6.09,  3.62, 15.03, 25.83, 36.37, 46.70, 57.00, 67.20, 77.40, 87.57, 97.68},{ -5.32,  4.44, 15.90, 26.70, 37.28, 47.60, 57.90, 68.10, 78.30, 88.52, 98.78},{ -3.49,  6.17, 17.52, 28.32, 38.85, 49.22, 59.52, 69.72, 79.92, 90.20,100.61},{ -0.81,  8.58, 19.73, 30.44, 40.90, 51.24, 61.52, 71.69, 81.87, 92.15,102.63},{  2.91, 11.82, 22.64, 33.17, 43.53, 53.73, 63.96, 74.09, 84.22, 94.45,104.89},{  6.68, 15.19, 25.71, 36.03, 46.25, 56.31, 66.45, 76.49, 86.54, 96.72,107.15},{ 10.43, 18.65, 28.94, 39.02, 49.01, 58.98, 68.93, 78.78, 88.69, 98.83,109.36},{ 13.56, 21.65, 31.78, 41.68, 51.45, 61.31, 71.07, 80.73, 90.48,100.51,111.01},{ 14.36, 22.91, 33.19, 43.09, 52.71, 62.37, 71.92, 81.38, 90.88,100.56,110.56},{ 15.06, 23.90, 34.23, 44.05, 53.48, 62.90, 72.21, 81.43, 90.65, 99.93,109.34},{ 15.36, 23.90, 33.89, 43.31, 52.40, 61.42, 70.29, 79.18, 88.00, 96.69,105.17},{ 15.60, 23.90, 33.60, 42.70, 51.50, 60.20, 68.70, 77.30, 85.80, 94.00,101.70},{ 15.60, 23.90, 33.60, 42.70, 51.50, 60.20, 68.70, 77.30, 85.80, 94.00,101.70},{ 15.60, 23.90, 33.60, 42.70, 51.50, 60.20, 68.70, 77.30, 85.80, 94.00,101.70},{ 15.60, 23.90, 33.60, 42.70, 51.50, 60.20, 68.70, 77.30, 85.80, 94.00,101.70},{ 15.60, 23.90, 33.60, 42.70, 51.50, 60.20, 68.70, 77.30, 85.80, 94.00,101.70},{ 15.60, 23.90, 33.60, 42.70, 51.50, 60.20, 68.70, 77.30, 85.80, 94.00,101.70}};
//double phons[11]={2,10,20,30,40,50,60,70,80,90,100};

//this data assumes LAGS is 128
float g_m[128]= {0.00054069,0.00108050,0.00161855,0.00215399,0.00268594,0.00321356,0.00373600,0.00425243,0.00476204,0.00526404,0.00575765,0.00624213,0.00671675,0.00718080,0.00763362,0.00807455,0.00850299,0.00891836,0.00932010,0.00970771,0.01008071,0.01043866,0.01078115,0.01110782,0.01141834,0.01171242,0.01198982,0.01225033,0.01249378,0.01272003,0.01292899,0.01312061,0.01329488,0.01345182,0.01359148,0.01371396,0.01381939,0.01390794,0.01397980,0.01403520,0.01407439,0.01409768,0.01410536,0.01409780,0.01407534,0.01403838,0.01398734,0.01392264,0.01384474,0.01375410,0.01365120,0.01353654,0.01341062,0.01327397,0.01312710,0.01297054,0.01280484,0.01263053,0.01244816,0.01225827,0.01206139,0.01185807,0.01164884,0.01143424,0.01121478,0.01099099,0.01076337,0.01053241,0.01029861,0.01006244,0.00982437,0.00958484,0.00934429,0.00910314,0.00886181,0.00862067,0.00838011,0.00814049,0.00790214,0.00766540,0.00743057,0.00719793,0.00696778,0.00674036,0.00651591,0.00629466,0.00607682,0.00586256,0.00565208,0.00544551,0.00524301,0.00504470,0.00485070,0.00466109,0.00447597,0.00429540,0.00411944,0.00394813,0.00378151,0.00361959,0.00346238,0.00330989,0.00316210,0.00301899,0.00288053,0.00274669,0.00261741,0.00249266,0.00237236,0.00225646,0.00214488,0.00203755,0.00193440,0.00183532,0.00174025,0.00164909,0.00156174,0.00147811,0.00139810,0.00132161,0.00124854,0.00117880,0.00111228,0.00104887,0.00098848,0.00093100,0.00087634,0.00082438,}; 
float g_mg[257]= {0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000004,0.00000055,0.00000627,0.00005539,0.00037863,0.00200318,0.00820201,0.02599027,0.06373712,0.12096648,0.17767593,0.20196826,0.17767593,0.12096648,0.06373712,0.02599027,0.00820201,0.00200318,0.00037863,0.00005539,0.00000627,0.00000055,0.00000004,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,}; 




//other functions
void AutoTrack_preparefft(AutoTrack *unit, float* in, int n);
void AutoTrack_dofft(AutoTrack *unit);

//used to be calculatedf here too, code now excised
void complexdf(AutoTrack *unit);

void finaldecision(AutoTrack *unit);

//amortisation
void autocorr(AutoTrack *unit,int j);
void beatperiod(AutoTrack *unit,int j, int whichm);
float findtor(AutoTrack *unit);

//as many amortisation steps as tor
void findphase(AutoTrack *unit,int j,int gaussflag,int predicted);

int detectperiodchange(AutoTrack *unit);
void findmeter(AutoTrack *unit); 
void setupphaseexpectation(AutoTrack *unit); //create Gaussian focussed matrix for phase 


void AutoTrack_Ctor(AutoTrack* unit) {
	int j;
	
	////////FFT data///////////
	
	unit->m_prepareFFTBuf = (float*)RTAlloc(unit->mWorld, N * sizeof(float));
	unit->m_FFTBuf = (float*)RTAlloc(unit->mWorld, N * sizeof(float));
	
	//N=1024
	unit->planTime2FFT = fftwf_plan_r2r_1d(N, unit->m_FFTBuf, unit->m_FFTBuf, FFTW_R2HC, FFTW_ESTIMATE);
	
	
	unit->m_prevmag= (float*)RTAlloc(unit->mWorld, NOVER2 * sizeof(float));
	unit->m_prevphase= (float*)RTAlloc(unit->mWorld, NOVER2 * sizeof(float));
	unit->m_predict= (float*)RTAlloc(unit->mWorld, NOVER2 * sizeof(float));
	
	
	unit->m_bufWritePos = 0;	
	
	////////vDSP///////////////
	
//	unit->m_vA.realp = (float*)RTAlloc(unit->mWorld, NOVER2 * sizeof(float)); 
//	unit->m_vA.imagp = (float*)RTAlloc(unit->mWorld, NOVER2 * sizeof(float));
//	unit->m_vlog2n = 10; //N is hard coded as 1024, so 10^2=1024 //log2max(N);
//	unit->m_vsetup = create_fftsetup(unit->m_vlog2n, 0);
//	
	//allocate memory for the Gaussian weighted comb filter matrix adaptive to period  
	//unit->m_Mg = (float*)RTAlloc(unit->mWorld, DFFRAMELENGTH*SKIP * sizeof(float)); 
	
	////////time positions//////////
	unit->m_frame=1; //don't decide immediately, wait for maximum period! 
	
	/////////df////////
	unit->m_dfcounter=DFSTORE-1;
	//random uncorrelated noise df store for initialisation
	//RGen& rgen = *unit->mParent->mRGen;
    
	//don't want this noise, want consistent starting point! 
	for(j=0;j<DFSTORE;++j) {
		unit->m_df[j]=0.0; //(2*rgen.frand() - 1.0);
	}
	
	
	unit->m_dfmemorycounter=14;
	
	for(j=0;j<15;++j) {
		unit->m_dfmemory[j]=0.0; 
	}

	/////////tempo assess///////////
	unit->m_currtempo=2;
	
	////////phase assess///////////
	
	unit->m_currphase=0.0;
	
	unit->m_phase=0.0;
	
	//default of 2bps
	unit->m_phaseperblock= ((float)unit->mWorld->mFullRate.mBufLength*2)/((float)unit->mWorld->mSampleRate);
	
	unit->m_outputphase= unit->m_phase;
	unit->m_outputtempo= unit->m_currtempo;
	unit->m_outputphaseperblock= unit->m_phaseperblock;
	
	unit->mCalcFunc = (UnitCalcFunc)&AutoTrack_next;
	
	unit->halftrig=0;
    unit->q1trig=0;
	unit->q2trig=0;
	
	//amortisation and states
	unit->m_amortisationstate=0; //off
	unit->m_stateflag=0;
	unit->m_timesig=4;
	unit->m_flagstep=0;
	
}



void AutoTrack_Dtor(AutoTrack *unit)
{
	
	RTFree(unit->mWorld, unit->m_prepareFFTBuf);
	RTFree(unit->mWorld, unit->m_FFTBuf);
	
	RTFree(unit->mWorld, unit->m_prevmag);
	RTFree(unit->mWorld, unit->m_prevphase);
	RTFree(unit->mWorld, unit->m_predict);
	
	//if (unit->m_vA.realp) RTFree(unit->mWorld, unit->m_vA.realp);
	//if (unit->m_vA.imagp) RTFree(unit->mWorld, unit->m_vA.imagp);
	//if (unit->m_vsetup) destroy_fftsetup(unit->m_vsetup);
	
	 fftwf_destroy_plan(unit->planTime2FFT);
	
}


void AutoTrack_next(AutoTrack *unit, int wrongNumSamples)
{
	//would normally be float,will be cast to int for Tristan's optimisation
	float *in = IN(0);
	
	//printf("%d \n",wrongNumSamples);
	
	int numSamples = unit->mWorld->mFullRate.mBufLength;
	
	//conditions in reverse order to avoid immediate spillover
	
	//printf("state %d \n",unit->m_amortisationstate);	
	
	//keeps incrementing but will be reset with each calculation run
	unit->m_amortisationsteps=unit->m_amortisationsteps+1;
	
	//if state nonzero do something
	switch(unit->m_amortisationstate) {
		case 0:
			break; //do nothing case
		case 1: //calculate acf
			autocorr(unit,unit->m_amortcount);
			
			unit->m_amortcount=unit->m_amortcount+1;
			
			if(unit->m_amortcount==unit->m_amortlength) {
				unit->m_amortisationstate=2;
				unit->m_amortlength=128;
				unit->m_amortcount=0;
				
				unit->m_bestcolumn=0;
				unit->m_besttorsum=(-1000.0);
				
			}
				
				break;
		case 2: //periodp
			beatperiod(unit,unit->m_amortcount,0);
			
			unit->m_amortcount=unit->m_amortcount+1;
			
			if(unit->m_amortcount==unit->m_amortlength) {
				
				unit->m_periodp=findtor(unit);
				
				if(unit->m_stateflag==1) {
					unit->m_amortisationstate=3;
					unit->m_amortlength=128;
					unit->m_amortcount=0;
					
					unit->m_bestcolumn=0;
					unit->m_besttorsum=(-1000.0);
					
				} else {
					unit->m_periodg= -1000; //will always trigger initially
					unit->m_amortisationstate=4;
				}
			}
				
				break;
		case 3: //periodg
			beatperiod(unit,unit->m_amortcount,1);
			unit->m_amortcount=unit->m_amortcount+1;
			
			if(unit->m_amortcount==unit->m_amortlength) {
				
				unit->m_periodg=findtor(unit);
				
				unit->m_amortisationstate=4;
			}
				
				break;
		case 4: //stepdetect/constdetect
			
			if(detectperiodchange(unit)) {
				
				unit->m_amortisationstate=5;
				unit->m_amortlength=128;
				unit->m_amortcount=0;				
				
				unit->m_bestcolumn=0;
				unit->m_besttorsum=(-1000.0);
				
				unit->m_stateflag=1;
				findmeter(unit); 
				
				//set up Gaussian weighting centred on periodp
				int startindex= 128- ((int)(unit->m_periodp+0.5));
				
				float * mg=unit->m_mg;
				
				for (int ii=0; ii<128;++ii){
					mg[ii]= g_mg[startindex+ii];
				}
				
			} else {
				
				if(unit->m_stateflag==1) 
					unit->m_tor= unit->m_periodg;
				else
					unit->m_tor= unit->m_periodp;
				
				unit->m_torround= int(unit->m_tor+0.5);
				
				unit->m_amortisationstate=7;
				unit->m_amortlength=unit->m_torround;
				unit->m_amortcount=0;
			}
			
			break;
			
		case 5: //redo periodg calculation
			beatperiod(unit,unit->m_amortcount,1);
			unit->m_amortcount=unit->m_amortcount+1;
			
			if(unit->m_amortcount==unit->m_amortlength) {
				
				unit->m_periodg=findtor(unit);
				
				unit->m_tor= unit->m_periodg;
				unit->m_torround= int(unit->m_tor+0.5);
				
				unit->m_amortisationstate=6;
				unit->m_amortlength=unit->m_torround;
				unit->m_amortcount=0;
				
				setupphaseexpectation(unit);
				
				//don't need to reset change flag since it isn't stored
			}
				
				break;
		case 6: //flat phase calc after move to context, avoids bias
			findphase(unit,unit->m_amortcount,0,0);
			unit->m_amortcount=unit->m_amortcount+1;
			
			if(unit->m_amortcount==unit->m_amortlength) {
				
				unit->m_amortisationstate=8; //final state
			}
				
				break;				
			
		case 7: //phase calc with possible gaussian narrowing of the allowed phases
			
			findphase(unit,unit->m_amortcount,unit->m_stateflag,(int)(unit->m_currphase*unit->m_torround+0.5));
			unit->m_amortcount=unit->m_amortcount+1;
			
			if(unit->m_amortcount==unit->m_amortlength) {
				
				unit->m_amortisationstate=8; //final state
			}
				
				break;		
		case 8:
			
			finaldecision(unit); 
			unit->m_amortisationstate=0;
			break;
			
		default:
			break;
	}		
	
	
	AutoTrack_preparefft(unit, in, numSamples);
	
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
void AutoTrack_preparefft(AutoTrack *unit, float* in, int n) {
	
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
		AutoTrack_dofft(unit);
	}
	
	
	unit->m_bufWritePos= bufpos;
	//printf("%d \n",bufpos);
	
}



//calculation function once FFT data ready
void AutoTrack_dofft(AutoTrack *unit) {
	
	int i;
	
	float * fftbuf= unit->m_FFTBuf;
	
	//multiply by Hanning window
	for (i=0; i<N; ++i)
		fftbuf[i] *= hanning1024[i];
	
	//in place	
	//fftwf_execute_r2r(unit->planTime2FFT, fftbuf, fftbuf);	
	
	//easy version, same buffer every time, so simple use of plan
	fftwf_execute(unit->planTime2FFT);
	
	
	/*			
    // Look at the real signal as an interleaved complex vector by casting it.
    // Then call the transformation function ctoz to get a split complex vector,
    // which for a real signal, divides into an even-odd configuration.
    ctoz ((COMPLEX *) fftbuf, 2, &unit->m_vA, 1, NOVER2);
	
    // Carry out a Forward FFT transform
    fft_zrip(unit->m_vsetup, &unit->m_vA, 1, unit->m_vlog2n, FFT_FORWARD);
	
    // The output signal is now in a split real form.  Use the function
    // ztoc to get a split real vector.
    ztoc ( &unit->m_vA, 1, (COMPLEX *) fftbuf, 2, NOVER2);
	
//	// Squared Absolute so get power
//	for (i=0; i<N; i+=2)
//		//i>>1 is i/2 
//		fftbuf[i>>1] = (fftbuf[i] * fftbuf[i]) + (fftbuf[i+1] * fftbuf[i+1]);
//	
//	//calculate loudness first, increments loudnesscounter needed in checkforonsets
//	calculatedf(unit);
//	
	
	*/
	
	//calculate loudness first, increments loudnesscounter needed in checkforonsets
	complexdf(unit);
	
	if (unit->m_frame%SKIP==0) {
		
		//printf("amortisation time \n");
		
		//amortisation- 8 control periods in a frame
		//have 2000 calcs to do, split over 100 control periods = 6400 samples, ie one tempo per control period
		
		unit->m_bestcolumn=0;
		unit->m_besttorsum=(-1000.0);
		
		unit->m_bestphasescore = -1000.0;
		unit->m_bestphase = 0;
		
		//state 0 is do nothing
		unit->m_amortisationstate=1;
		unit->m_amortcount=0;
		unit->m_amortlength=128;
		unit->m_amortisationsteps=0;
		
		//fix time reference for calculations, so it doesn't update during the amortisation- this is the beginning of the df frame
		unit->m_storedfcounter= unit->m_dfcounter+DFSTORE-DFFRAMELENGTH;
		
		//ref for phase calculations  
		unit->m_storedfcounterend= unit->m_dfcounter;  
		
		//unit->m_fftstoreposhold= unit->m_fftstorepos;
		
		unit->m_currphase=unit->m_phase;
		
	}
	
}	


void autocorr(AutoTrack *unit,int j) {
	int i,k, baseframe;
	
	baseframe=unit->m_storedfcounter+DFSTORE;
	float * df= unit->m_df;
	float * acf= unit->m_acf;
	
	//work out four lags each time
	for (k=0;k<4;++k) {
		
		int lag=4*j+k;
		
		int correction= abs(lag-DFFRAMELENGTH);
		
		float sum=0.0;
		
		for (i=lag;i<DFFRAMELENGTH; ++i) {
			
			float val1= df[(i+baseframe)%DFSTORE];
			float val2= df[(i+baseframe-lag)%DFSTORE];
			
			sum+= val1*val2;
		}
		
		acf[lag]=sum*correction;
		
	}
	
}



//timesig 4 has one more sum term
//indices as MATLAB but need to correct maxinds to be in range of tested, not in global range
float findtor(AutoTrack *unit) {
	
	int i;
	float maxval, val;
	int ind2,ind3,ind4;
	
	//put into MATLAB indexing, from 1 to 512
	int ind= unit->m_bestcolumn+1;
	
	float * acf= unit->m_acf-1;
	
	ind2=0;
	maxval=-1000;
	
	for(i=2*ind-1;i<=(2*ind+1);++i){
		
		val=acf[i];
		
		if(val>maxval) {
			
			maxval=val;
			ind2=i-(2*ind-1)+1;
		}
		
	}
	
	//[val2,ind2] = max(acf(2*ind-1:2*ind+1));
	ind2 = ind2 + 2*(ind+1)-2;
	
	ind3=0;
	maxval=-1000;	
	
	for(i=3*ind-2;i<=(3*ind+2);++i){
		
		val=acf[i];
		
		if(val>maxval) {
			
			maxval=val;
			ind3=i-(3*ind-2)+1;
		}
		
	}
	
	//[val3,ind3] = max(acf(3*ind-2:3*ind+2));
	ind3 = ind3 + 3*ind-4;
	
	float period;
	
	if (unit->m_timesig==4) {
		
		ind4=0;
		maxval=-1000;
		
		for(i=4*ind-3;i<=4*ind+3;++i){
			
			val=acf[i];
			
			if(val>maxval) {
				
				maxval=val;
				ind4=i-(4*ind-3)+1;
			}
			
		}
		
		//[val4,ind4] = max(acf(4*ind-3:4*ind+3));
		ind4 = ind4 + 4*ind-9;
		
		period= (ind+ ind2*0.5+ind3/3.f +ind4*0.25)*0.25;
		
	} else
		
		period= (ind+ ind2*0.5+ind3/3.f)*0.3333333;
	
	
	//printf("period %f ind %d ind2 %d ind3 %d ind4 %d \n",period, ind,ind2,ind3,ind4);
	
	//unit->m_tor=period;
	//unit->m_torround= int(period+0.5);
	//
	
	return period;
}




//128 calculation calls for multiplying M and acf, calculates M as it goes apart from precalculated Gaussian or Raleigh distribution
void beatperiod(AutoTrack *unit,int j, int whichm) {
	int i,k, baseframe;
	
	baseframe=unit->m_storedfcounter+DFSTORE;
	float * acf= unit->m_acf;
	
	//int startindex= 512*j;
	//int endindex=startindex+512;
	
	float sum=0.0;
	
	//unit->m_timesig harmonics
	for (i=1;i<=(unit->m_timesig); ++i) {
		
		int num = 2*i-1;
		float wt= 1.0/(float)num;
		
		for (k=0;k<num; ++k) {
			
			int pos= k+(i*j);
			
			//m[startindex+pos]
			if(pos<512)
				sum+= acf[pos]*wt;
		}
		
	}
	
	//assumes Mg appropriately rotated already
	float * m;
	
	if(whichm) 
		m=g_m; //Gaussian weighted context model
	else
		m=unit->m_mg; //general model even weighting
	
	sum=sum*m[j];
	
	if (sum>unit->m_besttorsum) {
		unit->m_besttorsum=sum;
		unit->m_bestcolumn=j;
	}
}


//j out of unit->m_torround
//differs to Davies original in that weight the most recent events more- want minimum reaction time
void findphase(AutoTrack *unit,int j,int gaussflag, int predicted) {
	
	int k;
	
	float * df= unit->m_df;
	
	int period= unit->m_torround;
	int baseframe=unit->m_storedfcounterend+DFSTORE;
	
	int numfit= -1;

	if(period != 0)
	//round down
	numfit= (int)(DFFRAMELENGTH/period)-1;
	
	//testing backwards from the baseframe, weighting goes down as 1/k
	float sum=0.0;
	
	for (k=0;k<numfit;++k) {
		
		//j is phase to test
		int location= (baseframe-(period*k)-j)%DFSTORE; 
		
		sum+= df[location]/((float)(k+1));
		
	}
	
	//Gaussian focus weighting if desired 
	if (gaussflag) {
		
		//difference of predicted from j, min distance within period
		int diff= sc_min(abs(predicted-j),abs(period-predicted+j));
		
		sum *= unit->m_phaseweights[diff];
		
	}
	
	if (sum>unit->m_bestphasescore) {
		
		unit->m_bestphasescore = sum;
		unit->m_bestphase = j;
		
	}
	
}

//, int predicted
void setupphaseexpectation(AutoTrack *unit) { //create Gaussian focussed matrix for phase 
	
	float * wts= unit->m_phaseweights;
	
	float sigma=(unit->m_torround)*0.25;
	//float mu=period;
	
	float mult= 1.0/(2.5066283*sigma);
	float mult2= 1.0/(2.0*sigma*sigma);
	
	//unit->m_torround
	for (int i=0; i<128;++i) {
		wts[i]= mult*(exp(-(i*i)*mult2));
	}
	
}


//why force a countdown each time? Why not keep a continuous buffer of previous periodp, periodg? 
int detectperiodchange(AutoTrack *unit) {
	
	//stepthresh = 3.9017;
	
	if(unit->m_flagstep==0) {
		
		if(fabs(unit->m_periodg-unit->m_periodp) > 3.9017) {
			unit->m_flagstep= 3;
		}
		
	} else {
		unit->m_flagstep= unit->m_flagstep-1;
	}
	
	if(unit->m_flagstep) {
		
		unit->m_prevperiodp[unit->m_flagstep-1]=unit->m_periodp;
	}
	
	if(unit->m_flagstep==1) {
		
		unit->m_flagstep= 0;
		
		if(fabs(2*unit->m_prevperiodp[0] - unit->m_prevperiodp[1] -unit->m_prevperiodp[2]) < 7.8034) //(2*3.9017)
			return 1;
		
	}
	
	return 0;
	
}

//add test
void findmeter(AutoTrack *unit) {
	
	//int i;	
	
	//float * acf= unit->m_acf;
	
//	float * acf= unit->m_acf-1;
//	
//	
//	int period = ((int)(unit->m_periodp+0.5)); 
//	
//	float three_energy=0.0;
//	float four_energy=0.0;
//	
//	for(i=(3*period-2);i<(3*period+3);++i)
//		three_energy += acf[i];
//	
//	for(i=(4*period-2);i<(4*period+3);++i)
//		four_energy += acf[i];
//	
//	if((6*period+2)<512) {
//		
//		for(i=(6*period-2);i<(6*period+3);++i)
//			three_energy += acf[i];
//		
//		for(i=(2*period-2);i<(2*period+3);++i)
//			four_energy += acf[i];
//	}
//    
//	if (three_energy > four_energy)
//		unit->m_timesig = 3;
//	else

//worked better in evaluation without any 3/4 at all!
		unit->m_timesig = 4;	
	
	//printf("time sig %d \n",unit->m_timesig);
	
}


//period is unit->m_tor, phase is unit->m_bestphase
//	float m_tor; int m_torround;
void finaldecision(AutoTrack *unit) {
	//int i,j;
	
	unit->m_currtempo= 1.0/(unit->m_tor*FRAMEPERIOD);
	
	unit->m_phaseperblock= ((float)unit->mWorld->mFullRate.mBufLength*(unit->m_currtempo))/((float)unit->mWorld->mSampleRate);
	
	//printf("SAMPLErate %f %f %f", unit->mWorld->mSampleRate,unit->m_phaseperblock,unit->m_currtempo);
	
	//unit->m_amortisationstate control periods worth = 512/64 = 8
	//float frameselapsed= 0.125*unit->m_amortisationstate;
	//float timeelapsed= frameselapsed*FRAMEPERIOD;
	
	float timeelapsed= ((float)(unit->m_amortisationsteps)*(unit->mWorld->mFullRate.mBufLength)/((float)unit->mWorld->mSampleRate));

	timeelapsed += 7*FRAMEPERIOD; //compensation for detection function being delayed by 7 frames
		
	float phaseelapsed= timeelapsed*(unit->m_currtempo);
	
	float phasebeforeamort= ((float)unit->m_bestphase/unit->m_torround);
	
	//add phase to compensate for ELAPSEDTIME
	unit->m_currphase= unit->m_phase = fmod(phasebeforeamort+phaseelapsed,1.0);
	
}



//if I'm right, this could be a big error in the original Altivec since MATLAB was [bin0 real, 0, bin1 real, bin1 imag, etc up to nyquist] but Altivec 
//was [dc, nyquist, r[1] , i[1], ...]
//no, was Ok before since started at 2*k and k=1 gives start at index 2

void complexdf(AutoTrack *unit) {
	
	int k;
	
	float * fftbuf= unit->m_FFTBuf;

	float * prevmag= unit->m_prevmag;
	float * prevphase= unit->m_prevphase;
	float * predict= unit->m_predict;
	
	float sum=0.0; 
											
	//sum bins 2 to 256			
	for (k=1; k<NOVER2; ++k){
		
			//Change to fftw
			int index= k; //2*k;
			
			float real=fftbuf[index];
			//N=1024 conventionally here
			float imag=fftbuf[N-index]; //fftbuf[index+1];
			
			float mag= sqrt(real*real+ imag*imag); // was  0.5*sqrt(real*real+ imag*imag); reduce by factor of 2 because of altivec side effect
			float qmag= prevmag[k];
			
			prevmag[k]=mag; 
			
			float phase= atan2(imag,real);
			float oldphase = predict[k];
			
			predict[k]= 2*phase- prevphase[k];
			prevphase[k]= phase;
			
			float phasediff= phase-oldphase;
			
			//if(k==2) printf("%f %f\n",phase, phasediff);
			
			//tables for cos/sin/sqrt speeds up? sqrt(1-c*c) slower than sin
			
			float realpart= (qmag-(mag*cos(phasediff)));
			float imagpart= (mag*sin(phasediff)); //no need for negative
			
			float detect= sqrt(realpart*realpart + imagpart*imagpart);
			
			//detect is always positive
			//if(k==1)
			sum+=detect; //(fmod(phase+(16*pi),twopi)); //detect;
			
			//if(k==1) sum+=mag;
	}
	

//smoothing and peak picking operation, delay of 8 frames, must be taken account of in final phase correction

unit->m_dfmemorycounter=(unit->m_dfmemorycounter+1)%15;
unit->m_dfmemory[unit->m_dfmemorycounter]=sum; //divide by num of bands to get a dB answer
	
float rating=0.0;

float * dfmemory=unit->m_dfmemory;

int refpos=unit->m_dfmemorycounter+15;
int centrepos=(refpos-7)%15;
float centreval=dfmemory[centrepos];

for (k=0;k<15; ++k) {
    
int pos=(refpos-k)%15;
	
float nextval= centreval-dfmemory[pos];
        
        if (nextval<0.0)
            nextval=nextval*10;
    
        rating+=nextval;
}
          
if(rating<0.0) rating=0.0;     
	
//increment first so this frame is unit->m_loudnesscounterdfcounter
unit->m_dfcounter=(unit->m_dfcounter+1)%DFSTORE;

unit->m_df[unit->m_dfcounter]=rating*0.1; //sum //divide by num of bands to get a dB answer

}






