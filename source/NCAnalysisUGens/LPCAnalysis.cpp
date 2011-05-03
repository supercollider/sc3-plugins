/*
 *  LPCAnalysis.cpp
 *
 *  Created by Nicholas Collins on 10/09/2009.
 *  Copyright 2009 Nicholas M Collins. All rights reserved.
 *
 */


//numerical stability issues; need to use doubles for R in particular to avoid round-off and keep positive definite
//can also test for stability by making sure all E greater than zero




#include "LPCAnalysis.h"

//blocksize MUST be less than or equal to windowsize

void LPCAnalysis::update(float * newinput, float * newsource, float * output, int numSamples, int p) {

	int i;

	int left= windowsize-pos;

	if(numSamples>=left) {

		for (i=0; i<left;++i) {

			input[pos++]= newinput[i];
		}

		//output up to here
		calculateOutput(newsource, output, windowsize-left, left);

		//update
		numpoles=p;
		calculatePoles();

		pos=0;

		int remainder= numSamples-left;

		for (i=0; i<remainder;++i) {

			input[pos++]= newinput[left+i];

		}

		//output too
		calculateOutput(newsource+left, output+left, pos-remainder, remainder);


	} else {

		for (i=0; i<numSamples;++i) {

			input[pos++]= newinput[i];
		}

		//output
		calculateOutput(newsource, output, pos-numSamples, numSamples);


	}

	//return output;
}


void LPCAnalysis::calculateOutput(float * source, float * target, int startpos, int num) {
	int i,j;

	int basepos,posnow;

	for(i=0; i<num; ++i) {

		basepos= startpos+i+windowsize-1; //-1 since coefficients for previous values starts here

		float sum=0.0;

		for(j=0; j<numpoles; ++j) {
			posnow= (basepos-j)%windowsize;

			//where is pos used?
			sum += last[posnow]*coeff[j]; //was coeff i
		}

		sum= G*source[i]-sum; //scale factor G calculated by squaring energy E below

		last[startpos+i]=sum;

		//ZXP(out)=
		target[i]+= sum*windowfunction[startpos+i];

	}

}

//recalculate poles based on recent window of input
void LPCAnalysis::calculatePoles() {

	//can test for convergence by looking for 1-((Ei+1)/Ei)<d

	int i, j;
	LPCfloat sum;

	//safety
	if(numpoles<1) numpoles=1;
	if(numpoles>windowsize) numpoles=windowsize;

	//printf("p? %d",p);

	//calculate new LPC filter coefficients following (Makhoul 1975) autocorrelation, deterministic signal, Durbin iterative matrix solver

	//float R[21];//autocorrelation coeffs;
	//float preva[21];
	//float a[21];
	LPCfloat E, k;

	//faster calculation of autocorrelation possible?

	for(i=0; i<=numpoles; ++i) {
		sum=0.0;

		for (j=0; j<= windowsize-1-i; ++j)
			sum+= input[j]*input[j+i];

		R[i]=sum;
	}

	E= R[0];
	k=0;

	if(E<0.00000000001) {

		//zero power, so zero all coeff
		for (i=0; i<numpoles;++i)
			coeff[i]=0.0;
//
		latesterror= E;
		G=0.0;
		//printf("zero power %f\n", E);
		return;

	};

	//rescaling may help with numerical instability issues?
//	float mult= 1.0/E;
//	for(i=1; i<=numpoles; ++i)
//		R[i]= R[i]*mult;
//
	for(i=0; i<=(numpoles+1); ++i) {
		a[i]=0.0;
		preva[i]=0.0; //CORRECTION preva[j]=0.0;
	}


//	for(i=0; i<numpoles; ++i) {
//		printf("sanity check a %f R %1.15f ",a[i+1], R[i]);
//	}
//	printf("\n");


	LPCfloat prevE= E;

	for(i=1; i<=numpoles; ++i) {

		sum=0.0;

		for(j=1;j<i;++j)
			sum+= a[j]*R[i-j];

		k=(-1.0*(R[i]+sum))/E;

		a[i]=k;

		for(j=1;j<=(i-1);++j)
			a[j]=preva[j]+(k*preva[i-j]);

		for(j=1;j<=i;++j)
			preva[j]=a[j];

		E= (1-k*k)*E;

		//printf("E check %f %d k was %f\n", E,i,k);

		//check for instability; all E must be greater than zero
		if(E<0.00000000001) {

			//zero power, so zero all coeff

			//leave coeff as previous values
			//for (j=0; j<numpoles;++j)
//				coeff[j]=0.0;

			latesterror= E;
			//printf("early return %1.15f %d\n", E,i);
			return;

		};

		if(testdelta) {

			LPCfloat ratio= E/prevE;
			if(ratio>delta) {
				//printf("variable order chose %d\n", i);
				break; //done to error bound
			}
			prevE= E;

		}


	}

	G= sqrt(E);

	latesterror= E;

	//solution is the final set of a
	for(i=0; i<numpoles; ++i) {
		//coeff[numpoles-1-i]=a[i+1];
		coeff[i]=a[i+1];
		//printf("a %f R %f",a[i+1], R[i]);
	}

	//MUST CHECK gain?


	//
//	for(i=0; i<numpoles; ++i) {
//		printf("a %f R %f ",a[i+1], R[i]);
//	}
//
//	printf("\n");
//
//	for(i=0; i<windowsize; ++i) {
//		printf("last %d %f ",i, last[i]);
//	}
//
//	printf("\n");

}