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

//Nick Collins 16/9/2014
//Beat Statistics; using first stages of a beat tracker. Form beat histogram, then take (assumed tempo independent) statistics: entropy, ratio of first to second peak, harmonicity (how much explained by multiple positions relating to main peak)
//44.1KHz only, precalculated ERB arrays

#include "SC_PlugIn.h"
#include "FFT_UGens.h"

InterfaceTable *ft; 

//hard coded FFT size
#define N 1024
#define NOVER2 512

//CONVERT TO m_frameperiod to cope with different sampling rates?
//#define FRAMEPERIOD 0.046439909297052


//1024 size FFT, hop 512, at 44.1kHz


//weighting parameters

int eqlbandbins[43]= {1,2,3,4,5,6,7,8,9,11,13,15,17,19,22,25,28,32,36,41,46,52,58,65,73,82,92,103,116,129,144,161,180,201,225,251,280,312,348,388,433,483,513};
int eqlbandsizes[42]= {1,1,1,1,1,1,1,1,2,2,2,2,2,3,3,3,4,4,5,5,6,6,7,8,9,10,11,13,13,15,17,19,21,24,26,29,32,36,40,45,50,30};
float contours[42][11]= {{ 47.88, 59.68, 68.55, 75.48, 81.71, 87.54, 93.24, 98.84,104.44,109.94,115.31},{ 29.04, 41.78, 51.98, 60.18, 67.51, 74.54, 81.34, 87.97, 94.61,101.21,107.74},{ 20.72, 32.83, 43.44, 52.18, 60.24, 67.89, 75.34, 82.70, 89.97, 97.23,104.49},{ 15.87, 27.14, 37.84, 46.94, 55.44, 63.57, 71.51, 79.34, 87.14, 94.97,102.37},{ 12.64, 23.24, 33.91, 43.27, 52.07, 60.57, 68.87, 77.10, 85.24, 93.44,100.90},{ 10.31, 20.43, 31.03, 40.54, 49.59, 58.33, 66.89, 75.43, 83.89, 92.34,100.80},{  8.51, 18.23, 28.83, 38.41, 47.65, 56.59, 65.42, 74.16, 82.89, 91.61,100.33},{  7.14, 16.55, 27.11, 36.79, 46.16, 55.27, 64.29, 73.24, 82.15, 91.06, 99.97},{  5.52, 14.58, 25.07, 34.88, 44.40, 53.73, 62.95, 72.18, 81.31, 90.44, 99.57},{  3.98, 12.69, 23.10, 32.99, 42.69, 52.27, 61.66, 71.15, 80.54, 89.93, 99.31},{  2.99, 11.43, 21.76, 31.73, 41.49, 51.22, 60.88, 70.51, 80.11, 89.70, 99.30},{  2.35, 10.58, 20.83, 30.86, 40.68, 50.51, 60.33, 70.08, 79.83, 89.58, 99.32},{  2.05, 10.12, 20.27, 30.35, 40.22, 50.10, 59.97, 69.82, 79.67, 89.52, 99.38},{  2.00,  9.93, 20.00, 30.07, 40.00, 49.93, 59.87, 69.80, 79.73, 89.67, 99.60},{  2.19, 10.00, 20.00, 30.00, 40.00, 50.00, 59.99, 69.99, 79.98, 89.98, 99.97},{  2.71, 10.56, 20.61, 30.71, 40.76, 50.81, 60.86, 70.96, 81.01, 91.06,101.17},{  3.11, 11.05, 21.19, 31.41, 41.53, 51.64, 61.75, 71.95, 82.05, 92.15,102.33},{  2.39, 10.69, 21.14, 31.52, 41.73, 51.95, 62.11, 72.31, 82.46, 92.56,102.59},{  1.50, 10.11, 20.82, 31.32, 41.62, 51.92, 62.12, 72.32, 82.52, 92.63,102.56},{ -0.17,  8.50, 19.27, 29.77, 40.07, 50.37, 60.57, 70.77, 80.97, 91.13,101.23},{ -1.80,  6.96, 17.77, 28.29, 38.61, 48.91, 59.13, 69.33, 79.53, 89.71, 99.86},{ -3.42,  5.49, 16.36, 26.94, 37.31, 47.61, 57.88, 68.08, 78.28, 88.41, 98.39},{ -4.73,  4.38, 15.34, 25.99, 36.39, 46.71, 57.01, 67.21, 77.41, 87.51, 97.41},{ -5.73,  3.63, 14.74, 25.48, 35.88, 46.26, 56.56, 66.76, 76.96, 87.06, 96.96},{ -6.24,  3.33, 14.59, 25.39, 35.84, 46.22, 56.52, 66.72, 76.92, 87.04, 97.00},{ -6.09,  3.62, 15.03, 25.83, 36.37, 46.70, 57.00, 67.20, 77.40, 87.57, 97.68},{ -5.32,  4.44, 15.90, 26.70, 37.28, 47.60, 57.90, 68.10, 78.30, 88.52, 98.78},{ -3.49,  6.17, 17.52, 28.32, 38.85, 49.22, 59.52, 69.72, 79.92, 90.20,100.61},{ -0.81,  8.58, 19.73, 30.44, 40.90, 51.24, 61.52, 71.69, 81.87, 92.15,102.63},{  2.91, 11.82, 22.64, 33.17, 43.53, 53.73, 63.96, 74.09, 84.22, 94.45,104.89},{  6.68, 15.19, 25.71, 36.03, 46.25, 56.31, 66.45, 76.49, 86.54, 96.72,107.15},{ 10.43, 18.65, 28.94, 39.02, 49.01, 58.98, 68.93, 78.78, 88.69, 98.83,109.36},{ 13.56, 21.65, 31.78, 41.68, 51.45, 61.31, 71.07, 80.73, 90.48,100.51,111.01},{ 14.36, 22.91, 33.19, 43.09, 52.71, 62.37, 71.92, 81.38, 90.88,100.56,110.56},{ 15.06, 23.90, 34.23, 44.05, 53.48, 62.90, 72.21, 81.43, 90.65, 99.93,109.34},{ 15.36, 23.90, 33.89, 43.31, 52.40, 61.42, 70.29, 79.18, 88.00, 96.69,105.17},{ 15.60, 23.90, 33.60, 42.70, 51.50, 60.20, 68.70, 77.30, 85.80, 94.00,101.70},{ 15.60, 23.90, 33.60, 42.70, 51.50, 60.20, 68.70, 77.30, 85.80, 94.00,101.70},{ 15.60, 23.90, 33.60, 42.70, 51.50, 60.20, 68.70, 77.30, 85.80, 94.00,101.70},{ 15.60, 23.90, 33.60, 42.70, 51.50, 60.20, 68.70, 77.30, 85.80, 94.00,101.70},{ 15.60, 23.90, 33.60, 42.70, 51.50, 60.20, 68.70, 77.30, 85.80, 94.00,101.70},{ 15.60, 23.90, 33.60, 42.70, 51.50, 60.20, 68.70, 77.30, 85.80, 94.00,101.70}};
double phons[11]={2,10,20,30,40,50,60,70,80,90,100};

