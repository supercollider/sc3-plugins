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

//Concat, Concat2: real-time concatenative synthesis UGens by Nick Collins (after Schwarz, Casey, Sturm et al)
//originally for my PhD 2006

//I developed for 44100 sample rate, with 256 FFT (without overlap) for low latency operation. It will probably work at 48000 without major problems
//assumes block size is 64

//features- ZCR over last 1024 samples = 4 frames, RMS energy over frame, then from the FFT- spec centroid, spec variance, spec rolloff

#include "SC_PlugIn.h"
//#include "SCComplex.h"
//#include "FFT_UGens.h"
#include "SC_fftlib.h"
//#include "SC_Constants.h"
//
//#include <vecLib/vecLib.h>
//#include <string.h>
//#include <math.h>
//#include <stdlib.h>
#include <stdio.h>

static InterfaceTable *ft;

struct Concat : Unit {

	//essential constants
	int m_sr;
	int m_blocksize;
	int m_fftsize;
	int m_nover2;
	int m_blocksperframe;
	int m_samplesforzcr;

	//FFT data
	int m_bufWritePos;

	scfft *m_scfftsource, *m_scfftcontrol;
	float * m_FFTBufsource, *m_FFTBufcontrol;

	//To fix a database once captured enough
	int m_freezestore;

	//stored samples

	float * m_controlstore;
	float * m_sourcestore;
	int m_controlcounter;
	int m_sourcecounter;
	int m_sourcesize;
	int m_controlsize;

	//stored features (for source stream only)
	int m_sourceframes;
	int m_featurecounter;

	//time domain
	float * m_rms;
	float * m_zcr;

	//frequency domain
	float * m_speccentroid;
	float * m_spectilt;

	//control playback
	int m_matchlocation;
	int m_matchcounter;
	int m_matchframes;
	int m_fadeoutlocation;

};

struct Concat2 : public Concat {

	int m_nover4;
	float m_matchamp,m_fadeoutamp;	//will also scale based on input amplitude, to avoid noise when no significant input

};


extern "C"
{
	//required interface functions
	void Concat_next(Concat *unit, int numSamples);
	void Concat_Ctor(Concat *unit);
	void Concat_Dtor(Concat *unit);

	int Concat_CtorCommon(Concat *unit);
	void Concat_DtorCommon(Concat *unit);

	void Concat2_next(Concat2 *unit, int numSamples);
	void Concat2_Ctor(Concat2 *unit);
	void Concat2_Dtor(Concat2 *unit);

}

//other functions
void Concat_dofft(Concat *unit, scfft *, float *);	//call FFT and in place data to powers
void sourcefeatures(Concat *unit, float * fftbuf);
void matchfeatures(Concat *unit, float * fftbuf);
float calcst(float * fftbuf);
float calcsc(float * fftbuf, int nover2);
float calcsc2(float * fftbuf, int nover2);
void sourcefeatures2(Concat2 *unit, float * fftbuf);
void matchfeatures2(Concat2 *unit, float * fftbuf);



