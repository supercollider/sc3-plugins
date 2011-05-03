/*
 *  LPCAnalyzer.cpp
 *  xSC3ExtPlugins-Universal
 *
 *  Created by Nick Collins on 10/04/2009.
 *  Copyright 2009 Nick Collins. All rights reserved.
 *
 */

#include "NCAnalysis.h"

#define SCMEMORYALLOC 1
#include "LPCAnalysis.h"

struct LPCAnalyzer : public Unit
{
	LPCAnalysis * lpc;
	LPCAnalysis * lpc2; //two of them, for corssfading when changing filter
};


extern "C"
{
	void LPCAnalyzer_next(LPCAnalyzer *unit, int inNumSamples);
	void LPCAnalyzer_Ctor(LPCAnalyzer* unit);
	void LPCAnalyzer_Dtor(LPCAnalyzer* unit);
}




//also can do test to return convergence point; clue to transient vs tonal?
//toggle to freeze or not on current filter coefficients?

void LPCAnalyzer_Ctor(LPCAnalyzer* unit) {

	int windowsize= (int)ZIN0(2);
	int windowtype= (int)ZIN0(6);

	int blocksize= unit->mWorld->mFullRate.mBufLength;

	if(windowsize<blocksize) windowsize=blocksize;
	//must be divisible by two?
	if((windowsize & 0x01))
		windowsize= windowsize+1;
	if(windowsize>1024) windowsize=1024;


	//overloaded new operator so realtime safe
	//unit->mWorld->mFullRate.mBufLength, no need to pass blocksize now
	unit->lpc= (LPCAnalysis*) new(unit->mWorld, ft) LPCAnalysis(windowsize,windowtype,0,unit->mWorld, ft);

	if(windowtype>0)
		unit->lpc2= (LPCAnalysis*) new(unit->mWorld, ft) LPCAnalysis(windowsize,windowtype,windowsize/2,unit->mWorld, ft);
	else
		unit->lpc2=NULL;

	//put them out of sync by half window for crossfading purposes
	//unit->lpc2->pos= windowsize/2;

	SETCALC(LPCAnalyzer_next);

}

void LPCAnalyzer_Dtor(LPCAnalyzer* unit) {

	//should work via overloaded delete
	delete unit->lpc;
	//should be OK even if NULL
	delete unit->lpc2;
}

void LPCAnalyzer_next(LPCAnalyzer *unit, int inNumSamples) {

	float * inoriginal= IN(0);
	float * indriver= IN(1);
	int p= (int)ZIN0(3);
	float * out= OUT(0);

	int testE= (int)ZIN0(4);
	LPCfloat delta= (LPCfloat)ZIN0(5);

	for (int i=0; i<inNumSamples; ++i) {
		out[i]= 0.0;
	}

	unit->lpc->testdelta= testE;
	unit->lpc->delta= delta;
	unit->lpc->update(inoriginal, indriver, out, inNumSamples, p);

	if(unit->lpc2) {
		unit->lpc2->testdelta= testE;
		unit->lpc2->delta= delta;
		unit->lpc2->update(inoriginal, indriver, out, inNumSamples, p);
	}

}

void loadLPCAnalyzer(InterfaceTable *inTable)
{

	//ft= inTable;

	DefineDtorCantAliasUnit(LPCAnalyzer);
}
