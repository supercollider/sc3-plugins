/*
 *  Created by Nicholas Collins on 13/08/2010.
 *  Copyright 2010 Nicholas M Collins. All rights reserved.
 */

//#define SC_DARWIN
#include "SC_PlugIn.h"
#include "FFT_UGens.h"

//Dec 6th 2013 added options for per frame normalisation, choice of octave ratio (allowing aggregation, say half-octave bands over 5 octave span, Bohlen-Pierce scale etc)


InterfaceTable *ft;



struct Chromagram : public Unit
{
	int fftsize_;
	float chromanorm_; //for normalisation
	int numdivisions_;
	int tuningbase_;
	int octaves_;
    float octaveratio_;
	//int integrateflag_;
	//float integrationconstant_;
    
	
	int numindices_;
	int * indexdata_;
	float * indexweights_;
	float * fftpower_;
	float * chroma_;
    
	
};




extern "C" {
	
	void Chromagram_next_k(Chromagram *unit, int inNumSamples);
	void Chromagram_Ctor(Chromagram* unit);
	void Chromagram_Dtor(Chromagram* unit);
    
}





void Chromagram_Ctor( Chromagram* unit ) {
	
	int i, j;
	
	double sr = unit->mWorld->mFullRate.mSampleRate; //never trust SAMPLERATE, gives UGens output rate, not audio rate
	float nyquist = sr*0.5;
	unit->fftsize_ = ZIN0(1);
	
    
	float freqperbin = sr / unit->fftsize_;
	
	
	int divisions = ZIN0(2);
	float tuningbase = ZIN0(3);
	int octaves = ZIN0(4);
	float octaveratio = ZIN0(7);
	
    if (octaves<1) octaves = 1;
	
	float nyminusonebin = nyquist-freqperbin;
	
	//must have room for at least one octave of divisions
	if ((tuningbase<0.0) || (tuningbase>(nyminusonebin/octaveratio))) {
		
		tuningbase = 32.703195662575 ; //C at MIDI note 24
		
	}
	
	unit->tuningbase_ = tuningbase;
	
	//if octaves can't fit within available fft bins, have to cut short
	float topfreq = tuningbase * pow(octaveratio,octaves);

	//if can't fit all octaves into Nyquist
	if (topfreq >= (nyminusonebin)) {
		
		octaves = (int) (log2(nyminusonebin/tuningbase)/(log2(octaveratio)));  //if octaveratio==2 denominator will be 1
		
	}
	
    if (octaves<1) octaves = 1;
	
	unit->octaves_ = octaves;
	
	//printf("checks  %d %f %d %f %f %f %f \n", divisions, tuningbase, octaves, sr, nyminusonebin, topfreq, octaveratio);
	
	//should include number of octaves as divisor too
	unit->chromanorm_ = 1.0/((float)unit->fftsize_ * octaves);
	
	
	//unit->integrateflag_ = ZIN0(5);
	//unit->integrationconstant_ = ZIN0(6);
	
	int totaldivisions = octaves * divisions;
	
	unit->numindices_ = 2*totaldivisions;
	int * indexdata = (int *)RTAlloc(unit->mWorld, sizeof(int)*unit->numindices_);
	float * weightdata = (float *) RTAlloc(unit->mWorld, sizeof(float)*unit->numindices_);
	
	unit->indexdata_ = indexdata;
	unit->indexweights_ = weightdata;
	
	//work out indices of fft bins corresponding to chroma positions, from basefreq up
	//avoid lots of pow calls by working out cumulative ratios as go
	
    float ratio = pow(octaveratio,1.0/divisions); //pow(2.0,1.0/divisions);
	
	//float rationow= 1.0;
	
	float octavenow = 1.0;
	
	float temp = 1.0f/freqperbin;
	
	int indexnow;
	
	for ( j=0; j<octaves; ++j) {
		
		float freqnow= tuningbase * octavenow;
        
		indexnow = 2*j*divisions;
        
		for ( i=0; i<divisions; ++i) {
			
			//express as fft bins
			float binpos = freqnow * temp;
			
			int lower = (int) binpos;
			float interp = binpos- lower;
			int upper = lower+1;
			
			//linear interpolation
			indexdata[indexnow] = lower;
			weightdata[indexnow] = 1.0 - interp;
			
			//printf("lower oct %d div %d value: %d %f \n",j, i, indexdata[indexnow], weightdata[indexnow]);
			
			++indexnow;
            
			indexdata[indexnow] = upper;
			weightdata[indexnow] = interp;
			
			
			//printf("upper oct %d div %d value: %d %f \n",j, i, indexdata[indexnow], weightdata[indexnow]);
			
			++indexnow;
            
			freqnow *= ratio;
			
		}
        
		octavenow *= octaveratio; //2.0;
        
	}
    
	//would depend if taking harmonics as well as pure bin evidence
	//int highestbin =
	//unit->fftpower_ = RTAlloc(unit->mWorld, sizeof(float)*highestbin);
	
	//for now only need to calculate as go, no need for intermediate storage
	unit->chroma_ = (float*)RTAlloc(unit->mWorld, sizeof(float)*divisions);
    
	SETCALC(Chromagram_next_k);
	
	//in case of later interpolation, first output must be set to zero
	for ( i=0; i<divisions; ++i) {
		
		unit->chroma_[i] = 0.0;
		
		ZOUT0(i) = 0.f;
        
	}
	
	unit->numdivisions_ = divisions;
    
}