int Concat_CtorCommon(Concat *unit) {

	//CHECK SAMPLING RATE AND BUFFER SIZE
	unit->m_blocksize = unit->mWorld->mFullRate.mBufLength;

	if(unit->m_blocksize!=64) {
		printf("Concat complains: block size not 64, you have %d\n", unit->m_blocksize);
		return 0;
	}

	unit->m_sr = unit->mWorld->mSampleRate; //(int)(unit->mWorld->mSampleRate+0.1);

	if(unit->m_sr!=44100) {
		printf("Concat complains: sample rate not 44100, you have %d\n", unit->m_sr);
		//return 0; //not necessarily catastrophic
	}

	//printf("sr %d block size %d \n", unit->m_sr, unit->m_blocksize);

	///set up sizes

	unit->m_fftsize=256; //could make conditional on SR, blocksize must divide this!

	unit->m_nover2=unit->m_fftsize/2;

	unit->m_blocksperframe=unit->m_fftsize/unit->m_blocksize; //should be an integer- excat if power of two, else an approximation (good enough for RMS, zerocrossing counts)

	unit->m_samplesforzcr= (unit->m_blocksperframe*unit->m_blocksize)*4; //will also give number of stored samples of the control input, calculated in this way to guarantee an integer

	////////FFT data///////////

	unit->m_FFTBufsource = (float*)RTAlloc(unit->mWorld, unit->m_fftsize * sizeof(float));
	unit->m_FFTBufcontrol = (float*)RTAlloc(unit->mWorld, unit->m_fftsize * sizeof(float));

	//unit->m_FFTBufsourceafter = (float*)RTAlloc(unit->mWorld, unit->m_fftsize * sizeof(float));
	//unit->m_FFTBufcontrolafter = (float*)RTAlloc(unit->mWorld, unit->m_fftsize * sizeof(float));

	unit->m_bufWritePos = 0;


	SCWorld_Allocator alloc(ft, unit->mWorld);
	//no overlap
	unit->m_scfftsource = scfft_create(unit->m_fftsize, unit->m_fftsize, kHannWindow, unit->m_FFTBufsource, unit->m_FFTBufsource, kForward, alloc);
	unit->m_scfftcontrol = scfft_create(unit->m_fftsize, unit->m_fftsize, kHannWindow, unit->m_FFTBufcontrol, unit->m_FFTBufcontrol, kForward, alloc);

	//stores
	unit->m_sourcesize=(((int)(floor(ZIN0(2)*unit->m_sr + .5f)))/(unit->m_fftsize))*(unit->m_fftsize); //integer divide then multiply to make sure its an integer multiple of frames
	unit->m_controlsize= unit->m_samplesforzcr;

	unit->m_controlstore = (float*)RTAlloc(unit->mWorld, unit->m_controlsize * sizeof(float));
	unit->m_sourcestore= (float*)RTAlloc(unit->mWorld, unit->m_sourcesize * sizeof(float));;
	unit->m_controlcounter=0;
	unit->m_sourcecounter=0;

	Clear(unit->m_controlsize, unit->m_controlstore);
	Clear(unit->m_sourcesize, unit->m_sourcestore);

	//features
	unit->m_sourceframes= unit->m_sourcesize/unit->m_fftsize; //should divide perfectly due to earlier correction
	unit->m_featurecounter=0;

	unit->m_rms= (float*)RTAlloc(unit->mWorld, unit->m_sourceframes * sizeof(float));
	unit->m_zcr= (float*)RTAlloc(unit->mWorld, unit->m_sourceframes * sizeof(float));
	unit->m_speccentroid= (float*)RTAlloc(unit->mWorld, unit->m_sourceframes * sizeof(float));
	unit->m_spectilt= (float*)RTAlloc(unit->mWorld, unit->m_sourceframes * sizeof(float));

	Clear(unit->m_sourceframes, unit->m_rms);
	Clear(unit->m_sourceframes, unit->m_zcr);
	Clear(unit->m_sourceframes, unit->m_speccentroid);
	Clear(unit->m_sourceframes, unit->m_spectilt);

	//match setup
	unit->m_matchlocation=0;
	unit->m_matchcounter=1; //immediately seek a match
	unit->m_matchframes=1;
	unit->m_fadeoutlocation= (-1); //negative means do nothing

	return 1;

}

void Concat_DtorCommon(Concat *unit) {

	if(unit->m_scfftsource) {
	SCWorld_Allocator alloc(ft, unit->mWorld);
	scfft_destroy(unit->m_scfftsource, alloc);
	scfft_destroy(unit->m_scfftcontrol, alloc);
	}

	RTFree(unit->mWorld, unit->m_FFTBufsource);
	RTFree(unit->mWorld, unit->m_FFTBufcontrol);
	//RTFree(unit->mWorld, unit->m_hanning);

	RTFree(unit->mWorld, unit->m_controlstore);
	RTFree(unit->mWorld, unit->m_sourcestore);

	RTFree(unit->mWorld, unit->m_rms);
	RTFree(unit->mWorld, unit->m_zcr);
	RTFree(unit->mWorld, unit->m_speccentroid);
	RTFree(unit->mWorld, unit->m_spectilt);


}




void Concat_Ctor(Concat* unit) {

	int check = Concat_CtorCommon(unit);

	if (check==1)
		unit->mCalcFunc = (UnitCalcFunc)&Concat_next;
	else
	{
		SETCALC(*ClearUnitOutputs);
		unit->mDone = true;
	}
}




void Concat2_Ctor(Concat2* unit) {

	int check = Concat_CtorCommon(unit);

	if (check==1) {

		unit->m_nover4=unit->m_fftsize/4;

		unit->m_matchamp=0.0;
		unit->m_fadeoutamp=0.0;

		unit->mCalcFunc = (UnitCalcFunc)&Concat2_next;

	}
	else {
		SETCALC(*ClearUnitOutputs);
		unit->mDone = true;
	}
}





