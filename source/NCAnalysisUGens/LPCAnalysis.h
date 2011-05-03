/*
 *  LPCAnalysis.h
 *
 *  Created by Nicholas Collins on 10/09/2009.
 *  Copyright 2009 Nicholas M Collins. All rights reserved.
 *
 */

//overloading operator_new for this class http://herosys.net/w/?p=143
//way to call RTFree/RTAlloc or new and delete via intermediate function call, to preserve implementation independence?
//no because need to call new if another class; only want variable functionality if creating array of floats, etc
//so create own classes for templates? Note that stl code already used via new anyway!
//really do just need global replacement of new and delete...
//or function calls for new and delete if creating primitives? or #ifdef SCMEMORYALLOC



//define above this header file if compiling inside SuperCollider
#define SCMEMORYALLOC 1
//

#ifdef SCMEMORYALLOC
#include "SC_PlugIn.h"
#endif

//swap to double later if necessary for greater resolution of autocorrelation calculations for numerical stability
typedef double LPCfloat;


class LPCAnalysis {

public:

	int windowsize;
	float * windowfunction; //can add later if start cross fading consecutive windows with hop
	int windowtype; //0 is rectangular, 1 is triangular, 2 is squared cosine, 3 is custom passed in buffer?

	float * input;
	//float * output;
	//int blocksize;
	int numpoles;
	int pos;
	float * coeff;
	float * last;


	int testdelta;
	LPCfloat delta;
	LPCfloat latesterror;
	float G; //gain;

	//matrix calculations at double resolution to help avoid errors?
	//storage for Levinson-Durbin iteration to calculate coefficients
	LPCfloat * R;
	LPCfloat * preva;
	LPCfloat * a;

#ifdef SCMEMORYALLOC
	struct World * world;
	InterfaceTable *ft;

	//int _blocksize, blocksize(_blocksize),
	//
	LPCAnalysis(int _windowsize, int _windowtype=0, int offset=0, World * _world=0, InterfaceTable * it=0): windowsize(_windowsize), windowtype(_windowtype), world(_world), ft(it) {

		input= (float *)RTAlloc(world, windowsize*sizeof(float));
		windowfunction= (float *)RTAlloc(world, windowsize*sizeof(float));
		//output= RTAlloc(world, blocksize*sizeof(float));
		coeff= (float *)RTAlloc(world, windowsize*sizeof(float));

		last= (float *)RTAlloc(world, windowsize*sizeof(float));
		R= (LPCfloat *)RTAlloc(world, (windowsize+1)*sizeof(LPCfloat));
		preva= (LPCfloat *)RTAlloc(world, (windowsize+1)*sizeof(LPCfloat));
		a= (LPCfloat *)RTAlloc(world, (windowsize+1)*sizeof(LPCfloat));

		zeroAll();

		pos=offset;
	}


	~LPCAnalysis() {
		RTFree(world, input);
		RTFree(world, windowfunction);
		//RTFree(world, output);
		RTFree(world, coeff);
		RTFree(world, last);
		RTFree(world, R);
		RTFree(world, preva);
		RTFree(world, a);
	}


	//must pass in world since not stored yet
	void* operator new(size_t sz, World* wd, InterfaceTable * ft) {
		return RTAlloc(wd, sizeof(LPCAnalysis));
	}

	//use stored world to sort this
	void operator delete(void* pObject) {

		LPCAnalysis * lpc = (LPCAnalysis*)pObject;
		InterfaceTable * ft= lpc->ft;

		RTFree(lpc->world, (LPCAnalysis*)pObject);
	}

#else
	//, int _blocksize ,blocksize(_blocksize)
	LPCAnalysis(int _windowsize, int _windowtype=0, int offset=0): windowsize(_windowsize), windowtype(_windowtype) {
		//need to know about stupid unit->mWorld
		input= new float[windowsize];
		windowfunction= new float[windowsize];
		//output= new float[blocksize];
		coeff= new float[windowsize];
		last= new float[windowsize];

		R= new LPCfloat[windowsize+1];
		preva= new LPCfloat[windowsize+1];
		a= new LPCfloat[windowsize+1];

		zeroAll();

		pos=offset;
	};


	~LPCAnalysis() {
		delete [] input;
		//delete [] output;
		delete [] windowfunction;
		delete [] coeff;
		delete [] last;
		delete [] R;
		delete [] preva;
		delete [] a;

	}



#endif

	void zeroAll() {

		int i;

		numpoles=10;

		pos=0;

		for (i=0; i<windowsize;++i) {
			input[i]= 0.0;
			coeff[i]=0.0;
			last[i]=0.0;
		}

		int half= windowsize>>1;

		switch(windowtype) {

		default:
		case 0:
			//rectangular window
			for (i=0; i<windowsize;++i) {
				windowfunction[i]= 1.0;
			}
			break;

		case 1:
				//triangular window
				float mult= 1.0/(float)half;
				for (i=0; i<half;++i) {
					float value= mult*((float)i);
					windowfunction[i]= value;
					windowfunction[half+i]=1.0-value;
				}


			break;
		//
//		case 2:
//
//
//			break;
//
		}


		//for (i=0; i<blocksize;++i) {
//			output[i]= 0.0;
//		}

		testdelta=0;
		delta= 0.999;
		latesterror=0.0;

		G=0.0; //gain starts at zero;
	}


	//latest value of p is number of poles for fitting; if -1 adapt using criteria
	//assess if tonal or transient via error?
	//source is driver signal for filtering, newinput is data arriving for analysis
	void update(float * newinput, float * newsource, float * output, int numSamples, int p);

	void calculateOutput(float * source, float * target, int startpos, int num);

	void calculatePoles();

};