void Chromagram_Dtor(Chromagram *unit)
{
	
	RTFree(unit->mWorld, unit->indexdata_);
	RTFree(unit->mWorld, unit->indexweights_);
	RTFree(unit->mWorld, unit->chroma_);
	
}


//NEXT: destructor then next function using octaves and divisions slots, with appropriate power calc from fft data as go


void Chromagram_next_k( Chromagram *unit, int inNumSamples ) {
	
	int i, j;
	
	int divisions = unit->numdivisions_;
	float * chroma =  unit->chroma_;
	int octaves = unit->octaves_;
	int perframenormalize = ZIN0(8);
    
	//float *input = IN(0);
	
	//int numSamples = unit->mWorld->mFullRate.mBufLength;
    
	//if input is legitimate buffer number:
	
	float fbufnum = ZIN0(0);
	
	//next FFT bufffer ready, update
	//assuming at this point that buffer precalculated for any resampling
	if (fbufnum > -0.01f) {
		
		int ibufnum = (uint32)fbufnum;
		
		World *world = unit->mWorld;
		SndBuf *buf;
		
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
		
		//make sure in real and imag form
		//SCComplexBuf * complexbuf = ToComplexApx(buf);
		
		float * data= (float *)ToComplexApx(buf);
		
		//float * data= buf->data;
		
		//int numindices= unit->numindices_;
		
		int * indexdata= unit->indexdata_;
		float * indexweights= unit->indexweights_;
		
		float real, imag;
		int index, indexnow; //index2,
		float weight1, intensity1, weight2, intensity2;
		
		
		float norm = unit->chromanorm_;
		
		//reset chroma unless keeping previous via leaky integration
		if((int)(ZIN0(5)) > 0) {
            
			float integration  = ZIN0(6);
			
			if (integration>0.999999999f) {
				integration = 0.999999999f;
			}
			
			for ( i=0; i<divisions; ++i) {
				chroma[i] *= integration;
			}
			
			
			
		} else {
			
            for ( i=0; i<divisions; ++i) {
                chroma[i] = 0.0;
            }
			
		}
		
		//for (int j=0; j< numindices; j+=2) {
        //
        //
        //
        //		}
        //
		
		for ( j=0; j<octaves; ++j) {
			
			indexnow = 2*j*divisions;
			
			for ( i=0; i<divisions; ++i) {
                
				index = 2*indexdata[indexnow];
				real= data[index];
				imag= data[index+1];
				intensity1 = (real*real) + (imag*imag);
				weight1= indexweights[indexnow];
				
				index+=2;
				real= data[index];
				imag= data[index+1];
				intensity2 = (real*real) + (imag*imag);
				weight2= indexweights[indexnow+1];
				
				
				chroma[i] += (norm*(weight1*intensity1 + weight2*intensity2));
				
				indexnow+=2;
			}
			
		}
		
        
        
        if(perframenormalize) {
            
//            //norm by max: actually want norm by sum
//            float maxval = 0.0f;
//            
//            for (i=0; i<divisions; ++i) {
//                
//                if(chroma[i]>maxval) maxval = chroma[i];
//            }
//            
//            //don't normalize if values negligible
//            if (maxval>0.0001f) {
//                
//                maxval = 1.0f/maxval;
//                
//                for (i=0; i<divisions; ++i)
//                    chroma[i] *= maxval;
//                
//            }
//            
            //norm by sum; makes probability distribution over chroma
            float sumval = 0.0f;
            
            for (i=0; i<divisions; ++i) {
                
                sumval += chroma[i];
            }
            
            //don't normalize if values negligible
            if (sumval>0.0001f) {
                
                sumval = 1.0f/sumval;
                
                for (i=0; i<divisions; ++i)
                    chroma[i] *= sumval;
                
            }
            
            
            
        }
        
        
        
	}

    
	for (i=0; i<divisions; ++i)
		ZOUT0(i) = chroma[i];
	
	
}


PluginLoad(Chromagram) {
	
	init_SCComplex(inTable);
	
	ft = inTable;
    
	DefineDtorCantAliasUnit(Chromagram);
    
}