void Concat_Dtor(Concat *unit)
{

	Concat_DtorCommon(unit);

}

void Concat2_Dtor(Concat2 *unit)
{

	Concat_DtorCommon(unit);

}

void Concat_next(Concat *unit, int numSamples) {

	float *controlin = IN(0);
	float *sourcein = IN(1);

	float *output = ZOUT(0);

	float * store=unit->m_sourcestore;

	int sourcecounter= unit->m_sourcecounter;
	int sourcesize=unit->m_sourcesize;

	float * control=unit->m_controlstore;
	int controlcounter= unit->m_controlcounter;
	int controlsize=unit->m_controlsize;

	int bufpos= unit->m_bufWritePos;

	float * fftbuf1=unit->m_FFTBufsource;
	float * fftbuf2=unit->m_FFTBufcontrol;

	float freeze = ZIN0(6);

	for(int j=0; j<numSamples; ++j) {

		float val1=sourcein[j];
		float val2= controlin[j];

		//only write into buffer if allowed
		if(freeze<0.5) {
			store[sourcecounter]= val1;

			sourcecounter= (sourcecounter+1)%sourcesize;
		}

		fftbuf1[bufpos]=val1;

		control[controlcounter]= val2;
		controlcounter= (controlcounter+1)%controlsize;
		fftbuf2[bufpos]=val2;

		++bufpos;
	}

	unit->m_sourcecounter=sourcecounter;
	unit->m_controlcounter=controlcounter;

	if (bufpos==unit->m_fftsize) {

		//only update source features if recording
		if (freeze<0.5) {
			//frame ready- could be within if?
			Concat_dofft(unit,unit->m_scfftsource, fftbuf1);
			//calc source FFT and features to store
			sourcefeatures(unit,fftbuf1);
		}


		//if new match required
		//calc control FFT and features for match
		if(unit->m_matchcounter>= unit->m_matchframes) {

			Concat_dofft(unit,unit->m_scfftcontrol, fftbuf2);
			matchfeatures(unit, fftbuf2);

			} else
			//otherwise increase matchcounter (it was reset in the other branch)
			++unit->m_matchcounter;

		//feature counter ALWAYS updated to keep in step with write position- updated after matchfeatures so matchfeatures references to the nearest last frame
		unit->m_featurecounter=(unit->m_featurecounter+1)%(unit->m_sourceframes);


		//restart FFT data collection
		bufpos=0;
	}

	unit->m_bufWritePos=bufpos;

	int readpos= unit->m_matchlocation;

	int fadepos= unit->m_fadeoutlocation;

	//crossfade on this block
	if(fadepos>=0) {

		for (int i=0; i<numSamples; ++i) {

			float fade= (float)i/((float)numSamples);
			//fade *= fade;

			*++output = ((1.0-fade)*store[fadepos]) + (fade*store[readpos]);
			++readpos;
			++fadepos;
		}

		unit->m_fadeoutlocation = (-1);
	} else { //else just read

		for (int i=0; i<numSamples; ++i) {
			*++output = store[readpos];
			++readpos;
		}

	}

	//only need to modulo here since 64 always divides store size
	unit->m_matchlocation= (readpos)%sourcesize;

}



