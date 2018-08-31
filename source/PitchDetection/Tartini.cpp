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

/***************************************************************************

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Tartini (version 1) is Copyright (C) 2002-2005 Philip McLeod.
email                : pmcleod@cs.otago.ac.nz

You may use, distribute and copy Tartini (version 1) under the terms of
GNU General Public License version 2.

The source for Tartini (version 1) can be downloaded from http://www.tartini.net
or write to Philip McLeod, Department of Computer Science, University of Otago,
PO Box 56, Dunedin 9015, New Zealand.

***************************************************************************/

//Using FFTW, also GNU GPL

//This file is part of MachineListening. Copyright (C) 2006  Nicholas M.Collins distributed under the terms of the GNU General Public License full notice in file MachineListening.license

//Conversion of Philip McLeod's Tartini pitch detector by Nick Collins, completed November 13, 2006 
//13 dec 2006, adapted for a general machine listening distro

// parameters- in, threshold (k-rate),  n (i-rate), k (i-rate), overlap (i-rate), smallcutoff(k-rate)

#include "PitchDetection.h"


void setupTartini(InterfaceTable *);
void preparefft(Tartini *unit, float* in);
void dofft(Tartini *unit);
void inversefft(Tartini *unit);
void nsdf(Tartini *unit);
void peakpicking(Tartini *unit);


#if SC_FFT_VDSP
#define FFT_VSP_MAXSIZE 3072
static int fftAllowedSizes[3] = {8,9,10}; //{256,512,1024}; 
static FFTSetup fftSetup3[3]; // vDSP setups, one per FFT size, allowing 512 + 256, 1024 + 512, 2048 + 1024
static COMPLEX_SPLIT splitBuf; // input buf for vDSP FFT
static COMPLEX_SPLIT splitBuf2; // output buf

#endif

void setupTartini(InterfaceTable * inTable) {

	ft= inTable;
	
	//must sort in Tartini plugin itself 	
	//for Tartini
	
	DefineDtorUnit(Tartini);
	//	prepareFFTW(1024+512);
	//	prepareFFTW(512+256);
	//	prepareFFTW(2048+1024);
	
	
	#if SC_FFT_VDSP
	
	for (int i=0; i<3; ++i) {
		fftSetup3[i] = vDSP_create_fftsetup(fftAllowedSizes[i],FFT_RADIX3);  //FFT has size 3*(2**n), SC_fftlib.cpp can't support this easily
		if(fftSetup3[i] == NULL)
			printf("FFT ERROR: Mac vDSP library could not allocate FFT setup for size %i\n", 1<<i);
	}
		
	splitBuf.realp = (float*) malloc ( FFT_VSP_MAXSIZE * sizeof(float) );
	splitBuf.imagp = (float*) malloc ( FFT_VSP_MAXSIZE * sizeof(float) );
	splitBuf2.realp = (float*) malloc ( FFT_VSP_MAXSIZE * sizeof(float) );
	splitBuf2.imagp = (float*) malloc ( FFT_VSP_MAXSIZE * sizeof(float) );

	#endif
}





