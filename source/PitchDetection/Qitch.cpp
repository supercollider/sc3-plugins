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

//this file 8/1/05 by Nicholas M. Collins after Brown/Puckette
//updated for fftw 13/12/06
//converted to SC fft interface, 26 March 2011

#include "PitchDetection.h"

#define TWOPI 6.28318530717952646f 


//float g_fmin;
//int g_qbands;
//float * g_qfreqs;
//float * g_kernels;

//non windowed FFT for the signal x[n] because the kernels are pre windowed
//float * g_hanning; 

//for pitch search, MATLAB calculated
//sieve= round(24*log(1:11)/log(2))       
//amps= fliplr(0.6:0.04:1.0);

int g_sieve[11]= {0,24,38,48,56,62,67,72,76,80,83};
float g_amps[11]={1,0.96,0.92,0.88,0.84,0.8,0.76,0.72,0.68,0.64,0.6}; 

//116 is near 5000 Hz in this , 90 is 2500Hz or so

//other functions
void Qitch_preparefft(Qitch *unit, float* in, int n);
void Qitch_dofft(Qitch *unit);

void Qitch_Ctor(Qitch* unit)
{
	int i;
	
	///////constant Q data in buffer passed in
	
	World *world = unit->mWorld;
    
	uint32 bufnum = (uint32)ZIN0(1);
	if (bufnum >= world->mNumSndBufs) bufnum = 0;
	
	SndBuf *buf = world->mSndBufs + bufnum; 
	
	int bufsize = buf->samples;
	
	//printf("bufnum %d size %d\n",bufnum, bufsize);			
	
	float * pdata= buf->data; 
	//get Q data	
	
	int SR= (int)pdata[0];
	int fftN= (int)pdata[1];
	
	int numbands= (int)pdata[2];
	
	unit->m_qbands=numbands;
	//int cumulsize= (int)pdata[3];
	
	//printf("SR %d N %d bands %d cumulsize %d \n",unit->m_SR, unit->m_N, unit->m_qbands, cumulsize);				
	
	//if(g_SR != SR){
	//		
	//		g_SR=SR; 
	//		g_Nyquist=(int)(SR/2);  
	//		g_framespersec= (float)g_overlap/g_SR;
	//		g_freqperbin= (float)g_SR/(float)g_N;
	//		
	//	};
	
	//other globals like g_N assumed correct for now
	//all FFTs are taken as a multiple of 1024
	unit->m_SR= SR;
	unit->m_Nyquist= SR/2;
	unit->m_N= fftN; 
	unit->m_log2N=(int)(log2(fftN)+0.5);  ;
	unit->m_Nover2= fftN/2;
	unit->m_overlap= fftN-1024;
	unit->m_overlapindex= (1024)%fftN;
	unit->m_framespersec= (float)(unit->m_overlap)/(float)SR;
	unit->m_fftscale= 1.0/(2.0*fftN); //altivec 1.0/(2.0*fftN);
	unit->m_freqperbin= (float)SR/(float)fftN;
	
	//constants for efficiency
	unit->m_twopioverN= TWOPI/(float)fftN;
	unit->realb=cos(unit->m_twopioverN);
	unit->imagb=sin(unit->m_twopioverN);
	
	////////FFT data///////////
	
	unit->m_prepareFFTBuf = (float*)RTAlloc(unit->mWorld, fftN * sizeof(float));
	unit->m_FFTBuf = (float*)RTAlloc(unit->mWorld, fftN * sizeof(float));
	unit->m_bufWritePos = 0;	
	
	SCWorld_Allocator alloc(ft, world);
	
	//no overlap
	//no windowing for constant Q
	unit->m_scfft = scfft_create(fftN, fftN, kRectWindow, unit->m_FFTBuf, unit->m_FFTBuf, kForward, alloc);
	
	
	float * qfreqs=(float*)RTAlloc(world, numbands * sizeof(float));
	int * startindex=(int*)RTAlloc(world, numbands * sizeof(int));
	int * numindices=(int*)RTAlloc(world, numbands * sizeof(int));
	float ** speckernelvals=(float**)RTAlloc(world, numbands * sizeof(float*));
	float * qmags= (float*)RTAlloc(world, numbands * sizeof(float));
	
	/*
	 unit->m_qfreqs= (float*)RTAlloc(world, numbands * sizeof(float));
	 unit->m_startindex= (int*)RTAlloc(world, numbands * sizeof(int));
	 unit->m_numindices= (int*)RTAlloc(world, numbands * sizeof(int));
	 //unit->m_cumulindices= (int*)RTAlloc(world, numbands * sizeof(int));
	 //unit->m_speckernelvals = (float*)RTAlloc(world, cumulsize * sizeof(float));
	 unit->m_speckernelvals = (float*)RTAlloc(world, numbands * sizeof(float*));
	 unit->m_qmags = (float*)RTAlloc(world, numbands * sizeof(float));
	 */
	
	//load data 
	int bufpos=3; //4;
	
	//printf("%d %p %d %p \n",i,&(pdata[bufpos]),bufpos,pdata);
	
	//can be made more efficient with pointers
	for (i=0;i<numbands; ++i) {
		
		//printf("%d %p %d %p \n",i,&(pdata[bufpos]),bufpos,pdata);
		
		//freq startind cumul numvals vals'
		qfreqs[i]= pdata[bufpos];
		startindex[i]= (int) pdata[bufpos+1];
		numindices[i]= (int) pdata[bufpos+2]; //+3
		
		//int specind= pdata[bufpos+2]; //cumulative position into this buffer
		//unit->m_cumulindices[i]=specind;
		
		//printf("%d startind %d numind %d cumul %d \n",i, unit->m_startindex[i], unit->m_numindices[i], unit->m_cumulindices[i]);
		bufpos+=3; //4;
		
		speckernelvals[i]= pdata+bufpos;
		
		//printf("%d %p %d %p %p \n",i,&(pdata[bufpos]),bufpos,pdata+bufpos, speckernelvals[i]);
		
		/*
		 for (j=0;j<unit->m_numindices[i]; ++j) {
			 
			 unit->m_speckernelvals[specind+j]= pdata[bufpos+j];
			 //if(pdata[bufpos+j]>1000) printf("%d big %f indtarget %d indsource %d",i,pdata[bufpos+j],specind+j,bufpos+j);
		 }*/
		
		bufpos+= numindices[i]; 
	}
	
	unit->m_qfreqs=qfreqs;
	unit->m_startindex= startindex;
	unit->m_numindices= numindices;
	unit->m_speckernelvals= speckernelvals;
	unit->m_qmags= qmags;
	
	
	/////storing complex numbers from previous frames for instananeous frequency calculation
	unit->m_topqcandidate=numbands-(g_sieve[10])-1; //85
	float tempfreq= unit->m_qfreqs[unit->m_topqcandidate];
	unit->m_ifbins=((int)ceil((tempfreq/(unit->m_freqperbin))+0.5))+1; //cover yourself for safety
	
	//printf("numbinsstored %d tempfreq %f topcand %d \n",unit->m_ifbins,tempfreq, unit->m_topqcandidate); //more info! 
	
	//if input amp template can correct search comb
	
	for (i=0;i<11;++i)
		unit->m_amps[i]= g_amps[i];
	
	uint32 ampbufnum = (uint32)ZIN0(4);
	if (!((ampbufnum > world->mNumSndBufs) || ampbufnum<0)) {
		SndBuf *buf2 = world->mSndBufs + ampbufnum; 
		
		bufsize = buf2->samples;
		
		pdata= buf2->data; 
		
		if(bufsize==11) {
			for (i=0;i<11;++i)
				unit->m_amps[i]= pdata[i];
		}
	}
	
	unit->m_minfreq= ZIN0(5);
	unit->m_maxfreq= ZIN0(6);
	
	//search qfreqs 
	unit->m_minqband= 0; 
	unit->m_maxqband=unit->m_topqcandidate;
	
	for(i=0;i<numbands; ++i) {
		
		if(qfreqs[i]>=unit->m_minfreq) {unit->m_minqband=i; break;} 
		
	}
	
	for(i=numbands-1;i>=0; --i) {
		
		if(qfreqs[i]<=unit->m_maxfreq) {unit->m_maxqband=i; break;} 
		
	}
	
	//unecessary, already true unit->m_maxqband= sc_min(unit->m_topqcandidate, unit->m_maxqband);
	unit->m_minqband= sc_min(unit->m_minqband, unit->m_maxqband); //necessary test if input stupid
	
	//printf("minfreq %f maxfreq %f minqband %d maxqband %d \n", unit->m_minfreq, unit->m_maxfreq, unit->m_minqband,unit->m_maxqband);
	
	unit->m_currfreq=440;  
	unit->m_hasfreq=0;
	
	unit->mCalcFunc = (UnitCalcFunc)&Qitch_next;
	
}



