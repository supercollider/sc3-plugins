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

//UGens by Nick Collins 
//Released under the GNU GPL as extensions for SuperCollider 3

//building: SC 3.5/3.4 compatible:
//cmake -DSC_PATH=/data/gitprojects/SuperCollider-Source3.5 -DCMAKE_OSX_ARCHITECTURES='i386;x86_64' ..

//3.6
//cmake -DSC_PATH=/data/gitprojects/scdev/supercollider -DCMAKE_OSX_ARCHITECTURES='x86_64' ..


#include "SC_PlugIn.h"
#include <math.h>
#include <stdlib.h>

InterfaceTable *ft; 


struct AttackSlope : public Unit  
{
    
    float lastenergy_; 
    
    int windowsize_; 
    float * windowdata_;
    float * runningsumdata_; 
    int datapointer_; //circular buffer position
    float runningsum_; 
    int runningsumcounter_; 
    int runningsumrecalculate_; //needs to be recalculated once in a while to avoid rounding error accumulation
    
    float leakyintegrator_; 
    
    float * peakpickdata_; 
    int peakpicksize_; 
    int peakpickpointer_; 
    
    //to search for minima of recent energy for actual onset time, and for slope estimate up to peak 
    float * maxenergydata_; 
    int maxenergysize_; 
    int maxenergypointer_; 
    
    float lastslope_;
    float slopeaverage_; 
    long lastonsettime_; 
    float * slopedata_; 
    int slopesize_; 
    int slopedatapointer_;
    int slopescollected_; //allows test only on slopes collected, avoids initial inaccurate readings
    
    
    int blockssincelastonset_;
    long ugenage_; 
    
};



extern "C" {  
	
	void AttackSlope_next(AttackSlope* unit, int inNumSamples);
	void AttackSlope_Ctor(AttackSlope* unit);
    void AttackSlope_Dtor(AttackSlope* unit);
    
}


void AttackSlope_Ctor( AttackSlope* unit ) {
	
    //input,windowsize=1024,peakpicksize=10,leak=0.999,energythreshold=0.01,sumthreshold=100,mingap=0
    
	//OUT0(0) = ZIN0(1); 

    //zero outputs at sample one
    OUT0(0) = 0.0f; 
//    OUT0(1) = 0.0f; 
//    OUT0(2) = 0.0f; 
//    OUT0(3) = 0.0f; 
//    OUT0(4) = 0.0f; 
//    
    unit->windowsize_ = ZIN0(1);
    unit->peakpicksize_ = ZIN0(2);
    unit->peakpickpointer_ = 0; 
    
    int blocksize = unit->mWorld->mFullRate.mBufLength; 
    
    if(unit->windowsize_%blocksize!=0) {
     
        unit->windowsize_ +=  blocksize - (unit->windowsize_%blocksize); 
        //round up
        
    }

    unit->windowdata_ = (float *) RTAlloc(unit->mWorld, sizeof(float)* unit->windowsize_ );    
    unit->runningsumdata_ = (float *) RTAlloc(unit->mWorld, sizeof(float)* unit->windowsize_ );
    unit->peakpickdata_ = (float *) RTAlloc(unit->mWorld, sizeof(float)* unit->peakpicksize_ ); //last N blocks max runningsums to make peak pick decision 
    
    int i; 
    
    for (i=0; i<unit->windowsize_; ++i) {
        unit->windowdata_[i] = 0.0f; 
        unit->runningsumdata_[i] = 0.0f; 
        
    }
      
    for (i=0; i<unit->peakpicksize_; ++i) 
        unit->peakpickdata_[i] = 0.0f; 

    unit->maxenergysize_ = 100; //100*64 = 6400 samples
    unit->maxenergydata_ = (float *) RTAlloc(unit->mWorld, sizeof(float)* unit->maxenergysize_ );  
    unit->maxenergypointer_= 0; 
    
    for (i=0; i<unit->maxenergysize_; ++i) 
        unit->maxenergydata_[i] = 0.0f; 

    
    unit->lastslope_ = 0.0f;
    unit->slopeaverage_ = 0.0f;
    unit->lastonsettime_ = -1.0f; 
    
    unit->slopesize_ = ZIN0(7); //10; numslopesaveraged argument to UGen
    unit->slopedata_ = (float *) RTAlloc(unit->mWorld, sizeof(float)* unit->slopesize_ ); 

    for (i=0; i<unit->slopesize_; ++i) 
        unit->slopedata_[i] = 0.0f; 

    unit->slopedatapointer_ = 0;
    unit->slopescollected_ = 0; 
    
    unit->runningsum_ = 0.0f; 
    unit->runningsumcounter_ = 0; 
    unit->runningsumrecalculate_ = unit->windowsize_/blocksize;  
   
    unit->lastenergy_ = 0.0f; 
    unit->leakyintegrator_ = 0.0f; 
    
    unit->datapointer_ = 0; 
    
    unit->blockssincelastonset_ = 999999; 
    
//    unit->magnitudes_ = (float *) RTAlloc(unit->mWorld, sizeof(float)* unit->mediansize_ );
//    

//    
    
    unit->ugenage_ = 0; 
    
	SETCALC(AttackSlope_next);
	
    
    
}