void Tartini_Ctor( Tartini* unit ) {
	
	//int i;
	
	double samplingrate = unit->mWorld->mSampleRate; //SAMPLERATE; //unit->mWorld->mFullRate->mSampleRate
	unit->blocklength= unit->mWorld->mFullRate.mBufLength; //BUFLENGTH; 
	
	int n = (int)(ZIN0(2)+0.1); //poor man's roundoff
	int k = (int)(ZIN0(3)+0.1); 
	int overlap= (int)(ZIN0(4)+0.1); 
	
	//printf("n %d k %d overlap %d \n", n, k, overlap); 
	
	if (n<256) n=256;
	
	//default
	if(k==0) k = (n + 1) / 2;
	
	if(k>n) k=n;
	
	//HARD CODED FOR INITIAL TESTS	
#if SC_FFT_VDSP
	
	switch(n) {
		case 2048: 
			k= 1024; 
			//m_whichfftindex = 2;
			break;
		case 1024:
			k=512;
			//m_whichfftindex = 1;
			break;
		case 512:
			k=256;
			//m_whichfftindex = 0; 
			break;
		default:
			n= 2048;
			k=1024;
			//printf("Tartini: should never occur, OS X use of vDSP requires n = 2048, 1024, 512, you choose %d \n",n);
			break;
	}
	
	unit->m_whichfftindex = LOG2CEIL(k)-8;

#endif	
	
	
	if (overlap<0) overlap=0;
	
	//allow time for amortisation
	if (overlap>(n-(4*unit->blocklength))) overlap=(n-(4*unit->blocklength));
	
	//int n=2048;
	//int k = (n + 1) / 2; //will lead to odd FFT 3072 or 1536 points, cheaper if power of 2
	int size = n + k;
	

	
	//unit->nover2=  (n + 1) / 2;
	unit->overlap= overlap; //unit->nover2; 
	unit->overlapindex= n-overlap; 
	
	//float freqPerBin = samplingrate / double(size); 
	
	//printf("size %d n %d k %d overlap %d overlapindex %d \n", size, n, k, overlap, unit->overlapindex); 
	
	unit->n= n;
	unit->k= k;
	unit->size=size;


	
	//unit->freqPerBin=freqPerBin;
	unit->rate=samplingrate;
	
	unit->m_bufWritePos = 0;
	

	unit->output = (float*)RTAlloc(unit->mWorld, sizeof(float) * k);
	unit->input = (float*)RTAlloc(unit->mWorld, sizeof(float) * n);
	
	unit->dataTemp = (float*)RTAlloc(unit->mWorld, sizeof(float) * n);
	unit->autocorrTime = (float*)RTAlloc(unit->mWorld, sizeof(float) * size);
	unit->autocorrFFT  = (float*)RTAlloc(unit->mWorld, sizeof(float) * size);
	
//	unit->dataTemp = (float*)fftwf_malloc(sizeof(float) * n);
//	unit->autocorrTime = (float*)fftwf_malloc(sizeof(float) * size);
//	unit->autocorrFFT  = (float*)fftwf_malloc(sizeof(float) * size);
	
	
#if SC_FFT_FFTW
	unit->planAutocorrTime2FFT = fftwf_plan_r2r_1d(size, unit->autocorrTime, unit->autocorrFFT, FFTW_R2HC, FFTW_ESTIMATE);
	unit->planAutocorrFFT2Time = fftwf_plan_r2r_1d(size, unit->autocorrFFT, unit->autocorrTime, FFTW_HC2R, FFTW_ESTIMATE);
#elif SC_FFT_VDSP	
	unit->log2n = fftAllowedSizes[unit->m_whichfftindex]; //LOG2CEIL(size)	

#endif
	
	unit->m_currfreq=440;  
	unit->m_hasfreq=0;
	
	//amortisation and states
	unit->m_amortisationstate=0; //off
	
	
	SETCALC(Tartini_next);
}

void Tartini_Dtor(Tartini *unit) {
	
#if SC_FFT_FFTW	
    fftwf_destroy_plan(unit->planAutocorrFFT2Time);
    fftwf_destroy_plan(unit->planAutocorrTime2FFT);
#endif	
//    fftwf_free(unit->autocorrFFT);
//    fftwf_free(unit->autocorrTime);
//    fftwf_free(unit->dataTemp);

	//fftwf_free(unit->output);
	//	fftwf_free(unit->input);
	RTFree(unit->mWorld, unit->autocorrFFT);
    RTFree(unit->mWorld, unit->autocorrTime);
    RTFree(unit->mWorld, unit->dataTemp);
	RTFree(unit->mWorld, unit->output);
	RTFree(unit->mWorld, unit->input);
	
	
}