//assumes 512 hop size
float g_periods[100] = { 57.421875, 56.84765625, 56.284808168317, 55.732996323529, 55.191899271845, 54.661207932692, 54.140625, 53.629864386792, 53.128650700935, 52.63671875, 52.153813073394, 51.6796875, 51.21410472973, 50.7568359375, 50.30766039823, 49.866365131579, 49.432744565217, 49.006600215517, 48.587740384615, 48.175979872881, 47.771139705882, 47.373046875, 46.981534090909, 46.59643954918, 46.217606707317, 45.844884072581, 45.478125, 45.1171875, 44.761934055118, 44.412231445313, 44.067950581395, 43.728966346154, 43.395157442748, 43.06640625, 42.742598684211, 42.423624067164, 42.109375, 41.799747242647, 41.49463959854, 41.193953804348, 40.89759442446, 40.60546875, 40.317486702128, 40.033560739437, 39.753605769231, 39.4775390625, 39.205280172414, 38.936750856164, 38.671875, 38.410578547297, 38.15278942953, 37.8984375, 37.647454470199, 37.399773848684, 37.155330882353, 36.9140625, 36.675907258065, 36.440805288462, 36.208698248408, 35.979529272152, 35.753242924528, 35.52978515625, 35.30910326087, 35.091145833333, 34.875862730061, 34.663205030488, 34.453125, 34.245576054217, 34.040512724551, 33.837890625, 33.637666420118, 33.439797794118, 33.244243421053, 33.050962936047, 32.859916907514, 32.671066810345, 32.484375, 32.2998046875, 32.117319915254, 31.936885533708, 31.758467178771, 31.58203125, 31.407544889503, 31.234975961538, 31.064293032787, 30.895465353261, 30.728462837838, 30.563256048387, 30.399816176471, 30.238115026596, 30.078125, 29.919819078947, 29.763170811518, 29.608154296875, 29.454744170984, 29.302915592784, 29.152644230769, 29.00390625, 28.856678299492, 28.7109375 };