void AttackSlope_Dtor( AttackSlope* unit ) {
    
    RTFree(unit->mWorld, unit->runningsumdata_); 
    RTFree(unit->mWorld, unit->windowdata_);
    RTFree(unit->mWorld, unit->peakpickdata_);
    RTFree(unit->mWorld, unit->maxenergydata_);
    RTFree(unit->mWorld, unit->slopedata_);
}


void AttackSlope_next( AttackSlope *unit, int inNumSamples ) {
	
	int i,j;
    
    int numSamples = unit->mWorld->mFullRate.mBufLength;
 
    float * windowdata = unit->windowdata_; 
    float * runningsumdata = unit->runningsumdata_; 
    float runningsum =  unit->runningsum_;

    int windowsize = unit->windowsize_; 
    int datapointer = unit->datapointer_; 
    
    float leakyintegrator = unit->leakyintegrator_; 
    float leak = ZIN0(3); //leak coefficient for leakyintegrator
    float energythreshold = ZIN0(4);
    float sumthreshold = ZIN0(5);
    int postonsetreset = ZIN0(6);
    //input,windowsize=1024,peakpicksize=10,leak=0.999,energythreshold=0.01,sumthreshold=100,mingap=0
    
    //printf("here in next\n"); 
    
//SC prototype    
//    var source = PlayBuf.ar(1,b);
//    var slope1 = Slope.ar(log(source.squared+1))*0.001; 
//    var peak1 = RunningSum.ar(if(slope1>0,slope1,0.0),1024);
//    //could have rule about number in a row here...
//    var leaky = Integrator.ar(peak1,0.99)*0.001;
//    //after onset event, sensitivity drops off; so leaks faster after onset

	float *input = IN(0);
    //float *out = OUT(0); // NB not ZOUT0
    
    float lastenergy = unit->lastenergy_; 
    
    float maxval = 0.0f; 
    float maxenergy = 0.0f; 
    float compensation = 1.0f/windowsize; 
    
    for (i=0; i<numSamples; ++i) {
        
        float source = input[i]; 
        
        float energy = log(source*source+1); 
        
        if(energy>maxenergy) maxenergy = energy; 
        
        windowdata[datapointer] = energy;
        
        float slope = energy - lastenergy; 
        
        //only positive going slope of interest
        if(slope<0.0f) slope = 0.0f; 
        
        runningsum += slope - runningsumdata[datapointer];  
        
        runningsumdata[datapointer] = slope;
        
        datapointer = (datapointer+1)%windowsize; 
        
        leakyintegrator = leakyintegrator * leak + (runningsum*compensation); 

        if(leakyintegrator>maxval) {
            
            maxval = leakyintegrator; 
            
        }
        
    }
 
    //decision time 
    //minimal conditions: 
    //
    //if energy at least X
    //if average recent energy < 0.5*Y where Y is current energy
    //if peak pick detection function 
    //if no recent onset trigger 
    //onset trigger
    //find recent amplitude minima
    //measure slope 
    
    //peak pick decision
    
    //maxenergy>
    
    float * peakpickdata = unit->peakpickdata_; 

//compare latest value to last N
//    float sumdiff=0.0f; 
//    
//    for (i=0; i<unit->peakpicksize_; ++i) {
//        
//        float difference = maxval - peakpickdata[i]; 
//        
//        //greater penalty if negative
//        //sumdiff += difference>0.0f?difference:(2*difference);  
//       
//        //greater contribution if positive
//        sumdiff += difference>0.0f?difference:10.0f*difference; 
//        
//    }
//    
//    if(sumdiff<0.0f) sumdiff = 0.0f; 
//    
//    if((maxenergy>energythreshold) && (sumdiff>sumthreshold) && (unit->blockssincelastonset_>30)) {
//        
//        //maxval=0.0f; 
//        
//        unit->blockssincelastonset_ = 0; 
//    }
    
    
    peakpickdata[unit->peakpickpointer_] = maxval; //log(maxval+1); 
    
    unit->peakpickpointer_ = (unit->peakpickpointer_+1)%unit->peakpicksize_; 
    
    ++unit->blockssincelastonset_; 

    float midval = peakpickdata[(unit->peakpickpointer_+unit->peakpicksize_-(unit->peakpicksize_/2))%unit->peakpicksize_];
    
    float sumdiff=0.0f; 
    
    for (i=0; i<unit->peakpicksize_; ++i) {
        
        float difference = midval - peakpickdata[i]; 
        
        //greater penalty if negative
        //sumdiff += difference>0.0f?difference:(2*difference);  
        
        //greater contribution if positive
        sumdiff += difference>0.0f?difference:3.0f*difference; 
        
    }

    
    if(sumdiff<0.0f) sumdiff = 0.0f; 
    
    float * maxenergydata = unit->maxenergydata_; 
    
    if((maxenergy>energythreshold) && (sumdiff>sumthreshold) && (unit->blockssincelastonset_>postonsetreset)) {
        
        //maxval=0.0f; 
        
        unit->blockssincelastonset_ = 0; 
        unit->lastonsettime_ = unit->ugenage_; //convert to actual time?
        
        //find recent minima
        //first time energy below X
        //else minima in window 
        
        int searchsize = unit->maxenergysize_; 
        
        int position = unit->maxenergypointer_+searchsize; 
        
        int minpos = 0;
        float minvalue = maxenergy; 
        
        for (i=0; i<searchsize; ++i) {
         
            int posnow = (position-i)%searchsize; 
            float energythen = maxenergydata[posnow]; 
            
            if(energythen<minvalue) {
                
                minvalue = energythen; 
                minpos = i; 
                
            }
            
        } 
        unit->lastonsettime_  -= (minpos+1); 
        
        //printf("slope detected %f %f %d \n",maxenergy,minvalue,minpos);
        
        //find slope
        unit->lastslope_ =  100.f*(maxenergy-minvalue)/(minpos+1); 
        
        //store new slope data
        unit->slopedata_[unit->slopedatapointer_] = unit->lastslope_; 
        unit->slopedatapointer_ = (unit->slopedatapointer_+1)%unit->slopesize_; 
        ++unit->slopescollected_; 
        
        
        //update average slope
        float averageslope = 0.f; 
        
        if(unit->slopescollected_>=unit->slopesize_) {
         
        for (i=0; i<unit->slopesize_; ++i) {
            averageslope += unit->slopedata_[i]; 
        
        }
        
        averageslope /= unit->slopesize_; 
            
        } else {
            
            averageslope = 0.0f; //unit->lastslope_; 
            
        }
        
        unit->slopeaverage_ = averageslope; 
        
        //printf("slope detected %f %f %d \n",averageslope,unit->lastslope_,(int)unit->lastonsettime_); 
        
        
    }
    
    maxenergydata[unit->maxenergypointer_] = maxenergy; 
    
    unit->maxenergypointer_ = (unit->maxenergypointer_+1)%unit->maxenergysize_; 
    
    unit->lastenergy_ = lastenergy; 
    
    ++unit->runningsumcounter_; 
    if(unit->runningsumrecalculate_ == unit->runningsumcounter_) {
        
        unit->runningsumcounter_ = 0; 
        
        //recalculate runningsum for robustness to avoid accumulation of error
        
        float sumnow = 0.0f; 
        for (i=0; i<unit->windowsize_; ++i) {
            sumnow += runningsumdata[i]; 
        }
            
        runningsum = sumnow; 
    }
     
    unit->runningsum_ = runningsum; 
    
    unit->datapointer_ = datapointer; 
    
    unit->leakyintegrator_ = leakyintegrator; 
    
    unit->lastenergy_ = lastenergy;  
    
    OUT0(0) = unit->blockssincelastonset_<postonsetreset?1.0:0.0;
    OUT0(1) = unit->lastonsettime_; 
    OUT0(2) = unit->lastslope_;
    OUT0(3) = unit->slopeaverage_; 
    OUT0(4) = maxval;  //maxval; //leakyintegrator; 
    OUT0(5) = sumdiff/unit->peakpicksize_; 
    
    //[0] onset trigger 
    //[1] corrected to minima time of last onset
    //[2] most recent slope measurement
    //[3] average slope calculation 
    //[4] detection function
    //[5] peakpick function

//    
//    OUT0(0) = -1.f;
//    OUT0(1) = -1.f;
//    
    //printf("actual output %f %f \n",OUT0(0),OUT0(1));
    
    
    ++unit->ugenage_; 
    
 }



PluginLoad(AttackSlope)
{
    
    ft = inTable;
	
    //DefineDtorUnit
    DefineDtorCantAliasUnit(AttackSlope);
}


