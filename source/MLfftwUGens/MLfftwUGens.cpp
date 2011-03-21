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

//Using FFTW, also GNU GPL

//This file is part of MachineListening. Copyright (C) 2006  Nicholas M.Collins distributed under the terms of the GNU General Public License full notice in file MachineListening.license

//Making a single plug-in containing a set of Machine Listening UGens by Nick Collins
//Tartini is ported from the work of Phil McLeod
//AutoTrack is my own conversion from a paper by Matthew Davies

#include "MLfftwUGens.h"


float hanning1024[1024];
InterfaceTable *ft; 




void prepareFFTW(int n);

void prepareFFTW(int n) {

	float * array1, * array2;

	array1 = (float*)fftwf_malloc(sizeof(float) * n);
	array2 = (float*)fftwf_malloc(sizeof(float) * n);
		
	//can use FFTW_ESTIMATE, FFTW_MEASURE, FFTW_PATIENT, FFTW_EXHAUSTIVE
	//FFTW_ESTIMATE is quickest to initialise
	//could make this a constant #define or something
	//fftwf_plan forward= 
	fftwf_plan_r2r_1d(n, array1, array2, FFTW_R2HC, FFTW_ESTIMATE);
	fftwf_plan_r2r_1d(n, array2, array1, FFTW_HC2R, FFTW_ESTIMATE);
	//fftwf_plan inverse= 
	
	fftwf_free(array1);
	fftwf_free(array2);

}




//probably have to put this in load() again since 256 or more cos calls
void prepareHanningWindow(float * hanning, int n){
	float ang;
	
	ang=(1.0/n)*twopi;
	
	for(int i=0;i<n;++i)
		hanning[i]=0.5 - 0.5*cos(ang*i);
	
}

PluginLoad(MLfftw)
{
	
	ft = inTable;
	
	//DefineDtorUnit(Tartini);

	DefineDtorCantAliasUnit(AutoTrack);
	DefineDtorUnit(AnalyseEvents2);
	DefineDtorUnit(Tartini);
	DefineDtorCantAliasUnit(Qitch);
	
	//adding once re-optimise- want an as fast as possible onset detector 
	//DefineDtorUnit(Onset);
	
	//printf("Machine Listening UGens by Nick Collins for SuperCollider 3 \n");
	//printf("Tartini adapted from Phil McLeod's Tartini project\n");
	//printf("AutoTrack adapted from Matthew Davies' autocorrelation beat tracker\n");
	//printf("Qitch based on algorithms published by Judith Brown and Miller Puckette\n");
	
//	prepareHanningWindow(hanning256, 256);		//for Concat
	prepareHanningWindow(hanning1024, 1024);	//for AE2 and AutoTrack
	
	//printf("Tartini adapted by Nick Collins from Phil McLeod's Tartini project\n");
	
	//If done at this point, won't interfere with realtime once running
	//printf("initialising FFTW for common plans \n");
	
	//for Tartini
	prepareFFTW(1024+512);
	prepareFFTW(512+256);
	prepareFFTW(2048+1024);
	
	
	//for AutoTrack/AE2
	prepareFFTW(1024);
	
	//for Qitch
	prepareFFTW(4096);
	prepareFFTW(2048);
		
	//printf("done FFTW initialisation \n ");
	
	//printf("comparing FFT constants\n");
	//compareFFT();
	
}




/*
//test code for FFT scale factors
#include <vecLib/vecLib.h>

unsigned long m_vlog2n;
COMPLEX_SPLIT m_vA;
FFTSetup m_vsetup;
float test[1024];
	
	
//Altivec vs FFTW
void compareFFT() {

int i;

	float * array1 = (float*)fftwf_malloc(sizeof(float) * 1024);

	for (i=0; i<10; ++i) {
	test[i]=1;
	array1[i]=1;
	}
	
	m_vA.realp = new float[512]; //(float*)fftwf_malloc(512 * sizeof(float)); 
	m_vA.imagp = new float[512]; //(float*)fftwf_malloc(512 * sizeof(float));
	m_vlog2n = 10; //N is hard coded as 1024, so 10^2=1024 //log2max(N);
	m_vsetup = create_fftsetup(m_vlog2n, 0);
			
    // Look at the real signal as an interleaved complex vector by casting it.
    // Then call the transformation function ctoz to get a split complex vector,
    // which for a real signal, divides into an even-odd configuration.
    ctoz ((COMPLEX *) test, 2, &m_vA, 1, 512);
	
    // Carry out a Forward FFT transform
    fft_zrip(m_vsetup, &m_vA, 1, m_vlog2n, FFT_FORWARD);
	
    // The output signal is now in a split real form.  Use the function
    // ztoc to get a split real vector.
    ztoc ( &m_vA, 1, (COMPLEX *) test, 2, 512);
	
	
	// Squared Absolute so get power
	//for (i=0; i<N; i+=2)
		//i>>1 is i/2 
	//	fftbuf[i>>1] = ((fftbuf[i] * fftbuf[i]) + (fftbuf[i+1] * fftbuf[i+1]))*0.25; //correction for Altivec stupid FFT
	
	
	
	fftwf_plan forward= fftwf_plan_r2r_1d(1024, array1, array1, FFTW_R2HC, FFTW_ESTIMATE);
	
	fftwf_execute_r2r(forward, array1, array1);	

	for (i=1; i<10; ++i) {
	printf("altivec %f fftw %f \n",test[2*i], array1[i]);
	}


	fftwf_free(array1);

	delete m_vA.realp;
	delete m_vA.imagp;


}
*/

//fftw = half altivec 
//fftw same as in MATLAB
//a= zeros(1024,1)
//b=fft(a,1024)
//get same real coefficients as fftw
/*
altivec 19.989273 fftw 9.994636 
altivec 19.957109 fftw 9.978555 
altivec 19.903574 fftw 9.951788 
altivec 19.828781 fftw 9.914391 
altivec 19.732876 fftw 9.866438 
altivec 19.616056 fftw 9.808027 
altivec 19.478556 fftw 9.739278 
altivec 19.320654 fftw 9.660326 
altivec 19.142666 fftw 9.571333 
*/



