/*
 *  Created by Nicholas Collins on 13/08/2010.
 *  Copyright 2010 Nicholas M Collins. All rights reserved.
 */

//William A. Sethares method (see e.g., Consonance-Based Spectral Mappings, CMJ 22(1): 56-72, 1998). 
//this is the harmonic dissonance method, looking for peaks in spectrum, and looking at proximity of peaks as indicating potential dissonance
//An alternative roughness formulation is a multiband filterbank with amplitude modulation detection in each band looking for rates of 15-100Hz or so 

//outputs dissonance measure
//could also output number of detected peaks as an interesting auxilliary noisiness measure (could get average dissonnance per peak from that, though not especially meaingful in itself?)


//#define SC_DARWIN
#include "SC_PlugIn.h"
#include "FFT_UGens.h"


InterfaceTable *ft; 


struct SensoryDissonance : public Unit  
{
	int fftsize_; 
	int topbin_; 
	int frequencyperbin_; 
	
	float dissonance_; 

	int maxnumpeaks_; 
	float peakthreshold_; 
	
	float * peakfreqs_;
	float * peakamps_; 
	
	float norm_; 
	int clamp_; 
	
	int initfftsize_; 
};



//convert FFT 
struct SpectralEntropy : public Unit  
{
	//float entropy_; 
    
    int numbands_; 
    int fftsize_;
    int * bandindices_; 
    float * intensities_; 
    float * entropies_; 
    
};




extern "C" {  
	
	void SensoryDissonance_next_k(SensoryDissonance *unit, int inNumSamples);
	void SensoryDissonance_Ctor(SensoryDissonance* unit);
	void SensoryDissonance_Dtor(SensoryDissonance* unit);
    
    void SpectralEntropy_next_k(SpectralEntropy *unit, int inNumSamples);
	void SpectralEntropy_Ctor(SpectralEntropy* unit);
	void SpectralEntropy_Dtor(SpectralEntropy* unit);

}





void SensoryDissonance_Ctor( SensoryDissonance* unit ) {
	
	//int i, j; 
	
	unit->initfftsize_ = 0; //must defer this till have a buffer to check
	
	unit->maxnumpeaks_ = ZIN0(1); //100; 
	unit->peakthreshold_ = ZIN0(2);
	unit->peakfreqs_ = (float *)RTAlloc(unit->mWorld, sizeof(float)*unit->maxnumpeaks_); 
	unit->peakamps_ = (float *)RTAlloc(unit->mWorld, sizeof(float)*unit->maxnumpeaks_);
	
	unit->norm_ = ZIN0(3); //0.01/unit->maxnumpeaks_; //unit->fftsize_; 
	
	unit->clamp_ = ZIN0(4); 
	
	SETCALC(SensoryDissonance_next_k);


}

void SensoryDissonance_Dtor(SensoryDissonance *unit)
{
	
	RTFree(unit->mWorld, unit->peakfreqs_);
	RTFree(unit->mWorld, unit->peakamps_);
	
}


//NEXT: destructor then next function using octaves and divisions slots, with appropriate power calc from fft data as go 


