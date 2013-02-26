/*

Tree UGens for SuperCollider, by Dan Stowell.
(c) Dan Stowell 2009-2011.

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
#include <cfloat> // for FLT_MAX

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

struct NearestN : public Unit
{
	// The GET_BUF macro fills in these two:
	float m_fbufnum;
	SndBuf *m_buf;

	int m_ndims;
	int m_num; // number of results requested
	float *m_inputdata; // Input data will be remembered in here
	float *m_bestlist;
};

// declare unit generator functions
extern "C"
{
	void load(InterfaceTable *inTable);

	void PlaneTree_Ctor(PlaneTree* unit);
	void PlaneTree_next(PlaneTree *unit, int inNumSamples);
	void PlaneTree_Dtor(PlaneTree* unit);

	void NearestN_Ctor(NearestN* unit);
	void NearestN_next(NearestN *unit, int inNumSamples);
	void NearestN_Dtor(NearestN* unit);
};

//////////////////////////////////////////////////////////////////

/**
 * Functions for manipulating the specially-formatted path integer.
 * The root is always 1, and you branch 'left' by appending a zero, 'right' by appending a 1.
 */
inline bool pathInt_isRoot  ( const int pathInt) { return (pathInt == 1    ); }
inline int  pathInt_goLeft(   const int pathInt) { return (pathInt << 1    ); }
inline int  pathInt_goRight(  const int pathInt) { return (pathInt << 1 | 1); }
inline int  pathInt_goSibling(const int pathInt) { return (pathInt ^ 1     ); }
inline int  pathInt_goParent( const int pathInt) { return (pathInt >> 1    ); }
inline int  pathInt_depth(    const int pathInt) { return ( NUMBITS(pathInt) - 1 ); } // root is depth 0, its kids are depth 1, etc

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
		pathInt = pathInt_goLeft(pathInt);
	}else{
		pathInt = pathInt_goRight(pathInt);
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

void NearestN_Ctor(NearestN* unit){
	// Infer the size of the "inputs" array which has been tagged on to the end of the arguments list.
	int ndims = unit->mNumInputs - 3;

	int num   = ZIN0(2);

	// Allocate a comfy bit of memory where we'll put the input data while we process it
	unit->m_inputdata   = (float*)RTAlloc(unit->mWorld, ndims * sizeof(float));
	unit->m_bestlist = (float*)RTAlloc(unit->mWorld, num * 3 * sizeof(float));
	Clear(num * 3, unit->m_bestlist);
	// Try and ensure that the first ever input won't get accidentally skipped:
	unit->m_inputdata[0] = -1e9f;

	// Get the buffer reference, and check that the size and num channels matches what we expect.
	unit->m_fbufnum = -1e9f;
	{
		GET_BUF

		// initialize the unit generator state variables.
		unit->m_ndims     = ndims;
		unit->m_num       = num;

		SETCALC(NearestN_next);
	}
	NearestN_next(unit, 1);
}

/**
 * kd-tree descent - recursive function simply drops down from its starting point to the closest leaf, returning that leaf's index.
 *    It doesn't update the bestlist; it's only ascent that does that.
 */
int NearestN_descend(const int index, const int ndims, const float* inputData, const float* bufData, const int bufChannels, const int bufFrames){
	if(index >= bufFrames) return pathInt_goParent(index); // protect against going out of our memory bounds
	int whichdim = pathInt_depth(index) % ndims;
	bool goRight = inputData[whichdim] > bufData[bufChannels * index + 2 + whichdim];
	if(goRight){
		if(bufData[bufChannels * index + 1] > 0.f)
			return index;
		else
			return NearestN_descend(pathInt_goRight(index), ndims, inputData, bufData, bufChannels, bufFrames);
	}else{
		if(bufData[bufChannels * index    ] > 0.f)
			return index;
		else
			return NearestN_descend(pathInt_goLeft( index), ndims, inputData, bufData, bufChannels, bufFrames);
	}
} // end descend func

/**
 * kd-tree ascent - Starts at index 'fromLeaf', and checks each sibling (inc if it exists!) until it's got as high as asFarAs.
 *   That main iteration is non-recursive, just a loop.
 *   But when going down a different trouser-leg, it calls 'descend' followed by a truncated 'ascend' (i.e. it recurses).
 */
void NearestN_ascend(const int fromLeaf, const int asFarAs, const int ndims, const float* inputData, const float* bufData, 
				float* bestlist, const int num, const int bufChannels, const int bufFrames){
	int index = fromLeaf;
	while(index >= asFarAs){
		// here we check if the actual distance of current node is near enough to add to the results list.
		float distsq = 0.f;
		for (int whichdim=0; whichdim<ndims; ++whichdim){
			float delta = bufData[bufChannels * index + 2 + whichdim] - inputData[whichdim];
			distsq += delta * delta;
		}
		// this can be more efficient: rather than iterating upwards, iterate downwards, and first time we are beaten, appendinsert and break
		for(int i=0; i<num; ++i){
			if(distsq < bestlist[i * 3 + 1]){
				// shuffle the remainder of the list down
				for(int k=num*3-4; k>=i*3; --k){
					bestlist[k + 3] = bestlist[k];
				}
				// and add ourselves in to the list
				bestlist[i * 3    ] = index;
				bestlist[i * 3 + 1] = distsq;
				bestlist[i * 3 + 2] = bufData[bufChannels * (index+1) - 1]; // last is label
				break;
			}
		}

		if(pathInt_isRoot(index) || (index == asFarAs)){
		       break;
		}else{
			// check if parent's splitting plane is near enough the splitting plane to warrant investigation down the other branch
			int parent = pathInt_goParent(index);
			int whichdim = pathInt_depth(parent) % ndims;
			float perpdist = bufData[bufChannels * parent + 2 + whichdim] - inputData[whichdim];
			float perpdistsq = perpdist * perpdist;
			// we don't need to check all items in the best-so-far since it's sorted; just check the last one
			if(perpdistsq <= bestlist[num * 3 - 2]){
				// ok so we're not yet so far away. descend the sibling, then ascend again.
				int sibling = pathInt_goSibling(index);
				int firstLeaf = NearestN_descend(sibling, ndims, inputData, bufData, bufChannels, bufFrames);
				NearestN_ascend(firstLeaf, sibling, ndims, inputData, bufData, bestlist, num, bufChannels, bufFrames);
			}
			// ok, so go up to the parent and continue checking
			index = parent;
		}
	}

} // end ascend func

void NearestN_next(NearestN *unit, int inNumSamples){
	GET_BUF
	int ndims = unit->m_ndims;
	if((int)bufChannels != (ndims + 3)){
		Print("NearestN: number of channels in buffer (%i) != number of input dimensions (%i) + 3\n",
			  bufChannels, ndims);
		SETCALC(*ClearUnitOutputs);
		return;
	}

	int num   = unit->m_num;
	float* bestlist = unit->m_bestlist;
	float* inputdata = unit->m_inputdata;

	for(int i=0; i<inNumSamples; ++i){
		if(IN(1)[i] > 0.f){ // If gate > 0
			// Get data inputs, ALSO checking whether they've changed
			bool inputchanged=false;
			float chanval;
			for(int chan=0; chan<ndims; ++chan){
				chanval = IN(chan + 3)[i];
				if(inputdata[chan] != chanval){
					inputdata[chan] = chanval;
					inputchanged = true;
				}
			}
			if(inputchanged){
				// init the search: must set the results array to infinitely bad
				for(int j=0; j<num; ++j){
					bestlist[3 * j    ] = -1;
					bestlist[3 * j + 1] = FLT_MAX;
					bestlist[3 * j + 2] = -1;
				}
				// First, recurse from very top to get to the 'first guess' leaf.
				int firstLeaf = NearestN_descend(1, ndims, inputdata, bufData, bufChannels, bufFrames);
				// Then ascend back up the full tree (which may itself involve more descend+ascend loops)
				NearestN_ascend(firstLeaf, 0, ndims, inputdata, bufData, bestlist, num, bufChannels, bufFrames);
			}
		} // End gate check
		// The results should now be in 'bestlist' - let's write them to the output
		for(int j=0; j< (num*3); ++j){
			OUT(j)[i] = bestlist[j];
		}
	} // end loop inNumSamples
}

void NearestN_Dtor(NearestN* unit){
	if(unit->m_inputdata  ) RTFree(unit->mWorld, unit->m_inputdata  );
	if(unit->m_bestlist   ) RTFree(unit->mWorld, unit->m_bestlist   );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// the load function is called by the host when the plug-in is loaded
PluginLoad(MCLDTree)
{
	ft = inTable;

	DefineDtorUnit(PlaneTree);
	DefineDtorUnit(NearestN);
}