//kr rate
void Tartini_next( Tartini *unit, int inNumSamples ) {
	
	float *in = IN(0);
	
	switch(unit->m_amortisationstate) {
		case 0:
			break; //do nothing case (will take fft if necessary)
		case 1: //calculate ifft and nsdf
			inversefft(unit);
			unit->m_amortisationstate=2;
			break;
		case 2: //calculate ifft and nsdf
			nsdf(unit);
			unit->m_amortisationstate=3;
			break;
		case 3: //calculate peak picking
			peakpicking(unit);
			unit->m_amortisationstate=0;	
			break;
		//default:
		//	break;
	}	
	preparefft(unit, in);
	
	ZOUT0(0)=unit->m_currfreq;
	ZOUT0(1)=unit->m_hasfreq;
	
}	


//rewritten this for arbitrary overlap, n

//Tristan Jehan recommends copying ints rather than floats- I say negligible compared to other algorithm costs for the moment
void preparefft(Tartini *unit, float* in) {
	
	int n=unit->n;
	
	//urk! 
	
	int bufpos= unit->m_bufWritePos;
	int blocklength=unit->blocklength;
	
	int i, index = 0, cpt = blocklength, maxindex;
	
	float * preparefftbuf=unit->dataTemp;
	float * fftbuf= unit->autocorrTime;
	
	// Copy input samples into prepare buffer	
	while ((bufpos < n) && (cpt > 0)) {
		preparefftbuf[bufpos] = in[index];
		++bufpos;
		++index;
		--cpt;
	}
	
	// When Buffer is full...
	if (bufpos >= n) {
		
		float * input= unit->input;
		
		// Make a copy of prepared buffer into FFT buffer for computation
		/*
		for (i=0; i<n; i++) {
			fftbuf[i] = preparefftbuf[i];
			input[i]=  preparefftbuf[i];
		}
		*/
		Copy(n, fftbuf, preparefftbuf);
		Copy(n, input , preparefftbuf);
		
		//zero padding: zero the top k elements
		int size=unit->size;
		Clear(size-n, fftbuf+n);
		
		//proving that fftw FFT doesn't do anything to input data array! 
		//	
		//		printf("before \n");
		//			for (i=0; i<20; i++) 
		//			printf("%f ",preparefftbuf[i]);
		//			
		//		printf("\n\n");
		
		//FFT buffer ready- calculate away!
		dofft(unit);
		
		//need to use preparefftbuf too, must be careful if amortising or make a copy of time domain too
		
		//		
		//		printf("after \n");
		//			for (i=0; i<20; i++) 
		//			printf("%f ",preparefftbuf[i]);
		//			
		//		printf("\n\n");
		
		
		
		//if(unit->m_overlap>0) will be safe as long as overlap=0l overlapindex=0 too
		
		// Save overlapping samples back into buffer- no danger since no indices overwritten
		/*
		for (i=0; i<unit->overlap; i++) 
			preparefftbuf[i] = preparefftbuf[unit->overlapindex+i];
		*/
		Copy(unit->overlap, preparefftbuf, preparefftbuf + unit->overlapindex);
		
		maxindex = blocklength - index + unit->overlap;
		
		//blockSize less than n-overlapindex so no problem
		// Copy the rest of incoming samples into prepareFFTBuffer
		for (i=unit->overlap; i<maxindex; i++) {
			preparefftbuf[i] = in[index];
			index++;
		}
		
		bufpos = maxindex;
		
	}
	
	
	unit->m_bufWritePos= bufpos;
	//printf("%d \n",bufpos);
	
}



