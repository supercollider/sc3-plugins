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

//UGens by Nick Collins  http://www.informatics.sussex.ac.uk/users/nc81/
//Released under the GNU GPL as extensions for SuperCollider 3

//Algorithm from
//Derry FitzGerald (2010) "Harmonic/Percussive Separation using Median Filtering" International Conference on Digital Audio Effects (DAFx)

#include "SC_PlugIn.h"
#include <math.h>
#include <stdlib.h>
#include "FFT_UGens.h"


//InterfaceTable *ft; 


//for efficiency, could fill two 2D arrays, one set to vertical, one to horizontal in indexing, for quick copy to array pre sort
//but via amortisation works OK 

struct MedianSeparation : public Unit  
{
    
    int mediansize_; 
    int midpoint_; 
    //int hardorsoft_;
    //float p_; 
    
    int fftsize_; 
    int fftbins_; 
    //int numframes_; 
    
    int magnitudeposition_; 
    int phaseposition_; //different since different sizes of data stored
    
    //separate storage
    float * magnitudes_;  //2D grid
    float * phases_;
    float * collection_;
    float * horizontalmedians_;
    float * verticalmedians_;
    
    int amortisationstep_; 
    int bufnum1_;
    int bufnum2_; 
    
};



extern "C" {  
	
	void MedianSeparation_next(MedianSeparation* unit, int inNumSamples);
	void MedianSeparation_Ctor(MedianSeparation* unit);
    void MedianSeparation_Dtor(MedianSeparation* unit);
    
}


//for sorting
int cmp(const void* vp, const void* vq);

int cmp(const void* vp,const void* vq) {
	const float *p= static_cast<const float*>(vp);
	const float *q= static_cast<const float*>(vq);
    
	float diff= *p - *q;
    
	return ((diff>=0.0) ? ((diff >0.0) ? -1 : 0) : +1);
}



void MedianSeparation_Ctor( MedianSeparation* unit ) {
	
    //fft, fftharmonic, fftpercussive, fftsize, mediansize=17, hardorsoft=0, p=1
    
    //IFFT needs to know what it is operating on
	OUT0(0) = ZIN0(1); //-1;
    OUT0(1) = ZIN0(2);
    
    unit->fftsize_ = ZIN0(3); //so can set things up in constructor, rather than later on
    unit->mediansize_ = ZIN0(4); //default 17, may cause trouble otherwise? 
    
    //printf("MedianSeparation starting %d %d \n",unit->fftsize_,unit->mediansize_); 
    
    if(unit->mediansize_<3)
        unit->mediansize_ = 17; 
    
    unit->midpoint_ = unit->mediansize_/2; 
    
    
    
    unit->fftbins_ = (unit->fftsize_/2) +1; //actually /2 + 1 but polar form has separate packed dc and nyquist 
    
    unit->magnitudeposition_= 0; 
    unit->phaseposition_= 0; 
    
    //separate storage
    unit->magnitudes_ = (float *) RTAlloc(unit->mWorld, sizeof(float)* unit->mediansize_ * unit->fftbins_);
    
    
    unit->phases_ = (float *) RTAlloc(unit->mWorld, sizeof(float)* (unit->midpoint_+1) * unit->fftbins_); //only store half data since only need phases for midpoint about to output; this necessitates holding midpoint+1 values (to include just arrived)
    
    //float * data_; //2D grid
    
    //zero out all magnitudes and phases 
    
    int i; 
    
    for (i=0; i<unit->mediansize_ * unit->fftbins_; ++i)
        unit->magnitudes_[i] = 0.0f; 
    
    for (i=0; i<(unit->midpoint_+1) * unit->fftbins_; ++i)
        unit->phases_[i] = 0.0f;
    
    
    unit->collection_ = (float *) RTAlloc(unit->mWorld, sizeof(float)* unit->mediansize_); //reusable array for sorting and finding median
    unit->horizontalmedians_ = (float *) RTAlloc(unit->mWorld, sizeof(float)* unit->fftbins_); 
    unit->verticalmedians_ = (float *) RTAlloc(unit->mWorld, sizeof(float)* unit->fftbins_);
    
    //need to go a step at a time over the vertical, horizontal sort and the eventual output (so takes four blocks to calculate, FFT must be at least 256)
    unit->amortisationstep_=0; 
    
	SETCALC(MedianSeparation_next);
	
    
    
}


void MedianSeparation_Dtor( MedianSeparation* unit ) {
    
    //assumes any NRT functionality cancelled by now so no danger
    
    RTFree(unit->mWorld, unit->magnitudes_); 
    RTFree(unit->mWorld, unit->phases_); 
    RTFree(unit->mWorld, unit->collection_); 
    RTFree(unit->mWorld, unit->horizontalmedians_);
    RTFree(unit->mWorld, unit->verticalmedians_);
    
}


