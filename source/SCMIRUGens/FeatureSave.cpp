/*
	SuperCollider real time audio synthesis system
 Copyright (c) 2002 James McCartney. All rights reserved.
	http://www.audiosynth.com
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


#include "SC_PlugIn.h"

#include "stdio.h"

InterfaceTable *ft; 


struct FeatureSave : public Unit    
{
 
	int frameswritten_; 
	int numfeatures_; 
	//char * filename_;
	int fileready_; 
	
	//open file data
	FILE *fp;
	
};

extern "C" {  
	
	void FeatureSave_next(FeatureSave *unit, int inNumSamples);
	void FeatureSave_Ctor(FeatureSave* unit);
	//void FeatureSave_Dtor(FeatureSave* unit);

}


void FeatureSaveUnitCmdFunc (Unit *unit, struct sc_msg_iter *args); 
void FeatureSaveUnitCmdFunc2 (Unit *unit, struct sc_msg_iter *args); 



//will need to create in NRT mode

void FeatureSave_Ctor(FeatureSave* unit) {
	
	unit->frameswritten_ = 0; 
	unit->fileready_ = 0; 
	unit->numfeatures_ = IN0(0); 
	
	//printf("FeatureSave: numfeatures %d \n",unit->numfeatures_); 
	
	
	DefineUnitCmd("FeatureSave","createfile",FeatureSaveUnitCmdFunc);
	DefineUnitCmd("FeatureSave","closefile",FeatureSaveUnitCmdFunc2);
	
	
	//printf("FeatureSave sanity check: unit pointer %p \n",unit->mUnitDef); 
	
	
	//mCmds undefined here 
	//unit->mUnitDef->mCmds; 
	
	
	SETCALC(FeatureSave_next); 
	
}



void FeatureSave_next(FeatureSave *unit, int inNumSamples) {

	float trig = IN0(1); //trigger input

	//if trigger and file open 

	//printf("%f %d %d %d \n",trig, unit->fileready_, unit->numfeatures_, trig>(-0.01f));
	//printf("FeatureSave:next sanity check: unit pointer %p \n",unit->mUnitDef); 
	
	if((trig>(-0.01f)) && (unit->fileready_==1)) {
		
		//printf("%f %d \n",trig, unit->fileready_); 
		
		//write to file
		
		for (int i=0; i<unit->numfeatures_; ++i) {
		
			float featureval = IN0(i+2);  
			//printf("%d %f trig %f \n",i, featureval, trig); 
			
			//printf("featureval %f %d \n",featureval, unit->fileready_);
			
			
			fwrite(&featureval, sizeof(float),1, unit->fp);
			
		}
		
		
		++unit->frameswritten_; 
		
	}
	
	
}



void FeatureSaveUnitCmdFunc (Unit *unit, struct sc_msg_iter *args) {
	
	FeatureSave * fsave = (FeatureSave*) unit; 
	
	//message type:
	//int cmdtype = args->geti();
	 
	
	const char * stringarg = args->gets();
	
	//printf("got here at least %s\n",stringarg);
	
	//strcpy(fsave->filename_, stringarg);  
	
	//create file for writing 
	//won't push to NRT thread for now since assuming only using this UGen in NRT mode to start with
	
	//fsave->fp = fopen(fsave->filename_, "wb");
	fsave->fp = fopen(stringarg, "wb+");

	//dummy header data for now
	fwrite(&fsave->frameswritten_, sizeof(int),1, fsave->fp);
	
	fwrite(&fsave->numfeatures_, sizeof(int),1, fsave->fp);
		
	fsave->fileready_ = 1; 
	
}



void FeatureSaveUnitCmdFunc2 (Unit *unit, struct sc_msg_iter *args) {
	
	FeatureSave * fsave = (FeatureSave*) unit; 
	
	//message type:
	//int cmdtype = args->geti();
	
	fsave->fileready_ = 0; 
	
	//printf("ever called? %d \n", fsave->frameswritten_);
	
	//finalise header data: 
	
	//back to start
	//rewind(fsave->fp); 
	
	fseek(fsave->fp, 0, SEEK_SET);
	
	//int testval = 64; 
	//fwrite(&testval, sizeof(int),1, fsave->fp);
	
	fwrite(&fsave->frameswritten_, sizeof(int),1, fsave->fp);
	
//	int test; 
//	fread(&test, sizeof(int),1, fsave->fp);
//	printf("ever called 2? %d \n", test);
//	
//	
//	fseek(fsave->fp, 0, SEEK_END);
	
	//close file
	fclose(fsave->fp); 
	
	
}

PluginLoad(FeatureSave) {
	
	//FILE * testfile = fopen("blah", "wb");
//	
//	int numelements = 1000; 
//	
//	fwrite(&numelements, sizeof(int),1, testfile);
//	
//	for (int i=0; i<numelements; ++i) {
//		float now = i*0.67; 
//		fwrite(&now, sizeof(int),1, testfile);
//	
//	}
//
//	fclose(testfile); 
	
	
	ft = inTable;
	
	DefineSimpleUnit(FeatureSave); 
	
}