//calculation function once FFT data ready, will be removing windowing! 
//I've split the autocorr calculation over two functions, as below, for amortisation, hence a small amount of repeated code
void dofft(Tartini *unit) {
	
	int j;
	int size=unit->size;
	float * autocorrFFT= unit->autocorrFFT; //results of FFT
	
	
#if SC_FFT_FFTW	
	fftwf_execute(unit->planAutocorrTime2FFT);

	//do half of the calculations
	for(j=1; j<size/4; ++j) {
		float val1= autocorrFFT[j];
		float val2= autocorrFFT[size-j];
		
		autocorrFFT[j] = (val1*val1) + (val2*val2); 
		autocorrFFT[size-j] = 0.0f;
	}
	
#else
	
	float * autocorrTime= unit->autocorrTime; //input to FFT
	
	//different packing: as complex data	
	//leave junk data in FFT_VSP_MAXSIZE, just prepare up to size
	for(j=0; j<size; ++j) {
		splitBuf.realp[j] = autocorrTime[j]; 
		splitBuf.imagp[j] = 0.0f; 
	}
	
	// Now the actual FFT; out of place COMPLEX to COMPLEX FFT
	vDSP_fft3_zop(fftSetup3[unit->m_whichfftindex], &splitBuf, 1, &splitBuf2, 1, unit->log2n, 1);
	
	unit->m_nyquist = splitBuf2.realp[size/2]; 
	
	// Copy the data to the public output buf, transforming it back out of "split" representation
	vDSP_ztoc(&splitBuf2, 1, (DSPComplex*)autocorrFFT, 2, size >> 1);
	
	//NO SCALING REQUIRED FOR FORWARDS FFT if complex rather  than real input
	//http://developer.apple.com/library/ios/#documentation/Performance/Conceptual/vDSP_Programming_Guide/UsingFourierTransforms/UsingFourierTransforms.html
	//float scale = 0.5f;
	//vDSP_vsmul(autocorrFFT, 1, &scale, autocorrFFT, 1, size);

	//do half of the calculations
	for(j=1; j<size/4; ++j) {
		float val1= autocorrFFT[2*j];
		float val2= autocorrFFT[2*j+1];
		
		autocorrFFT[j] = (val1*val1) + (val2*val2); 
		autocorrFFT[size-j] = 0.0f;
	}
#endif

	unit->m_amortisationstate=1; //on	
}

void inversefft(Tartini *unit) {

	int j;
	int size=unit->size;
	
	int start= size/4;
	float * autocorrFFT= unit->autocorrFFT; //results of FFT

#if SC_FFT_FFTW	
	
	
	for(j=start; j<size/2; j++) {
		float val1= autocorrFFT[j];
		float val2= autocorrFFT[size-j];
		
		autocorrFFT[j] = (val1*val1) + (val2*val2); 
		autocorrFFT[size-j] = 0.0f;
	}
	autocorrFFT[0] = autocorrFFT[0]*autocorrFFT[0];
	autocorrFFT[size/2] = autocorrFFT[size/2]*autocorrFFT[size/2];
	

			//Do an inverse FFT
	fftwf_execute(unit->planAutocorrFFT2Time);
	
#else
	
	float * autocorrTime= unit->autocorrTime; //input to FFT
	
	for(j=start; j<size/2; j++) {
		float val1= autocorrFFT[2*j];
		float val2= autocorrFFT[2*j+1];
		
		autocorrFFT[j] = (val1*val1) + (val2*val2); 
		autocorrFFT[size-j] = 0.0f;
		
	}
	
	autocorrFFT[0] = autocorrFFT[0]*autocorrFFT[0];	//dc

	float nyquist = unit->m_nyquist; 
	
	autocorrFFT[size/2] = nyquist*nyquist; //autocorrFFT[size/2]*autocorrFFT[size/2];	//nyquist, packed format from vDSP
	
	//leave junk data in FFT_VSP_MAXSIZE, just prepare up to size
	for(j=0; j<size; ++j) {
		splitBuf.realp[j] = autocorrFFT[j]; 
		splitBuf.imagp[j] = 0.0f; 
	}
	
	vDSP_fft3_zop(fftSetup3[unit->m_whichfftindex], &splitBuf, 1, &splitBuf2, 1, unit->log2n, 0);
	
	
	for(j=0; j<size; j++) {
		autocorrTime[j] = splitBuf2.realp[j];
	}
	
	//No scaling required because later comparative stage does this, otherwise IFFT would be divided by size
	//float scale = 1.0/((float) size) ;
	//vDSP_vsmul(autocorrTime, 1, &scale, autocorrTime, 1, size);

	
#endif
	
	
}

