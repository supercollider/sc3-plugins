//SuperCollider is under GNU GPL version 3, http://supercollider.sourceforge.net/
//these extensions released under the same license
/*
 *  Meddis.h
 *
 *  Created by Nick Collins on 10/06/2009.
 *  Copyright 2009 Nick Collins. All rights reserved.
 *
 */

//implementation based on Implementation details of a computation model of the inner hair-cell/auditory-nerve synapse 
//Ray Meddis, Michael J. Hewitt and Trevor M. Shackleton JASA 87(4) April 1990

#pragma once

class MeddisHairCell {

public:
	//fixed constants for model; problem is that may be different for two types of hair cell
	//can make them variables of each instance later, or at least have lookup table based on type? 
	//static const int A=5;
//	static const int B=300;
//	static const int g=2000;
//	static const float y=5.05;
//	static const int l=2500;
//	static const float x=66.3;
//	static const int r=6580;
//	static const float m=1.0;
//	static const float h= 50000.0; //convert cleft contents to firing probability

	int A;
	int B;
	int g;
	float y;
	int l;
	float x;
	int r;
	float m;
	float h; //convert cleft contents to firing probability
	
	
	float q, c, w, k;
	float dt; 
	
	long countsincelastfired; 
	long firingdelay; 
	
	MeddisHairCell(float samplingrate=44100.0) {
		
		//params from p. 1815
		//Meddis 1988
		//A=5;
//		B=300;
//		g=2000;
		
		//medium spontaneous rate
		A=10;
		B=3000;
		g=1000;
		y=5.05; //per second would return about 5 times the amount q is under the total transmitter resource m 
		l=2500;
		x=66.31;
		r=6580;
		
		
		m=1.0;
		h= 50000.0; //convert cleft contents to firing probability
		
		dt= 1.0/samplingrate; 
		
		//1.0 msec delay between firing
		firingdelay= (int) (samplingrate*0.001); 
		countsincelastfired=0; 
		
		//spontaneous firing rate
		//these all really constants, so could be precalculated
		//p.1815
		k= g*A/(A+B); //membrane permeability
		
		c= k*y*m/(y*(l+r)+k*l); //cleft contents, linked to firing probability 
		q= c*(l+r)/k; //free transmitter
		w= c*r/x; //reprocessing store
		
	}
	
	
	~MeddisHairCell() {
		
	}
	
	void compute(float *, float *, int); 
};


