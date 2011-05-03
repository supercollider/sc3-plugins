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



//find maximum value within last x blocks
struct WalshHadamard : public Unit {
	//float * store;
	int m_log2, m_size, m_count; //
	float * m_data;
};



extern "C"
{
	//required interface functions
	void WalshHadamard_next(WalshHadamard *unit, int wrongNumSamples);
	void WalshHadamard_Ctor(WalshHadamard *unit);
	void WalshHadamard_Dtor(WalshHadamard *unit);
}



//from music DSP list site: http://www.musicdsp.org/showArchiveComment.php?ArchiveID=18

void inline wht_bfly (float& a, float& b);
int inline l2 (long x);
void FWHT(float * data, int log2);

void inline wht_bfly (float& a, float& b)
{
	float tmp = a;
	a += b;
	b = tmp - b;
}

// just a integer log2
int inline l2 (long x)
{
	int l2;
	for (l2 = 0; x > 0; x >>=1)
	{
		++ l2;
	}

	return (l2);
}

////////////////////////////////////////////
// Fast in-place Walsh-Hadamard Transform //
////////////////////////////////////////////

void FWHT(float * data, int log2)
{
	for (int i = 0; i < log2; ++i) {
		for (int j = 0; j < (1 << log2); j += 1 << (i+1)) {

			for (int k = 0; k < (1 << i); ++k) {
				wht_bfly (data [j + k], data [j + k + (1 << i)]);
			}

		}
	}
}


void WalshHadamard_Ctor( WalshHadamard* unit ) {

	//int msamp= (int) ZIN0(1);
	unit->m_size= 64;
	unit->m_log2 = l2 (unit->m_size) - 1;
	unit->m_data=  (float*)RTAlloc(unit->mWorld, unit->m_size * sizeof(float));

	unit->m_count=0;

	SETCALC(WalshHadamard_next);

}

void WalshHadamard_Dtor(WalshHadamard *unit) {
	RTFree(unit->mWorld, unit->m_data);
}

void WalshHadamard_next( WalshHadamard *unit, int inNumSamples ) {

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

	float * data= unit->m_data;

	for(j=0; j<inNumSamples; ++j) {
		data[j] = in[j];
	}


	//forwards WH
	FWHT(data, unit->m_log2);

	int which= ZIN0(1);

	//processing (zero every other one)
	for(j=0; j<which; ++j) {
		//data[2*j+1] =0.0;
		data[j] =0.0;
	}

	//inverse WH
	FWHT(data, unit->m_log2);

	//output
	for(j=0; j<inNumSamples; ++j) {
		out[j] =  data[j]*0.015625;
	}

//	unit->m_count= left;

}


void loadWalshHadamard(InterfaceTable *inTable)
{

	//ft= inTable;

	DefineDtorCantAliasUnit(WalshHadamard);

}

//void datafun( WalshHadamard *unit) {
//
//}