//amortised
void nsdf(Tartini *unit) {
	
	int j;
	int size=unit->size;

	float * autocorrTime= unit->autocorrTime; //results ofinverse FFT
	float * output= unit->output;
	float fsize_rec = 1.f / float(size);
	//buffer for k outputs (512 autocorr coefficients- check if same if take 2048 point autocorr?)
	
	int k= unit->k;
	int n= unit->n;
	
	for(float *p1=output, *p2=autocorrTime+1; p1<output+k;)
	    *p1++ = *p2++ * fsize_rec;
	//    
	double sumSq= double(autocorrTime[0]) * double(fsize_rec);
	
	//NSDF
	double sumRightSq = sumSq, sumLeftSq = sumSq;
	
	float * input= unit->input; //dataTemp; //safety because of amortisation
	
	for(j=0; j<k; ++j) {
		float left= input[n-1-j];
		float right= input[j];
		
		sumLeftSq  -= left*left;
		sumRightSq -= right*right;
		//output[j] = 1.0 - (sumLeftSq + sumRightSq - 2*output[j]) / (sumLeftSq + sumRightSq);
		output[j] *= 2.f / static_cast<float>(sumLeftSq + sumRightSq);
	}
	
	//now have k lags efficiently calculated, peak picking time
	//double rms= sumSq/double(n)
	//peakpicking(unit); //now amortised as next step, will happen for next block
	
	//see Qitch
	//float ampthresh = ZIN0(2);
	//section 7 of paper, confidence measure test power against threshold and clarity of pitch estimate unit->m_hasfreq=0;	
}	



inline void parabolaTurningPoint2(float y_1, float y0, float y1, float xOffset, float *x, float *y)
{
	float yTop = y_1 - y1;
	float yBottom = y1 + y_1 - 2 * y0;
	if(yBottom != 0.0) {
		*x = xOffset + yTop / (2 * yBottom);
		*y = y0 - ((yTop*yTop) / (8 * yBottom));
	} else {
		*x = xOffset;
		*y = y0;
	}
}


#include <vector>

//can move vectors to permanent membership, clear each time used