struct BeatStatistics : Unit {
    
	//FFT data
	float * m_FFTBuf;
    int numERBbands_;
    float * ERBpowers_;
    float * previousERBpowers_;
    
	//coping with different sampling rates
	float m_srate;		//use as a flag to check sample rate is correct
	float m_frameperiod;
 
    int numperiods_;
	int numpreviousvalues_;
	int storepos_;
	float * store_;  //for previous values
	float * crosscomby_;
    
    //statistics output
    float entropy_;
    float toptworatio_;
    float diversity_;
    float metricity_;
    
};



extern "C"
{
    
	void BeatStatistics_next(BeatStatistics *unit, int wrongNumSamples);
	void BeatStatistics_Ctor(BeatStatistics *unit);
	void BeatStatistics_Dtor(BeatStatistics *unit);

}






//other functions
static void BeatStatistics_calculate(BeatStatistics *, uint32);


void BeatStatistics_Ctor(BeatStatistics* unit)
{
	unit->m_srate = unit->mWorld->mFullRate.mSampleRate;

	//if sample rate is 88200 or 96000, assume taking double size FFT to start with
	if(unit->m_srate > (44100.0*1.5)) unit->m_srate = unit->m_srate*0.5;

	if(((int)(unit->m_srate+0.01))==44100)
	{
		//unit->m_weights = g_weights44100;
		//unit->m_bins = g_bins44100;
		unit->m_frameperiod = 0.046439909297052;
	}
	else  //else 48000; potentially dangerous if it isn't! Fortunately, shouldn't write any data to unknown memory
	{
		//unit->m_weights = g_weights48000;
		//unit->m_bins = g_bins48000;
		unit->m_frameperiod = 0.042666666666667;
	}

	//only need space for half!
	unit->m_FFTBuf = (float*)RTAlloc(unit->mWorld, NOVER2 * sizeof(float));

    unit->numERBbands_ = 40;
    unit->ERBpowers_ = (float*)RTAlloc(unit->mWorld, unit->numERBbands_ * sizeof(float));
    unit->previousERBpowers_ = (float*)RTAlloc(unit->mWorld, unit->numERBbands_ * sizeof(float));

    //start zeroed out
    Clear(unit->numERBbands_, unit->previousERBpowers_);
    
    unit->numperiods_ = 100;
    unit->numpreviousvalues_ = 350;
    unit->storepos_ = 0;
    unit->store_ = (float*)RTAlloc(unit->mWorld, unit->numpreviousvalues_ * sizeof(float)); //for previous values
    unit->crosscomby_ = (float*)RTAlloc(unit->mWorld, unit->numperiods_ * sizeof(float));
    
    Clear(unit->numpreviousvalues_,unit->store_);
    Clear(unit->numperiods_, unit->crosscomby_);

    
    //for(int i=0; i<unit->numperiods_; ++i)
    //    printf("initial cross i %d at %f\n",i,unit->crosscomby_[i]);
    
    
    unit->entropy_ = 0.f;
    unit->toptworatio_ = 0.f;
    unit->diversity_ = 0.f;
    unit->metricity_ = 0.f;
    
	unit->mCalcFunc = (UnitCalcFunc)&BeatStatistics_next;
}


void BeatStatistics_Dtor(BeatStatistics *unit)
{
	RTFree(unit->mWorld, unit->m_FFTBuf);
    RTFree(unit->mWorld, unit->ERBpowers_);
    RTFree(unit->mWorld, unit->previousERBpowers_);
    RTFree(unit->mWorld, unit->store_);
    RTFree(unit->mWorld, unit->crosscomby_);
    
}


void BeatStatistics_next(BeatStatistics *unit, int wrongNumSamples)
{
	//int numSamples = unit->mWorld->mFullRate.mBufLength;

	//float *output = ZOUT(0);

	float fbufnum = ZIN0(0)+0.001;

	//next FFT bufffer ready, update
	//assuming at this point that buffer precalculated for any resampling
	if (fbufnum > -0.01f) {  // && ( ZIN0(3)<0.5)

		//unit->m_frame= unit->m_frame+1;
		BeatStatistics_calculate(unit, (uint32)fbufnum);
	}

	//control rate output
	ZOUT0(0)=unit->entropy_;
    ZOUT0(1)=unit->toptworatio_;
    ZOUT0(2)=unit->diversity_;
    ZOUT0(3)=unit->metricity_;
    
}


