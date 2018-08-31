//SuperCollider is under GNU GPL version 3, http://supercollider.sourceforge.net/
//these extensions released under the same license

/*
 *  SimpleHairCell.cpp
 *
 *  Created by Nick Collins on 10/06/2009.
 *  Copyright 2009 Nick Collins. All rights reserved.
 *
 */

#include "SimpleHairCell.h"
#include <stdio.h>
#include <math.h>

//nest one integrate and fire unit within another, one local and one longer term? 

//should be able to optimise a lot of these constants later on, ie g*dt or A+B

void SimpleHairCell::compute(float * input, float * output, int numsamples) {
	
	int i; 
		
	for (i=0; i<numsamples; ++i) {
		
		float latest= input[i];
		
		//rectification and nonlinearity
		if(latest>0.0) latest= latest; //sqrt(latest); //*latest; //or square root, or whatever
		else latest=0.0; 
		
		float newflow= minflow+(feedflow*latest); 
		
		if(newflow>store) newflow=store; 
		
		//if enough transmitter available
		
		store-=newflow; 
		
		if(store<0.0) store=0.0; 
		
		level+= newflow; 
				//if(latest>0.0) level += dt*500*(1.0+latest); //(gain*10*((latest+5.0)/(latest+300.0)));
				
		if(level>1.0){
			
			//assuming 100 Hz resting rate
			outputlevel = 1.0; //could make peak dependent on how long it took it get there
			//
			//0.001/(countsincelastfired*dt); 
			
			//outputlevel+=0.0005/(countsincelastfired*dt);  
			
			//if(outputlevel>1.0) outputlevel=1.0; 
			
			level=0.0; //hair cell wiped out
			//countsincelastfired= 0;
		}
		
		store+=restoreflow; 
		
		//level=1.0; 
		
		//loss is proportional to amount there
		//level*=loss2; //+ (gain*((latest+5.0)/(latest+300.0))); //h*c; 
		
		//printf("%f %f %f %f \n", latest, dt*2000*(1.0+latest), level, outputlevel); 
		
	//	if((countsincelastfired>firingdelay) && (level>0.15)) {
//			
//			outputlevel+=latest; 
//			
//			//level=0.0; 
//			
//			countsincelastfired= 0;
//		}
		
		//loss is a fixed amount every sample
		//level-= 0.015;
//		
//		if(level<0.0) level=0.0; //can't go negative
//		
		
		output[i]= outputlevel; 
		
		outputlevel*=loss; 
		
		//should check for denormals rather than have decay go on and on? 
		//if(outputlevel<1e-15) 
		//	outputlevel= 0.0; 
		
		
		//++countsincelastfired; 
	}
}