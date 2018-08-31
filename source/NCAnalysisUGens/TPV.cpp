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

//Tracking Phase Vocoder following McAualay and Quatieri model from IEEE Trans acoustics, speech and signal processing vol assp-34(4) aug 1986

//for spectral interpolation would like zero padding!

//big efficiency loss because of ifs for rounding in next function; just add very large multiple of g_costableTPV to guarantee positive?

//potential problems= be more careful with difference of S and T, may need an overlap add structure

//WORKS!
//theta1 goes really negative, for faster frequencies; can take mod 2pi?
//needs tidying up, but basically sound; make more efficient in rendering loop?

//debugging UGens:
//http://lists.create.ucsb.edu/pipermail/sc-dev/2003-October/005103.html

//synthesis seems to be the most expensive part
//Hanning window ten times better



//InterfaceTable *ft;

//const int g_maxpeaks = 80;
#include "NCAnalysis.h"
#include <stdio.h>


//cubic interpolation of phase parameters for formula (37) where t is from 0 to 1 as interpolation parameter
struct PartialTrack {
	float theta1, omega1, theta2, omega2, alpha, beta; //cubic interpolation of phase
	float amp1, amp2; //linear interpolation of amplitude
};


//freq in this case is omega, angular frequency in radians per sample = 2*PI*f/SR
struct TPVPeak {
	float mag, freq, phase;  //improve frequency estimate by interpolation over amplitude of nearby points, or by time-frequency reassignment
};

//peak must satisfy amp(f-1)<amp(f)>amp(f+1), then cubic interpolation over local points (see wavetable interpolation code for processing)

struct TPV : Unit {

	//final list of peaks is size at most numpeaks(n) + numpeaks(n+1). reasonably around max of the two.
	//as long as have birth and death resolved for each peak in the two lists, can synthesise up to curent frame. So output latency is one FFT frame

	int m_sr;
	int m_blocksize;
	int m_windowsize;
	int m_hopsize;
	int m_nover2;

	int m_maxpeaks; //80?
	//int m_maxlistsize; //double m_maxpeaks

	//or for each partial to be rendered persistent data for rendering need phasek, angfreqk (omegak), alphak, betak as per (37)
	PartialTrack * m_tracks; //space for double maxpeaks if all birth and die at once!
	int m_numtracks;

	//use buffer swapping of pointer as needed
	TPVPeak * m_prevpeaks;
	TPVPeak * m_newpeaks;
	int m_numprevpeaks;
	int m_numnewpeaks;

	//keep track of how many samples resynthesised in current run
	int m_resynthesisposition;
	//all precalculated to save cycles in main rendering loop
	float * tcache, *t2cache, *t3cache, *tpropcache;

};



extern "C"
{
	//required interface functions
	void TPV_next(TPV *unit, int wrongNumSamples);
	void TPV_Ctor(TPV *unit);
	void TPV_Dtor(TPV *unit);
}


const int g_costableTPVsize=1024;
float g_costableTPV[g_costableTPVsize+1]; //extra value for wraparound linear interpolation calculations


//void calculatefeatures(TPV *unit, int ibufnum);

//calculate by summing
//void oscillatorbankresynthesis(TPV *unit, int numsamples);
//void peakmatch(TPV *unit);
void newframe(TPV* unit, int ibufnum);


