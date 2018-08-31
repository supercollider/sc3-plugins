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

//two modes? last N events, or events in last N seconds


#include "SC_PlugIn.h"
#include <math.h>
#include <stdlib.h>

InterfaceTable *ft; 

//http://see-programming.blogspot.co.uk/2013/02/double-ended-queue-dequeue.html
//http://www.dreamincode.net/forums/topic/35344-c-deque-tutorial/

#include <deque>
using namespace std; //::deque;

const int g_maxnumonsets = 200;

struct OnsetStatistics : public Unit  {

    //int maxnumonsets;
    
    int samplecounter_;
    
    float windowsize_;
    float hopsize_;
    int windowsizeinsamples_;
    int hopsizeinsamples_;
    
    float krsampletotime_; //conversion factor to get from kr samples to actual time in seconds
    
    //int * onsettimes_; //relative to last updated master position
    //int onsettimesindex_;

    deque<int> * onsettimes_;
    float iois_[g_maxnumonsets];
    
    float previnput_;
};



extern "C" {  
	
	void OnsetStatistics_next(OnsetStatistics* unit, int inNumSamples);
	void OnsetStatistics_Ctor(OnsetStatistics* unit);
    void OnsetStatistics_Dtor(OnsetStatistics* unit);
    
}


void OnsetStatistics_Ctor( OnsetStatistics* unit ) {
	
    //input,windowsize=1024,peakpicksize=10,leak=0.999,energythreshold=0.01,sumthreshold=100,mingap=0
    
	//OUT0(0) = ZIN0(1); 

    //zero outputs at sample one
    //OUT0(0) = 0.0f;
    //OUT0(1) = 0.0f;
    //OUT0(2) = 0.0f;
    
    unit->previnput_ = 0.f;
    
    unit->krsampletotime_ = unit->mWorld->mBufRate.mSampleDur;
    unit->samplecounter_ = 0;
    //unit->maxnumonsets = 100;
    
    
    //could change dynamically if within next function
    unit->windowsize_ = ZIN0(1);
    unit->windowsizeinsamples_ = unit->windowsize_/unit->krsampletotime_;
    
    //printf("sample time %f window in samples %d\n",unit->krsampletotime_,unit->windowsizeinsamples_);
    
    //should use placement new but you only live once so not bothering right now; would also require custom allocator for deque to use etc
    //http://en.wikipedia.org/wiki/Placement_syntax#Placement_delete
    
    unit->onsettimes_ = new deque<int>; //(deque<int> *)RTAlloc(unit->mWorld, sizeof(deque<int>))
    
    //printf("sanity check %lu\n",unit->onsettimes_->size());
 
   // unit->onsettimes_ = (int *) RTAlloc(unit->mWorld, sizeof(int)* unit->maxnumonsets);
    //unit->onsettimesindex_ = 0;
    
    int i; 
    
//    for (i=0; i<unit->maxnumonsets; ++i) {
//        unit->onsettimes_[i] = -99999999;
//        
//    }
    
    
	SETCALC(OnsetStatistics_next);
	
    
    
}


void OnsetStatistics_Dtor( OnsetStatistics* unit ) {
    
    //RTFree(unit->mWorld, unit->onsettimes_);
    
    delete unit->onsettimes_;
}


void OnsetStatistics_next( OnsetStatistics *unit, int inNumSamples ) {
	
	int i,j;
    
    //int numSamples = unit->mWorld->mFullRate.mBufLength;
    //int samplesperblock = unit->mWorld->mFullRate.mBufLength;
    
 	float input = IN0(0); //(0)[0];
    float prev = unit->previnput_;
    
    int sampletimenow = unit->samplecounter_;
    
    int earliestallowed = sampletimenow - unit->windowsizeinsamples_ + 1;
    //if(earliestallowed<0)
    
    //printf("input %f earliestallowed %d sampletimenow %d\n",input, earliestallowed, sampletimenow);
    
    
    deque<int>& onsettimes = *unit->onsettimes_;
    
    bool stillgoing = true;
    //http://www.dreamincode.net/forums/topic/35344-c-deque-tutorial/
    //pop_front, pop_back, push_front, push_back
    
    
    //printf("remove old %lu\n",onsettimes.size()); //,onsettimes[0]);
    //remove any too old
    while(onsettimes.size()>0 && stillgoing) {
        
        if ((onsettimes[0]<earliestallowed) || (onsettimes.size()>g_maxnumonsets))
            onsettimes.pop_front();
        else
            stillgoing = false;
    
    }
    
    //for (i=0; i<numSamples; ++i) {
    //}
 
    
    //printf("trigger?\n");
    
    //trigger
    if(prev<=0.f && input>0.f) {
        
        onsettimes.push_back(sampletimenow); //push_back is most recent, highest index
    }
    
    //printf("calculate statistics \n");
    
    //calculate statistics
    float ioi;
    float mean = 0.f;
    float conversion = unit->krsampletotime_;
    
    float * iois = unit->iois_;
    
    deque<int>::iterator itr;
    
    i = 0;
    
    for (itr=onsettimes.begin(); itr!=onsettimes.end(); ++itr) {
    
        iois[i] = *itr;
        
        //ioi = (*itr - sampletimenow) * conversion;
        
        //mean += ioi;
        
        //iois[i] = ioi;
        
        ++i;
    }
    
    
    int numiois = onsettimes.size()-1; //was -1
    if (numiois<0) numiois = 0;
    
    //printf("numiois %d\n",numiois);
    
    for (int i=0; i<numiois; ++i) {
        iois[i] = (iois[i+1] - iois[i])*conversion;
        mean += iois[i];
    }
    
    if(numiois>=1)
    mean /= numiois;
    //else
    //mean = -1.0f;
    
    //printf("mean %f\n",mean);
    
    float variance = 0.f;
    float summand;
    
    
    for (int i=0; i<numiois; ++i) {
        
        summand = iois[i]-mean;
        
        variance += summand*summand;
    
    }
    
    if(numiois>=1)
        variance /= numiois;
    //else
    //    variance = 0.0f;
    

    //printf("variance %f\n",variance);
    
    
    OUT0(0) = (float)numiois;
    OUT0(1) = mean;
    OUT0(2) = sqrt(variance); //return standard deviation
    //OUT0(3) =

    
    unit->previnput_ = input;
    
    ++sampletimenow;
    
    //correct all times to avoid spillage
    if(sampletimenow>10000) {
        
        for (int i=0; i<onsettimes.size(); ++i)
            onsettimes[i] -= sampletimenow;
        
        sampletimenow = 0;
        
    }
    
    unit->samplecounter_ = sampletimenow;
    
 }



PluginLoad(OnsetStatistics)
{
    
    ft = inTable;
	
    //DefineDtorUnit
    DefineDtorCantAliasUnit(OnsetStatistics);
}


