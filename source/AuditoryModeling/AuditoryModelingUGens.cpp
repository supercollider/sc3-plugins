//SuperCollider is under GNU GPL version 3, http://supercollider.sourceforge.net/
//these extensions released under the same license

/*
 *  AuditoryModelingUGens.cpp
 *
 *  Created by Nicholas Collins on 1/07/2010.
 *  Copyright 2010 Nicholas M Collins. All rights reserved.
 *
 */

#include "SC_PlugIn.h"
#include "MeddisHairCell.h"
#include "SimpleHairCell.h"

InterfaceTable *ft; 

//int g_pitchnotetaken =0;  //only one UGen can be running with this at once, else zero outputs. 

//PitchNote g_pitchnote; //will just create, at default 44100 sampling rate. 

struct Meddis : public Unit  
{
	MeddisHairCell * cell;  //will need to create and destroy in NRT thread 

	void * mydata; 
	
};

struct HairCell : public Unit  
{
	SimpleHairCell * cell;  //will need to create and destroy in NRT thread 

	void * mydata; 
};

//based on V Hohmann Frequency analysis and synthesis using a Gammatone filterbank Acta Acustica vol 88 (2002): 433--442
//converted to straight struct form for SuperCollider from my own GammatoneComplexBandpass class code
struct Gammatone : public Unit  
{
	double centrefrequency; 
	double bandwidth; 
	double normalisation; 
	double reala, imaga; 
	double oldreal[4];
	double oldimag[4]; 
	
	
};


//data to be shared between RT and NRT threads 
//struct CmdData {
//	enum Type
//    {
//		NRTCtorMeddis,
//		NRTDtorMeddis,
//		NRTCtorHairCell,
//		NRTDtorHairCell
//    };
//	
//	Type type; 
//	Unit * unit; 
//	void *	nrtallocated; 
//	float samplingrate_; 
//	
//};


extern "C" {  
	
	void Meddis_next(Meddis *unit, int inNumSamples);
	void Meddis_Ctor(Meddis* unit);
	void Meddis_Dtor(Meddis* unit);
	
	void HairCell_next(HairCell *unit, int inNumSamples);
	void HairCell_Ctor(HairCell* unit);
	void HairCell_Dtor(HairCell* unit);
	
	void Gammatone_next(Gammatone *unit, int inNumSamples);
	void Gammatone_Ctor(Gammatone* unit);
	//void Gammatone_Dtor(Gammatone* unit);

}


//since classes are simple, can use basic placement new and explicit destructor call 
/*
//for NRT allocation and deallocation 

bool cmdStage2(World* inWorld, CmdData* cmd) // NRT
{
	//Unit* unit = cmd->unit;
	
	switch (cmd->type) {
		case CmdData::NRTCtorMeddis: 
			cmd->nrtallocated = (void *)(new MeddisHairCell(cmd->samplingrate_));
			return true;
		case CmdData::NRTCtorHairCell: 
			cmd->nrtallocated = (void *)(new SimpleHairCell(cmd->samplingrate_));
			return true;
		case CmdData::NRTDtorMeddis: 
			delete ((Meddis*)cmd->nrtallocated);
			return true;
		case CmdData::NRTDtorHairCell: 
			delete ((HairCell*)cmd->nrtallocated);
			return true;
			
	}
	
	return false;
}

bool cmdStage3(World* world, CmdData* cmd) // RT
{
	switch (cmd->type) {
		case CmdData::NRTCtorMeddis: {
			((Meddis*)cmd->unit)->cell = (MeddisHairCell*)cmd->nrtallocated;
            return true; 
		}
		case CmdData::NRTCtorHairCell: {
			((HairCell*)cmd->unit)->cell = (SimpleHairCell*)cmd->nrtallocated;
		}
			return true;
	}
	return true;
}

bool cmdStage4(World* world, CmdData* cmd) // NRT
{
	return true;
}

void cmdCleanup(World* world, void* cmd)
{
	RTFree(world, cmd);
}






//if just using for construction and destruction, can have named functions for construction and destruction, don't need to share same function with switch on type

void Meddis_Ctor( Meddis* unit ) {
	
	unit->cell= NULL; 
	
	CmdData* cmd = (CmdData*)RTAlloc(unit->mWorld, sizeof(CmdData));
	cmd->samplingrate_ = unit->mRate->mSampleRate; //unit->mWorld->mFullRate.mSampleRate; 
	cmd->unit = (Unit *)unit; 
	cmd->nrtallocated = NULL; //will be allocated in NRT thread 
	cmd->type = CmdData::NRTCtorMeddis; 
	
	//(AsyncStageFn)PitchNoteUGencmdStage4
	
	DoAsynchronousCommand(unit->mWorld, 0, "", (void*)cmd,
						  (AsyncStageFn)cmdStage2,
						  (AsyncStageFn)cmdStage3,
						  NULL,
						  cmdCleanup,
						  0, 0);
	
		SETCALC(Meddis_next);
}


void HairCell_Ctor( HairCell* unit ) {
	
	unit->cell= NULL; 

	CmdData* cmd = (CmdData*)RTAlloc(unit->mWorld, sizeof(CmdData));
	cmd->samplingrate_ = unit->mRate->mSampleRate; //unit->mWorld->mFullRate.mSampleRate; 
	cmd->unit = (Unit *)unit; 
	cmd->nrtallocated = NULL; //will be allocated in NRT thread 
	cmd->type = CmdData::NRTCtorHairCell; 
	
	//(AsyncStageFn)PitchNoteUGencmdStage4
	
	DoAsynchronousCommand(unit->mWorld, 0, "", (void*)cmd,
						  (AsyncStageFn)cmdStage2,
						  (AsyncStageFn)cmdStage3,
						  NULL,
						  cmdCleanup,
						  0, 0);
	
	SETCALC(HairCell_next);
}







void Meddis_Dtor(Meddis *unit)
{
	
	if (unit->cell) {
		
		CmdData* cmd = (CmdData*)RTAlloc(unit->mWorld, sizeof(CmdData));
		cmd->unit = unit; 
		cmd->nrtallocated = unit->cell; 
		unit->cell = NULL; //no longer available, will be deallocated in NRT thread
		cmd->type = CmdData::NRTDtorMeddis; 

		DoAsynchronousCommand(unit->mWorld, 0, "", (void*)cmd,
							  (AsyncStageFn)cmdStage2,
							  NULL,NULL,
							  cmdCleanup,
							  0, 0);
		
	}
	
}


void HairCell_Dtor(HairCell *unit)
{
	
	if (unit->cell) {
		
		CmdData* cmd = (CmdData*)RTAlloc(unit->mWorld, sizeof(CmdData));
		cmd->unit = unit; 
		cmd->nrtallocated = unit->cell; 
		unit->cell = NULL; //no longer available, will be deallocated in NRT thread
		cmd->type = CmdData::NRTDtorHairCell; 
		
		DoAsynchronousCommand(unit->mWorld, 0, "", (void*)cmd,
							  (AsyncStageFn)cmdStage2,
							  NULL,NULL,
							  cmdCleanup,
							  0, 0);
		
	}
	
}


*/