void SensoryDissonance_next_k( SensoryDissonance *unit, int inNumSamples ) {
	
	//int i, j; 
	
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
		
		
		if(unit->initfftsize_ ==0) {
	
			double sr = unit->mWorld->mFullRate.mSampleRate; //never trust SAMPLERATE, gives UGens output rate, not audio rate
			//float nyquist = sr*0.5; 
			
			unit->fftsize_ = buf->frames; //ZIN0(1);
			//printf("check fftsize %d \n",unit->fftsize_);
			unit->topbin_= unit->fftsize_*0.25; 
			
			unit->frequencyperbin_ = sr / unit->fftsize_; 
			
			unit->initfftsize_ = 1; 
		}
		
		//make sure in real and imag form
		//SCComplexBuf * complexbuf = ToComplexApx(buf);  
		
		float * data= (float *)ToComplexApx(buf);
		
		//float * data= buf->data;
		
		//int numindices= unit->numindices_; 
		
		float * peakfreqs= unit->peakfreqs_; 
		float * peakamps= unit->peakamps_; 
		
		float real, imag; 
		int index; 
		
		int numpeaks = 0; 
		int maxnumpeaks = unit->maxnumpeaks_; 
		
		float intensity; 
		float position; 
		
		float threshold = unit->peakthreshold_; 
		
		//create powerspectrum
		
		float prev=0.0, now=0.0, next=0.0; 
		
		float frequencyperbin = unit->frequencyperbin_; 
		
		//float totalpeakpower = 0.0f; 
		float temp1, refinement; 
		
		for (int j=1; j<=unit->topbin_; ++j) {
				
				index = 2*j; 
				real= data[index];			
				imag= data[index+1];
				intensity = (real*real) + (imag*imag);  
//				
				
			
			next= intensity; 
			
			if(j>=3) {
			
				//hunt for peaks
				
				//look for peak by scoring within +-3
				//assume peak must be centrally greater than 60dB say
				
				//powertest_
				//minpeakdB_ was 60
				
				if (now>threshold)  {
					
					//y1= powerspectrum_[i-1];
					//				//y2= valuenow;
					//				y3= powerspectrum_[i+1];
					//				
					if ((now>prev) && (now>next)) {
						
						//second peak condition; sum of second differences must be positive
						//NCfloat testsum= (valuenow - powerspectrum_[i-2]) + (valuenow - powerspectrum_[i+2]);
						
						//if (testsum>0.0) {
						
						//refine estimate of peak using quadratic function
						//see workbook 28th Jan 2010
						
						temp1= prev+next-(2*now);  					
						
						if (fabs(temp1)>0.00001) {
							position=(prev-next)/(2*temp1);
							
							//running quadratic formula
							refinement = (0.5*temp1*(position*position)) + (0.5*(next-prev)*position) + now;
							//refinement= y2 -  (((y3-y1)^2)/(8*temp1));
							
						} else {
							//degenerate straight line case; shouldn't occur
							//since require greater than for peak, not equality
							
							position=0.0; //may as well take centre
							
							//bettervalue= max([y1,y2,y3]); %straight line through them, find max
							
							refinement= now; //must be max for else would have picked another one in previous calculation! %max([y1,y2,y3]);
							
						}
						
						//correct??????????????????????????????
						peakfreqs[numpeaks] = (j-1+position)*frequencyperbin; //frequencyconversion; 
						//printf("peakfrequencies %d is %f from i %d position %f freqperbin %f \n", numpeaks_,peakfrequencies_[numpeaks_],i, position, frequencyperbin_); 
						
						peakamps[numpeaks] = sqrt(refinement); //Sethares formula requires amplitudes  
						//totalpeakpower += refinement; 
						
						//cout << " peak " << numpeaks_ << " " << peakfrequencies_[numpeaks_] << " " << refinement << " " ; 
						
						++numpeaks; 
						
						//}
						
					}
					
				}
				
				//test against maxnumberpeaks_
				if ( numpeaks == maxnumpeaks )
					break;
				
				
				
			}
			
			prev = now; now=next; 
			
			
					
			
			
			
			
		}
		
		
		//now have list of peaks: calculate total dissonance: 
		
		//iterate through peaks, matching each to min of next 10, and no more than octave, using Sethares p. 58 CMJ article
		
		float dissonancesum = 0.0; 
		
		float f1, v1, f2, v2; 
		float d; 
		float diff; //, minf; 
		float s, a, b; 
		float octave; 
		
		for (int i=0; i<(numpeaks-1); ++i) {
			
			f1 = peakfreqs[i]; 
			v1 = peakamps[i]; 
			s = 0.24f/(0.21f*f1+19.f); //constant needed as denominator in formula
			a = -3.5f*s; 
			b= -5.75f*s; 
			
			octave = 2.0f*f1; 
			
			for (int k=i+1; k<sc_min(i+20,numpeaks); ++k) {
					
				f2 = peakfreqs[k]; 
				v2 = peakamps[k]; 
				
				if(f2>octave) break; //shortcut escape if separated by more than an octave
				
				diff = f2-f1; //no need for fabs, f2>f1
				//minf =  //always f1 lower
				
				
				
				d = v1*v2*(exp(a*diff) - exp(b*diff));
				
				dissonancesum += d; 
			}
			
		}
		
		unit->dissonance_ = sc_min(unit->clamp_,dissonancesum*unit->norm_); //numpeaks; //dissonancesum;  //divide by fftsize as compensation for amplitudes via FFT

	}
	

	//ZOUT0(i) = unit->dissonance_;
	ZOUT0(0) = unit->dissonance_;
	
}






