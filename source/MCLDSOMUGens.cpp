/*

Self-Organising Map (SOM) UGens for SuperCollider, by Dan Stowell.
(c) Dan Stowell 2008.

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

static InterfaceTable *ft;

// This struct holds things that both the trainer and the reader need to use
struct SOMUnit : public Unit
{
	// The GET_BUF macro fills in these two:
	float m_fbufnum;
	SndBuf *m_buf;
	
	float *m_inputdata; // Input data will be dropped in here for processing
	int m_netsize, m_numdims, m_numinputdims;
	int *m_bestcoords;
	float m_reconsterror;
};
struct SOMTrain : public SOMUnit
{
	int m_traindur, m_traincountdown;
	double m_nhood, m_nhooddelta, m_alpha, m_alphadelta;
};
struct SOMRd : public SOMUnit
{
};

// declare unit generator functions 
extern "C"
{
	void load(InterfaceTable *inTable);
	
	void SOMTrain_Ctor(SOMTrain* unit);
	void SOMTrain_reset(SOMTrain* unit);
	void SOMTrain_next(SOMTrain *unit, int inNumSamples);
	
	void SOMRd_Ctor(SOMRd* unit);
	void SOMRd_reset(SOMRd* unit);
	void SOMRd_next(SOMRd *unit, int inNumSamples);
};

//////////////////////////////////////////////////////////////////

// How to get a reference to the intended Buffer frame, given the spatial coordinates
#define SOM_GETFRAME_1D(i0)          (bufData + (i0 * numinputdims))
#define SOM_GETFRAME_2D(i0,i1)       (bufData + (i1 * netsize + i0) * numinputdims)
#define SOM_GETFRAME_3D(i0,i1,i2)    (bufData + ((i2 * netsize + i1) * netsize + i0) * numinputdims)
#define SOM_GETFRAME_4D(i0,i1,i2,i3) (bufData + (((i3 * netsize + i2) * netsize + i1) * netsize + i0) * numinputdims)

// Grabbing the buffer object contents. Note that some of this chunk is copied from GET_BUF
#define SOM_GET_BUF \
		SndBuf *buf = unit->m_buf; \
		float *bufData __attribute__((__unused__)) = buf->data; \
		uint32 bufChannels __attribute__((__unused__)) = buf->channels; \
		uint32 bufSamples __attribute__((__unused__)) = buf->samples; \
		uint32 bufFrames __attribute__((__unused__)) = buf->frames; \
		int numdims = unit->m_numdims; \
		int numinputdims = unit->m_numinputdims; \
		float* inputdata = unit->m_inputdata; \
		int* bestcoords = unit->m_bestcoords; \
		int netsize = unit->m_netsize;

// This constant affects the "shape" of the updating neighbourhood.
// When set at 0.0 the updating is "flat" - all units in the neighbourhood are updated to the same extent.
// When set at 1.0 the updating is "triangular" - update strength tails off linearly towards the edge of the neighbourhood.
// A compromise allows for plenty of learning but still concentrating the learning more strongly at the centre.
// #define LRNSCALE 0.0
#define LRNSCALE 0.6
// #define LRNSCALE 1.0

//////////////////////////////////////////////////////////////////

inline double SOM_findnearest_1d(float *bufData, float *inputdata, int *bestcoords, int netsize, int numinputdims);
inline double SOM_findnearest_2d(float *bufData, float *inputdata, int *bestcoords, int netsize, int numinputdims);
inline double SOM_findnearest_3d(float *bufData, float *inputdata, int *bestcoords, int netsize, int numinputdims);
inline double SOM_findnearest_4d(float *bufData, float *inputdata, int *bestcoords, int netsize, int numinputdims);

inline double SOM_findnearest_getdist(float *celldata, int numinputdims, float *inputdata);

inline void SOMTrain_updatenodes_1d(float *bufData, float *inputdata, int *bestcoords, int netsize, int numinputdims, double alpha, int nhoodi, int nhoodisq);
inline void SOMTrain_updatenodes_2d(float *bufData, float *inputdata, int *bestcoords, int netsize, int numinputdims, double alpha, int nhoodi, int nhoodisq);
inline void SOMTrain_updatenodes_3d(float *bufData, float *inputdata, int *bestcoords, int netsize, int numinputdims, double alpha, int nhoodi, int nhoodisq);
inline void SOMTrain_updatenodes_4d(float *bufData, float *inputdata, int *bestcoords, int netsize, int numinputdims, double alpha, int nhoodi, int nhoodisq);

inline void SOMTrain_updatenodes_update(float* celldata, int numinputdims, double alpha, float *inputdata);

// Some constructor stuff is common to all units
void SOM_Ctor_base(SOMUnit* unit, int inputsOffset);
void SOM_Ctor_base(SOMUnit* unit, int inputsOffset)
{
	int netsize = (int)ZIN0(1);
	int numdims = (int)ZIN0(2);
	int numnodes = (int)pow(netsize, numdims);
	
	// Infer the size of the "inputs" array which has been tagged on to the end of the arguments list.
	int numinputdims = unit->mNumInputs - inputsOffset;
	// Allocate a comfy bit of memory where we'll put the input data while we process it
	unit->m_inputdata = (float*)RTAlloc(unit->mWorld, numinputdims * sizeof(float));
	// And here's where we'll cache the net coord of the best-node-so-far
	unit->m_bestcoords = (int*)RTAlloc(unit->mWorld, numdims * sizeof(int));
	
	// Get the buffer reference, and check that the size and num channels matches what we expect.
	unit->m_fbufnum = -1e9f;
	GET_BUF
	
	if((int)bufChannels != numinputdims){
		Print("SOM_Ctor_base: number of channels in buffer (%i) != number of input dimensions (%i)\n", 
									bufChannels, numinputdims);
		SETCALC(*ClearUnitOutputs);
		return;
	}
	if((int)bufFrames != numnodes){
		Print("SOM_Ctor_base: number of frames in buffer (%i) != requested number of nodes in net (%i)\n", 
									bufFrames, numnodes);
		SETCALC(*ClearUnitOutputs);
		return;
	}

	// initialize the unit generator state variables.
	unit->m_netsize    = netsize;
	unit->m_numdims    = numdims;
	unit->m_numinputdims = numinputdims;
	unit->m_reconsterror = 0.f;
}

void SOM_Dtor_base(SOMUnit* unit);
void SOM_Dtor_base(SOMUnit* unit)
{
	RTFree(unit->mWorld, unit->m_inputdata);
	RTFree(unit->mWorld, unit->m_bestcoords);
}

//////////////////////////////////////////////////////////////////

void SOMTrain_Ctor(SOMTrain* unit)
{
	// set the calculation function. do this before the base ctor because it may want to change it!
	SETCALC(SOMTrain_next);

	SOM_Ctor_base(unit, 7); // 7 is the offset before we get input data
	
	int traindur = (int)ZIN0(3);
	int traincountdown = traindur; // Decrement by one on each occasion
	
	double nhood = ZIN0(4) * 0.5; // Neighbourhood size (fraction of total) to be included in a training update. Will decrement slowly.
	// The reason we halve it is for convenience: we look nhood/2 in positive direction, nhood/2 in negative direction.
	double nhooddelta = nhood / traindur; // This is how much it decrements by, each occasion.
	
	double alpha = ZIN0(6); // How much the node "bends" towards the datum
	double alphadelta = alpha / traindur; // This is how much it decrements by, each occasion.
	
	// initialize the unit generator state variables.
	unit->m_traindur   = traindur;
	unit->m_traincountdown = traincountdown;
	unit->m_nhood      = nhood;
	unit->m_nhooddelta = nhooddelta;
	unit->m_alpha      = alpha;
	unit->m_alphadelta = alphadelta;
	
	// calculate one sample of output.
	ZOUT0(0) = 0.f;
	ZOUT0(1) = 0.f;
}

void SOMTrain_next(SOMTrain *unit, int inNumSamples)
{
	int traincountdown = unit->m_traincountdown;
	
	if(ZIN0(5) > 0.f){ // If gate > 0
		
		// Get the buffer and some other standard stuff...
		SOM_GET_BUF
		
		// Get data inputs
		for(int chan=0; chan<numinputdims; ++chan){
			inputdata[chan] = ZIN0(chan + 7);
		}
		
		// Get state from struct
		float alpha = (float)unit->m_alpha;
		// get "nhood" as an integer, NB use ceil to make sure the neighbourhood errs on side of bigness
		int nhoodi = (int)ceil(unit->m_nhood);
		// squared distance comparisons are used in the neighbourhood-update function. The "plus one" is done so we can use "<" rather than "<=" later
		int nhoodisq = nhoodi * nhoodi + 1;
		
		// DO THE NEAREST-NEIGHBOUR SEARCH
		switch(numdims){
			case 1: unit->m_reconsterror = SOM_findnearest_1d(bufData, inputdata, bestcoords, netsize, numinputdims); break;
			case 2: unit->m_reconsterror = SOM_findnearest_2d(bufData, inputdata, bestcoords, netsize, numinputdims); break;
			case 3: unit->m_reconsterror = SOM_findnearest_3d(bufData, inputdata, bestcoords, netsize, numinputdims); break;
			case 4: unit->m_reconsterror = SOM_findnearest_4d(bufData, inputdata, bestcoords, netsize, numinputdims); break;
		}
				
		if(traincountdown != 0){
			// UPDATE THE NODES
			switch(numdims){
				case 1: SOMTrain_updatenodes_1d(bufData, inputdata, bestcoords, netsize, numinputdims, alpha, nhoodi, nhoodisq); break;
				case 2: SOMTrain_updatenodes_2d(bufData, inputdata, bestcoords, netsize, numinputdims, alpha, nhoodi, nhoodisq); break;
				case 3: SOMTrain_updatenodes_3d(bufData, inputdata, bestcoords, netsize, numinputdims, alpha, nhoodi, nhoodisq); break;
				case 4: SOMTrain_updatenodes_4d(bufData, inputdata, bestcoords, netsize, numinputdims, alpha, nhoodi, nhoodisq); break;
			}
						
			// Save state to struct.
			// NB Update "alpha" and "nhood" (both shrink slightly) 
			unit->m_nhood = unit->m_nhood - unit->m_nhooddelta;
			unit->m_alpha = unit->m_alpha - unit->m_alphadelta;
			unit->m_traincountdown = traincountdown = traincountdown - 1;
			if(traincountdown==0){
				unit->mDone = true;
			}
		} // End still-training-check
	} // End gate check
	
	ZOUT0(0) = traincountdown;
	ZOUT0(1) = unit->m_reconsterror;
}
void SOMTrain_Dtor(SOMTrain* unit)
{
	SOM_Dtor_base(unit);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void SOMRd_Ctor(SOMRd* unit)
{
	// set the calculation function. do this before the base ctor because it may want to change it!
	SETCALC(SOMRd_next);

	SOM_Ctor_base(unit, 4); // 4 is the offset before we get input data
	
	ClearUnitOutputs(unit, 1);
}

void SOMRd_next(SOMRd *unit, int inNumSamples)
{
	// Get the buffer and some other standard stuff...
	SOM_GET_BUF
	
	for(int i=0; i<inNumSamples; ++i){
		
		if(IN(3)[i] > 0.f){ // If gate > 0
			
			// Get data inputs, ALSO checking whether they've changed
			bool inputchanged=false;
			float chanval;
			for(int chan=0; chan<numinputdims; ++chan){
				chanval = IN(chan + 4)[i];
				if(inputdata[chan] != chanval){
					inputdata[chan] = chanval;
					inputchanged = true;
				}
			}
			
			if(inputchanged){
				// DO THE NEAREST-NEIGHBOUR SEARCH
				switch(numdims){
					case 1: unit->m_reconsterror = SOM_findnearest_1d(bufData, inputdata, bestcoords, netsize, numinputdims); break;
					case 2: unit->m_reconsterror = SOM_findnearest_2d(bufData, inputdata, bestcoords, netsize, numinputdims); break;
					case 3: unit->m_reconsterror = SOM_findnearest_3d(bufData, inputdata, bestcoords, netsize, numinputdims); break;
					case 4: unit->m_reconsterror = SOM_findnearest_4d(bufData, inputdata, bestcoords, netsize, numinputdims); break;
				}
			}
		} // End gate check
		
		// So now we output bestcoords, which has been filled in by the findnearest. bestcoords is the INDEX of the best node, in the SOM grid
		switch(numdims){
			// NOTE: the cases are SUPPOSED to fall through to each other here :) so no "break"
			case 4:		OUT(3)[i] = (float)bestcoords[3];
			case 3:		OUT(2)[i] = (float)bestcoords[2];
			case 2:		OUT(1)[i] = (float)bestcoords[1];
			case 1:		OUT(0)[i] = (float)bestcoords[0];
		}
	} // end loop inNumSamples
}
void SOMRd_Dtor(SOMRd* unit)
{
	SOM_Dtor_base(unit);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// THE FIND-NEAREST-NEIGHBOUR FUNCTIONS

// Find closest cell, using simplistic iteration over nodes.
// Note: We store/compare SQUARED distances since that's more efficient.

#define SOM_findnearest_INIT \
	double curdist, bestdist = INFINITY; \
	float* celldata;

inline double SOM_findnearest_1d(float *bufData, float *inputdata, int *bestcoords, int netsize, int numinputdims){
	SOM_findnearest_INIT
	
	for(int i0 = 0; i0 < netsize; ++i0){
		celldata = SOM_GETFRAME_1D(i0); // a float-pointer to the desired frame
		
		curdist = SOM_findnearest_getdist(celldata, numinputdims, inputdata);
		// OK, so curdist is the (squared) distance. But is it better?
		if(curdist < bestdist){
			bestdist = curdist;
			bestcoords[0] = i0;
		}
	}
	return bestdist;
}
inline double SOM_findnearest_2d(float *bufData, float *inputdata, int *bestcoords, int netsize, int numinputdims){
	SOM_findnearest_INIT
	
	for(int i0 = 0; i0 < netsize; ++i0){
	for(int i1 = 0; i1 < netsize; ++i1){
		celldata = SOM_GETFRAME_2D(i0, i1); // a float-pointer to the desired frame
		
		curdist = SOM_findnearest_getdist(celldata, numinputdims, inputdata);
		// OK, so curdist is the (squared) distance. But is it better?
		if(curdist < bestdist){
			bestdist = curdist;
			bestcoords[0] = i0;
			bestcoords[1] = i1;
		}
	}
	}
	return bestdist;
}
inline double SOM_findnearest_3d(float *bufData, float *inputdata, int *bestcoords, int netsize, int numinputdims){
	SOM_findnearest_INIT
	
	for(int i0 = 0; i0 < netsize; ++i0){
	for(int i1 = 0; i1 < netsize; ++i1){
	for(int i2 = 0; i2 < netsize; ++i2){
		celldata = SOM_GETFRAME_3D(i0, i1, i2); // a float-pointer to the desired frame
		
		curdist = SOM_findnearest_getdist(celldata, numinputdims, inputdata);
		// OK, so curdist is the (squared) distance. But is it better?
		if(curdist < bestdist){
			bestdist = curdist;
			bestcoords[0] = i0;
			bestcoords[1] = i1;
			bestcoords[2] = i2;
		}
	}
	}
	}
	return bestdist;
}
inline double SOM_findnearest_4d(float *bufData, float *inputdata, int *bestcoords, int netsize, int numinputdims){
	SOM_findnearest_INIT
	
	for(int i0 = 0; i0 < netsize; ++i0){
	for(int i1 = 0; i1 < netsize; ++i1){
	for(int i2 = 0; i2 < netsize; ++i2){
	for(int i3 = 0; i3 < netsize; ++i3){
		celldata = SOM_GETFRAME_4D(i0, i1, i2, i3); // a float-pointer to the desired frame
		
		curdist = SOM_findnearest_getdist(celldata, numinputdims, inputdata);
		// OK, so curdist is the (squared) distance. But is it better?
		if(curdist < bestdist){
			bestdist = curdist;
			bestcoords[0] = i0;
			bestcoords[1] = i1;
			bestcoords[2] = i2;
			bestcoords[3] = i3;
		}
	}
	}
	}
	}
	return bestdist;
}

inline double SOM_findnearest_getdist(float *celldata, int numinputdims, float *inputdata){
	double curdist = 0.0;
	float dist1d;
	
	switch(numinputdims){
		// Unrolled versions for small input dimensionality
		case 1:
			dist1d = celldata[0] - inputdata[0];
			curdist = dist1d * dist1d;
			break;
		case 2:
			dist1d = celldata[0] - inputdata[0];
			curdist = dist1d * dist1d;
			dist1d = celldata[1] - inputdata[1];
			curdist += dist1d * dist1d;
			break;
		case 3:
			dist1d = celldata[0] - inputdata[0];
			curdist = dist1d * dist1d;
			dist1d = celldata[1] - inputdata[1];
			curdist += dist1d * dist1d;
			dist1d = celldata[2] - inputdata[2];
			curdist += dist1d * dist1d;
			break;
		case 4:
			dist1d = celldata[0] - inputdata[0];
			curdist = dist1d * dist1d;
			dist1d = celldata[1] - inputdata[1];
			curdist += dist1d * dist1d;
			dist1d = celldata[2] - inputdata[2];
			curdist += dist1d * dist1d;
			dist1d = celldata[3] - inputdata[3];
			curdist += dist1d * dist1d;
			break;
		
		default:
			// non-unrolled version, for any input dimensionality
			for(int chan = 0; chan < numinputdims; ++chan){
				dist1d = *celldata - inputdata[chan]; //[chan]; - instead of indexing use postincrement; "celldata" address moves through the channels
				curdist = curdist + (dist1d * dist1d);
				++celldata;
			}
	}
	return curdist;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// THE NODE-UPDATING FUNCTIONS

// Determine the start and end points of the neighbourhood. NB "lo0" is the starting point but "hi0" is never reached
#define SOMTrain_UN_INITFORDIM(x) \
	int lo##x, hi##x, i##x; \
	lo##x = bestcoords[x] - nhoodi; \
	if(lo##x < 0) \
		lo##x = 0; \
	hi##x = bestcoords[x] + nhoodi + 1; \
	if(hi##x > netsize) \
		hi##x = netsize;

inline void SOMTrain_updatenodes_1d(float *bufData, float *inputdata, int *bestcoords, int netsize, int numinputdims, double alpha, int nhoodi, int nhoodisq){
	float* celldata;
	int distsq;
	SOMTrain_UN_INITFORDIM(0)
	
	// Foreach cell in the NN's neighbourhood:
	// NB "lo0" is the starting point but "hi0" is never reached
	for(i0 = lo0; i0 < hi0; ++i0){
		// NB for 1D, we don't need to narrow down from hypercubic region to hyperspherical region - they're the same
		distsq = (i0-bestcoords[0]) * (i0-bestcoords[0]);
			celldata = SOM_GETFRAME_1D(i0); // a float-pointer to the desired frame
			SOMTrain_updatenodes_update(celldata, numinputdims, alpha * (1.0 - LRNSCALE * sqrt((double)distsq/nhoodisq)), inputdata);
	}
}
inline void SOMTrain_updatenodes_2d(float *bufData, float *inputdata, int *bestcoords, int netsize, int numinputdims, double alpha, int nhoodi, int nhoodisq){
	float* celldata;
	int distsq;
	SOMTrain_UN_INITFORDIM(0)
	SOMTrain_UN_INITFORDIM(1)
	
	// Foreach cell in the NN's neighbourhood:
	// NB "lo0" is the starting point but "hi0" is never reached
	for(i0 = lo0; i0 < hi0; ++i0){
	for(i1 = lo1; i1 < hi1; ++i1){
		if((distsq = (i0-bestcoords[0])*(i0-bestcoords[0]) + (i1-bestcoords[1])*(i1-bestcoords[1])) < nhoodisq){ // Narrow down from hypercubic region to hyperspherical region
			celldata = SOM_GETFRAME_2D(i0, i1); // a float-pointer to the desired frame
			SOMTrain_updatenodes_update(celldata, numinputdims, alpha * (1.0 - LRNSCALE * sqrt((double)distsq/nhoodisq)), inputdata);
		}
	}
	}
}
inline void SOMTrain_updatenodes_3d(float *bufData, float *inputdata, int *bestcoords, int netsize, int numinputdims, double alpha, int nhoodi, int nhoodisq){
	float* celldata;
	int distsq;
	SOMTrain_UN_INITFORDIM(0)
	SOMTrain_UN_INITFORDIM(1)
	SOMTrain_UN_INITFORDIM(2)
	
	// Foreach cell in the NN's neighbourhood:
	// NB "lo0" is the starting point but "hi0" is never reached
	for(i0 = lo0; i0 < hi0; ++i0){
	for(i1 = lo1; i1 < hi1; ++i1){
	for(i2 = lo2; i2 < hi2; ++i2){
		if((distsq = (i0-bestcoords[0])*(i0-bestcoords[0]) + (i1-bestcoords[1])*(i1-bestcoords[1]) + (i2-bestcoords[2])*(i2-bestcoords[2])) < nhoodisq){ // Narrow down from hypercubic region to hyperspherical region
			celldata = SOM_GETFRAME_3D(i0, i1, i2); // a float-pointer to the desired frame
			SOMTrain_updatenodes_update(celldata, numinputdims, alpha * (1.0 - LRNSCALE * sqrt((double)distsq/nhoodisq)), inputdata);
		}
	}
	}
	}
}
inline void SOMTrain_updatenodes_4d(float *bufData, float *inputdata, int *bestcoords, int netsize, int numinputdims, double alpha, int nhoodi, int nhoodisq){
	float* celldata;
	int distsq;
	SOMTrain_UN_INITFORDIM(0)
	SOMTrain_UN_INITFORDIM(1)
	SOMTrain_UN_INITFORDIM(2)
	SOMTrain_UN_INITFORDIM(3)
	
	// Foreach cell in the NN's neighbourhood:
	// NB "lo0" is the starting point but "hi0" is never reached
	for(i0 = lo0; i0 < hi0; ++i0){
	for(i1 = lo1; i1 < hi1; ++i1){
	for(i2 = lo2; i2 < hi2; ++i2){
	for(i3 = lo3; i3 < hi3; ++i3){
		if((distsq = (i0-bestcoords[0])*(i0-bestcoords[0]) + (i1-bestcoords[1])*(i1-bestcoords[1]) + (i2-bestcoords[2])*(i2-bestcoords[2]) + (i3-bestcoords[3])*(i3-bestcoords[3])) < nhoodisq){ // Narrow down from hypercubic region to hyperspherical region
			celldata = SOM_GETFRAME_4D(i0, i1, i2, i3); // a float-pointer to the desired frame
			SOMTrain_updatenodes_update(celldata, numinputdims, alpha * (1.0 - LRNSCALE * sqrt((double)distsq/nhoodisq)), inputdata);
		}
	}
	}
	}
	}
}

// Update a single node to "push" its vector towards the input values:     data <--- data + alpha * (indata - data)
inline void SOMTrain_updatenodes_update(float* celldata, int numinputdims, double alpha, float *inputdata){
	for(int chan=0; chan < numinputdims; ++chan){
		// NB instead of indexing into the channels use postincrement; "celldata" address moves through the channels
		*celldata = (*celldata) + alpha * (inputdata[chan] - *celldata);
		++celldata;
	}
}

////////////////////////////////////////////////////////////////////

// the load function is called by the host when the plug-in is loaded
void load(InterfaceTable *inTable)
{
	ft = inTable;

	DefineDtorUnit(SOMTrain);
	DefineDtorUnit(SOMRd);
}