void Concat2_next(Concat2 *unit, int numSamples)
{
	float *controlin = IN(0);
	float *sourcein = IN(1);

	float *output = ZOUT(0);

	float * store=unit->m_sourcestore;

	int sourcecounter= unit->m_sourcecounter;
	int sourcesize=unit->m_sourcesize;

	float * control=unit->m_controlstore;
	int controlcounter= unit->m_controlcounter;
	int controlsize=unit->m_controlsize;

	int bufpos= unit->m_bufWritePos;

	float * fftbuf1=unit->m_FFTBufsource;
	float * fftbuf2=unit->m_FFTBufcontrol;

	float freeze = ZIN0(6);

	for(int j=0; j<numSamples; ++j) {

		float val1=sourcein[j];
		float val2= controlin[j];

		//only write into buffer if allowed
		if(freeze<0.5) {
			store[sourcecounter]= val1;

			sourcecounter= (sourcecounter+1)%sourcesize;
		}

		fftbuf1[bufpos]=val1;

		control[controlcounter]= val2;
		controlcounter= (controlcounter+1)%controlsize;
		fftbuf2[bufpos]=val2;

		++bufpos;
	}

	unit->m_sourcecounter=sourcecounter;
	unit->m_controlcounter=controlcounter;

	if (bufpos==unit->m_fftsize) {

		//printf("fft block\t");

		//frame ready
		Concat_dofft(unit,unit->m_scfftsource, fftbuf1);

		//only update source features if recording
		if (freeze<0.5)
			//calc source FFT and features to store
			sourcefeatures2(unit,fftbuf1);

		//frame ready, amazingly, this line was missing before! so was calculating based on time domain versus frequency domain!

		if(unit->m_matchcounter>= unit->m_matchframes) {
			//if new match required
			//calc control FFT and features for match
			Concat_dofft(unit,unit->m_scfftcontrol, fftbuf2);
			matchfeatures2(unit, fftbuf2);

		} else
			//otherwise increase matchcounter (it was reset in the other branch)
			++unit->m_matchcounter;

		//feature counter ALWAYS updated to keep in step with write position- updated after matchfeatures so matchfeatures references to the nearest last frame
		unit->m_featurecounter=(unit->m_featurecounter+1)%(unit->m_sourceframes);


		//restart FFT data collection
		bufpos=0;
	}

	unit->m_bufWritePos=bufpos;

	int readpos= unit->m_matchlocation;

	int fadepos= unit->m_fadeoutlocation;

	float readamp= unit->m_matchamp;

	float fadeamp= unit->m_fadeoutamp;

	//crossfade on this block
	if(fadepos>=0) {

		for (int i=0; i<numSamples; ++i) {

			float fade= (float)i/((float)numSamples);
			//fade *= fade;

			*++output = ((1.0-fade)*store[fadepos]*fadeamp) + (fade*store[readpos]*readamp);
			++readpos;
			++fadepos;
		}

		unit->m_fadeoutlocation = (-1);
	} else { //else just read

		for (int i=0; i<numSamples; ++i) {
			*++output = store[readpos]*readamp;
			++readpos;
		}

	}

	//only need to modulo here since 64 always divides store size
	unit->m_matchlocation= (readpos)%sourcesize;

}





//calculated with SC findlogbins program

//find log bins program for 10 bins, for sr=44100, fftsize=256
//
//(
//var sr, binsize, fftsize, nyquist, num, numbins, divisions;
//var logbin;
//var top, bottom;
//var tmp;
//
//sr=44100;
//fftsize=256;
//numbins=fftsize.div(2);
//
//binsize=sr/fftsize;
//
//num=10;
//
//nyquist= sr*0.5;
//
//top= log10(nyquist);
//bottom=log10(binsize);
//
//tmp= (top-bottom)*((num).reciprocal);
//
//divisions=bottom+Array.series(num,tmp,tmp);
//
//Post << divisions << nl;
//
////logbin= Array.fill(numbins, {arg i; log10((i+1)*binsize +0.001)});
//
////invert to find bin positions
//((10**divisions)/binsize).floor.asInteger; //.round(1.0).asInteger
//)

int stbins[11]= {0, 1, 2, 4, 6, 11, 18, 29, 48, 78, 128 };

//avoid bin 0
//int stbins[11]= {1, 2, 3, 5, 9, 14, 21, 34, 52, 82, 128};

float calcst(float * fftbuf) {

	int i;

	float sum;

	//take bins into 10 log spaced bins (averaging power), take log of the power, calculate the slope of the least-squares line ((8) prob and stats, p280 , 2nd ed., Schaum's)
	float average[10];

	int j, firstindex, secondindex;

	float avav=0.0;

	for (i=0; i<10; ++i) {

		sum=0.0;

		firstindex=stbins[i];
		secondindex=stbins[i+1];

		for (j=firstindex; j<secondindex; ++j) {
			sum += fftbuf[j];
		}

		average[i]= log10(sum/(secondindex-firstindex)+0.001);
		avav +=average[i];
	}

	avav /=10;

	float topsum=0.0, bottomsum=0.0;
	float tmp;

	for (i=0; i<10; ++i) {

		tmp=(average[i]-avav);

		topsum += tmp*((i+1)-5.5);
		bottomsum += tmp*tmp;

	}

	float st;

	if (bottomsum>0.001)
		st= topsum/bottomsum;
	else
		st=(-1000.0);

	//
//	if(st<0.0) st= -(log10(1-st));
//	else st= log10(1+st);
//
//	//clamp
//	if(st<-2) st=-2;
//	if (st>2) st=2;
//
//	st= (st+2)*0.25;
//

	st= atan(st); //now between -pi/2 and pi/2

	st= (st+pi2)/pi;

	//printf("c1 %f avav %f topsum %f bottomsum %f st %f test %f test2 %f \n", average[0], avav, topsum, bottomsum, st, topsum/bottomsum, atan(topsum/bottomsum));

	return st;

}