void Qitch_Dtor(Qitch *unit)
{
	
	RTFree(unit->mWorld, unit->m_prepareFFTBuf);
	RTFree(unit->mWorld, unit->m_FFTBuf);
	
	RTFree(unit->mWorld, unit->m_qfreqs);
	RTFree(unit->mWorld, unit->m_startindex);
	RTFree(unit->mWorld, unit->m_numindices);
	//RTFree(unit->mWorld, unit->m_cumulindices);
	RTFree(unit->mWorld, unit->m_speckernelvals);
	
	//RTFree(unit->mWorld, unit->m_store[0]);
	//RTFree(unit->mWorld, unit->m_store[1]);

	if(unit->m_scfft) {
		SCWorld_Allocator alloc(ft, unit->mWorld);
		scfft_destroy(unit->m_scfft, alloc);
	}
}


void Qitch_next(Qitch *unit, int wrongNumSamples)
{
	//would normally be float,will be cast to int for Tristan's optimisation
	float *in = IN(0);
	
	int numSamples = unit->mWorld->mFullRate.mBufLength;
	
	//float *output = ZOUT(0);
	
	Qitch_preparefft(unit, in, numSamples);
	
	ZOUT0(0)=unit->m_currfreq;
	ZOUT0(1)=unit->m_hasfreq;
	
	//float outval= 0.0;	
	//	for (int i=0; i<numSamples; ++i) {
	//		*++output = outval;
	//	}
	//	
}