void TPV_Ctor(TPV* unit) {

	//CHECK SAMPLING RATE AND BUFFER SIZE
	unit->m_blocksize = unit->mWorld->mFullRate.mBufLength;

	if(unit->m_blocksize!=64) printf("TPV complains: block size not 64, you have %d\n", unit->m_blocksize);

	unit->m_sr = unit->mWorld->mSampleRate;

	if(unit->m_sr!=44100) printf("TPV complains: sample rate not 44100, you have %d\n", unit->m_sr);

	//assumption for now is that FFT size and hop rate match the temporal window size and hop rate
	unit->m_windowsize=(int)(ZIN0(1)+0.0001); //defaults for now, may have to set as options later
	unit->m_hopsize=(int)(ZIN0(2)+0.00001);

	//unit->tcache=  (float*)RTAlloc(unit->mWorld, unit->m_hopsize * sizeof(float));
	unit->t2cache=  (float*)RTAlloc(unit->mWorld, unit->m_hopsize * sizeof(float));
	unit->t3cache=  (float*)RTAlloc(unit->mWorld, unit->m_hopsize * sizeof(float));
	unit->tpropcache=  (float*)RTAlloc(unit->mWorld, unit->m_hopsize * sizeof(float));

	float rhop= 1.0/unit->m_hopsize;

	for (int i=0; i<unit->m_hopsize; ++i) {
		unit->t2cache[i]=i*i;
		unit->t3cache[i]=unit->t2cache[i]*i;
		unit->tpropcache[i]= (float)i*rhop;
	}

	//printf("another check %d windowsize %d hopsize %d \n", unit->m_numoutputs, unit->m_windowsize, unit->m_hopsize);

	unit->m_nover2=unit->m_windowsize/2;
	//unit->m_nover4=unit->m_windowsize/4;

	//	World *world = unit->mWorld;
	//
	//	uint32 bufnum = (uint32)(ZIN0(2)+0.001);
	//	if (bufnum >= world->mNumSndBufs) bufnum = 0;
	//	//unit->m_bufNum=bufnum;
	//
	//	SndBuf *buf = world->mSndBufs + bufnum;
	//	//unit->m_bufSize = buf->samples; //should just be size 4


	unit->m_maxpeaks=(int)(ZIN0(3)+0.0001);
	//int m_maxlistsize; //double m_maxpeaks

	unit->m_tracks= (PartialTrack*)RTAlloc(unit->mWorld, 2*unit->m_maxpeaks * sizeof(PartialTrack));

	//use buffer swapping of pointer as needed
	unit->m_prevpeaks = (TPVPeak*)RTAlloc(unit->mWorld, unit->m_maxpeaks * sizeof(TPVPeak));
	unit->m_newpeaks=(TPVPeak*)RTAlloc(unit->mWorld, unit->m_maxpeaks * sizeof(TPVPeak));

	//no need to initialise these arrays since filled as needed

	unit->m_numprevpeaks =0;
	unit->m_numnewpeaks =0;
	unit->m_numtracks=0;
	unit->m_resynthesisposition=0;

	unit->mCalcFunc = (UnitCalcFunc)&TPV_next;

}



void TPV_Dtor(TPV *unit)
{
	RTFree(unit->mWorld, unit->m_tracks);
	RTFree(unit->mWorld, unit->m_prevpeaks);
	RTFree(unit->mWorld, unit->m_newpeaks);

	//RTFree(unit->mWorld, unit->tcache);
	RTFree(unit->mWorld, unit->t2cache);
	RTFree(unit->mWorld, unit->t3cache);
	RTFree(unit->mWorld, unit->tpropcache);
}


//can dynamically reduce or increase the number of peaks stored (trails will automatically birth and die)
//TPV(chain, windowsize, hopsize,maxpeaks,currentpeaks,freqmult);