float calcsc(float * fftbuf, int nover2) {

	int i;
	float sum=1.0f, val;
	float centroid=0.0f;

	for (i=1; i<nover2; ++i) {

		val= fftbuf[i];

		centroid+= val*i;
		sum+= val;
	}

	//printf("sum %f\n", sum);
	//centroid=centroid/sum;

	//avoid division by zero threat
	if(sum>0.01f)
		centroid=centroid/sum;
	else
		centroid=centroid;

	centroid=log2((centroid/(nover2))+1.0f);

	return centroid; //normalised bin location
}



float calcsc2(float * fftbuf, int nover2) {

	int i;
	float sum=1.0f, val;
	float centroid=0.0f;

	for (i=1; i<nover2; ++i) {

		val= log(fftbuf[i]+1); //no log before!

		centroid+= val*i;
		sum+= val;
	}

	//causes divide by zero
	//centroid=centroid/sum;

	if(sum>0.01f)
		centroid=centroid/sum;
	else
		centroid=centroid;

	centroid=log2((centroid/(nover2))+1);

	return centroid; //normalised bin location
}



void sourcefeatures(Concat *unit, float * fftbuf) {
	int i, pos;

	float * store=unit->m_sourcestore;

	int sourcecounter= unit->m_sourcecounter;
	int sourcesize=unit->m_sourcesize;

	int featurecounter= unit->m_featurecounter;

	////////ZCR feature (counts - to + only)

	int zcrsize= unit->m_samplesforzcr;

	int zcrstart= (sourcecounter + sourcesize- zcrsize)%sourcesize;

	int zcrcount=0;

	float prevval=0.0, val;

	for (i=0; i<zcrsize; ++i) {

		pos= (zcrstart+i)%sourcesize;

		val= store[pos];

		if ((val>=(-0.00000001)) && (prevval<0.00000001))
			++zcrcount;

		prevval=val;
	}

	float zcrfeature= log10(zcrcount/(0.5*zcrsize*0.2)+1);

	if(zcrfeature>2) zcrfeature=2;

	unit->m_zcr[featurecounter]= zcrfeature*0.5; //normalise how? divide by zcrsize/2 or more and clamp to 1? Could correct with weighting

	////RMS
	int framesize=unit->m_fftsize;
	int framestart= (sourcecounter + sourcesize- framesize)%sourcesize;

	//float sum=0.0;
	float maxrms=0.0;

	for (i=0; i<framesize; ++i) {

		pos= (framestart+i)%sourcesize;

		val= store[pos];

		val=val*val;

		if(val>maxrms) {
		maxrms=val;
		}

		//sum+= val*val;
	}

	//sum=sum/framesize;

	//take into pseudo-decibels to even out volume differences on a more psychoacoustic scale
	unit->m_rms[featurecounter]= log10(1+(maxrms*9)); //sqrt(sum/framesize); //root mean square from 0.0 to 1.0

	//printf("SOURCE rms %f counter %d\n",unit->m_rms[featurecounter], featurecounter);


	unit->m_speccentroid[featurecounter]=calcsc(fftbuf, unit->m_nover2);

	//spectral tilt
	unit->m_spectilt[featurecounter]=calcst(fftbuf);

	//update
	//printf("zcr %f rms %f sc %f st %f counter %d\n",unit->m_zcr[featurecounter],unit->m_rms[featurecounter],unit->m_speccentroid[featurecounter],unit->m_spectilt[featurecounter], featurecounter);

}