void Meddis_Ctor( Meddis* unit ) {
	
	unit->cell= NULL; 
	
	float samplingrate = unit->mRate->mSampleRate; //unit->mWorld->mFullRate.mSampleRate; 
	
	unit->mydata = RTAlloc(unit->mWorld, sizeof(MeddisHairCell));
	unit->cell= new(unit->mydata)MeddisHairCell(samplingrate);
	
	SETCALC(Meddis_next);
}


void HairCell_Ctor( HairCell* unit ) {
	
	unit->cell= NULL; 
	
	float samplingrate = unit->mRate->mSampleRate; //unit->mWorld->mFullRate.mSampleRate; 
	
	unit->mydata = RTAlloc(unit->mWorld, sizeof(SimpleHairCell));
	unit->cell= new(unit->mydata)SimpleHairCell(samplingrate);
	
	SETCALC(HairCell_next);
}







void Meddis_Dtor(Meddis *unit)
{
	//explicitly call destructor
	unit->cell->~MeddisHairCell();
	//free associated memory explicitly via SC mechanism
	RTFree(unit->mWorld, unit->mydata);
	
}


void HairCell_Dtor(HairCell *unit)
{
	unit->cell->~SimpleHairCell();
	//free associated memory explicitly via SC mechanism
	RTFree(unit->mWorld, unit->mydata);
		
}





void Meddis_next( Meddis *unit, int inNumSamples ) {
	
	//if(unit->cell) {
	
		float *input = IN(0); 
		float *output = OUT(0);
	
		unit->cell->compute( input, output, inNumSamples ); 
	
	//} else
	//ClearUnitOutputs(unit, inNumSamples);
	
}

void HairCell_next( HairCell *unit, int inNumSamples ) {
	
	//if(unit->cell) {
		
		float *input = IN(0); 
		float *output = OUT(0);
		
		SimpleHairCell * cell= unit->cell; 
		
		//k-rate or i-rate controls for transfer of chemicals and stimulation decay 
		float minflow = ZIN0(1);
		float feedflow = ZIN0(2);
		float restoreflow = ZIN0(3);
		float loss = ZIN0(4);
		
		float dt= cell->dt; 
		
		if(minflow<0.f) minflow=0.f; 
		if(minflow>20000.f) minflow= 20000.f;
		
		if(feedflow<0.f) feedflow=0.f; 
		if(feedflow>20000.f) feedflow= 20000.f;
		
		if(restoreflow<0.f) restoreflow=0.f; 
		if(restoreflow>20000.f) restoreflow= 20000.f;
		
		if(loss<0.f) loss=0.f; 
		if(loss>1.0f) loss= 1.0f;
		
		cell->loss = loss; 
//		
		cell->minflow = dt*2.8284271247462f*minflow; //compensation for half cycle and RMS
		cell->feedflow = dt*2.8284271247462f*feedflow; 
		cell->restoreflow = dt*restoreflow; 
//		
		unit->cell->compute( input, output, inNumSamples ); 
		
	//} else
	//	ClearUnitOutputs(unit, inNumSamples);
	
}