//update for unknown SR, overlap

//Tristan Jehan recommends copying ints rather than floats- I say negligible compared to over algorithm costs for the moment
void Qitch_preparefft(Qitch *unit, float* in, int n) {
	
	int i, index = 0, cpt = n, maxindex;
	
	int bufpos= unit->m_bufWritePos;
	
	float * preparefftbuf=unit->m_prepareFFTBuf;
	float * fftbuf= unit->m_FFTBuf;
	
	// Copy input samples into prepare buffer	
	while ((bufpos < unit->m_N) && (cpt > 0)) {
		preparefftbuf[bufpos] = in[index];
		bufpos++;
		index++;
		cpt--;
	}
	
	// When Buffer is full...
	if (bufpos >= unit->m_N) {
		
		// Make a copy of prepared buffer into FFT buffer for computation
		for (i=0; i<unit->m_N; i++) 
			fftbuf[i] = preparefftbuf[i];
		
		//if(unit->m_overlap>0) will be safe as long as overlap=0l overlapindex=0 too
		
		// Save overlapping samples back into buffer- no danger since no indices overwritten
		for (i=0; i<unit->m_overlap; i++) 
			preparefftbuf[i] = preparefftbuf[unit->m_overlapindex+i];
		
		maxindex = n - index + unit->m_overlapindex;
		
		//blockSize less than g_N-g_overlapindex so no problem
		// Copy the rest of incoming samples into prepareFFTBuffer
		for (i=unit->m_overlapindex; i<maxindex; i++) {
			preparefftbuf[i] = in[index];
			index++;
		}
		
		bufpos = maxindex;
		
		//FFT buffer ready- calculate away!
		Qitch_dofft(unit);
	}
	
	
	unit->m_bufWritePos= bufpos;
	//printf("%d \n",bufpos);
	
}