//calculate features for current control input then find a match in the source
void matchfeatures(Concat *unit, float * fftbuf) {
	int i, pos;

	float * store=unit->m_controlstore;

	int controlcounter= unit->m_controlcounter;
	int controlsize=unit->m_controlsize;

	////////ZCR feature (counts - to + only)
	int zcrsize= unit->m_samplesforzcr;

	int zcrcount=0;

	float prevval=0.0, val;

	for (i=0; i<zcrsize; ++i) {

		val= store[i];

		if ((val>=(-0.00000001)) && (prevval<0.00000001))
			++zcrcount;

		prevval=val;
	}

	float zcrfeature= log10(zcrcount/(0.5*zcrsize*0.2)+1);

	if(zcrfeature>2) zcrfeature=2;

	zcrfeature= zcrfeature*0.5;

	//float zcrfeature= zcrcount/(0.5*zcrsize); //normalise how? divide by zcrsize/2 or more and clamp to 1? Could correct with weighting

	////RMS
	int framesize=unit->m_fftsize;
	int framestart= (controlcounter + controlsize- framesize)%controlsize;

	//float sum=0.0;
	float maxrms=0.0;

	for (i=0; i<framesize; ++i) {

		pos= (framestart+i)%controlsize;

		val= store[pos];

			val=val*val;

		if(val>maxrms) {
		maxrms=val;
		}

		//sum+= val*val;
	}

	//sum/framesize
	float rmsfeature= log10(1+((maxrms)*9)); //sqrt(sum/framesize); //root mean square from 0.0 to 1.0

	//printf("TARGET rms %f\n",rmsfeature);

	float scfeature=calcsc(fftbuf, unit->m_nover2);

	float stfeature=calcst(fftbuf);

	//find locations to search
	float seekstart= ZIN0(3); //in seconds
	float seekdur= ZIN0(4);	  //in seconds

	int featurecounter= unit->m_featurecounter;


	int sourceframes= unit->m_sourceframes;

	int beginsearch= (featurecounter + sourceframes - ((int)((seekstart*unit->m_sr)/(unit->m_fftsize))))%(sourceframes);
	int searchlength= (int)((seekdur*unit->m_sr)/(unit->m_fftsize));

	searchlength= sc_max(searchlength,1);

	//find best match

	float * zcr= unit->m_zcr;
	float * rms= unit->m_rms;
	float * sc= unit->m_speccentroid;
	float * st= unit->m_spectilt;

	float zcrweight= ZIN0(7);
	float rmsweight= ZIN0(8);
	float scweight=	 ZIN0(9);
	float stweight=	 ZIN0(10);

	int best=0;
	float bestscore= 1000000000.0;

	float zcrdiff, rmsdiff, scdiff, stdiff, score;

	//not sure if I like this or not!
	float randscore= ZIN0(11);

	RGen& rgen = *unit->mParent->mRGen;

	for (i=0; i<searchlength; ++i) {

		pos= (beginsearch+i)%sourceframes;

		zcrdiff= zcrfeature- zcr[pos];
		rmsdiff= rmsfeature- rms[pos];
		scdiff= scfeature- sc[pos];
		stdiff= stfeature- st[pos];

		score= zcrweight*(zcrdiff*zcrdiff) + rmsweight*(rmsdiff*rmsdiff) + scweight*(scdiff*scdiff) + stweight*(stdiff*stdiff)+(rgen.frand()*randscore);

		//could add match randomisation factor here score=score+ZIN0()

		if (score< bestscore) {
			bestscore= score;
			best=i;
		}
	}

	int answer= (beginsearch+best)%sourceframes;

	//printf("zcr %f rms %f sc %f\n",zcrfeature,rmsfeature,scfeature);

	zcrdiff= zcrfeature- zcr[answer];
	rmsdiff= rmsfeature- rms[answer];
	scdiff= scfeature- sc[answer];

	score= zcrweight*(zcrdiff*zcrdiff) + rmsweight*(rmsdiff*rmsdiff) + scweight*(scdiff*scdiff);

	//printf("zcrdiff %f rmsdiff %f scdiff %f score %f answer %d\n",zcrdiff,rmsdiff,scdiff,score, answer);

	//store best match sample and reset read pointer and fft frame counter
	//setting read out on current sample read

	unit->m_fadeoutlocation= unit->m_matchlocation; //next sample to be read now faded
	unit->m_matchlocation=answer*(unit->m_fftsize);
	unit->m_matchcounter=0;

	float matchlength=  ZIN0(5);

	int matchframes= (int)((matchlength*unit->m_sr)/(unit->m_fftsize));

	unit->m_matchframes= sc_max(matchframes,1);

	//printf("begin %d length %d answer %d\n",beginsearch, searchlength, answer);

}