void SpectralEntropy_Ctor( SpectralEntropy* unit ) {
	
	int i, j; 
    
    unit->fftsize_ = ZIN0(1);  
    unit->numbands_ = ZIN0(2);
    
    int numbins = unit->fftsize_/2; //won't use actual Nyquist bin in this UGen

    int split = numbins/(unit->numbands_); 
    
    if(split<1) {
        
        split = 1; 
        unit->numbands_ = numbins; 
    }
    
    
    //will include guard element at top
	unit->bandindices_ = (int *)RTAlloc(unit->mWorld, sizeof(int)*(unit->numbands_+1)); 
    unit->entropies_ = (float *)RTAlloc(unit->mWorld, sizeof(float)*unit->numbands_); 
    unit->intensities_ = (float *)RTAlloc(unit->mWorld, sizeof(float)*numbins); 
    
    for (i=0; i<unit->numbands_; ++i) {
        
        unit->entropies_[i] = 0.0f; 
        
        unit->bandindices_[i] = split*i; 
        
    }
    
    //guard can be one above actual final array slot index since always use less than in code below
    unit->bandindices_[unit->numbands_] = numbins; //Nyquist position 
	
	SETCALC(SpectralEntropy_next_k);
    
    
}


void SpectralEntropy_Dtor(SpectralEntropy *unit)
{
	
	RTFree(unit->mWorld, unit->bandindices_);
	RTFree(unit->mWorld, unit->entropies_);
	RTFree(unit->mWorld, unit->intensities_);
    
}




void SpectralEntropy_next_k( SpectralEntropy *unit, int inNumSamples ) {
    
    int i,j; 
    
    int numbands = unit->numbands_; 
    int * bandindices = unit->bandindices_; 
    float * entropies = unit->entropies_; 
    float * intensities = unit->intensities_; 
    
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
              
        if(unit->fftsize_ == buf->frames) {
		
		//make sure in real and imag form
		//SCComplexBuf * complexbuf = ToComplexApx(buf);  
		
		float * data= (float *)ToComplexApx(buf);
		
		//float * data= buf->data;
    
		float real, imag; 
        float intensity; 
		
        data[1] = 0.0f; //avoid issues with dc, nyquist packed together, just want dc here
        
        for  (j=0; j<numbands; ++j) {
            
            int start = bandindices[j]; 
            int end = bandindices[j+1]; 
            
            float max = 0.0f; 
            float entropysum = 0.0f; 
            
            //less than because of guard elements
            for  (i=start; i<end; ++i) {
                
                int index = 2*i; 
                real= data[index];			
                imag= data[index+1];
                intensity = (real*real) + (imag*imag);  
                
                intensities[i] = intensity; 
                
                if(intensity>max) {
                    
                    max = intensity; 
                    
                }
                
            }
            
            if(max>0.0f) {
                
                max = 1.0f/max; //will be used as straight multiplier in calculation now
                
                for  (i=start; i<end; ++i) {
                        
                    float prob = intensities[i] * max; 
                    
                    //negative worked in via -= 
                    if(prob>0.0f) entropysum -= prob * log2(prob);  
                    
                    //leave sum alone otherwise, has no contribution if 0.0 probability
                }
                
                entropies[j] = entropysum; 
                
            } else 
                entropies[j] = 0.0f; 
                   
        }
    
            
        }
        
	}
	
    for (i=0; i<numbands; ++i)
		ZOUT0(i) = entropies[i];
    
	
}






PluginLoad(SensoryDissonance) {
	
	init_SCComplex(inTable);
	
	ft = inTable;

	DefineDtorCantAliasUnit(SensoryDissonance);
	DefineDtorCantAliasUnit(SpectralEntropy);	
}