//calculation function once FFT data ready, will be removing windowing! 
void Qitch_dofft(Qitch *unit) {
	
	int i,j;
	
	int fftN= unit->m_N;
	
	float * fftbuf= unit->m_FFTBuf;
	
	float ampthresh = ZIN0(2);
	
	bool ampok=false;
	
	for (j = 0; j < fftN; ++j) {	
		if (fabs(fftbuf[j]) >= ampthresh) {
			ampok = true;
			break;
		}
	}
	
	if(ampok) {
		
		
		//NO WINDOWING FOR CONSTANT Q TRANSFORM 
		//	for (i=0; i<g_N; ++i)
		//		fftbuf[i] *= g_hanning[i];
		//				
		
	//	// Look at the real signal as an interleaved complex vector by casting it.
//		// Then call the transformation function ctoz to get a split complex vector,
//		// which for a real signal, divides into an even-odd configuration.
//		ctoz ((COMPLEX *) fftbuf, 2, &unit->m_vA, 1, unit->m_Nover2);
//		
//		// Carry out a Forward FFT transform
//		fft_zrip(unit->m_vsetup, &unit->m_vA, 1, unit->m_vlog2n, FFT_FORWARD);
//		
//		//correct for scaling error in ALTIVEC
//		//float scale = (float)1.0/(2*n);
//		//vsmul( A.realp, 1, &scale, A.realp, 1, nOver2 );
//		//vsmul( A.imagp, 1, &scale, A.imagp, 1, nOver2 );
//		
//		// The output signal is now in a split real form, ie two arrays for real and imag.  Use the function
//		// ztoc to get a real vector, in format [dc,nyq, bin1realm bin1imag, bin2real, bin2imag, ....] etc
//		ztoc ( &unit->m_vA, 1, (COMPLEX *) fftbuf, 2, unit->m_Nover2);
//		

//need to convert to a usable form for calculations below? fftw output comes out split

		//fftwf_execute(unit->planTime2FFT);
		scfft_dofft(unit->m_scfft);

		//will probably want to store phase first 
		
		// Squared Absolute so get power
		//for (i=0; i<g_N; i+=2)
		//		//i>>1 is i/2 
		//		fftbuf[i>>1] = (fftbuf[i] * fftbuf[i]) + (fftbuf[i+1] * fftbuf[i+1]);
		//	
		
		//amortise state changes:
		
		///////////////////////////////////////////////////////////////
		//constant Q conversion, only need magnitudes
		int qtodo= unit->m_qbands;
		
		float * qfreqs= unit->m_qfreqs;
		int * startindex= unit->m_startindex;
		int * numindices= unit->m_numindices;
		float ** speckernelvals= unit->m_speckernelvals ;
		
		float * qmags = unit->m_qmags;
		
		//int cumul=0;
		
		float magtotal=0.0;
		
		//printf("here 2 %p %p %p \n",speckernelvals, speckernelvals[0], speckernelvals[0]-6);
		
		for (i=0; i<qtodo; ++i) {
			
			float realsum=0.0;
			float imagsum=0.0;
			
			int start= startindex[i];
			int end=start+numindices[i];
			
			float * readbase= speckernelvals[i]-start; //+(unit->m_cumulindices[i])-start;
			
			//printf("%d %p %p %p \n",i, speckernelvals[i], speckernelvals[i]-start, readbase);
			
			for (j=start; j<end; ++j) {
				float mult= readbase[j]; 
				
				//Altivec version
				//realsum+= mult*fftbuf[2*j];
				//imagsum+= mult*fftbuf[2*j+1];
				
				//fftw version
				//realsum+= mult*fftbuf[j];
				//imagsum+= mult*fftbuf[fftN-j];
				
				//sc fft version
				realsum+= mult*fftbuf[2*j];
				imagsum+= mult*fftbuf[2*j+1];
			}
			
			//scale here by 1/(2*g_N)
			
			//sclaing unecessary
			//realsum*=unit->m_fftscale;
			//imagsum*=unit->m_fftscale;
			
			qmags[i]= realsum*realsum+imagsum*imagsum; 
			magtotal+=qmags[i];
			//if(i>70) printf("%d %f   ",i,qmags[i]);
			
		}
		//printf("\n");
		
		/////////////////////////////////////////////////////////
		float max=0.0;
		int maxindex=0;
		
		//done as per Pitch UGen now
		//float intensitycheck = ZIN0(2); //intensity check
		
		//only bother to test if amplitude is sufficient
		//printf("intensity %f check %f \n",magtotal, intensitycheck);
		
		//if(magtotal<intensitycheck) {unit->m_hasfreq=0;}
		//else {
		
		float * pamps= unit->m_amps;
		
		unit->m_hasfreq=1; //could turn off if too close to call...won't bother for now
		
		//pitch detection by cross correlation, only check roots up to 2000 or so, also don't need guard element then! 
		
		//can check even less if use minqband, qmaxband
		
		//int minqband= ZIN0(5);
		//int maxqband= sc_min(unit->m_topqcandidate, ZIN0(6));
		
		//for (i=0; i<unit->m_topqcandidate; ++i) {
		
		for (i=unit->m_minqband; i<unit->m_maxqband; ++i) {
			
			float sum=0.0;
			for (j=0; j<11; ++j) {
				sum+= pamps[j]*qmags[i+g_sieve[j]];
			}
			
			if(sum>max) {max=sum; maxindex=i; 
			//printf("maxsum %f maxind %d \n",max, maxindex);
			}
			
		}
		
		//printf("pitch %f \n",qfreqs[maxindex]);
		
		float pitchcheck = ZIN0(3); 
		
		if(pitchcheck<0.5) { unit->m_currfreq= qfreqs[maxindex];}
		else {
			
			//////////////////////////////////////////////////////////INSTANTANEOUS FREQUENCY TRACK
			
			int k= (int)((qfreqs[maxindex]/unit->m_freqperbin)+0.5);
			
			//printf("check k %f %f %f %d \n",qfreqs[maxindex],unit->m_freqperbin,(qfreqs[maxindex]/unit->m_freqperbin)+0.5,k);
			
			
			//k can't be zero else trouble
			
			//Xhk=0.5*(F.data(k,ii)-0.5*F.data(k+1,ii)-0.5*F.data(k-1,ii));
			//    Xhk2= 0.5*exp(j*2*pi*k/F.N)*(F.data(k,ii)- (0.5*exp(j*2*pi/F.N)*F.data(k+1,ii)) - (0.5*exp(-j*2*pi/F.N)*F.data(k-1,ii)));
			//    
			//    theta2= angle(Xhk2); %atan(imag(Xhk2)/real(Xhk2));
			//    theta= angle(Xhk); %atan(imag(Xhk)/real(Xhk));
			//    
			//    w(ii)= 44100*(abs(theta2-theta))/(2*pi);
			//    
			//	
			
			//instantaneous frequency correction
			float Xhkreal, Xhkimag, Xhk2real, Xhk2imag; 
			
			//Altivec
			//Xhkreal=0.5*((fftbuf[2*k])-(0.5*fftbuf[2*(k+1)])-(0.5*fftbuf[2*(k-1)]));
			//Xhkimag=0.5*((fftbuf[2*k+1])-(0.5*fftbuf[2*(k+1)+1])-(0.5*fftbuf[2*(k-1)+1]));
			
			
			Xhkreal=0.5*((fftbuf[k])-(0.5*fftbuf[k+1])-(0.5*fftbuf[k-1]));
			Xhkimag=0.5*((fftbuf[fftN- k])-(0.5*fftbuf[fftN- (k+1)])-(0.5*fftbuf[fftN- (k-1)]));
			
			
			//complex exponentials to calculate a= exp(j*2*pi*k/F.N)   b= exp(j*2*pi/F.N)  c= exp(-j*2*pi/F.N)
			//float areal= cos(TWOPI*k/g_N);
			//float aimag= sin(TWOPI*k/g_N);
			
			//		float breal= cos(TWOPI/g_N);
			//			float bimag= sin(TWOPI/g_N);
			//			
			//			float creal= breal;
			//			float cimag= -bimag;	
			//			
			//			float tmpreal= fftbuf[2*k] - (0.5*((breal*fftbuf[2*(k+1)]) - (bimag*fftbuf[2*(k+1)+1]))) - (0.5*((creal*fftbuf[2*(k-1)]) - (cimag*fftbuf[2*(k-1)+1])));
			//			float tmpimag= fftbuf[2*k+1] - (0.5*((breal*fftbuf[2*(k+1)+1]) + (bimag*fftbuf[2*(k+1)]))) - (0.5*((creal*fftbuf[2*(k-1)+1]) + (cimag*fftbuf[2*(k-1)])));
			//			
			float calc= (unit->m_twopioverN)*k;
			float areal= cos(calc);
			float aimag= sin(calc);
			
			float breal= unit->realb;
			float bimag= unit->imagb;
			
			
			//float tmpreal= fftbuf[2*k] - (0.5*((breal*fftbuf[2*(k+1)]) - (bimag*fftbuf[2*(k+1)+1]))) - (0.5*((breal*fftbuf[2*(k-1)]) + (bimag*fftbuf[2*(k-1)+1])));
			//float tmpimag= fftbuf[2*k+1] - (0.5*((breal*fftbuf[2*(k+1)+1]) + (bimag*fftbuf[2*(k+1)]))) - (0.5*((breal*fftbuf[2*(k-1)+1]) - (bimag*fftbuf[2*(k-1)])));
			
			
			float tmpreal= fftbuf[k] - (0.5*((breal*fftbuf[k+1]) - (bimag*fftbuf[fftN- (k+1)]))) - (0.5*((breal*fftbuf[k-1]) + (bimag*fftbuf[fftN- (k-1)])));
			float tmpimag= fftbuf[fftN-k] - (0.5*((breal*fftbuf[fftN- (k+1)]) + (bimag*fftbuf[k+1]))) - (0.5*((breal*fftbuf[fftN- (k-1)]) - (bimag*fftbuf[k-1])));
			
			
			Xhk2real= 0.5*(areal*tmpreal- aimag*tmpimag);
			Xhk2imag= 0.5*(areal*tmpimag+ aimag*tmpreal);
			
			//float Xhk2= 0.5*exp(j*2*pi*k/F.N)*(F.data(k,ii)- (0.5*exp(j*2*pi/F.N)*F.data(k+1,ii)) - (0.5*exp(-j*2*pi/F.N)*F.data(k-1,ii)));
			
			float theta2= atan(Xhk2imag/Xhk2real);
			float theta= atan(Xhkimag/Xhkreal);
			
			float freq= ((float)unit->m_SR)*(fabs(theta2-theta))/(TWOPI);
			
			//printf("do you believe freq? %d max %f min %f result %f\n",k,unit->m_maxfreq, unit->m_minfreq, freq);
			
			//check no dodgy answers
			if((freq<unit->m_minfreq) || (freq>unit->m_maxfreq)) {unit->m_hasfreq=0;}
			else
				unit->m_currfreq= freq;
			
		}
		
		
		}	else {unit->m_hasfreq=0;}	
}	