void sourcefeatures2(Concat2 *unit, float * fftbuf) {
	int i, pos;

	float * store=unit->m_sourcestore;

	int sourcecounter= unit->m_sourcecounter;
	int sourcesize=unit->m_sourcesize;

	int featurecounter= unit->m_featurecounter;

	////////ZCR feature (counts - to + only)

	int zcrsize= unit->m_samplesforzcr;

	int zcrstart= (sourcecounter + sourcesize- zcrsize)%sourcesize;

	int zcrcount=0;

	float prevval=0.0, val;

	for (i=0; i<zcrsize; ++i) {

		pos= (zcrstart+i)%sourcesize;

		val= store[pos];

		if ((val>=(-0.00000001)) && (prevval<0.00000001))
			++zcrcount;

		prevval=val;
	}

	float zcrfeature= log10(zcrcount/(0.5*zcrsize*0.2)+1);

	if(zcrfeature>2) zcrfeature=2;

	unit->m_zcr[featurecounter]= zcrfeature*0.5; //normalise how? divide by zcrsize/2 or more and clamp to 1? Could correct with weighting

	////RMS
	int framesize=unit->m_fftsize;
	int framestart= (sourcecounter + sourcesize- framesize)%sourcesize;

	float maxrms=0.0; //sum=0.0,

	for (i=0; i<framesize; ++i) {

		pos= (framestart+i)%sourcesize;

		val= store[pos];

		val=val*val;

		if(val>maxrms) {
			maxrms=val;
		}

		//sum+= val*val;
	}

	//sum=sum/framesize;

	//rmsfeature= sc_min(rmsfeature,1.0); //if want clamped to 1

	//take into pseudo-decibels to even out volume differences on a more psychoacoustic scale
	unit->m_rms[featurecounter]= log10(1+(maxrms*9)); //sqrt(sum/framesize); //root mean square from 0.0 to 1.0

	//printf("SOURCE rms %f counter %d\n",unit->m_rms[featurecounter], featurecounter);


	unit->m_speccentroid[featurecounter]=sc_min(2*calcsc2(fftbuf, unit->m_nover4),1.0); //was unit->m_nover2 and no clamp

	//spectral tilt
	unit->m_spectilt[featurecounter]=calcst(fftbuf);

	//update
	//printf("zcr %f rms %f sc %f st %f counter %d\n",unit->m_zcr[featurecounter],unit->m_rms[featurecounter],unit->m_speccentroid[featurecounter],unit->m_spectilt[featurecounter], featurecounter);

}