//assumes audio rate, else auditory frequencies make less sense
void Gammatone_Ctor(Gammatone* unit) {
	
	
	for(int i=0; i<4; ++i) {
		unit->oldreal[i]=0.0;  
		unit->oldimag[i]=0.0;  
	}
	
	
	float centrefrequency= ZIN0(1);
	float bandwidth= ZIN0(2);
	
	float samplingrate = unit->mRate->mSampleRate;
	
	double samplingperiod= 1.0/samplingrate;
	
	float nyquist= samplingrate*0.5; 
	
	if (centrefrequency< 20.0) centrefrequency = 20.0; 
	if (centrefrequency>nyquist) centrefrequency = nyquist;  
	
	if ((centrefrequency-(0.5*bandwidth))<1.0) bandwidth = 2.0*(centrefrequency-1.0); 
	
	//if ((centrefrequency+(0.5*bandwidth))>nyquist) bandwidth = 
	
	if (bandwidth>nyquist) bandwidth= nyquist; //assuming there is even room! 
		
	
	
	unit->centrefrequency = centrefrequency; 
	
	//actually need to convert ERBs to 3dB bandwidth
	bandwidth= 0.887*bandwidth; //converting to 3dB bandwith in Hz, 	//PH96 pg 3
	
	unit->bandwidth= bandwidth; 
	
	// filter coefficients to calculate, p.435 hohmann paper
	
	double beta= 6.2831853071796*centrefrequency*samplingperiod;
	double phi= 3.1415926535898*bandwidth*samplingperiod; 
	double p=  (1.6827902832904*cos(phi) -2)*6.3049771007832;  
	double lambda= (p*(-0.5))-(sqrt(p*p*0.25-1.0)); 
	
	unit->reala= lambda*cos(beta); 
	unit->imaga= lambda*sin(beta);
	
	//avoid b= 0 or Nyquist, otherise must remove factor of 2.0 here
	unit->normalisation= 2.0*(pow(1-fabs(lambda),4)); 	
	
	
	//printf("set-up gammatone filter %f %f %f %f %f \n",centrefrequency, bandwidth, unit->normalisation, unit->reala, unit->imaga);
	
	
	SETCALC(Gammatone_next);
	
}


// this is a function for preventing pathological math operations in ugens.
// can be used at the end of a block to fix any recirculating filter values.
//inline float zapgremlins(float x)
//{
//	float absx = fabs(x);
//	// very small numbers fail the first test, eliminating denormalized numbers
//	//    (zero also fails the first test, but that is OK since it returns zero.)
//	// very large numbers fail the second test, eliminating infinities
//	// Not-a-Numbers fail both tests and are eliminated.
//	return (absx > (float)1e-15 && absx < (float)1e15) ? x : (float)0.;
//}

void Gammatone_next(Gammatone *unit, int inNumSamples) {
	
	int i,j; 
	
	float *input = IN(0); 
	float *output = OUT(0);
	
	double newreal, newimag; 
	
	double * oldreal = &(unit->oldreal[0]); 
	double * oldimag = &(unit->oldimag[0]); 
	double reala = unit->reala; 
	double imaga = unit->imaga; 
	double normalisation = unit->normalisation; 
	
	for (i=0; i<inNumSamples; ++i) {
		
		newreal= input[i]; //real input 
		newimag=0.0; 
		
		for (j=0; j<4; ++j) {
			
			newreal= newreal + (reala*oldreal[j])-(imaga*oldimag[j]);
			newimag= newimag + (reala*oldimag[j])+(imaga*oldreal[j]);
			
			oldreal[j]= newreal; //zapgremlins(newreal); //trying to avoid denormals which mess up processing via underflow
			oldimag[j]= newimag; //zapgremlins(newimag); 
		}
		
		output[i]= newreal*normalisation; 
		
		//imaginary output too could be useful
		
	}
	
	//printf("testing a %f %f %f %f \n",newreal, newimag, normalisation, output[0]);
	
}

//void Gammatone_Dtor(Gammatone* unit) {
//	
//	
//}



PluginLoad(AuditoryModeling) {
	
	ft = inTable;
	
	//DefineSimpleCantAliasUnit(PitchNoteUGen);

	DefineSimpleUnit(Gammatone);
	DefineDtorCantAliasUnit(Meddis);
	DefineDtorCantAliasUnit(HairCell);
	
	
	//see http://www.listarc.bham.ac.uk/lists/sc-dev-2003/msg03275.html
	//DefinePlugInCmd; at scope of whole plugin
	
	//DefineUnitCmd; at scope of single UGen; I believe which node is passed in so can be specific to one running instance 
	
}





