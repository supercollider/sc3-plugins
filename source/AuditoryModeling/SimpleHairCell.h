//SuperCollider is under GNU GPL version 3, http://supercollider.sourceforge.net/
//these extensions released under the same license

/*
 *  SimpleHairCell.h
 *
 *  Created by Nick Collins on 10/06/2009.
 *  Copyright 2009 Nick Collins. All rights reserved.
 *
 */

//adjusted this to be a store and a hair cell. Hair cell fires if level reaches 1.0, and output time since last fired


//integrate and fire, and leaky integrator, with gain per sample proportional to rectified and squared signal
//minimal 1 msec recovery period

#pragma once

class SimpleHairCell {
public:
	float dt; 
	float store, level, outputlevel; //
	float loss; //, gain, loss2; 
	//long countsincelastfired;
 	//long firingdelay; 
	float minflow, feedflow, restoreflow; 
	
	SimpleHairCell(float samplingrate=44100.0) {
		
		dt= 1.0/samplingrate; 
		
		//gain=0.5; 
		loss=0.99; 
		//loss2=0.9; 
		
		store=1.0; 
		minflow= 0.0; //(1.0/0.01)*dt;	//no spontaneous firing
		restoreflow= (1.0/0.001)*dt; 
		feedflow = (restoreflow-minflow)*2.8284271247462; //2 times root 2, because rectification means effective only half a cycle, and RMS of 1/root2 must be compensated for 
		
		//firingdelay= (int) (samplingrate*0.01); //(int) (samplingrate*0.001); 
		//countsincelastfired=1; 
		
		level=0.0;
		outputlevel=0.0; 
	}
	
	~SimpleHairCell() {
		
	}
	
	void compute(float *, float *, int); 
	
};