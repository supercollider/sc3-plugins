/*
 *  NCAnalysis.cpp
 *  xSC3ExtPlugins-Universal
 *
 *  Created by Nick Collins on 24/03/2009.
 *  Copyright 2009 Nick Collins. All rights reserved.
 *
 */

#include "NCAnalysis.h"



extern "C"
{
	void load(InterfaceTable *inTable);
}


InterfaceTable *ft; 


SCPolarBuf* ToPolarApx(SndBuf *buf);
SCPolarBuf* ToPolarApx(SndBuf *buf)
{
	if (buf->coord == coord_Complex) {
		SCComplexBuf* p = (SCComplexBuf*)buf->data;
		int numbins = buf->samples - 2 >> 1;
		for (int i=0; i<numbins; ++i) {
			p->bin[i].ToPolarApxInPlace();
		}
		buf->coord = coord_Polar;
	}
	return (SCPolarBuf*)buf->data;
}

SCComplexBuf* ToComplexApx(SndBuf *buf);
SCComplexBuf* ToComplexApx(SndBuf *buf)
{
	if (buf->coord == coord_Polar) {
		SCPolarBuf* p = (SCPolarBuf*)buf->data;
		int numbins = buf->samples - 2 >> 1;
		for (int i=0; i<numbins; ++i) {
			p->bin[i].ToComplexApxInPlace();
		}
		buf->coord = coord_Complex;
	}
	return (SCComplexBuf*)buf->data;
}


void init_SCComplex(InterfaceTable *inTable);

extern void loadSMS(InterfaceTable *inTable);
extern void loadTPV(InterfaceTable *inTable);
extern void loadWaveletDaub(InterfaceTable *inTable); 
extern void loadWalshHadamard(InterfaceTable *inTable);
extern void loadLPCAnalyzer(InterfaceTable *inTable);


void load(InterfaceTable *inTable)
{
	
	ft= inTable;
	
	init_SCComplex(inTable);
	
	scfft_global_init();
	
	loadSMS(inTable); 
	loadTPV(inTable);
	loadWaveletDaub(inTable); 
	loadWalshHadamard(inTable);
	loadLPCAnalyzer(inTable);
	
}