//calculation function once FFT data ready
void BeatStatistics_calculate(BeatStatistics *unit, uint32 ibufnum)
{
    
    int i, j, k;
	int prev, next;
	float interp;
    
	World *world = unit->mWorld;
	
    SndBuf *buf;
    
    if (ibufnum >= world->mNumSndBufs) { 
		int localBufNum = ibufnum - world->mNumSndBufs; 
		Graph *parent = unit->mParent; 
		if(localBufNum <= parent->localBufNum) { 
			buf = parent->mLocalSndBufs + localBufNum; 
		} else { 
			buf = world->mSndBufs; 
			if(unit->mWorld->mVerbosity > -1){ Print("BeatStatistics error: Buffer number overrun: %i\n", ibufnum); } 
		} 
	} else { 
		buf = world->mSndBufs + ibufnum; 
	} 
    
	LOCK_SNDBUF(buf);

	//assumed in this representation
	ToComplexApx(buf);

	const float * data= buf->data;

	//memcpy(unit->m_FFTBuf, data, NOVER2);

	//to hold powers
	float * fftbuf= unit->m_FFTBuf;

	//get powers for bins
	//don't need to calculate past half Nyquist, because no indices involved of harmonics above 10000 Hz or so (see index data at top of file)
	for (i=0; i<N; i+=2) {
		//i>>1 is i/2
		fftbuf[i>>1] = ((data[i] * data[i]) + (data[i+1] * data[i+1]));
	}

    //ERB channel power accumulation
    
    int numERBbands = unit->numERBbands_;
    float * ERBpowers = unit->ERBpowers_;
    
    //was 0, looking for problems
	for (k=0; k<numERBbands; ++k){
		
		int bandstart=eqlbandbins[k];
		//int bandend=eqlbandbins[k+1];
		int bandsize= eqlbandsizes[k];
		
		float bsum=0.0;
		
        //printf("%d %d %d \n",k,bandstart,bandsize);
        
		for (int h=0; h<bandsize;++h) {
			bsum += fftbuf[h+bandstart];
		}
		
    
		//store recips of bandsizes?
		//why averaging wihin band?
		bsum= bsum/bandsize;
		
		//into dB, avoid log of 0
		//float db= 10*log10((bsum*10000000)+0.001);
		float db= 10*log10((bsum*32382)+0.001);
		
		//printf("bsum %f db %f \n",bsum,db);
		
		//convert via contour
		if(db<contours[k][0]) db=0;
        else if (db>contours[k][10]) db=phons[10];
        else {
            
            float prop=0.0;
			
            for (j=1; j<11; ++j) {
                if(db<contours[k][j]) {
                    prop= (db-contours[k][j-1])/(contours[k][j]-contours[k][j-1]);
                    break;
				}
				
				if(j==10)
					prop=1.0;
            }
			
            db= (1.0-prop)*phons[j-1]+ prop*phons[j];
			//printf("prop %f db %f j %d\n",prop,db,j);
			
		}
		
		//float lastloud=unit->m_loudbands[k];
		//float lastloud=0.0;
        //
        //		for(j=0;j<PASTERBBANDS; ++j)
        //			lastloud+=unit->m_loudbands[k][j];
        //
        //		lastloud /= PASTERBBANDS;
        //
        //		float diff= sc_max(db-lastloud,0.0);
		
		//dfsum=dfsum+diff; //(bweights[k]*diff);
		
		ERBpowers[k]=db;
        
        //printf("erb k %d is %f \n",k,db);
		
		//must sum as intensities, not dbs once corrected, pow used to be other way around
		//loudsum= loudsum+((pow(10, 0.1*db)-0.001)*0.0000308813538386);
		
	}
    
    
    //accumulate over channels to make onset detection function
    
	float diffsum= 0.0;
	float diffnow;
	
    float * previous = unit->previousERBpowers_;
    
	//just sum first 10 for bassier signal?
	
	//40
	for (i=0; i<numERBbands; ++i) {
		diffnow= ERBpowers[i]-previous[i];
		
		if(diffnow>0) diffsum += diffnow;
		
		previous[i]= ERBpowers[i]; //ready for next time
	}
	
	//value[0]= diffsum; //will auto adjust to normalized range
	
    float value = diffsum*0.01; //up to 100dB?
	
    //printf("diffsum %f, value %f \n",diffsum, value);
    
    
    float * store = unit->store_;
    float * crosscomby = unit->crosscomby_;
    int storepos = unit->storepos_;
    int numpreviousvalues = unit->numpreviousvalues_;
    int numperiods = unit->numperiods_;
    
	store[storepos] = value;
	
    float total = 0.f;
    
    //0.995 default for accumulator
    float leak = ZIN0(1);
    
    int numpreviousbeats = ZIN0(2);
    
    if(numpreviousbeats<1) numpreviousbeats = 1;
    if(numpreviousbeats>4) numpreviousbeats = 4;
    
    
	//update leaky integrators
	for (i=0; i<numperiods; ++i) {
		
		float periodtotest = g_periods[i];
		
		float sumup = 0.0;
		
		//sum up to previous four beats compared to now
		
		float basepos = ( storepos + numpreviousvalues );
		
        if(value>0.0000001f) {
		for (k=1; k<=numpreviousbeats; ++k) {
			
			float posthen = fmod( basepos - (k*periodtotest), numpreviousvalues);
			
			prev = posthen;
            
			next = (prev+1)%numpreviousvalues;
			
			interp = posthen-prev;
			
            //if(crosscomby[i]>10.f) {
            //printf("k %d posthen %f prev %d next %d interp %f result %f %f %f %f %f \n",k,posthen,prev,next,interp, store[prev], store[next], value, ((store[prev]*(1.0-interp)) + ((interp)*store[next])), value * ((store[prev]*(1.0-interp)) + ((interp)*store[next])));
            //}
            
			sumup +=  value * ((store[prev]*(1.0-interp)) + ((interp)*store[next]));
			
		}
        }
		
		//0.996
        //avoid denormal accumulation just in case
        if(crosscomby[i]>0.000001f)
            crosscomby[i] = (crosscomby[i] * leak) + sumup;
        else
            crosscomby[i] = sumup;
        
        //if(i<10)
        //printf("cross i %d with sum %f is %f \n",i,sumup,crosscomby[i]);
        
        total += crosscomby[i];
	}
	
    
    //for(i=0; i<10; ++i)
    //    printf("current cross i %d at %f\n",i,crosscomby[i]);
    
	storepos = (storepos + 1) % numpreviousvalues;
    
    unit->storepos_ = storepos;
    
    float totalmult = 1.f;
    
    if(total>0.0000001f) {
        totalmult = 1.0f/total; //normalization factor for working with all periods as probability distribution summing to 1.0
    }
    
    float entropy = 0.f;
    float largest = 0.f;
    float secondlargest = 0.f;
    float diversity = 1.0f;
    float metricity = 1.f;
    int largestindex = 0;
    
    for (i=0; i<numperiods; ++i) {
        
        float now = crosscomby[i]*totalmult;
        
        if (now>largest) {
            
            secondlargest = largest;
            largest = now;
            largestindex = i;
            
        } else if (now>secondlargest) {
            
            secondlargest = now;
        }
        
        if (now>0.00001f) {
        
            entropy -= now * log2(now);
        }
        
        diversity -= now*now;
        
    }
    
    float topperiod = g_periods[largestindex];
    float relativeperiod;
    
    //knowing largest, score energy in others relative to this with weighting based on how metrically related period is (e.g. look for near integer ratios)
    for (i=0; i<numperiods; ++i) {
     
        float now = crosscomby[i]*totalmult;
        float periodnow = g_periods[i];
        
        if(periodnow<topperiod)
            relativeperiod = topperiod/periodnow;
        else
            relativeperiod = periodnow/topperiod;
        
        float offinteger = relativeperiod - (int)relativeperiod;
        
        if(offinteger>0.5) offinteger = 1.f - offinteger;
        
        //if(offinteger<0.05) offinteger = 0.f;
        //else offinteger *= 5.0;
        
        metricity -= offinteger*now; //penalty for energy at off integer multiples/divisors
        
    }
    
    
    float ratio = 0.f;
    
    if(secondlargest>0.00001f) {
        
        ratio = largest/secondlargest;
        
    }

    //proportion largest versus total?
    //printf("diffsum %f, value %f \n",diffsum, value);
    //printf("entropy %f toptworatio %f \n",entropy,ratio);
    
    unit->entropy_ = entropy;
    unit->toptworatio_ = ratio;
    unit->diversity_ = diversity;
    unit->metricity_ = metricity;
    

	//float keyleak= ZIN0(1); //fade parameter to 0.01 for histogram in seconds, convert to FFT frames

	//unit->m_currentKey=bestkey;
        
        
        
        
}



PluginLoad(BeatStatistics) {
	
	init_SCComplex(inTable);
	
	ft = inTable;
    
	DefineDtorCantAliasUnit(BeatStatistics);
    
}


