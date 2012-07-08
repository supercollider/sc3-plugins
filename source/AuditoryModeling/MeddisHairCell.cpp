//SuperCollider is under GNU GPL version 3, http://supercollider.sourceforge.net/
//these extensions released under the same license
/*
 *  Meddis.cpp
 *
 *  Created by Nick Collins on 10/06/2009.
 *  Copyright 2009 Nick Collins. All rights reserved.
 *
 */

#include "MeddisHairCell.h"
#include <stdio.h>

//but see also
//A revised model of the inner-hair cell and auditory-nerve complex
//Christian J. Sumner, Enrique A. Lopez-Poveda, Lowel P. O'Mard, and Ray Meddis, J. Acoust. Soc. Am. 111, 2178 (2002), DOI:10.1121/1.1453451




//should be able to optimise a lot of these constants later on, ie g*dt or A+B

void MeddisHairCell::compute(float * input, float * output, int numsamples) {
	
	int i; 
	
	//need to rescale amplitude to fit the example code p.1814; may need to push further, depends what is being taken as dB corresponding to full range -1 to 1
	//10**(80/20-1.35) = 446.68359215096
	//apparently 30 dB is RMS =1; 
	
	for (i=0; i<numsamples; ++i) {
	
		float latest= input[i]* 446.68359215096; // * 100.0;  //100 is additional gain... 
		
		//k is membrane permeability
		if((latest+A) > 0.0)
			k= g*dt*(latest+A)/(latest+A+B);
		else k= 0; 
		
		float replenish= 0.0; 
		
		//if any store available, m is maximum 
		if (m>=q) replenish= y*dt*(m-q);  
		
		float eject= k*q; 
		float loss= l*dt*c; 
		float reuptake= r*dt*c; 
		float reprocess = x*dt*w; 
		
		q= q+replenish-eject+reprocess; 
		c= c+eject-loss-reuptake; 
		w= w+reuptake-reprocess; 
		
		//test for values over 0.02 
		//since hdt= 50000/44100.0 c is about 0.035 maximum, 
		//1.1337868*0.035 is about 0.04
		////Meddis paper has a random number here; I prefer to keep deterministic? maybe a mistake, let's try it and see how it works out...
		
	//	if(i%10==0) printf("count %d testval %f \n",countsincelastfired, h*dt*c); 
//		
//		if((countsincelastfired>firingdelay) && (h*dt*c>0.005)) {
//			
//			output[i]=20*c; 
//			countsincelastfired= 0; 
//		}else
//			output[i]=0.0; 
//		
		
		output[i]= 100.f*c; //h*c; 
		
		
	//	if((countsincelastfired>firingdelay) && (h*c>0.5)) {
//			//		
//			//c = 0.0; //c*0.5; 	
//			//w=0.0; 
//			countsincelastfired= 0;
//		}
//		
//		++countsincelastfired; 
	}
}