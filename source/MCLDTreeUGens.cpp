/*

Tree UGens for SuperCollider, by Dan Stowell.
(c) Dan Stowell 2009-2010.

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
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA

*/

#include "SC_PlugIn.h"

static InterfaceTable *ft;

struct PlaneTree : public Unit
{
	// The GET_BUF macro fills in these two:
	float m_fbufnum;
	SndBuf *m_buf;
	
	float *m_inputdata; // Input data will be remembered in here
	float *m_workingdata; // ...and mangled in here
	int m_ndims;
	float m_result;
};

// declare unit generator functions 
extern "C"
{
	void load(InterfaceTable *inTable);
	
	void PlaneTree_Ctor(PlaneTree* unit);
	void PlaneTree_next(PlaneTree *unit, int inNumSamples);
	void PlaneTree_Dtor(PlaneTree* unit);
};

//////////////////////////////////////////////////////////////////

void PlaneTree_Ctor(PlaneTree* unit)
{
	// Infer the size of the "inputs" array which has been tagged on to the end of the arguments list.
	int ndims = unit->mNumInputs - 2;
	
	//Print("PlaneTree_Ctor: ndims is %i\n", ndims);

	// Allocate a comfy bit of memory where we'll put the input data while we process it
	unit->m_inputdata   = (float*)RTAlloc(unit->mWorld, ndims * sizeof(float));
	unit->m_workingdata = (float*)RTAlloc(unit->mWorld, ndims * sizeof(float));
	// Try and ensure that the first ever input won't get accidentally skipped:
	unit->m_inputdata[0] = -1e9f;
	
	// Get the buffer reference, and check that the size and num channels matches what we expect.
	unit->m_fbufnum = -1e9f;
	GET_BUF
	
	if((int)bufChannels != (ndims * 2 + 2)){
		Print("PlaneTree_Ctor: number of channels in buffer (%i) != number of input dimensions (%i) * 2 + 2\n", 
									bufChannels, ndims);
		SETCALC(*ClearUnitOutputs);
		return;
	}

	// initialize the unit generator state variables.
	unit->m_ndims    = ndims;
	unit->m_result   = -1e9f; // hopefully this will get filled in soon by a classification...

	SETCALC(PlaneTree_next);
	PlaneTree_next(unit, 1);
}

// recursive function, returns the classification result.
// NB the "pathInt" val MUST be one-indexed (i.e. "1" is the FIRST frame)
float PlaneTree_recurse(int ndims, float *inputdata, float *workingdata, float *bufData, uint32 bufChannels, unsigned long pathInt);
float PlaneTree_recurse(int ndims, float *inputdata, float *workingdata, float *bufData, uint32 bufChannels, unsigned long pathInt){
	// This pointer will be incremented as we go along:
	float* ourFrame = bufData + (pathInt - 1) * bufChannels;   // Note the "-1" - because the indexing starts at 1 not 0 !
	// First subtract the offset, the first D items in the frame
	int i;
	for(i=0; i<ndims; ++i){
		workingdata[i] = inputdata[i] - *(ourFrame++);
	}
	// Then multiply by the normal vector, the next D items; and sum
	float sum = 0.f;
	for(i=0; i<ndims; ++i){
		sum += workingdata[i] * *(ourFrame++);
	}
	// Now if the sum is positive we go left (ourFrame already pointing at the correct place), otherwise right.
	// "pathInt" here becomes the index of the child, which is either returned or branched into.
	if(sum > 0.f){
		pathInt = (pathInt << 1);     // left
	}else{
		pathInt = (pathInt << 1) | 1; // right
		++ourFrame; // increment to find the float indicating whether we'll be branching or not
	}
	// So now we can check if we want to branch and recurse, or if we've hit a leaf and want to return
	if(*(ourFrame) == 0.f){
		return PlaneTree_recurse(ndims, inputdata, workingdata, bufData, bufChannels, pathInt); // branch further
	}else{
		return static_cast<float>(pathInt); // leaf
	}
}

void PlaneTree_next(PlaneTree *unit, int inNumSamples)
{
	GET_BUF
	int ndims = unit->m_ndims;
	float *inputdata   = unit->m_inputdata;
	float *workingdata = unit->m_workingdata;
	float result = unit->m_result;
	
	for(int i=0; i<inNumSamples; ++i){
		
		if(IN(1)[i] > 0.f){ // If gate > 0
			
			// Get data inputs, ALSO checking whether they've changed
			bool inputchanged=false;
			float chanval;
			for(int chan=0; chan<ndims; ++chan){
				chanval = IN(chan + 2)[i];
				if(inputdata[chan] != chanval){
					inputdata[chan] = chanval;
					inputchanged = true;
				}
			}
			if(inputchanged){
				result = PlaneTree_recurse(ndims, inputdata, workingdata, bufData, bufChannels, 1);
			}
		} // End gate check
		OUT(0)[i] = result;
	} // end loop inNumSamples
	unit->m_result = result;
}

void PlaneTree_Dtor(PlaneTree* unit)
{
	if(unit->m_inputdata  ) RTFree(unit->mWorld, unit->m_inputdata  );
	if(unit->m_workingdata) RTFree(unit->mWorld, unit->m_workingdata);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// the load function is called by the host when the plug-in is loaded
void load(InterfaceTable *inTable)
{
	ft = inTable;

	DefineDtorUnit(PlaneTree);
}