void TPV_next(TPV *unit, int numSamples)
{
	int i,j;

	float fftbufnum = IN0(0)+0.001;
	//float* in = IN(1);
	float* out = OUT(0);

	//int numSamples = unit->mWorld->mFullRate.mBufLength;
    

    
	if (fftbufnum> (-0.5)) {

		newframe(unit, (int)fftbufnum);
		unit->m_resynthesisposition=0;
	}

	//implement here in code
	//oscillatorbankresynthesis

	PartialTrack * tracks = unit->m_tracks;
	int numtracks = unit->m_numtracks;

	int resynthesisposition = unit->m_resynthesisposition;

	float T = unit->m_hopsize;

	PartialTrack * pointer;

	float output;
	float t,t2,t3;
	float tpos;
	int index;

	//precalculated for efficiency
	float * t2cache = unit->t2cache;
	float * t3cache = unit->t3cache;
	float * tpropcache = unit->tpropcache;


	//zero output first in case silent output
	for (j=0; j<numSamples; ++j) {
		out[j]=0.0;
	}

	//printf("numtracks %d \n", numtracks);

	for (i=0; i<numtracks; ++i) {

		pointer = &(tracks[i]);

		float amp1= pointer->amp1;
		float amp2=pointer->amp2;
		float theta1=pointer->theta1;
		float omega1= pointer->omega1;
		float alpha=pointer->alpha;
		float beta= pointer->beta;

		for (j=0; j<numSamples; ++j) {

			index= resynthesisposition+j;

			t= index; ///T;
			t2=t2cache[index]; //t*t;
			t3=t3cache[index]; //t*t2;
			tpos= tpropcache[index]; //((float)t/T);

			//linear interpolation of amplitude
			float amp= amp1 + (tpos*(amp2- amp1));
			//printf("amp %f temp3 %f amp2 %f number %f \n",amp,temp3, tracks[i].amp2, ((float)t/T));

			//cubic interpolation of phase; probably this is the chief cause of inefficiency...
			float phase = (theta1) + (t*omega1)+(t2*alpha) +(t3*beta);

			float phasetemp= phase*rtwopi*g_costableTPVsize;

			//linear interpolation into costable
			//could use fmod if add very big multiple of pi so modulo works properly; ie, no negative phases allowed BUT fmod is really inefficient!
			float wrapval= sc_wrap(phasetemp,0.0f,1024.0f); //modulo or fold won't work correctly- i.e., we need -1 = 1023
			int prev= (int)wrapval;
			float prop=  wrapval-prev; //linear interpolation parameter
			float interp= ((1.0-prop)*(g_costableTPV[prev])) + (prop*(g_costableTPV[prev+1]));

			//printf("pos %d amp %f phase %f interp %f \n",resynthesisposition,amp, phase, interp);

			out[j] += amp*interp; //g_costableTPV[((int)(phasetemp))%g_costableTPVsize];
		}

	}


	resynthesisposition += numSamples;

	/*		///OLD VERSION: LESS EFFICIENT
	//zero output first in case silent output
	for (j=0; j<numSamples; ++j) {

		output= 0.0;

		t= resynthesisposition; ///T;
		t2=t2cache[resynthesisposition]; //t*t;
		t3=t3cache[resynthesisposition]; //t*t2;
		tpos= tpropcache[resynthesisposition]; //((float)t/T);

		//sum over all active tracks
		for (i=0; i<numtracks; ++i) {

			pointer = &(tracks[i]);

			//if(i==20) {

			float temp3= pointer->amp1;
			//linear interpolation of amplitude
			float amp= temp3 + (tpos*(pointer->amp2- temp3));
			//printf("amp %f temp3 %f amp2 %f number %f \n",amp,temp3, tracks[i].amp2, ((float)t/T));

			//cubic interpolation of phase
			float phase = (pointer->theta1) + (t*pointer->omega1)+(t2*pointer->alpha) +(t3*pointer->beta);
			//divide omegas by sampling rate?

			//just use naive linear calc for now as test
			//float phase= t*((i+1)*(pi/512)); //(tracks[i].theta1) + (t*tracks[i].omega1);

			float phasetemp= phase*rtwopi*g_costableTPVsize;

			//if(phasetemp<0.0)
			//phasetemp= phasetemp-0.5;
			//else
			//phasetemp=phasetemp+0.5;

			//linear interpolation into costable
			float wrapval= sc_wrap(phasetemp,0.0f,1024.0f); //modulo or fold won't work correctly- i.e., we need -1 = 1023
			int prev= (int)wrapval;
			float prop=  wrapval-prev; //linear interpolation parameter
			float interp= ((1.0-prop)*(g_costableTPV[prev])) + (prop*(g_costableTPV[prev+1]));

			//printf("pos %d amp %f phase %f interp %f \n",resynthesisposition,amp, phase, interp);

			output += amp*interp; //g_costableTPV[((int)(phasetemp))%g_costableTPVsize];

			//need cosine lookup else will be too many trignometric calls per second
			//naive closest point algorithm for now; will make subtle distortions but can switch to linear or cubic interpolation later

			//if negative phase, gets moduloed wrongly? NO, OK, see tests below
			//output += amp*g_costableTPV[((int)(0.5+(phase*rtwopi*g_costableTPVsize)))%g_costableTPVsize];   //cos(phase);

			//}
		}

		out[j]=output;
		++resynthesisposition;
	}
	*/

	unit->m_resynthesisposition=resynthesisposition;

}



