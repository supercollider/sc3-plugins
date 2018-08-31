/*
 *  WalshHadamard.cpp
 *  xSC3ExtPlugins-Universal
 *
 *  Created by Nick Collins on 10/04/2009.
 *  Copyright 2009 Nick Collins. All rights reserved.
 *
 */

///#include "WalshHadamard.h"
#include "NCAnalysis.h"

#include "kaplandaub.h"


Daubechies * g_DaubechiesTransform;

//find maximum value within last x blocks
struct WaveletDaub : public Unit {
	//float * store;
	int m_size, m_count; // m_log2,
	float * m_data;
	float * m_outputstore;
};



extern "C"
{
	//required interface functions
	void WaveletDaub_next(WaveletDaub *unit, int wrongNumSamples);
	void WaveletDaub_Ctor(WaveletDaub *unit);
	void WaveletDaub_Dtor(WaveletDaub *unit);
}



void WaveletDaub_Ctor( WaveletDaub* unit ) {

	//int msamp= (int) ZIN0(1);
	unit->m_size= (int) ZIN0(1); //must be multiple of 64

	if(unit->m_size%64!=0) unit->m_size=64;

	//unit->m_log2 = l2 (unit->m_size) - 1;
	unit->m_data=  (float*)RTAlloc(unit->mWorld, unit->m_size * sizeof(float));
	unit->m_outputstore=  (float*)RTAlloc(unit->mWorld, unit->m_size * sizeof(float));

	for (int i=0; i<unit->m_size; ++i)
		unit->m_outputstore[i] = 0.0;

	unit->m_count=0;

	SETCALC(WaveletDaub_next);

}

void WaveletDaub_Dtor(WaveletDaub *unit) {
	RTFree(unit->mWorld, unit->m_data);
	RTFree(unit->mWorld, unit->m_outputstore);
}

void WaveletDaub_next( WaveletDaub *unit, int inNumSamples ) {

	int j;
	float *in = IN(0);
	float* out = OUT(0);

	//float *out = ZOUT(0);

	//only to be used at .kr
	//printf("samp to calc %d", inNumSamples);

	//come back to while loop
//	int siz = unit->m_size; //unit->mWorld->mFullRate.mBufLength;
//    int left= siz- unit->m_count;

	//ASSUMES block size of 64!

	//assume transform size divisible by block size


	int count= unit->m_count;
	int size= unit->m_size;
	float * data= unit->m_data + count;
	float * outputstore= unit->m_outputstore+count;

	for(j=0; j<inNumSamples; ++j) {
		data[j] = in[j];
		out[j]= outputstore[j];
	}

	count+=inNumSamples;

	//ready for transform?
	if(count>=size) {

		data= unit->m_data; //full window now
		//forwards WH
		g_DaubechiesTransform->daubTrans(data, size);

		int which= ZIN0(2);

		//processing (zero every other one)
		for(j=0; j<which; ++j) {
			//data[2*j+1] =0.0;
			data[j] =0.0;
		}

		//inverse WH
		g_DaubechiesTransform->invDaubTrans(data, size);

		outputstore= unit->m_outputstore;

		for (int i=0; i<size; ++i)
			outputstore[i] = data[i];


		count=0;
	}

	unit->m_count= count;

//	//output
//	for(j=0; j<inNumSamples; ++j) {
//		out[j] =  data[j]; //*0.015625;
//	}

//	unit->m_count= left;

}


void loadWaveletDaub(InterfaceTable *inTable)
{

	//ft= inTable;

	DefineDtorCantAliasUnit(WaveletDaub);

	g_DaubechiesTransform = new Daubechies();

//	int testsize= 64;
//
//	float test[64];
//
//	for(int i=0; i<64; ++i) {
//		test[i]= ((i*i*6)%1000) * 0.001 -0.4;
//	}
//
//	for(int i=0; i<64; ++i) printf("before test i %d val %f \n",i, test[i]);
//
//	g_DaubechiesTransform->daubTrans(test, 64);
//
//	for(int i=0; i<64; ++i) printf("mid test i %d val %f \n",i, test[i]);
//
//	g_DaubechiesTransform->invDaubTrans(test, 64);
//
//	for(int i=0; i<64; ++i) printf("post test i %d val %f \n",i, test[i]);
//

}