//relatively direct port of Tartini code, even keeping the std library code ; )
//doesn't push up plug-in size too dramatically
void peakpicking(Tartini *unit) {
	
	float * output= unit->output;
	int k=unit->k;
	//int n=unit->n;
	
	std::vector<int> maxPositions; //std library include
	int pos = 0;
	int curMaxPos = 0;
	
	/**
		Find the highest maxima between each pair of positive zero crossings.
	 Including the highest maxima between the last +ve zero crossing and the end if any.
	 Ignoring the first (which is at zero)
	 In this diagram the disired maxima are marked with a *
	 
	 *             *
	 \      *     /\      *     /\
	 _\____/\____/__\/\__/\____/__
	 \  /  \  /      \/  \  /
	 \/    \/            \/
	 */
    
    //while(pos < k-1 && output[pos] > 0.0f) pos++; //find the first negitive zero crossing
    while(pos < (k-1)/3 && output[pos] > 0.0f) pos++; //find the first negitive zero crossing
    while(pos < k-1 && output[pos] <= 0.0f) pos++; //loop over all the values below zero
    if(pos == 0) pos = 1; // can happen if output[0] is NAN
    
    while(pos < k-1) {
		
		if(output[pos] > output[pos-1] && !(output[pos] < output[pos+1])) { //a local maxima
																		  //if(output[pos] >= threshold) { maxPos = pos; break; } //the first maxima above threshold. Stop there.
			if(curMaxPos == 0) curMaxPos = pos; //the first maxima (between zero crossings)
			else if(output[pos] > output[curMaxPos]) curMaxPos = pos; //a higher maxima (between the zero crossings)
		}
		pos++;
		if(pos < k-1 && !(output[pos] > 0.0f)) { //a negative zero crossing
			if(curMaxPos > 0) { //if there was a maximum
				maxPositions.push_back(curMaxPos); //add it to the vector of maxima
				curMaxPos = 0; //clear the maximum position, so we start looking for a new ones
			}
			while(pos < k-1 && !(output[pos] > 0.0f)) pos++; //loop over all the values below zero
		}
    }
	
    if(curMaxPos > 0) { //if there was a maximum in the last part
		maxPositions.push_back(curMaxPos); //add it to the vector of maxima
		curMaxPos = 0; //clear the maximum position, so we start looking for a new ones
    }
    
	std::vector<float> periodEstimates;
	std::vector<float> periodEstimatesAmp;
	
    //store some of the best period estimates
    periodEstimates.clear();
    periodEstimatesAmp.clear();
	
    //float smallThreshold = 0.7f;
    //float smallCutoff = output[overallMaxIndex] * smallThreshold;
    float smallCutoff = ZIN0(5); //0.5f;
    for(std::vector<int>::iterator iter = maxPositions.begin(); iter < maxPositions.end(); iter++) {
		if(output[*iter] >= smallCutoff) {
			
			float x, y;
			//do a parabola fit to find the maximum
			parabolaTurningPoint2(output[*iter-1], output[*iter], output[*iter+1], float(*iter + 1), &x, &y);
			
			if(y < -1.0f) y = -1.0f;
			if(y > 1.0f) y = 1.0f;
			
			//y = bound(y, -1.0f, 1.0f);
			periodEstimates.push_back(x);
			periodEstimatesAmp.push_back(y);
		}
    }
    
    //if(maxPositions.empty()) { //no period found
    if(periodEstimates.empty()) { //no period found
		
		//  analysisData.correlation = 0.0f;
		//      analysisData.fundamentalFreq = 0.0f;
		//      analysisData.note = 0.0f;
		//      analysisData.volumeValue = 0.0f;
		//      analysisData.highestCorrelationIndex = -1;
		//      analysisData.chosenCorrelationIndex = -1;
		//      analysisData.noteIndex = -1;
		//      analysisData.done = true;
		//	  
		unit->m_hasfreq=0;
		
		
		return;
    }
    
    
    //find the overall maximum position
    int overallMaxIndex = 0;
	unsigned int iterPos;
    //for(std::vector<float>::iterator iterPos = analysisData.periodEstimatesAmp.begin()+1; iterPos < analysisData.periodEstimatesAmp.end(); iterPos++) {
    for(iterPos = 1; iterPos < periodEstimatesAmp.size(); iterPos++) {
		if(periodEstimatesAmp[iterPos] > periodEstimatesAmp[overallMaxIndex]) overallMaxIndex = iterPos;
    }
    int highestCorrelationIndex = overallMaxIndex;
	
	//chooseCorrelationIndex(analysisData, threshold);
    
	//uint iterPos;
	int choosenMaxIndex = 0;
	
	//already tested!
	//if(periodEstimates.empty()) return; //no period found
	
	
	//seems to be an int of 97 in setting code but I'll ignore for now
	float threshold= ZIN0(1); //0.93; //93% is GUI default in Tartini program
	
    //choose a cutoff value based on the highest value and a relative threshold
    float cutoff = periodEstimatesAmp[highestCorrelationIndex] * threshold;
    //find the first of the maxPositions which is above the cutoff
    for(iterPos = 0; iterPos < periodEstimatesAmp.size(); iterPos++) {
		if(periodEstimatesAmp[iterPos] >= cutoff) { choosenMaxIndex = iterPos; break; }
    }
	
	//int chosenCorrelationIndex = choosenMaxIndex;
	//float correlation = periodEstimatesAmp[choosenMaxIndex];
	
	double period = periodEstimates[choosenMaxIndex];
	double freq = (unit->rate) / period;
	float fundamentalFreq = float(freq);
	
	//was unit->m_hasfreq= 1, Dan Stowell spotted this quick fix:
	unit->m_hasfreq = periodEstimatesAmp[iterPos]; // According to McLeod (2005), the "clarity" (between 0 and 1) is simply this value
	
	unit->m_currfreq= fundamentalFreq;
	
	//printf("freq %f period %f rate %f \n",fundamentalFreq, period, unit->rate);
	
	}