void MedianSeparation_next( MedianSeparation *unit, int inNumSamples ) {
	
	int i,j;
    
    //calculate medians around pos-midpoint
    int medsize = unit->mediansize_; 
    int mid = unit->midpoint_; 
    
    float * mags = unit->magnitudes_; 
    float * phases = unit->phases_;
    
    float * array = unit->collection_; 
    
    int numbands = unit->fftbins_; 
    int top = numbands-1; 
    
    int pos = unit->magnitudeposition_; 
    
    int midindex = (pos+medsize-mid)%medsize; 
    
    
    float * vertical = unit->verticalmedians_; 
    
    float * horizontal = unit->horizontalmedians_; 
    
    float * magsnow = mags + (midindex*numbands);
    
    
    //cover outputs before early return in macro
    OUT0(0) = -1.f;
    OUT0(1) = -1.f;
    
    
    //0 normal, 
    //1 vertical sort
    //2 horizontal sort
    //3 final output
    
    switch (unit->amortisationstep_) {
        case 0:
        {
            
            //printf("case 0 %f \n",ZIN0(0));
            
            PV_GET_BUF
            
            //avoid output being overwritten by macro to pass input FFT buffer
            OUT0(0) = -1.f;
            OUT0(1) = -1.f;
            
            SCPolarBuf *polar = ToPolarApx(buf);
            
            //update data stored
            
            SCPolar * data = polar->bin; 
            
            int base = unit->magnitudeposition_ * numbands; 
            
            mags[base] = polar->dc; 
            mags[base+numbands-1] = polar->nyq; 
            //no need to set phases for these indices, since will always be 0.0 and initialised in constructor
            
            for (i=0; i<numbands-2; ++i)
                mags[base+i+1] = data[i].mag; 
            
            base = unit->phaseposition_ * numbands;
            
            for (i=0; i<numbands-2; ++i)
                phases[base+i+1] = data[i].phase; 
            
            uint32 bufnum1 = (uint32) IN0(1); //ZIN0(1);
            uint32 bufnum2 = (uint32) IN0(2); //ZIN0(2);
            
            //store for recall in step 3
            
            unit->bufnum1_ = bufnum1;
            unit->bufnum2_ = bufnum2;
            
            //printf("store for recall %d %d what? %d uh? %d %f %f \n",bufnum1,bufnum2,(uint32) IN0(3), (uint32) ZIN0(3), IN0(1),IN0(2));
            
            
            //will have returned early otherwise
            unit->amortisationstep_ =1; 
            break;
            
        }
        case 1:
        {   
            
            //printf("case 1\n");
            
            float medianormean = ZIN0(7); 
            
            //to avoid if inside loop, code copied twice with difference at point of array calculation (median or mean)
            if(medianormean<0.5f) {
                
                //printf("median calc vertical\n");     
                
                //vertical; easier to do in indexing 
                
                for (i=0; i<numbands; ++i) {
                    
                    int lower = i-mid; 
                    if(lower<0) lower =0; 
                    int higher = i+mid; 
                    if(higher>top) higher = top; 
                    int number = higher-lower+1; 
                    
                    float * pnow = magsnow + lower; 
                    
                    //collect into array
                    for (j=0; j<number; ++j)
                        array[j] = pnow[j]; 
                    
                    qsort(array,number,sizeof(float),cmp);
                    //result is midpoint of magnitudes
                    
                    vertical[i] = array[number/2];
                    
                }
                
            } else {
                
                float sum; 
                
                for (i=0; i<numbands; ++i) {
                    
                    int lower = i-mid; 
                    if(lower<0) lower =0; 
                    int higher = i+mid; 
                    if(higher>top) higher = top; 
                    int number = higher-lower+1; 
                    
                    float * pnow = magsnow + lower; 
                    //                    
                    //                    //collect into array
                    //                    for (j=0; j<number; ++j)
                    //                        array[j] = pnow[j]; 
                    //                    
                    sum = 0.0f; 
                    
                    //no need for intermediate array 
                    for (j=0; j<number; ++j)
                        sum += pnow[j]; 
                    
                    vertical[i] = sum/number;
                    
                }
                
                
                
            }
            
            
            unit->amortisationstep_ = 2; 
            break;
        }    
        case 2:
        {   
            
            //printf("case 2\n");
            //horizontal, requires cross steps within 2D array
            
            float medianormean = ZIN0(7); 
            
            if(medianormean<0.5f) {
                
                
                //printf("median calc horizontal\n");   
                
                for (i=0; i<numbands; ++i) {
                    
                    //no checks required for the medsize, but have to get correct indices
                    
                    //collect into array
                    for (j=0; j<medsize; ++j)
                        array[j] = mags[(j*numbands) + i]; 
                    
                    qsort(array,medsize,sizeof(float),cmp);
                    //result is midpoint of magnitudes
                    
                    horizontal[i] = array[mid];
                    
                }
                
            } else {
                
                //mean
                
                float sum; 
                float recip = 1.0f/medsize; 
                
                for (i=0; i<numbands; ++i) {
                    
                    //no checks required for the medsize, but have to get correct indices
                    
                    sum = 0.0f; 
                    //sum up directly, no need for array 
                    for (j=0; j<medsize; ++j)
                        sum+= mags[(j*numbands) + i]; 
                    
                    horizontal[i] = sum*recip;
                    
                }                
                
            }
            
            unit->amortisationstep_ = 3; 
            break;
            
        }     
        case 3:
        {
            
            //printf("case 3\n");
            uint32 bufnum1 = unit->bufnum1_; //(uint32) ZIN0(1);
            uint32 bufnum2 = unit->bufnum2_; //(uint32) ZIN0(2);
            
            //printf("now recall %d %d \n",bufnum1,bufnum2);
            
            World *world = unit->mWorld;
            
            if ((bufnum1 >= 0) && (bufnum2 >= 0) ) { 
                
                SndBuf *buf1, *buf2;
                
                if (bufnum1 >= world->mNumSndBufs) { 
                    int localBufNum = bufnum1 - world->mNumSndBufs; 
                    Graph *parent = unit->mParent; 
                    if(localBufNum <= parent->localBufNum) { 
                        buf1 = parent->mLocalSndBufs + localBufNum; 
                    } else { 
                        buf1 = world->mSndBufs; 
                    } 
                } else { 
                    buf1 = world->mSndBufs + bufnum1; 
                } 
                
                LOCK_SNDBUF(buf1); 
                
                if (bufnum2 >= world->mNumSndBufs) { 
                    int localBufNum = bufnum2 - world->mNumSndBufs; 
                    Graph *parent = unit->mParent; 
                    if(localBufNum <= parent->localBufNum) { 
                        buf2 = parent->mLocalSndBufs + localBufNum; 
                    } else { 
                        buf2 = world->mSndBufs; 
                    } 
                } else { 
                    buf2 = world->mSndBufs + bufnum2; 
                } 
                
                LOCK_SNDBUF(buf2); 
                
                int numbins1 = (buf1->samples >> 1)  + 1; 
                int numbins2 = (buf2->samples >> 1)  + 1; 
                
                //printf("check %d %d numbands %d further check %d %d \n",numbins1,numbins2,numbands,buf1->samples,buf2->samples);
                
                
                if((numbins1 == numbands) && (numbins2 ==numbands)) {
                    
                    int hardorsoft = ZIN0(5); 
                    
                    //unit->hardorsoft_ = ZIN0(5); 
                    //unit->p_ = ZIN0(6);
                    
                    //to magnitude and phase representation 
                    SCPolarBuf *polar1 = ToPolarApx(buf1);
                    SCPolarBuf *polar2 = ToPolarApx(buf2);
                    
                    SCPolar * data1 = polar1->bin; 
                    SCPolar * data2 = polar2->bin; 
                    
                    //writing into the two output arrays of the buffers 
                    
                    
                    //magsnow already pointing to write place in magnitudes
                    //magsnow = mags + (midindex*numbands);
                    
                    //+mid+2, adding so no danger, just add 1
                    int phaseindex = (unit->phaseposition_+1)%(mid+1); //midpoint is next one (about to be overwritten after increment below)
                    
                    float * phasesnow = phases + (phaseindex*numbands); 
                    
                    //dc, nyquist, phase to zero
                    
                    //buf1,polar 1 is harmonic, 2 is percussive
                    
                    //0 larger of horizontal and vertical is winner, or 1 more subtle blend 
                    if(hardorsoft==0) {
                        //hard
                        
                        //printf("hard calc \n");     
                        
                        if(horizontal[0]>vertical[0]) {
                            
                            polar1->dc = magsnow[0]; 
                            polar2->dc = 0.f;
                            
                        } else {
                            
                            polar2->dc = magsnow[0]; 
                            polar1->dc = 0.f;
                        }
                        
                        
                        if(horizontal[top]>vertical[top]) {
                            
                            polar1->nyq = magsnow[top]; 
                            polar2->nyq = 0.f;
                            
                        } else {
                            
                            polar2->nyq = magsnow[top]; 
                            polar1->nyq = 0.f;
                        }
                        
                        int count = 0;  
                        
                        //setting 
                        for (i=0; i<numbands-2; ++i) {
                            
                            int indexnow = i+1; 
                            
                            //if(i==20) {data1[i].mag=1024; data2[i].mag=1024;}
                            
                            //else {data1[i].mag = 0.0f; data2[i].mag = 0.f;}
                            
                            
                            if(horizontal[indexnow]>vertical[indexnow]) {
                                
                                ++count; 
                                
                                data1[i].mag = magsnow[indexnow];
                                data1[i].phase = phasesnow[indexnow];
                                data2[i].mag = 0.0f;
                                data2[i].phase = 0.0f; //phasesnow[i+1];
                                
                            } else {
                                
                                data2[i].mag = magsnow[indexnow];
                                data2[i].phase = phasesnow[indexnow];
                                data1[i].mag = 0.0f;
                                data1[i].phase = 0.0f; //phasesnow[i+1];
                                
                            }
                            
                            //                        if(i<10) {
                            //                            
                            //                            printf("mag %d %f %f horiz %f vert %f further %f %f \n ",i, data1[i].mag,data2[i].mag,horizontal[indexnow],vertical[indexnow],polar1->bin[i].mag,polar2->bin[i].mag);
                            //                            
                            //                        }
                            //                        
                        }
                        
                        
                        //printf("count %d \n",count);
                        
                        
                    } else {
                        
                        
                        //printf("I hope not!\n");
                        
                        //soft, Wiener filtering
                        float pfactor = ZIN0(6); 
                        
                        float maskp, maskh, hp, pp, combine;
                        
                        //dc
                        hp = powf(horizontal[0],pfactor);
                        pp = powf(vertical[0],pfactor);
                        
                        combine = hp+pp; 
                        
                        maskh = 0.f; 
                        maskp = 0.f; 
                        
                        //watch for zeroes
                        if(combine>0.00000000001f) {
                            maskh = hp/combine; 
                            maskp = pp/combine; 
                        }
                        
                        polar1->dc = magsnow[0] * maskh;
                        polar2->dc = magsnow[0] * maskp;
                        
                        //nyquist
                        hp = powf(horizontal[top],pfactor);
                        pp = powf(vertical[top],pfactor);
                        
                        combine = hp+pp; 
                        
                        maskh = 0.f; 
                        maskp = 0.f; 
                        
                        //watch for zeroes
                        if(combine>0.00000000001f) {
                            maskh = hp/combine; 
                            maskp = pp/combine; 
                        }
                        
                        polar1->nyq = magsnow[top] * maskh;
                        polar2->nyq = magsnow[top] * maskp;
                        
                        //setting 
                        for (i=0; i<numbands-2; ++i) {
                            
                            int indexnow = i+1; 
                            
                            hp = powf(horizontal[indexnow],pfactor);
                            pp = powf(vertical[indexnow],pfactor);
                            
                            combine = hp+pp; 
                            
                            //if(i<5) {                       
                            //     printf("mag %d %f %f %f %f %f \n",i ,horizontal[indexnow],vertical[indexnow],hp,pp,combine);
                            //}
                            
                            maskh = 0.f; 
                            maskp = 0.f; 
                            
                            //watch for zeroes
                            if(combine>0.00000000001f) {
                                maskh = hp/combine; 
                                maskp = pp/combine; 
                            }
                            
                            data1[i].mag = magsnow[indexnow] * maskh;
                            data1[i].phase = phasesnow[indexnow];
                            data2[i].mag = magsnow[indexnow] * maskp;
                            data2[i].phase = phasesnow[indexnow];
                            
                        }
                        
                        
                    }
                    
                    
                    //printf("now output %d %d \n",bufnum1,bufnum2);
                    
                    //update output
                    
                    OUT0(0) = bufnum1; //2.f; //-1.f; //bufnum1; //-1;
                    OUT0(1) = bufnum2; //-1;
                    
                }
                
                
            } 
            
            unit->magnitudeposition_ = (unit->magnitudeposition_+1)%medsize; 
            unit->phaseposition_ = (unit->phaseposition_+1)%(mid+1); 
            
            unit->amortisationstep_ = 0; 
            break;
        }   
        default:
            //printf("default\n");
            break;
    }
    
    //printf("actual output %f %f \n",OUT0(0),OUT0(1));
    
 
 
    
    //    for(j=0; j<inNumSamples; ++j) {
    //		
    //		output[j]= input[j]*0.1; //((float)j/inNumSamples); 
    //	}
}



//#define DefinePVUnit(name) \
//(*ft->fDefineUnit)(#name, sizeof(PV_Unit), (UnitCtorFunc)&name##_Ctor, 0, 0);


void loadMedianSeparation(InterfaceTable *inTable)
{
    
    //init_SCComplex(inTable);
	
    //ft = inTable;
	
	//DefineDtorCantAliasUnit(MedianSeparation);
    
    //DefineDtorUnit
    //DefinePVUnit(MedianSeparation);
	DefineDtorCantAliasUnit(MedianSeparation);
}


