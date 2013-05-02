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


void init_SCComplex(InterfaceTable *inTable);

extern void loadSMS(InterfaceTable *inTable);
extern void loadTPV(InterfaceTable *inTable);
extern void loadWaveletDaub(InterfaceTable *inTable);
extern void loadWalshHadamard(InterfaceTable *inTable);
extern void loadLPCAnalyzer(InterfaceTable *inTable);
extern void loadMedianSeparation(InterfaceTable *inTable);


//void load(InterfaceTable *inTable)
PluginLoad(NCAnalysis)
{

	ft= inTable;

	init_SCComplex(inTable);

	//scfft_global_init();

	loadSMS(inTable);
	loadTPV(inTable);
	loadWaveletDaub(inTable);
	loadWalshHadamard(inTable);
	loadLPCAnalyzer(inTable);
    loadMedianSeparation(inTable);
}