void newframe(TPV *unit, int ibufnum) {

	int i,j;
    
	//get FFT buf, allow for local buffers and supernova LOCK_SNDBUF
	World *world = unit->mWorld;
    SndBuf *buf; // = world->mSndBufs + ibufnum;
    
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
    
	LOCK_SNDBUF(buf); 

	//int numbins = buf->samples - 2 >> 1;

	//float * data= buf->data;
	//assumed in this representation
	//dc, nyquist then complex pairs
	//SCComplexBuf *p = ToComplexApx(buf);
	SCPolarBuf *p = ToPolarApx(buf);

	int nover2= unit->m_nover2;
	//int nover4= unit->m_nover4;

	//swap new peaks to old; current now safe to overwrite;

	TPVPeak * prevpeaks= unit->m_prevpeaks;
	TPVPeak * newpeaks= unit->m_newpeaks;
	int numprevpeaks= unit->m_numprevpeaks;
	int numnewpeaks= unit->m_numnewpeaks;

	//printf("prev pointer %p new pointer %p \n",prevpeaks, newpeaks);

	//ditch old
	numprevpeaks= numnewpeaks;
	numnewpeaks=0;

	//swap pointers ready to write new peaks
	TPVPeak * temp= prevpeaks;
	prevpeaks=newpeaks;
	newpeaks=temp;

	//printf("prev pointer %p new pointer %p temp %p \n",prevpeaks, newpeaks, temp);


	float phase, prevmag, mag, nextmag;

	//bin 1 can't be pick since no interpolation possible! dc should be ignored
	//test each if peak candidate; if so, add to list and add to peaks total

	prevmag= p->bin[0].mag; //this is at analysis frequency, not dc
	mag= p->bin[1].mag;

	int numpeaksrequested= (int)ZIN0(4); //(int)(ZIN0(4)+0.0001);
	int maxpeaks= unit->m_maxpeaks;

	maxpeaks = sc_min(maxpeaks,numpeaksrequested);

	//angular frequency is pi*(i/nover2)

	float angmult= pi/nover2;
	float ampmult= (1.0/unit->m_windowsize); //*(1.0/unit->m_maxpeaks);

		//defined here since needed in backdating phase for track births (and potentially for track deaths too)
	//T = number of samples per interpolaion frame, so equals hopsize
	float T = unit->m_hopsize;

	//float invT= 1.0/T;

	//should also adjust tolerance? (ie change angmult itself)
	float freqmult= ZIN0(5); //(int)(ZIN0(4)+0.0001);

	float ampcheck= ZIN0(7); //0.001

	//could restrict not to go above nover4!
	for (i=2; i<(nover2-1); ++i) {

		//phase= p->bin[i].phase;
		nextmag= p->bin[i].mag;

		if ((prevmag<mag) && (nextmag<mag) && (mag>ampcheck) && (numnewpeaks<maxpeaks)) {
			//found a peak

			//could use cubic interpolation// successive parabolic interpolation to refine peak location; or should have zero padded
			newpeaks[numnewpeaks].mag = mag * ampmult; //must divide by fftsize before resynthesis!
			newpeaks[numnewpeaks].freq =(i-1)*angmult*freqmult; //if should be angular frequency per sample, divide by T
			newpeaks[numnewpeaks].phase = p->bin[i-1].phase;	//is this in range -pi to pi? more like -1 to 5 or so, but hey, is in radians

			//printf("newpeak %d amp %f freq %f phase %f \n",numnewpeaks, mag * ampmult,(i-1)*angmult, p->bin[i-1].phase);

			++numnewpeaks;

		}

		prevmag=mag;
		mag=nextmag;

	}

	unit->m_prevpeaks = prevpeaks;
	unit->m_newpeaks = newpeaks;
	unit->m_numprevpeaks = numprevpeaks;
	unit->m_numnewpeaks = numnewpeaks;


	//now peak matching algorithm
	//int leftsort=0;
	int rightsort=0;
	bool flag= true;
	//float rightfreq= newpeaks[0].freq;

	PartialTrack * tracks = unit->m_tracks;
	int numtracks = 0; //unit->m_numtracks;

	//increase tolerance
	float tolerance= ZIN0(6)*angmult;


	float testfreq;

//	SEEMS OK
//	printf("numprevpeaks %d numnewpeaks %d \n",numprevpeaks, numnewpeaks);
//	//print all and look for junk data
//	for (i=0; i<numnewpeaks; ++i)
//	printf("new i %d amp %f freq %f phase %f \n",i,newpeaks[i].mag,newpeaks[i].freq,newpeaks[i].phase);
//
//	for (i=0; i<numprevpeaks; ++i)
//	printf("prev i %d amp %f freq %f phase %f \n",i,prevpeaks[i].mag,prevpeaks[i].freq,prevpeaks[i].phase);
//
//


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
					tracks[numtracks].omega1=newpeaks[rightsort].freq;
					tracks[numtracks].theta2=newpeaks[rightsort].phase;
					tracks[numtracks].omega2=newpeaks[rightsort].freq; //match to itself
					tracks[numtracks].theta1=newpeaks[rightsort].phase - (T*(newpeaks[rightsort].freq)); //should really be current phase + freq*hopsize
					tracks[numtracks].amp1=0.0;
					tracks[numtracks].amp2=newpeaks[rightsort].mag;
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

						tracks[numtracks].omega1=newpeaks[j].freq;
						tracks[numtracks].theta2=newpeaks[j].phase;
						tracks[numtracks].omega2=newpeaks[j].freq; //match to itself
						tracks[numtracks].theta1=sc_wrap(newpeaks[j].phase - (T*(newpeaks[j].freq)),0.0f,(float)twopi); //backcalculate starting phase
						tracks[numtracks].amp1=0.0;
						tracks[numtracks].amp2=newpeaks[j].mag;
						++numtracks;
						++rightsort;
					}

					//printf("match! \n ");

					//MATCH!
					tracks[numtracks].theta1=prevpeaks[i].phase;
					tracks[numtracks].omega1=prevpeaks[i].freq;
					tracks[numtracks].theta2=newpeaks[rightsort].phase; //match to itself; should really be current phase + freq*hopsize
					tracks[numtracks].omega2=newpeaks[rightsort].freq; //match to itself
					tracks[numtracks].amp1=prevpeaks[i].mag;
					tracks[numtracks].amp2=newpeaks[rightsort].mag;

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
			tracks[numtracks].theta1=prevpeaks[i].phase;
			tracks[numtracks].omega1=prevpeaks[i].freq;
			tracks[numtracks].theta2=sc_wrap(prevpeaks[i].phase + (T*prevpeaks[i].freq),0.0f,(float)twopi); //match to itself; should really be current phase + freq*hopsize
			tracks[numtracks].omega2=prevpeaks[i].freq; //match to itself
			tracks[numtracks].amp1=prevpeaks[i].mag;
			tracks[numtracks].amp2=0.0;
			++numtracks;

			//ADDCODE
			//++leftsort;
		}

	}

	//rightsort should equal numnewpeaks!

	//now iterate through PartialTracks, preparing them for synthesis
	unit->m_numtracks = numtracks;

	float theta1, omega1, theta2, omega2; //, amp1, amp2;  //, alpha, beta


	int M;
	float Tover2= T/2.0;
	//float oneovertwopi = 1.0/(2*PI);
	double temp1, temp2;

	//matrix elements common to all track calculations: eqn (34)
	//for hyperefficiency could precalculate some of this material in constructor of course...
	double r1c1=3.0/(T*T);
	double r1c2= (-1.0)/T;
	double r2c1= (-2.0)/(T*T*T);
	double r2c2= 1.0/(T*T);

	//printf("matrix checks %f %f %f %f \n",r1c1,r1c2,r2c1,r2c2);


	//precalculate cubic interpolation parameters alpha and beta as per eqn (37) in McAulay and Quatieri
	//must go via M, the integer of extra phase for theta2
	for (i=0; i<numtracks; ++i) {

			theta1= tracks[i].theta1;
			theta2= tracks[i].theta2;
			omega1= tracks[i].omega1;
			omega2= tracks[i].omega2;

			//rpitwo= 1/2pi see SC_constants
			//round off as (int)(0.5+val)
			float mtemp= rtwopi*((theta1 + (omega1*T) - theta2) + ((omega2-omega1)*Tover2));

			if(mtemp<0.0)
			M= (int)(mtemp-0.5);
			else
			M= (int)(mtemp+0.5);

			temp1= theta2 - theta1 - (omega1*T) + (twopi*M);
			temp2= omega2-omega1;

			//matrix solution
			tracks[i].alpha= r1c1*temp1 + r1c2*temp2;
			tracks[i].beta= r2c1*temp1 + r2c2*temp2;

			//if(i==20) {
			//printf("track check %d theta1 %f theta2 %f omega1 %f omega2 %f amp1 %f amp2 %f M %d alpha %f beta %f \n",i,theta1,theta2,omega1,omega2,tracks[i].amp1, tracks[i].amp2,M,tracks[i].alpha, tracks[i].beta);
			//}
	}


	//struct PartialTrack {
	//float theta1, omega1, theta2, omega2, alpha, beta; //cubic interpolation of phase
	//float startamp, endamp; //linear interpolation of amplitude
	//};



}