//calculate features for current control input then find a match in the source
void matchfeatures2(Concat2 *unit, float * fftbuf) {
	int i, pos;
	float val;
	int matchfailure;

	float threshold= ZIN0(12);

	matchfailure= 0;

	float * store=unit->m_controlstore;

	int controlcounter= unit->m_controlcounter;
	int controlsize=unit->m_controlsize;

	////RMS
	int framesize=unit->m_fftsize;
	int framestart= (controlcounter + controlsize- framesize)%controlsize;

	float maxrms=0.0; //sum=0.0,

	for (i=0; i<framesize; ++i) {

		pos= (framestart+i)%controlsize;

		val= store[pos];

		val=val*val;

		if(val>maxrms) {
			maxrms=val;
		}

		//sum+= val*val;
	}

	//sum/framesize

	//log of sum?

	float rmsfeature= log10(1+((maxrms)*9)); //sqrt(sum/framesize); //root mean square from 0.0 to 1.0

	//could sc_min(rmsfeature,1.0) if want clamped to 0.0 to 1

	//rmsfeature>0.01
	//printf("TARGET rms %f maxrms %f \n",rmsfeature, maxrms);


	//may have as param
	if(rmsfeature>threshold) {


		////////ZCR feature (counts - to + only)
		int zcrsize= unit->m_samplesforzcr;

		int zcrcount=0;

		float prevval=0.0;

		for (i=0; i<zcrsize; ++i) {

			val= store[i];

			if ((val>=(-0.00000001)) && (prevval<0.00000001))
				++zcrcount;

			prevval=val;
		}

		float zcrfeature= log10(zcrcount/(0.5*zcrsize*0.2)+1);

		if(zcrfeature>2) zcrfeature=2;

		zcrfeature= zcrfeature*0.5;

		//float zcrfeature= zcrcount/(0.5*zcrsize); //normalise how? divide by zcrsize/2 or more and clamp to 1? Could correct with weighting


		float scfeature=calcsc2(fftbuf, unit->m_nover4); //was m_nover2

		scfeature= sc_min(2*scfeature,1.0);

		float stfeature=calcst(fftbuf);

		//find locations to search
		float seekstart= ZIN0(3); //in seconds
		float seekdur= ZIN0(4);	  //in seconds

		int featurecounter= unit->m_featurecounter;


		int sourceframes= unit->m_sourceframes;

		int beginsearch= (featurecounter + sourceframes - ((int)((seekstart*unit->m_sr)/(unit->m_fftsize))))%(sourceframes);
		int searchlength= (int)((seekdur*unit->m_sr)/(unit->m_fftsize));

		searchlength= sc_max(searchlength,1);

		//find best match

		float * zcr= unit->m_zcr;
		float * rms= unit->m_rms;
		float * sc= unit->m_speccentroid;
		float * st= unit->m_spectilt;

		float zcrweight= ZIN0(7);
		float rmsweight= ZIN0(8);
		float scweight=	 ZIN0(9);
		float stweight=	 ZIN0(10);

		int best=(-1);
		float bestscore= 1000000000.0;

		float zcrdiff, rmsdiff, scdiff, stdiff, score;

		//not sure if I like this or not!
		float randscore= ZIN0(11);

		RGen& rgen = *unit->mParent->mRGen;

		for (i=0; i<searchlength; ++i) {

			pos= (beginsearch+i)%sourceframes;

			zcrdiff= zcrfeature- zcr[pos];

			float rmstest= rms[pos];

			if(rmstest>threshold) {

				rmsdiff= rmsfeature- rmstest;
				scdiff= scfeature- sc[pos];
				stdiff= stfeature- st[pos];

				score= zcrweight*(zcrdiff*zcrdiff) + rmsweight*(rmsdiff*rmsdiff) + scweight*(scdiff*scdiff) + stweight*(stdiff*stdiff)+(rgen.frand()*randscore);

				//could add match randomisation factor here score=score+ZIN0()

				if (score< bestscore) {
					bestscore= score;
					best=i;
				}

			}
		}

		if(best>=0) {

			int answer= (beginsearch+best)%sourceframes;

			//printf("zcr %f rms %f sc %f\n",zcrfeature,rmsfeature,scfeature);

			zcrdiff= zcrfeature- zcr[answer];
			rmsdiff= rmsfeature- rms[answer];
			scdiff= scfeature- sc[answer];

			score= zcrweight*(zcrdiff*zcrdiff) + rmsweight*(rmsdiff*rmsdiff) + scweight*(scdiff*scdiff);

			//printf("zcrdiff %f rmsdiff %f scdiff %f score %f answer %d\n",zcrdiff,rmsdiff,scdiff,score, answer);

			//store best match sample and reset read pointer and fft frame counter
			//setting read out on current sample read

			unit->m_fadeoutlocation= unit->m_matchlocation; //next sample to be read now faded

			unit->m_fadeoutamp= unit->m_matchamp;
			unit->m_matchlocation=answer*(unit->m_fftsize);
			unit->m_matchcounter=0;

			unit->m_matchamp=1.0;

			float matchlength=  ZIN0(5);

			int matchframes= (int)((matchlength*unit->m_sr)/(unit->m_fftsize));

			unit->m_matchframes= sc_max(matchframes,1);

		} else {
			matchfailure=1;

		}


		//printf("begin %d length %d answer %d\n",beginsearch, searchlength, answer);
	} else {


		matchfailure=1;

	}


	//cope with match failure- so output zeroes, must finish fade of any current material
	if(matchfailure==1) {

		//always fade out old at this point, will be silent
		unit->m_fadeoutlocation= unit->m_matchlocation; //next sample to be read now faded
		unit->m_fadeoutamp= unit->m_matchamp;

		//just read something random but at zero amp
		unit->m_matchamp=0.0;
		unit->m_matchlocation=0;
		unit->m_matchcounter=0;
		unit->m_matchframes= 1;

	}

}







//calculation function once FFT data ready, in place on fftbuf passed in
void Concat_dofft(Concat *unit,scfft * scf, float * fftbuf) {

	int i;

	scfft_dofft(scf);

	int n= unit->m_fftsize;

	//format is dc, nyquist, bin[1] ,real, bin[1].imag, etc

	//fftbuf[0] already bin 0
	fftbuf[0]= fftbuf[0]* fftbuf[0]; //get power

	float val1, val2;
	// Squared Absolute so get power
	for (i=2; i<n; i+=2) {
		val1 = fftbuf[i];
		val2 = fftbuf[i+1];
		//i>>1 is i/2
		fftbuf[i>>1] = (val1*val1)+(val2*val2);
	}

}



PluginLoad(Concat) {

	ft = inTable;

	DefineDtorUnit(Concat);
	DefineDtorUnit(Concat2);
}