//void peakmatch(TPV *unit) {
//}


void loadTPV(InterfaceTable *inTable)
{

	ft= inTable;

//	init_SCComplex(inTable);

	//printf("TPV LOADED CHECK\n");

	//DefineDtorUnit(TPV);
	//
	DefineDtorCantAliasUnit(TPV);

	for (int i=0; i<=g_costableTPVsize; ++i) {

		float temp= twopi*((float)i/g_costableTPVsize);
		g_costableTPV[i]= cos(temp); //or sin

		//printf("cos check %d %f",i,g_costableTPV[i]);

	}

	//check modulo for negatives
	//printf("modulo test %d %d \n",(-512)%1024,(-2678)%1024);

	//check round off for negatives; ah, yes, rounds down towards zero
	//printf("round off test %d %d \n",(int)(0.5+(-1.4)),(int)(0.5+(-1.7)));

	//gives wrong results for contiguous phase position
	//printf("modulo test 2 %f %f \n",fmod(-34.7,1024), fmod(-1003.7,1024));

	//printf("modulo test 2 %f %f \n",sc_wrap(-34.7,0.0,1024.0), sc_wrap(-1003.7,0.0,1024.0));

	//tests
	//	Peak peaks[3];
	//
	//	Peak * peakpoint=peaks;
	//
	//
	//	peakpoint[0].freq= 4;
	//	peakpoint[1].freq= 7;
	//	peakpoint[0].phase=15;
	//	peakpoint[2].phase=16;
	//
	//	printf("tests %f %f %f %f \n",peakpoint[0].freq, peakpoint[1].freq, peakpoint[0].phase, peakpoint[2].phase);
	//
	//yes, still works with a pointed

	//this works
	//	peaks[0].freq= 4;
	//	peaks[1].freq= 7;
	//	peaks[0].phase=15;
	//	peaks[2].phase=16;
	//
	//	printf("tests %f %f %f %f \n",peaks[0].freq, peaks[1].freq, peaks[0].phase, peaks[2].phase);
	//
	//
	//	int u=0;
	//	printf("u %d \n",u);
	//	printf("tests pre %d \n",++u);
	//	printf("tests post %d \n",u++);  \\returns previous value before increment (ie increment is post)
	//	 //
	//	//works as expected
	//	printf("tests2 %d \n",(int)(10.0));
	//	printf("tests2 %d %d %d \n",(int)(1024.0),(int)0.0, (int)(-1.0));
	//

	//memcpy to same array tests; memcpy requires memcpy(target, source, number)
	//float test1[5]= {1.0f,1.1f,1.2f,1.3f,1.4f};
	//float test2[5]= {2.0f,2.1f,2.2f,2.3f,2.4f};
	//int j;
	//
	//for(j=0; j<5;++j){
	//printf("testmemcpy1 %f \n",test1[j]);
	//}
	//
	//memcpy(test1, test1+2, 3 * sizeof(float));
	//
	//for(j=0; j<5;++j){
	//printf("testmemcpy2 %f \n",test1[j]);
	//}


}
